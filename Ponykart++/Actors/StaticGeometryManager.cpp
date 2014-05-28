#include "pch.h"
#include <sstream>
#include "Actors/LThing.h"
#include "Actors/StaticGeometryManager.h"
#include "Actors/Components/ModelComponent.h"
#include "Core/Options.h"
#include "Kernel/LKernel.h"
#include "Kernel/LKernelOgre.h"
#include "Levels/LevelManager.h"
#include "Thing/ThingDefinition.h"
#include "Thing/Blocks/ModelBlock.h"
#include "Thing/Blocks/ThingBlock.h"

using namespace std;
using namespace Ogre;
using namespace PonykartParsers;
using namespace Ponykart;
using namespace Ponykart::Actors;
using namespace Ponykart::Core;
using namespace Ponykart::LKernel;
using namespace Ponykart::Levels;

// Static members
const float StaticGeometryManager::_staticRegionSize = 4;

StaticGeometryManager::StaticGeometryManager()
 : regionDimensions(_staticRegionSize, 200, _staticRegionSize)
{
	LevelManager::onLevelUnload.push_back(bind(&StaticGeometryManager::onLevelUnload,this,placeholders::_1));
}

void StaticGeometryManager::add(ModelComponent* mc, ThingBlock* thingTemplate, ModelBlock* block, ThingDefinition* def)
{
	// if the model detail option is low and this model wants imposters, don't even make any static geometry of it
	if (Options::modelDetail == ModelDetailOption::Low)
		if (def->getBoolProperty("Imposters", false))
			return;

	auto sceneMgr = LKernel::gSceneManager;

	string meshName = block->getStringProperty("mesh");
	// map region goes first
	string sgeomName = thingTemplate->getStringProperty("MapRegion", "(Default)");
	// static group can override map region
	sgeomName = block->getStringProperty("StaticGroup", sgeomName);
	Entity* ent;

	// get our entity if it already exists
	auto it = ents.find(meshName);
	if (it == ents.end())
	{
		ostringstream ss; ss<<meshName<<mc->getId();
		// getting the entity was not successful, so we have to create it
		ent = sceneMgr->createEntity(ss.str(), meshName);
		auto materialIt = block->getStringTokens().find("material");
		if (materialIt != block->getStringTokens().end())
			ent->setMaterialName(materialIt->second);

		ents.insert(pair<string,Entity*>(meshName, ent));
	}
	else
		ent = it->second;

	Vector3 pos;
	// two ways to get the position
	// inherit it from the lthing, the default (if we were using nodes, this would be the default too)
	if (block->getBoolProperty("InheritOrientation", true))
		pos = (mc->getOwner()->getSpawnOrientation()
				* block->getVectorProperty("position", Vector3::ZERO))
				+ thingTemplate->getVectorTokens().at("position");
	// or we can choose not to inherit it for whatever reason
	else
		pos = block->getVectorProperty("position", Vector3::ZERO) + thingTemplate->getVectorTokens().at("position");
	Quaternion orient = block->getQuatProperty("orientation", Quaternion::IDENTITY)
						* thingTemplate->getQuatProperty("orientation", Quaternion::IDENTITY);
	Vector3 sca = block->getVectorProperty("scale", Vector3::UNIT_SCALE);

	StaticGeometry* sg;
	auto sgIt = sgeoms.find(sgeomName);
	if (sgIt == sgeoms.end())
	{
		sg = LKernel::gSceneManager->createStaticGeometry(sgeomName);

		sg->setRegionDimensions(regionDimensions);
		sg->setRenderingDistance(300.0/5.0);

		sgeoms.insert(pair<string,StaticGeometry*>(sgeomName, sg));
	}
	else
		sg=sgIt->second;

	sg->addEntity(ent, pos, orient, sca);
}

void StaticGeometryManager::build()
{
	for (pair<String, StaticGeometry*> sg : sgeoms) 
	{
		log("Building static geometry: " + sg.second->getName());
		sg.second->build();
	}

	SceneManager* sceneMgr = get<SceneManager>();
	for (pair<string,Entity*> e : ents) 
	{
		sceneMgr->destroyEntity(e.second);
		//delete e.second; TODO: FIXME: Memory leak. Uncommenting crashes.
	}
}

void StaticGeometryManager::onLevelUnload(LevelChangedEventArgs* eventArgs)
{
	ents.clear();

	auto sceneMgr = LKernel::gSceneManager;
	for (auto pair : sgeoms)
	{
		if (pair.second != nullptr)
		{
			sceneMgr->destroyStaticGeometry(pair.second);
			delete pair.second;
		}
	}
	sgeoms.clear();
}
