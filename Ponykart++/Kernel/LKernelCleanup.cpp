#include "pch.h"
#include "Kernel/LKernel.h"
#include "Kernel/LKernelCleanup.h"
#include "Kernel/LKernelOgre.h"
#include "UI/UIMain.h"

using namespace Ponykart::LKernel;
using namespace Ponykart::LKernel::details;
using namespace Ponykart::UI;
using namespace Ogre;

namespace Ponykart
{
namespace LKernel
{
	void cleanup()
	{
		remakeSceneManager();
		levelObjects.clear();
	}

	void remakeSceneManager()
	{
		log("[Loading] Remaking SceneManager...");
		// get our old scene manager
		SceneManager* oldMgr = getG<SceneManager>();
		// destroy it
		getG<Root>()->destroySceneManager(oldMgr);
		// make our new scene manager
		SceneManager* newMgr = getG<Root>()->createSceneManager(SceneType::ST_EXTERIOR_FAR, "sceneMgr");

		// re-add it to the kernel objects
		gSceneManager = newMgr;
		// tell miyagi that it changed
		getG<UIMain>()->changeSceneManager(newMgr);
		// then dispose of the old one
		delete oldMgr;
	}
}
}

