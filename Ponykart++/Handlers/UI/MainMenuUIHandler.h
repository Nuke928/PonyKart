#ifndef MAINMENUUIHANDLER_H_INCLUDED
#define MAINMENUUIHANDLER_H_INCLUDED

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
	class MainMenuUIHandler
	{
	public:
		MainMenuUIHandler();
		GameTypeEnum getGameType();

	public:
		UI::MainMenuManager* mmm; ///< just keeping this as a field since we'll be using it so much

	private:
		GameTypeEnum gameType;
	};
} // Handlers
} // Ponykart

#endif // MAINMENUUIHANDLER_H_INCLUDED
