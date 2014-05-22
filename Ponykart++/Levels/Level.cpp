#include "pch.h"
#ifdef _WIN32
	#include <io.h>
	#include "WindowsCompat/dirent.h"
#else
	#include <unistd.h>
	#include <dirent.h>
#endif
#include "Actors/LThing.h"
#include "Core/Settings.h"
#include "Core/Spawner.h"
#include "Kernel/LKernel.h"
#include "Kernel/LKernelOgre.h"
#include "Levels/Level.h"
#include "Lua/LuaMain.h"
#include "Muffin/MuffinImporter.h"
#include "Physics/PhysicsMain.h"

using namespace std;
using namespace Ponykart;
using namespace Ponykart::Actors;
using namespace Ponykart::Core;
using namespace Ponykart::Levels;
using namespace Ponykart::LKernel;
using namespace Ponykart::Lua;
using namespace Ponykart::Physics;
using namespace PonykartParsers;

Level::Level(const std::string& Name)
{
	name = Name;

	if (!name.size())
		type = LevelType::EmptyLevel;
	else if (name == Settings::MainMenuName)
		type = LevelType::Menu;
}

const std::string& Level::getName() const
{
	return name;
}

LevelType Level::getType() const
{
	return type;
}

void Level::addThing(Actors::LThing* newThing)
{
	// try adding it without its ID
	auto it = things.find(newThing->getName());
	if (it != things.end())
	{
		stringstream ss; ss << newThing->getName() << newThing->getID();
		// okay that didn't work, now try adding it with its ID
		it = things.find(ss.str());
		if (it != things.end())
		{
			// still didn't work so we must've had a problem while adding it.
            LKernel::log("[Level][WARNING] addThing: Couldn't add LThing "+newThing->getName()+", id "+to_string(newThing->getID()));
		}
		else
			things.insert(pair<string, LThing*>(ss.str(), newThing));
	}
	else
		things.insert(pair<string, LThing*>(newThing->getName(), newThing));
}

const PonykartParsers::MuffinDefinition* const Level::getDefinition() const
{
	return definition;
}

void Level::readMuffin()
{
	MuffinImporter importer;
	definition = new MuffinDefinition(name);
	importer.parseByName(name, definition);
	for (string& file : definition->extraFiles)
		definition = importer.parseByName(file, definition);

	// get the type of the level
	ThingEnum tempType = definition->getEnumProperty("type");
	if (tempType == ThingEnum::All)
		type = LevelType::All;
	else if (tempType == ThingEnum::Race)
		type = LevelType::Race;
	else if (tempType == ThingEnum::EmptyLevel)
		type = LevelType::EmptyLevel;
	else if (tempType == ThingEnum::Menu)
		type = LevelType::Menu;
	else
		throw string("[ERROR] Level::readMuffin(): Can't convert ThingEnum to LevelEnum");
	
}

void Level::readDotSceneAndSetupPhysics()
{
	getG<PhysicsMain>()->loadPhysicsLevel(name);
}

void Level::createEntities() 
{
	// load up everything into this world
	Spawner* spawner = getG<Spawner>();
	for (ThingBlock& tb : definition->thingBlocks)
		spawner->spawn(tb.getThingName(), &tb);
}

void Level::runLevelScript() 
{
	DIR* dir_point = opendir((LuaMain::luaLevelFileLocation + name + "/").c_str());
	if (dir_point)
	{
		// TODO: Implement this function. It depends on Lua stuff.
		//getG<LuaMain>()->luaVM.Lua.GetFunction(name).call(this);
		//throw string("Level::runLevelScript(): Not implemented !");
	}
}

void Level::runThingScripts() 
{
	for (pair<const string, LThing*>& l : things)
		l.second->runScript();
}
