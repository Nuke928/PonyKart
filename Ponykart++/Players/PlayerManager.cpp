#include "pch.h"
#include <map>
#include "Core/Options.h"
#include "Kernel/LKernel.h"
#include "Kernel/LKernelOgre.h"
#include "Levels/LevelManager.h"
#include "Networking/NetworkEntity.h"
#include "Networking/NetworkManager.h"
#include "Players/Player.h"
#include "Players/PlayerManager.h"

using namespace std;
using namespace Ponykart;
using namespace Ponykart::Core;
using namespace Ponykart::Levels;
using namespace Ponykart::LKernel;
using namespace Ponykart::Networking;
using namespace Ponykart::Players;

PlayerManager::PlayerManager()
{
	log("[Loading] Creating PlayerManager...");
	LevelManager::onLevelLoad.push_back(bind(&PlayerManager::onLevelLoad,this,placeholders::_1));
	LevelManager::onLevelUnload.push_back(bind(&PlayerManager::onLevelUnload, this, placeholders::_1));

	RaceCountdown::onCountdown.push_back(bind(&PlayerManager::raceCountdown_onCountdown, this, placeholders::_1));
}

const Player* const PlayerManager::getMainPlayer() const
{
	return mainPlayer;
}

void PlayerManager::raceCountdown_onCountdown(RaceCountdownState state)
{
	if (state == RaceCountdownState::Go)
		for (auto player : players)
			player->isControlEnabled = true;
}

void PlayerManager::onLevelUnload(LevelChangedEventArgs* eventArgs)
{
	if (eventArgs->oldLevel.getType() == LevelType::Race) 
	{
		for (int a = 0; a < players.size(); a++) 
		{
			if (players[a] != nullptr) 
			{
				players[a]->detach();
				delete players[a];
				players[a] = nullptr;
			}
		}
		mainPlayer = nullptr;
	}
}

void PlayerManager::onLevelLoad(LevelChangedEventArgs* eventArgs)
{
	if (eventArgs->newLevel.getType() == LevelType::Race) 
	{
		if (!eventArgs->request.isMultiplayer) 
		{
			players.clear();
			players.resize(Settings::NumberOfPlayers);

			eventArgs->request.characterNames = fillCharacterString(eventArgs->request.characterNames);
			string controllerOption = Options::get("Controller");
			transform(begin(controllerOption), end(controllerOption), begin(controllerOption), ::tolower);
			if (controllerOption == "keyboard")
				mainPlayer = new HumanPlayer(eventArgs, 0);
			else if (controllerOption == "wiimote")
				mainPlayer = new WiiMotePlayer(eventArgs, 0);
			else
				throw string("Illegal Controller type : " + controllerOption);
			players[0] = mainPlayer;

			for (int a = 1; a < Settings::NumberOfPlayers; a++)
				players[a] = new ComputerPlayer(eventArgs, a);

			if (onPostPlayerCreation.size())
				for (auto& fun : onPostPlayerCreation)
					fun();
		}
		else 
		{
			Networking::NetworkManager* netMgr = LKernel::getG<Networking::NetworkManager>();
			players.clear();
			players.resize(netMgr->players.size());

			//auto indexedPlayers = (from player in netMgr.Players select new{ id = player.GlobalID, p = player }).ToDictionary((tuple) = >tuple.id, (tuple) = >tuple.p);
			//int localid = (from id in IndexedPlayers.Keys where IndexedPlayers[id].local select id).First();
			map<int, NetworkEntity*> indexedPlayers;
			int localid;
			bool localidFound=false; // We want the first found
			for (NetworkEntity* p : netMgr->players)
			{
				indexedPlayers.emplace(pair<int, NetworkEntity*>(p->getGlobalId(), p));
				if (localidFound && p->isLocal)
				{
					localid = p->getGlobalId();
					localidFound = true;
				}
			}
			
			//eventArgs.Request.CharacterNames = FillCharacterString(eventArgs.Request.CharacterNames);
			string controllerOption = Options::get("Controller");
			transform(begin(controllerOption), end(controllerOption), begin(controllerOption), ::tolower);
			if (controllerOption == "keyboard")
				mainPlayer = new HumanPlayer(eventArgs, localid);
			else if (controllerOption == "wiimote")
				mainPlayer = new WiiMotePlayer(eventArgs, localid);
			else
				throw string("Illegal Controller type : " + controllerOption);

			for (int i = 0; i < netMgr->players.size(); i++) 
			{
				if (indexedPlayers[i]->isLocal)
					players[i] = mainPlayer;
				else
					players[i] = new ComputerPlayer(eventArgs, i);
				indexedPlayers[i]->player = players[i];
			}

			if (onPostPlayerCreation.size())
				for (auto& fun : onPostPlayerCreation)
					fun();
		}
	}
}
