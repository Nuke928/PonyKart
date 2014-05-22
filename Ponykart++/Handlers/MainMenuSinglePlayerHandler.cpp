#include "pch.h"
#include "Handlers/MainMenuSinglePlayerHandler.h"
#include "Handlers/UI/MainMenuUIHandler.h"
#include "Kernel/LKernel.h"
#include "Levels/LevelChangeRequest.h"
#include "Levels/LevelManager.h"
#include "UI/MainMenuManager.h"

#include <iostream>

using namespace Ponykart::Handlers;
using namespace Ponykart::Levels;
using namespace Ponykart::LKernel;
using namespace Ponykart::UI;
using namespace std;

MainMenuSinglePlayerHandler::MainMenuSinglePlayerHandler()
{
	MainMenuManager* mmm = getG<MainMenuManager>();
	mmm->onLevelSelect.push_back(bind(&MainMenuSinglePlayerHandler::onLevelSelect, this, _1));
	mmm->onCharacterSelect.push_back(bind(&MainMenuSinglePlayerHandler::onCharacterSelect, this, _1));
}

void MainMenuSinglePlayerHandler::onLevelSelect(const string& name)
{
	if (get<MainMenuUIHandler>()->getGameType() == GameTypeEnum::SinglePlayer)
		levelSelection = name;
}

void MainMenuSinglePlayerHandler::onCharacterSelect(const string& name)
{
	if (get<MainMenuUIHandler>()->getGameType() == GameTypeEnum::SinglePlayer) 
	{
		characterSelection = name;

		LevelChangeRequest request {
			levelSelection,
			std::vector<string> { characterSelection },
			false
		};
		getG<LevelManager>()->loadLevel(request,0);
	}
}

