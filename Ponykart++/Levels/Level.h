#ifndef LEVEL_H_INCLUDED
#define LEVEL_H_INCLUDED

#include <string>
#include <unordered_map>
#include "Levels/LevelType.h"

namespace PonykartParsers
{
	class MuffinDefinition;
} // PonykartParsers

namespace Ponykart
{

namespace Actors
{
	class LThing;
} // Actors

namespace Levels
{
/// Represents a level or world in the game.
class Level // TODO: Translate the whole class correctly
{
public:
	Level(const std::string& name);
	void addThing(Actors::LThing* newThing); ///< Runs whenever we spawn something. This just adds it to the level's dictionary of Things.
	void readMuffin(); ///< Reads the main .muffin file for this level and loads any extra ones that were "linked" from the main one
	void readDotSceneAndSetupPhysics(); ///< Parses a .scene file and sets up physics stuff
	void createEntities(); ///< Parses the level's save file and creates all of the Things that will be in it. Also creates the player.
	/// Runs the lua function that uses this level's name, if it exists.
	/// If this documentation is up to date, it should be in "media/scripts/levelNameGoesHere"
	void runLevelScript();
	void runThingScripts(); ///< Runs all of the scripts that the .things have defined, if any
	// Getters
	const std::string& getName() const;
	LevelType getType() const;
	const PonykartParsers::MuffinDefinition* const getDefinition() const;
	const std::unordered_map<std::string, Actors::LThing*>& getThings() const;
private:
	// Set-private members. Getters are public.
	std::string name; ///< The world's name - this serves as its identifier
	LevelType type; ///< The type of this level
	PonykartParsers::MuffinDefinition* definition;
	std::unordered_map<std::string, Actors::LThing*> things; ///< We use the thing's Name as the key
};
} // Levels
} // Ponykart

#endif // LEVEL_H_INCLUDED
