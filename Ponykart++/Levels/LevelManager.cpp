#include "pch.h"
#include "Actors/StaticGeometryManager.h"
#include "Actors/InstancedGeometryManager.h"
#include "Core/Pauser.h"
#include "Kernel/LKernel.h"
#include "Kernel/LKernelCleanup.h"
#include "Kernel/LKernelHandler.h"
#include "Kernel/LKernelOgre.h"
#include "Levels/LevelManager.h"
#include "Levels/LevelChangedEventArgs.h"
#include "Lua/LuaMain.h"
#include "Misc/ogreExtensions.h"
#include "Physics/PhysicsMain.h"

using namespace Extensions;
using namespace Ponykart::Actors;
using namespace Ponykart::Core;
using namespace Ponykart::Levels;
using namespace Ponykart::LKernel;
using namespace Ponykart::Lua;
using namespace Ponykart::Physics;
using namespace Ponykart;
using namespace Ogre;
using namespace std;

const float LevelManager::INITIAL_DELAY{0.1f};
LevelProgressEvent LevelManager::onLevelLoadProgress;
LevelEvent LevelManager::onLevelPreUnload; // Is fired a few frames before we even start unloading anything. Mostly used for stuff that still requires screen rendering, such as putting up a loading screen
LevelEvent LevelManager::onLevelLoad; // Is fired after the .muffins have been read and the .scene file has been used, but before we start actually creating any Things
LevelEvent LevelManager::onLevelUnload; // Is fired after the level handlers have been disposed, but before we clean out the scene manager.
LevelEvent LevelManager::onLevelPostLoad; // Is fired a few frames after the entire level load process, including after scripts have been run.

LevelManager::LevelManager()
{
	isValidLevel = false;
	hasRunPostInitEvents = false;
	elapsed = 0;
	frameOneRendered = frameTwoRendered = false;

}

void LevelManager::loadLevel(LevelChangeRequest request, float delay)
{
	Pauser::isPaused = false;
	LevelChangedEventArgs* eventArgs = new LevelChangedEventArgs(Level(request.newLevelName), *currentLevel, request);

	// fire our preUnload events
	for (auto f : onLevelPreUnload)
		if (f)
			f(eventArgs);

	if (delay > 0) 
	{
		// we need to have this because otherwise we start loading before we manage to render a frame, and we need to render a frame to
		// show stuff like a loading screen

		// reset these
		elapsed = 0;
		frameOneRendered = frameTwoRendered = false;

		// set up a little frame started handler with our events
		auto func = bind(&LevelManager::loadLevelNow, this, _1);
		preUnloadFrameStartedHandler = new LambdaFrameStartedHandler(
			[&](const Ogre::FrameEvent& evt){return delayedRun_FrameStarted(evt, delay, func, eventArgs); });
		get<Root>()->addFrameListener(preUnloadFrameStartedHandler);
	}
	else 
	{
		// if our delay is 0, just load the level and don't do any of the delayed stuff
		loadLevelNow(eventArgs);
	}
}

void LevelManager::loadLevelNow(LevelChangedEventArgs* args)
{
	Level* newLevel = &args->newLevel;

	// Unload current level
	unloadLevel(args);

	currentLevel = newLevel;

	// Load new Level
	if (newLevel != nullptr) 
	{
		LKernel::log("==========================================================");
		LKernel::log("======= Level loading: " + newLevel->getName() + " =======");
		LKernel::log("==========================================================");

		// load our resource group, if we have one
		invokeLevelProgress(args, "Initialising new resource group...");
		if (ResourceGroupManager::getSingleton().resourceGroupExists(newLevel->getName()) 
			&& !ResourceGroupManager::getSingleton().isResourceGroupInitialised(newLevel->getName()))
		{
			LKernel::log("[Loading] Initialising resource group: " + newLevel->getName());
			ResourceGroupManager::getSingleton().initialiseResourceGroup(newLevel->getName());
		}

		// load up the world definition from the .muffin file
		invokeLevelProgress(args, "Reading .muffin files...");
		newLevel->readMuffin();

		// set up shadows, create stuff in the .scene file, and set up physics
		invokeLevelProgress(args, "Reading .scene file and setting up shadows and physics...");
		setupShadows(getG<SceneManager>(), *newLevel); // TODO: It's an extension, implement it
		newLevel->readDotSceneAndSetupPhysics();

		// run our level loading events
		log("[Loading] Loading everything else...");
		invokeLevelProgress(args, "Invoking level load event...");
		invoke(onLevelLoad, args);

		// then put Things into our world
		invokeLevelProgress(args, "Creating entities...");
		newLevel->createEntities();
		// then load the rest of the handlers
		invokeLevelProgress(args, "Loading level handlers...");
		loadLevelHandlers(*newLevel);

		isValidLevel = true;

		if (isPlayableLevel())
			getG<PhysicsMain>()->startSimulation();

		// run our scripts
		invokeLevelProgress(args, "Running scripts...");
		getG<LuaMain>()->loadScriptFiles(newLevel->getName());
		newLevel->runLevelScript();
		newLevel->runThingScripts();

		invokeLevelProgress(args, "Building static and instanced geometry...");
		getG<StaticGeometryManager>()->build();
		getG<InstancedGeometryManager>()->build();
	}

	// if we're on the main menu, pause it
	if (newLevel->getName() == Settings::MainMenuName)
		Pauser::isPaused = true;

	// post load event needs to be delayed
	if (newLevel != nullptr) 
	{
		// reset these
		elapsed = 0;
		frameOneRendered = frameTwoRendered = false;

		// set up our handler
		auto lambda = [&](const Ogre::FrameEvent& evt)
		{
			auto invokeLambda = [&](LevelChangedEventArgs* a){invoke(onLevelPostLoad, a); };
			return delayedRun_FrameStarted(evt, INITIAL_DELAY, invokeLambda, args);
		};
		postLoadFrameStartedHandler = new LambdaFrameStartedHandler(lambda);
		get<Root>()->addFrameListener(postLoadFrameStartedHandler);
	}
}

/// Unloads the current level
/// - Sets IsValidLevel to false
/// - Runs the levelUnload events
/// - Tells the kernel to unload all level objects
/// - Delete the current level
void LevelManager::unloadLevel(LevelChangedEventArgs* eventArgs)
{
	if (currentLevel->getName().size()) 
	{
		LKernel::log("==========================================================");
		LKernel::log("==== Level unloading: " + currentLevel->getName() + " ====");
		LKernel::log("==========================================================");

		isValidLevel = false;

		//currentLevel->save();

		invokeLevelProgress(eventArgs, "Unloading level handlers...");
		unloadLevelHandlers();

		invokeLevelProgress(eventArgs, "Invoking level unload event...");
		// invoke the level unloading events
		invoke(onLevelUnload, eventArgs);

		invokeLevelProgress(eventArgs, "Cleaning up...");
		cleanup();

		delete currentLevel;
		currentLevel = nullptr;
	}
}

void LevelManager::invoke(LevelEvent e, LevelChangedEventArgs* args)
{
	for (auto& f : e)
		if (f)
			f(args);
}

void LevelManager::invokeLevelProgress(LevelChangedEventArgs* args, std::string message)
{
	for (auto& f : onLevelLoadProgress)
		f(args, message);
}

bool LevelManager::delayedRun_FrameStarted(const FrameEvent& evt, float delay,
	function<void(LevelChangedEventArgs*)> action, LevelChangedEventArgs* args)
{
	if (!frameOneRendered && elapsed > delay) // rendered one frame
	{
		frameOneRendered = true;
		elapsed = delay;
	}
	else if (!frameTwoRendered && elapsed > delay) // rendered two frames
	{
		frameTwoRendered = true;
		elapsed = delay;
	}
	else if (frameTwoRendered && elapsed > delay) // rendered three frames! Detach and run our method
	{
		detach();
		action(args);
	}

	elapsed += evt.timeSinceLastFrame;
	return true;
}
