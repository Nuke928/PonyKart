#include "pch.h"
#include <algorithm>
#include "Core/Options.h"
#include "Core/Spawner.h"
#include "Handlers/SceneEnvironmentHandler.h"
#include "Kernel/LKernel.h"
#include "Levels/LevelManager.h"
#include "Levels/LevelChangedEventArgs.h"
#include "Muffin/MuffinDefinition.h"
#include "Misc/ogreExtensions.h"

using namespace PonykartParsers;
using namespace Ponykart::Core;
using namespace Ponykart::Handlers;
using namespace Ponykart::Levels;
using namespace Ponykart::LKernel;
using namespace std;
using namespace Ogre;
using namespace Extensions;

SceneEnvironmentHandler::SceneEnvironmentHandler()
{
	LevelManager::onLevelPostLoad.push_back(bind(&SceneEnvironmentHandler::onLevelPostLoad,this,placeholders::_1));
}

void SceneEnvironmentHandler::onLevelPostLoad(LevelChangedEventArgs* eventArgs)
{
	if (eventArgs->newLevel.getType() == LevelType::Race) 
	{
		SceneManager* sceneMgr = getG<SceneManager>();
		const MuffinDefinition* def = eventArgs->newLevel.getDefinition();

		// background color
		getG<Viewport>()->setBackgroundColour(toColourValue(def->getVectorProperty("Background", Vector3::UNIT_SCALE)));

		// ambient
		sceneMgr->setAmbientLight(toColourValue(def->getVectorProperty("Ambient", Vector3::UNIT_SCALE)));

		// sunlight
		Light* light = sceneMgr->createLight("sun");
		light->setType(Light::LightTypes::LT_DIRECTIONAL);
		Vector3 direction = def->getVectorProperty("SunlightDirection", Vector3(0.1f, -1.f, 0.1f));
		direction.normalise();
		light->setDirection(direction);
		light->setDiffuseColour(toColourValue(def->getVectorProperty("SunlightColour", Vector3::UNIT_SCALE)));
		light->setSpecularColour(toColourValue(def->getVectorProperty("SunlightColour", Vector3::UNIT_SCALE)));
		// cast shadows if we want some
		if (Options::shadowDetail != ShadowDetailOption::None)
			light->setCastShadows(true);

		// skybox
		auto strtoks = def->getStringTokens();
		if (strtoks.find("skybox") != end(strtoks))
			sceneMgr->setSkyBox(true, def->getStringProperty("Skybox", {}), 399.f);

		// fog
		FogMode mode = FogMode::FOG_NONE;
		string sMode = def->getStringProperty("FogType", "None");
		transform(begin(sMode), end(sMode), begin(sMode), ::tolower);

		if (sMode == "none")
			mode = FogMode::FOG_NONE;
		else if (sMode == "exp")
			mode = FogMode::FOG_EXP;
		else if (sMode == "exp2")
			mode = FogMode::FOG_EXP2;
		else if (sMode == "linear")
			mode = FogMode::FOG_LINEAR;

		if (mode != FogMode::FOG_NONE) 
		{
			sceneMgr->setFog(
				mode,
				toColourValue(def->getQuatProperty("FogColour", Quaternion::IDENTITY)),
				0.001f,
				def->getFloatProperty("FogStart", 20),
				def->getFloatProperty("FogEnd", 100));
		}
	}
	else if (eventArgs->newLevel.getType() == LevelType::Menu)
		getG<SceneManager>()->setAmbientLight(ColourValue(1, 1, 1));
}
