#include "pch.h"
#include "Handlers/UI/MainMenuUIHandler.h"
#include "Kernel/LKernel.h"

using namespace Ponykart::Handlers;
using namespace Ponykart::LKernel;
using namespace Ponykart::UI;

MainMenuUIHandler::MainMenuUIHandler()
{
	mmm = getG<MainMenuManager>();
	gameType = GameTypeEnum::None;

	// TODO: Implement the rest of the ctor (registering events,...)
}

GameTypeEnum MainMenuUIHandler::getGameType()
{
	return gameType;
}

void MainMenuUIHandler::onGameType_SelectSinglePlayer()
{
	gameType = GameTypeEnum::SinglePlayer;
	// TODO: Draw the singleplayer GUI here
}
