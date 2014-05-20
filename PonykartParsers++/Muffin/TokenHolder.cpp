#include "TokenHolder.h"

using namespace std;
using namespace Ogre;
using namespace PonykartParsers;

ThingEnum TokenHolder::getEnumProperty(const string& propertyName) const
{
	string lowerPropName = propertyName;
	transform(begin(lowerPropName), end(lowerPropName), begin(lowerPropName), tolower);
	auto it = enumTokens.find(lowerPropName);
	if (it != enumTokens.end())
		return it->second;
	else
		throw string("That property was not found in the .thing file : "+propertyName);
}

ThingEnum TokenHolder::getEnumProperty(const string& propertyName, const ThingEnum& defaultValue) const
{
	string lowerPropName = propertyName;
	transform(begin(lowerPropName), end(lowerPropName), begin(lowerPropName), tolower);
	auto it = enumTokens.find(lowerPropName);
	if (it != enumTokens.end())
		return it->second;
	else
		return defaultValue;
}

Vector3 TokenHolder::getVectorProperty(const string& propertyName) const
{
	string lowerPropName = propertyName;
	transform(begin(lowerPropName), end(lowerPropName), begin(lowerPropName), tolower);
	auto vecIt = vectorTokens.find(lowerPropName);
	if (vecIt != vectorTokens.end())
		return vecIt->second;
	else
		throw string("That property was not found in the .thing file : "+propertyName);
}

Vector3 TokenHolder::getVectorProperty(const string& propertyName, const Vector3& defaultValue) const
{
	string lowerPropName = propertyName;
	transform(begin(lowerPropName), end(lowerPropName), begin(lowerPropName), tolower);
	auto vecIt = vectorTokens.find(lowerPropName);
	if (vecIt != vectorTokens.end())
		return vecIt->second;
	else
		return defaultValue;
}

Quaternion TokenHolder::getQuatProperty(const string& propertyName) const
{
	string lowerPropName = propertyName;
	transform(begin(lowerPropName), end(lowerPropName), begin(lowerPropName), tolower);
	auto it = quatTokens.find(lowerPropName);
	if (it != quatTokens.end())
		return it->second;
	else
		throw string("That property was not found in the .thing file : "+propertyName);
}

Quaternion TokenHolder::getQuatProperty(const string& propertyName, const Quaternion& defaultValue) const
{
	string lowerPropName = propertyName;
	transform(begin(lowerPropName), end(lowerPropName), begin(lowerPropName), tolower);
	auto it = quatTokens.find(lowerPropName);
	if (it != quatTokens.end())
		return it->second;
	else
		return defaultValue;
}

string TokenHolder::getStringProperty(const string& propertyName) const
{
	string lowerPropName = propertyName;
	transform(begin(lowerPropName), end(lowerPropName), begin(lowerPropName), tolower);
	auto sIt = stringTokens.find(lowerPropName);
	if (sIt != stringTokens.end())
		return sIt->second;
	else
		throw string("That property was not found in the .thing file : "+propertyName);
}

string TokenHolder::getStringProperty(const string& propertyName, const string& defaultValue) const
{
	string lowerPropName = propertyName;
	transform(begin(lowerPropName), end(lowerPropName), begin(lowerPropName), tolower);
	auto sIt = stringTokens.find(lowerPropName);
	if (sIt != stringTokens.end())
		return sIt->second;
	else
		return defaultValue;
}

float TokenHolder::getFloatProperty(const string& propertyName) const
{
	string lowerPropName = propertyName;
	transform(begin(lowerPropName), end(lowerPropName), begin(lowerPropName), tolower);
	auto fIt = floatTokens.find(lowerPropName);
	if (fIt != floatTokens.end())
		return fIt->second;
	else
		throw string("That property was not found in the .thing file : "+propertyName);
}

float TokenHolder::getFloatProperty(const string& propertyName, const float defaultValue) const
{
	string lowerPropName = propertyName;
	transform(begin(lowerPropName), end(lowerPropName), begin(lowerPropName), tolower);
	auto fIt = floatTokens.find(lowerPropName);
	if (fIt != floatTokens.end())
		return fIt->second;
	else
		return defaultValue;
}

bool TokenHolder::getBoolProperty(const string& propertyName) const
{
	string lowerPropName = propertyName;
	transform(begin(lowerPropName), end(lowerPropName), begin(lowerPropName), tolower);
	auto it = boolTokens.find(lowerPropName);
	if (it != boolTokens.end())
		return it->second;
	else
		throw string("That property was not found in the .thing file : "+propertyName);
}

bool TokenHolder::getBoolProperty(const string& propertyName, const bool defaultValue) const
{
	string lowerPropName = propertyName;
	transform(begin(lowerPropName), end(lowerPropName), begin(lowerPropName), tolower);
	auto it = boolTokens.find(lowerPropName);
	if (it != boolTokens.end())
		return it->second;
	else
		return defaultValue;
}

const unordered_map<string, Ogre::Vector3>& TokenHolder::getVectorTokens() const
{
	return vectorTokens;
}

const unordered_map<string, Ogre::Quaternion>& TokenHolder::getQuatTokens() const
{
	return quatTokens;
}

const unordered_map<string, float>& TokenHolder::getFloatTokens() const
{
	return floatTokens;
}

const unordered_map<string, string>& TokenHolder::getStringTokens() const
{
	return stringTokens;
}

void TokenHolder::setEnumProperty(const std::string& propertyName, ThingEnum value)
{
	string lowerPropName = propertyName;
	transform(begin(lowerPropName), end(lowerPropName), begin(lowerPropName), tolower);
	enumTokens[lowerPropName] = value;
}

void TokenHolder::setStringProperty(const std::string& propertyName, const std::string& value)
{
	string lowerPropName = propertyName;
	transform(begin(lowerPropName), end(lowerPropName), begin(lowerPropName), tolower);
	stringTokens[lowerPropName] = value;
}

void TokenHolder::setFloatProperty(const std::string& propertyName, float value)
{
	string lowerPropName = propertyName;
	transform(begin(lowerPropName), end(lowerPropName), begin(lowerPropName), tolower);
	floatTokens[lowerPropName] = value;
}

void TokenHolder::setBoolProperty(const std::string& propertyName, bool value)
{
	string lowerPropName = propertyName;
	transform(begin(lowerPropName), end(lowerPropName), begin(lowerPropName), tolower);
	boolTokens[lowerPropName] = value;
}

void TokenHolder::setVectorProperty(const std::string& propertyName, const Ogre::Vector3& value)
{
	string lowerPropName = propertyName;
	transform(begin(lowerPropName), end(lowerPropName), begin(lowerPropName), tolower);
	vectorTokens[lowerPropName] = value;
}

void TokenHolder::setQuatProperty(const std::string& propertyName, const Ogre::Quaternion& value)
{
	string lowerPropName = propertyName;
	transform(begin(lowerPropName), end(lowerPropName), begin(lowerPropName), tolower);
	quatTokens[lowerPropName] = value;
}
