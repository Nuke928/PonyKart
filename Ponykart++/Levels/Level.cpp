#include "pch.h"
#include "Actors/LThing.h"
#include "Core/Settings.h"
#include "Kernel/LKernelOgre.h"
#include "Levels/Level.h"

using namespace std;
using namespace Ponykart;
using namespace Ponykart::Actors;
using namespace Ponykart::Levels;

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
	if (it == things.end())
	{
		stringstream ss; ss << newThing->getName() << newThing->getID();
		// okay that didn't work, now try adding it with its ID
		it = things.find(ss.str());
		if (it == things.end())
		{
			// still didn't work so we must've had a problem while adding it.
			LKernel::log("[Level] **WARNING** (AddThing) A problem occurred when we tried to add this new LThing to the Things dictionary!");
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