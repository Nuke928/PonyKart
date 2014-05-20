#include "pch.h"
#include <functional>
#include "Actors/InstancedGeometryManager.h"
#include "Actors/LThing.h"
#include "Actors/Components/ModelComponent.h"
#include "Core/Options.h"
#include "Kernel/LKernel.h"
#include "Kernel/LKernelOgre.h"
#include "Levels/LevelManager.h"
#include "Thing/ThingDefinition.h"
#include "Thing/Blocks/ModelBlock.h"
#include "Thing/Blocks/ThingBlock.h"

using namespace std;
using namespace std::placeholders;
using namespace Ogre;
using namespace PonykartParsers;
using namespace Ponykart;
using namespace Ponykart::Actors;
using namespace Ponykart::Core;
using namespace Ponykart::Levels;
using namespace Ponykart::LKernel;

// Define static members
const float InstancedGeometryManager::_instancedRegionSize = 60.0;
const int InstancedGeometryManager::_numInstancesPerBatch = 1; ///< maximum 80

InstancedGeometryManager::InstancedGeometryManager()
 : regionDimensions(_instancedRegionSize, 200, _instancedRegionSize)
{
	LevelManager::onLevelUnload.push_back(bind(&InstancedGeometryManager::onLevelUnload,this,placeholders::_1));
}

void InstancedGeometryManager::add(ModelComponent* mc, ThingBlock* thingTemplate, ModelBlock* block, ThingDefinition* def)
{
	// if the model detail option is low and this model wants imposters, don't even make any instanced geometry of it
	if (Options::modelDetail == ModelDetailOption::Low)
		if (def->getBoolProperty("Imposters", false))
			return;

	auto sceneMgr = LKernel::gSceneManager;

	string meshName = block->getStringProperty("mesh");
	string mapRegion = thingTemplate->getStringProperty("MapRegion", string());
	string key = mapRegion + meshName;

	// create our entity if it doesn't exist
	if (ents.find(key) == ents.end())
	{
		ostringstream ss; ss<<mc->getName()<<mc->getId();
		Entity* ent = sceneMgr->createEntity(ss.str(), meshName);
		ent->setMaterialName(block->getStringProperty("Material", string()));
		// then add it to our dictionary
		ents.insert(pair<string, Entity*>(key, ent));
	}

	// get our transforms
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

	// put them in one class
	Transform trans{pos,orient,sca};

	// if the transforms dictionary doesn't contain the mesh yet, add a new one
	std::vector<Transform> v;
	if (transforms.find(key) == transforms.end())
		transforms.insert(pair<string,std::vector<Transform>>(key, v));

	// then put our transform into the dictionary
	transforms[key].push_back(trans);
}

/// 1: Create the entity
/// 2: Create the instanced geometry
/// 3: Add the entity 80 times or less to the instanced geometry
/// 4: Build the instanced geometry
/// 5: Add as much batches as you need to the geometry (650/80)
/// 6: Go through the instanced objects of the batches and adept the position, scale and orientation
/// 7: Release the entity (1)
/// 
/// http://www.ogre3d.org/forums/viewtopic.php?f=2&t=67093#p442869
void InstancedGeometryManager::build()
{
	SceneManager* sceneMgr = getG<SceneManager>();

	// first create all of the InstancedGeometry objects
	for (pair<string, Entity*> ent : ents) 
	{
		// its name is the mesh name
		InstancedGeometry* igeom = sceneMgr->createInstancedGeometry(ent.first);
		igeom->setCastShadows(false);
		igeom->setBatchInstanceDimensions(regionDimensions);

		// add the entities to our batch
		int numEnts = transforms[ent.first].size();
		int numEntsToAdd = numEnts > _numInstancesPerBatch ? _numInstancesPerBatch : numEnts;

		// add the entity ~80 times or less to the geometry
		for (int a = 0; a < numEntsToAdd; a++)
			igeom->addEntity(ent.second, Vector3::ZERO);

		igeom->setOrigin(Vector3::ZERO);
		igeom->build();

		// number of batch instances we need is number of entities / ~80, since that's the maximum number of instances a batch can do
		for (int a = 0; a < (int)((float)numEnts / (float)_numInstancesPerBatch); a++)
			igeom->addBatchInstance();

		// now we need to go through each instanced object and update its transform
		int transformIndex = 0;
		auto batchIterator = igeom->getBatchInstanceIterator();
		// first we go through each batch
		for (pair<Ogre::uint32, InstancedGeometry::BatchInstance*> batch : batchIterator) 
		{
			std::vector<Transform> entTransforms = transforms[ent.first];

			// then go through each object in the batch
			auto objectIterator = batch.second->getObjectIterator();

			for (pair<short, InstancedGeometry::InstancedObject*> obj : objectIterator) 
			{
				if (transformIndex >= entTransforms.size()) 
				{
					delete obj.second;
					continue;
				}

				// get the transform we'll use for this object
				Transform t = entTransforms[transformIndex];

				// update the object with the transform
				obj.second->setPosition(t.position);
				obj.second->setOrientation(t.orientation);
				obj.second->setScale(t.scale);

				// then increment the index we use to get transforms
				transformIndex++;
			}
		}
		sceneMgr->destroyEntity(ent.second);
		delete ent.second;

		log("Instanced Geometry: " + string(igeom->getName().c_str()));
		igeoms.emplace(ent.first, igeom);
	}
}

void InstancedGeometryManager::onLevelUnload(LevelChangedEventArgs* eventArgs)
{
	ents.clear();
	transforms.clear();

	auto sceneMgr = LKernel::gSceneManager;
	for (auto pair : igeoms)
	{
		sceneMgr->destroyInstancedGeometry(pair.second);
		delete pair.second;
	}
	igeoms.clear();
}
