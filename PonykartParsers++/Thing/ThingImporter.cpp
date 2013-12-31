#include <fstream>
#include <OgreLogManager.h>
#include <OgreResourceGroupManager.h>
#include "Misc/direntSearch.h"
#include "Thing/ThingDefinition.h"
#include "Thing/ThingImporter.h"
#include "Thing/ThingParser.h"

using namespace std;
using namespace Extensions;
using namespace PonykartParsers;
using namespace PonykartParsers::ThingParser;

std::unordered_map<std::string, std::string> ThingImporter::fileList;
#if !DEBUG
	bool ThingImporter::hasPreparedFileList(false);
#endif

ThingImporter::ThingImporter()
{
}

ThingDefinition* ThingImporter::parse(const std::string& nameOfThing)
{
	prepareFileList();

	// make the file path
	// this just searches for "media/things/foo.thing"
	string filePath;
	auto filePathIt = fileList.find(nameOfThing);
	// this searches subfolders for .things
	if (filePathIt == fileList.end())
		throw new string(nameOfThing + ".thing does not exist!");
	else
		filePath = filePathIt->second;

	Ogre::LogManager::getSingleton().logMessage("[ThingImporter] Importing and parsing thing: " + filePath);

	// read stuff
	ifstream fileStream(filePath, ios::in);
	if (!fileStream)
		throw string("ThingImporter: Can't open "+filePath);

	string fileContents;
    fileStream.seekg(0, std::ios::end);
    fileContents.resize(fileStream.tellg());
    fileStream.seekg(0, std::ios::beg);
    fileStream.read(&fileContents[0], fileContents.size());
    fileStream.close();

	ThingParser::Parser p;
	root = p.parse(fileContents);

	ThingDefinition* thingDef = new ThingDefinition(nameOfThing);

	parse(thingDef);

	thingDef->finish();

	return thingDef;
}

void ThingImporter::prepareFileList()
{
#if !DEBUG
	if (!hasPreparedFileList)
	{
#endif
		Ogre::ResourceGroupManager& rgm = Ogre::ResourceGroupManager::getSingleton();
		for (string group : rgm.getResourceGroups())
		{
			if (!rgm.isResourceGroupInitialised(group) || group == "Bootstrap")
				continue;

			auto resourceLocations = *(rgm.listResourceLocations(group));

			for (string loc : resourceLocations)
			{
				auto scripts = direntSearch(loc, ".thing");

				for (string file : scripts)
					fileList[getFilenameWithoutExtension(file)] = file;
			}
		}
#if !DEBUG
		hasPreparedFileList = true;
	}
#endif
}

void ThingImporter::parse(ThingDefinition* thingDef)
{
	for (unsigned a = 0; a < root->children.size(); a++)
	{
		Node* prop = root->children[a];
		switch (prop->type)
		{
			case NodeType::Rule_Property:
				parseProperty(thingDef, (RuleInstance*)((RuleInstance*)prop)->children[0]);
				break;
			case NodeType::Rule_Shape:
				parseShape(thingDef, (RuleInstance*)prop);
				break;
			case NodeType::Rule_Model:
				parseModel(thingDef, (RuleInstance*)prop);
				break;
			case NodeType::Rule_Ribbon:
				parseRibbon(thingDef, (RuleInstance*)prop);
				break;
			case NodeType::Rule_BillboardSet:
				parseBillboardSet(thingDef, (RuleInstance*)prop);
				break;
			case NodeType::Rule_Sound:
				parseSound(thingDef, (RuleInstance*)prop);
				break;
			default:
				break;
		}
	}
}

void ThingImporter::parseProperty(TokenHolder* holder, ThingParser::RuleInstance* prop)
{
	string propName = tolower(getNameFromProperty(prop), std::locale());
	switch (prop->type)
	{
		case NodeType::Rule_StringProperty:
			holder->setStringProperty(propName, parseStringProperty(prop));
			break;
		case NodeType::Rule_BoolProperty:
			holder->setBoolProperty(propName, parseBoolProperty(prop));
			break;
		case NodeType::Rule_EnumProperty:
			holder->setBoolProperty(propName, parseEnumProperty(prop));
			break;
		case NodeType::Rule_NumericProperty:
			holder->setFloatProperty(propName, parseFloatProperty(prop));
			break;
		case NodeType::Rule_Vec3Property:
			holder->setVectorProperty(propName, parseVectorProperty(prop));
			break;
		case NodeType::Rule_QuatProperty:
			holder->setQuatProperty(propName, parseQuatProperty(prop));
			break;
		default:
			break;
	}
}

/** Note that the .thing format uses xyzw but ogre uses wxyz! **/
Ogre::Quaternion ThingImporter::parseQuatProperty(ThingParser::RuleInstance* prop)
{
	Token* tok1 = (Token*)prop->children[2];
	Token* tok2 = (Token*)prop->children[4];
	Token* tok3 = (Token*)prop->children[6];
	Token* tok4 = (Token*)prop->children[8];

	float x = atof(tok1->image.c_str());
	float y = atof(tok2->image.c_str());
	float z = atof(tok3->image.c_str());
	float w = atof(tok4->image.c_str());

	return Ogre::Quaternion(w, x, y, z);
}
