#ifndef MUFFINIMPORTER_H_DEFINED
#define MUFFINIMPORTER_H_DEFINED

#include <string>
#include "Muffin/MuffinDefinition.h"

namespace PonykartParsers
{
	class MuffinImporter
	{
	public:
		MuffinDefinition* parseByName(const std::string& nameOfMuffin, MuffinDefinition* worldDef = nullptr);
		MuffinDefinition* parseByFile(const std::string& filePath, MuffinDefinition* worldDef = nullptr);
	};
}

#endif // MUFFINIMPORTER_H_DEFINED