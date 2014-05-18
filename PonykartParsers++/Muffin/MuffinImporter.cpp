#include "pch.h"
#include <fstream>
#include "Core/Settings.h"
#include "Muffin/MuffinImporter.h"


using namespace PonykartParsers;
using namespace std;

MuffinDefinition* MuffinImporter::parseByName(const string& nameOfMuffin, MuffinDefinition* worldDef)
{
	// the initial level before we start loading one is "null", so we need to avoid doing anything with that.
	if (!nameOfMuffin.size()) 
	{
		MuffinDefinition* emptyDef = new MuffinDefinition("");
		emptyDef->setEnumProperty("type", ThingEnum::EmptyLevel);
		emptyDef->finish();
		return emptyDef;
	}

	// make the file path
	string filePath = Settings::MuffinFileLocation + nameOfMuffin + Settings::MuffinFileExtension;
	// if the muffin file was not found
	ifstream file;
	file.open(filePath);
	if (!file.is_open())
		throw string(nameOfMuffin + ".muffin not found!");
	else
		file.close();

	return parseByFile(filePath, worldDef);
}
