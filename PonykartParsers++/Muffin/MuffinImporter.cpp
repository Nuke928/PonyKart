#include "pch.h"
#include <algorithm>
#include <fstream>
#include "Core/Settings.h"
#include "Muffin/MuffinImporter.h"

using namespace PonykartParsers;
using namespace MuffinParser;
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

/// Parses a .muffin file and puts it into a MuffinDefinition
/// @param filePath The filepath of the muffin to load.
/// @param worldDef If you've already got a muffin definition, pass it here and this method will add to it instead of making a new one.
/// @return A muffin definition with the stuff from the specified muffin file.
MuffinDefinition* MuffinImporter::parseByFile(const string& filePath, MuffinDefinition* worldDef)
{
	if (Ogre::LogManager::getSingletonPtr() != nullptr)
		Ogre::LogManager::getSingletonPtr()->logMessage("[MuffinImporter] Importing and parsing world: " + filePath);

	// read stuff
	ifstream file;
	file.open(filePath, ios_base::binary);
	if (!file.is_open())
		throw string("MuffinImporter::parseByFile: Can't open file");

	file.seekg(0, ios::end);
	int size = file.tellg();
	file.seekg(0, ios::beg);
	char* rawcontent = new char[size];
	file.read(rawcontent, size);
	string contents(rawcontent,size);
	delete rawcontent;
	file.close();

	// Parse
	MuffinParser::Parser* p = new MuffinParser::Parser();
	root = p->parse(contents);

	if (worldDef == nullptr)
		worldDef = new MuffinDefinition(filePath);

	parse(worldDef);

	for (auto file : extraFiles)
		worldDef->extraFiles.push_back(file);
	extraFiles.clear();


	worldDef->finish();

	return worldDef;
}

void MuffinImporter::parse(MuffinDefinition* worldDef)
{
	for (int a = 0; a < root->children.size(); a++) 
	{
		Node* prop = root->children[a];
		switch (prop->type) 
		{
		case NodeType::Rule_Property:
			parseProperty(worldDef, (RuleInstance*)((RuleInstance*)prop)->children[0]);
			break;
		case NodeType::Rule_Block:
			parseBlock(worldDef, (RuleInstance*)prop);
			break;
		}
	}
}

void MuffinImporter::parseProperty(TokenHolder* holder, MuffinParser::RuleInstance* prop)
{
	string propName = getNameFromProperty(prop);
	transform(begin(propName), end(propName), begin(propName), ::tolower);
	
	if (propName == "loadfile") // if we have some extra files, load these too
		extraFiles.push_back(parseStringProperty(prop));
	else // otherwise continue as normal
	{
		switch (prop->type) 
		{
		case NodeType::Rule_StringProperty:
			holder->setStringProperty(propName, parseStringProperty(prop));
			break;
		case NodeType::Rule_BoolProperty:
			holder->setBoolProperty(propName, parseBoolProperty(prop));
			break;
		case NodeType::Rule_EnumProperty:
			holder->setEnumProperty(propName, parseEnumProperty(prop));
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
		}
	}
}

string MuffinImporter::getNameFromProperty(MuffinParser::RuleInstance* prop)
{
	RuleInstance* nameRule = (RuleInstance*)prop->children[0];
	Token* nameTok = (Token*)nameRule->children[0];
	return nameTok->image;
}

void MuffinImporter::parseBlock(MuffinDefinition* worldDef, MuffinParser::RuleInstance* block)
{
	Token* nameTok = (Token*)((RuleInstance*)block->children[0])->children[0];

	ThingBlock thingBlock(nameTok->image, worldDef);

	for (int a = 2; a < block->children.size() - 1; a++) 
	{
		RuleInstance* rule = (RuleInstance*)block->children[a];
		if (rule->type == NodeType::Rule_Property)
			parseProperty(&thingBlock, (RuleInstance*)rule->children[0]);
	}
	worldDef->thingBlocks.push_back(thingBlock);
}

std::string MuffinImporter::parseStringProperty(MuffinParser::RuleInstance* prop)
{
	Token* valTok = (Token*)prop->children[2];
	string val = valTok->image;

	// need to substring because we get "\"foo\"" from the file, and we don't want extra quotes
	return val.substr(1, val.size() - 2);
}

bool MuffinImporter::parseBoolProperty(MuffinParser::RuleInstance* prop)
{
	Token* valTok = (Token*)prop->children[2];
	if (valTok->type == NodeType::Tok_KeyTrue)
		return true;
	else if (valTok->type == NodeType::Tok_KeyFalse)
		return false;
	else
		throw string("MuffinImporter::parseBoolProperty: Boolean property is not true nor false!");
}


ThingEnum MuffinImporter::parseEnumProperty(RuleInstance* prop) 
{
	RuleInstance* valRule = (RuleInstance*)prop->children[2];
	Token* valTok = (Token*)valRule->children[0];
	ThingEnum te;


	int result = -1;
	for (unsigned i = 0; i < ThingEnumMap.size(); i++) // Parse valTok->image into a ThingEnum
	{
		string s1{ ThingEnumMap[i] };
		string s2{ valTok->image };
		transform(s1.begin(), s1.end(), s1.begin(), ::tolower);
		transform(s2.begin(), s2.end(), s2.begin(), ::tolower);
		if (s1 == s2)
			result = i;
	}

	if (result != -1)
		return ThingEnum(result);
	else
		throw string("MuffinImporter::parseEnumProperty: Unable to parse Enum property!");
}

float MuffinImporter::parseFloatProperty(MuffinParser::RuleInstance* prop)
{
	Token* tok = (Token*)prop->children[2];
	return (float)atof(tok->image.c_str());
}

Ogre::Vector3 MuffinImporter::parseVectorProperty(RuleInstance* prop)
{
	Token* tok1 = (Token*)prop->children[2];
	Token* tok2 = (Token*)prop->children[4];
	Token* tok3 = (Token*)prop->children[6];

	float x = (float)atof(tok1->image.c_str());
	float y = (float)atof(tok2->image.c_str());
	float z = (float)atof(tok3->image.c_str());

	return Ogre::Vector3(x, y, z);
}

Ogre::Quaternion MuffinImporter::parseQuatProperty(RuleInstance* prop)
{
	Token* tok1 = (Token*)prop->children[2];
	Token* tok2 = (Token*)prop->children[4];
	Token* tok3 = (Token*)prop->children[6];
	Token* tok4 = (Token*)prop->children[8];

	float x = (float)atof(tok1->image.c_str());
	float y = (float)atof(tok2->image.c_str());
	float z = (float)atof(tok3->image.c_str());
	float w = (float)atof(tok4->image.c_str());

	return Ogre::Quaternion(w, x, y, z);
}
