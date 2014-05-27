#include "pch.h"
#include "Handlers/UI/MainMenuUIHandler.h"
#include "Handlers/BackgroundMusicHandler.h"
#include "Handlers/CameraCreator.h"
#include "Handlers/MainMenuSinglePlayerHandler.h"
#include "Handlers/SceneEnvironmentHandler.h"
#include "Kernel/LKernel.h"
#include "Kernel/LKernelHandler.h"
#include "Kernel/LKernelOgre.h"
#include "Levels/Level.h"

using namespace Ponykart;
using namespace Ponykart::Handlers;
using namespace Ponykart::LKernel;
using namespace Ponykart::Levels;
using namespace std;

namespace Ponykart
{
namespace LKernel
{
	void loadLevelHandlers(Levels::Level& newLevel)
	{
		// TODO: Implement, requires implementing a RTTI mess
		addGlobalObject(new CameraCreatorHandler());
	}

	void unloadLevelHandlers()
	{
		// TODO: Implement, requires implementing a RTTI mess
		//throw string("unloadLevelHandlers(): Not implemented");
	}

	void loadGlobalHandlers()
	{
		// TODO: Add the rest of the handlers
		log("[Loading] Initialising global handlers...");
		addGlobalObject(new SceneEnvironmentHandler());
		addGlobalObject(new MainMenuUIHandler());
		addGlobalObject(new MainMenuSinglePlayerHandler());
		addGlobalObject(new BackgroundMusicHandler());
	}
}
}
