#ifndef MUFFINIMPORTER_H_DEFINED
#define MUFFINIMPORTER_H_DEFINED

#include <string>
#include <vector>
#include "Muffin/MuffinDefinition.h"
#include "Muffin/MuffinParser.h"

namespace PonykartParsers
{
	class MuffinImporter
	{
	public:
		MuffinDefinition* parseByName(const std::string& nameOfMuffin, MuffinDefinition* worldDef = nullptr);
		MuffinDefinition* parseByFile(const std::string& filePath, MuffinDefinition* worldDef = nullptr);
		void parse(MuffinDefinition* worldDef); ///< Parses right from the root
		void parseProperty(TokenHolder* holder, MuffinParser::RuleInstance* prop); ///< Takes a property and calls the appropriate parser method depending on its type
		std::string getNameFromProperty(MuffinParser::RuleInstance* prop); ///< Gets a name out of a property. It's in its own method because it's used all the time.
		void parseBlock(MuffinDefinition* worldDef, MuffinParser::RuleInstance* block); ///< Shape blocks
		std::string parseStringProperty(MuffinParser::RuleInstance* prop); ///< Parse a string property
		bool parseBoolProperty(MuffinParser::RuleInstance* prop); ///< Parse a bool property
		ThingEnum parseEnumProperty(MuffinParser::RuleInstance* prop); ///< Parse an enum property. The value must exist in MuffinEnum, but it is not case sensitive.
		float parseFloatProperty(MuffinParser::RuleInstance* prop); ///< Parse a float property
		Ogre::Vector3 parseVectorProperty(MuffinParser::RuleInstance* prop); ///< Parse a vector property, i.e. a triplet of floats separated by commas
		/// Parse a quaternion property, i.e. a quartet of floats separated by commas.
		/// Note that the .scene format uses xyzw but ogre uses wxyz!
		Ogre::Quaternion parseQuatProperty(MuffinParser::RuleInstance* prop);

	private:
		std::vector<std::string> extraFiles;
		MuffinParser::RuleInstance* root;
	};
}

#endif // MUFFINIMPORTER_H_DEFINED