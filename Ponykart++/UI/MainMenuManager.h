#ifndef MAINMENUMANAGER_H_INCLUDED
#define MAINMENUMANAGER_H_INCLUDED

#include <string>
#include "Kernel/LKernelObject.h"

namespace Ponykart
{
namespace UI
{
	using MainMenuCharacterSelectEvent = std::vector<std::function<void(std::string&)>>;
	using MainMenuLevelSelectEvent = std::vector<std::function<void(std::string&)>>;

	/// This class handles all of the main menu GUI event-firing
	class MainMenuManager : public LKernel::LKernelObject
	{
	public:
		MainMenuManager();

	public:
		MainMenuLevelSelectEvent onLevelSelect;
		MainMenuCharacterSelectEvent onCharacterSelect;
	};
}
}

#endif // MAINMENUMANAGER_H_INCLUDED