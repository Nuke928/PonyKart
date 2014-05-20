#include "pch.h"
#include <fstream>
#include <OgreConfigFile.h>
#include "Core/Options.h"
#include "Kernel/LKernel.h"
#include "Kernel/LKernelOgre.h"

using namespace Ponykart;
using namespace Ponykart::Core;
using namespace Ponykart::LKernel;
using namespace Ogre;
using namespace std;

// Define the static members
unordered_map<string, string> Options::dict;
unordered_map<string, string> Options::defaults;
ModelDetailOption Options::modelDetail;
ShadowDetailOption Options::shadowDetail;

void Options::setupDictionaries()
{
	defaults=unordered_map<string,string>({
		{"FSAA","0"},
		{"Floating-point mode","Fastest"}, // Fastest or Accurate
		{"Full Screen","No"}, // Yes or No
		{"VSync","Yes"}, // Yes or No
		{"Video Mode","1024x768"},
		{"sRGB Gamma Conversion","No"}, // Yes or No
		{"Sounds","Yes"},
		{"Music","Yes"}, // Yes or No
		{"Sound Volume", "50"},
		{"Music Volume", "50"},
		{"Ribbons","Yes"},
		{"ModelDetail","Medium"}, // Low, Medium or High
		{"ShadowDetail","Some"}, // None, Some or Many
		{"ShadowDistance","40"},
		{"Twh","No"},
		{"Controller", "Keyboard"},
		{"Mouse Sensitivity", "1.0"},
	});
	dict = defaults; // copy it into the regular dictionary
}

void Options::initialize()
{
	log("[Loading] Loading configuration...");

	setupDictionaries();

	string optionsPath = LKernel::prefPath + "ponykart.cfg";
	
#ifdef WIN32
	// On Windows, create the directory first
	string folder;
	auto end = prefPath.find('\\');
	while (end != string::npos)
	{
		folder = prefPath.substr(0,end+1);
		if (!CreateDirectory(folder.c_str(), NULL))
		{
			DWORD err = GetLastError();
			if (err != ERROR_ALREADY_EXISTS)
				log("[Error] Couldn't create options directory : " + prefPath + " (error " + to_string(err)+")");
		}
		end = prefPath.find('\\', ++end);
	}
#endif

	fstream file;
	file.open(optionsPath.c_str(), ios::in);
	if (!file.is_open()) // create it if the file doesn't exist, and write out some defaults
	{
		file.open(optionsPath.c_str(), ios::out);
		if (!file.is_open()) throw string("Cannot initialize " + optionsPath);
		for (auto setting : defaults)
			file << setting.first << "=" << setting.second << endl;
		file.close();
		modelDetail = ModelDetailOption::Medium;
		shadowDetail = ShadowDetailOption::Some;
	}
	else // otherwise we just read from it
	{
		file.close(); // We're going to open it with cfile
		ConfigFile cfile;
		cfile.load(optionsPath.c_str(), "=", true);
		auto sectionIterator = cfile.getSectionIterator();
		ConfigFile::SettingsMultiMap* settings=sectionIterator.getNext();
		for (auto curPair : *settings)
			dict[curPair.first] = curPair.second;
		// Convert the settings for ModelDetail and ShadowDetail (string) into enum values (int)
		string enumStr = dict["ModelDetail"];
		modelDetail = enumStr=="High"?ModelDetailOption::High: (enumStr=="Low"?ModelDetailOption::Low: (ModelDetailOption::Medium));
		enumStr = dict["ShadowDetail"];
		shadowDetail = enumStr=="Many"?ShadowDetailOption::Many: (enumStr=="None"?ShadowDetailOption::None: (ShadowDetailOption::Some));
	}

#ifdef DEBUG
	// since we sometimes add new options, we want to make sure the .ini file has all of them
	save();
#endif

	log("[Loading] Configuration loaded.");
}

void Options::save()
{
	string optionsPath = LKernel::prefPath + "ponykart.cfg";

	dict["ModelDetail"] = modelDetail==ModelDetailOption::High?"High": (modelDetail==ModelDetailOption::Low?"Low": ("Medium"));
	dict["ShadowDetail"] = shadowDetail==ShadowDetailOption::Many?"Many": (shadowDetail==ShadowDetailOption::None?"None": ("Some"));

	fstream file;
	file.open(optionsPath.c_str(), ios::out);
	if (!file.is_open()) throw string("Cannot save " + optionsPath);

	for (auto setting : dict)
		file << setting.first << "=" << setting.second << endl;
}

string Options::get(const string &keyName)
{
	// TODO: The string comparisons should be case-insensitive, like in the C# code
	if (keyName == "ModelDetail")
		throw new string("Options::getBool: Use the Options.ModelDetail enum instead of this method!");
	else if (keyName == "ShadowDetail")
		throw new string("Use the Options.ShadowDetail enum instead of this method!");

	return dict[keyName];
}

bool Options::getBool(const string &keyName)
{
	string value = dict[keyName];
	if (value == "Yes" || value == "yes" || value == "True" || value == "true")
		return true;
	else if (value == "No" || value == "no" || value == "False" || value == "false")
		return false;
	else
		throw string("Options::getBool: The key '"+keyName+"' does not have a valid boolean value!");
}

int Options::getInt (const string &keyName)
{
	string value = dict[keyName];
	try {
		return std::strtol(value.c_str(), nullptr, 10);
	} catch (...) {
		throw string("Options::getInt: The key '"+keyName+"' does not have a valid integer value!");
	}
}

float Options::getFloat (const string &keyName)
{
	string value = dict[keyName];
	try {
		return std::strtof(value.c_str(), nullptr);
	} catch (...) {
		throw string("Options::getFloat: The key '"+keyName+"' does not have a valid floating point value!");
	}
}

