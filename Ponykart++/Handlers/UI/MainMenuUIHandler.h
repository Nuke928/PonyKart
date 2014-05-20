#ifndef MAINMENUUIHANDLER_H_INCLUDED
#define MAINMENUUIHANDLER_H_INCLUDED

#include "Kernel/LKernelObject.h"
#include "UI/MainMenuManager.h"

namespace Ponykart
{
namespace Handlers
{
	enum GameTypeEnum
	{
		None,
		SinglePlayer,
		NetworkedHost,
		NetworkedClient
	};

	/// Handler to respond to some of the main menu UI events and change between the different menus
	class MainMenuUIHandler : public LKernel::LKernelObject
	{
	public:
		MainMenuUIHandler();
		GameTypeEnum getGameType();
		void onGameType_SelectSinglePlayer();

	public:
		UI::MainMenuManager* mmm; ///< just keeping this as a field since we'll be using it so much

	private:
		GameTypeEnum gameType;
	};
} // Handlers
} // Ponykart

#endif // MAINMENUUIHANDLER_H_INCLUDED
