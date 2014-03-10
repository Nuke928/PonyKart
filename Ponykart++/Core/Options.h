#ifndef OPTIONS_H_INCLUDED
#define OPTIONS_H_INCLUDED

#include <string>
#include <unordered_map>
#include "Core/OptionsEnums.h"

namespace Ponykart
{
namespace Core
{
class Options // C#-static class : All member variables/functions should be static
{
public:
	 static void initialize(); ///< Creates the folder and file if they don't exist, and either prints some data to it (if it doesn't exist) or reads from it (if it does)
	 static void save(); ///< Writes out the current settings to our options file
	 static std::string get(const std::string &keyName); ///< Gets an option.
	 static bool getBool(const std::string &keyName); ///< Gets an option as a boolean.
	 static int getInt(const std::string &keyName); ///< Gets an options as an int.
	 static float getFloat(const std::string &keyName); ///< Get an option as a float.

private:
	explicit Options();
	static void setupDictionaries(); ///< set up our dictionary with some default stuff in it

private:
	static std::unordered_map<std::string, std::string> dict;
	static std::unordered_map<std::string, std::string> defaults;
public:
	static ModelDetailOption modelDetail;
	static ShadowDetailOption shadowDetail;
};
} // Core
} // Ponykart

#endif // OPTIONS_H_INCLUDED
