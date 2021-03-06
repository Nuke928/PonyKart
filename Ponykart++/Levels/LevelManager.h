#ifndef LEVELMANAGER_H_INCLUDED
#define LEVELMANAGER_H_INCLUDED

#include <string>
#include <functional>
#include <Ogre.h>
#include "Kernel/LKernelObject.h"
#include "Levels/Level.h"
#include "Levels/LevelChangeRequest.h"
#include "Core/Settings.h"

// TODO: Implement all the required events

namespace Ponykart
{
namespace Levels
{

class LevelChangedEventArgs;

using LevelEvent = std::vector<std::function<void (LevelChangedEventArgs*)>>;
using LevelProgressEvent = std::vector<std::function<void (LevelChangedEventArgs*,std::string)>>;

class LevelManager : public LKernel::LKernelObject
{
public:
	explicit LevelManager();
	void runPostInitEvents(); ///< runs level manager stuff that needs to run immediately after kernel setup
	/// Loads a level!
	/** @param delay Minimum time to wait (in seconds) before we load the level, to let stuff like loading screens have a chance to render.) */
	void loadLevel(LevelChangeRequest request, float delay = INITIAL_DELAY);
	// Getters
	inline Level* getCurrentLevel() {return currentLevel;};
	inline bool getIsValidLevel() {return isValidLevel;};
	inline bool isPlayableLevel() {return currentLevel && currentLevel->getType()==LevelType::Race && currentLevel->getName()!=Settings::MainMenuName;};

private:
	void unloadLevel(LevelChangedEventArgs* eventArgs); ///< Unloads the current level
	void loadLevelNow(LevelChangedEventArgs* args); ///< Unloads the current level and loads the new level
	bool delayedRun_FrameStarted(const Ogre::FrameEvent& evt, float delay, 
					std::function<void(LevelChangedEventArgs*)> action, LevelChangedEventArgs* args); ///< Runs something after both the specified time has passed and two frames have been rendered.
	void detach(); ///< Unhook from the frame started event
	void invoke(LevelEvent e, LevelChangedEventArgs* args); ///< Helper
	void invokeLevelProgress(LevelChangedEventArgs* args, std::string message);

private:
	/// This class is an ogre FrameListener taking a lambda to be called in frameStarted as ctor argument
	class LambdaFrameStartedHandler : public Ogre::FrameListener
	{
	public:
		LambdaFrameStartedHandler(std::function<bool(const Ogre::FrameEvent& evt)> lambda)
			: frameStartedLambda(lambda) {}
		bool frameStarted(const Ogre::FrameEvent& evt) override
		{
			return frameStartedLambda(evt);
		}

	private:
		std::function<bool(const Ogre::FrameEvent& evt)> frameStartedLambda;
	};
	LambdaFrameStartedHandler* preUnloadFrameStartedHandler;
	LambdaFrameStartedHandler* postLoadFrameStartedHandler;

private:
	Level* currentLevel;
	bool hasRunPostInitEvents;
	bool isValidLevel;
	static const float INITIAL_DELAY; ///< time to wait until we run the event
	float elapsed; ///< used in the FrameStarted method
	bool frameOneRendered, frameTwoRendered; ///< keeps track of how many frames we've rendered
	// a little hacky workaround so we can still have a FrameStarted event run but with a few extra arguments
	//Ogre::FrameListener.FrameStartedHandler preUnloadFrameStartedHandler; //TODO: Fix those
	//Ogre::FrameListener.FrameStartedHandler postLoadFrameStartedHandler;
public:
	static LevelProgressEvent onLevelLoadProgress;
	/// Is fired a few frames before we even start unloading anything.
	/** Mostly used for stuff that still requires screen rendering, such as putting up a loading screen */
	static LevelEvent onLevelPreUnload;
	static LevelEvent onLevelLoad; ///< Is fired after the .muffins have been read and the .scene file has been used, but before we start actually creating any Things
	static LevelEvent onLevelUnload; ///< Is fired after the level handlers have been disposed, but before we clean out the scene manager.
	static LevelEvent onLevelPostLoad; ///< Is fired a few frames after the entire level load process, including after scripts have been run.
};

} // Levels
} // Ponykart

#endif // LEVELMANAGER_H_INCLUDED
