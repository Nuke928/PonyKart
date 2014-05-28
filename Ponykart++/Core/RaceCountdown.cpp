#include "pch.h"
#include "Core/Pauser.h"
#include "Core/RaceCountdown.h"
#include "Kernel/LKernel.h"
#include "Levels/LevelChangedEventArgs.h"
#include "Levels/LevelManager.h"
#include "Players/PlayerManager.h"
#include "Players/Player.h"

using namespace Ponykart::Core;
using namespace Ponykart::Levels;
using namespace Ponykart::LKernel;
using namespace Ponykart::Players;
using namespace std;
using namespace Ogre;

RaceCountdownEvent RaceCountdown::onCountdown;

RaceCountdown::RaceCountdown()
{
	LevelManager::onLevelPostLoad.push_back(bind(&RaceCountdown::onLevelPostLoad, this, placeholders::_1));
	LevelManager::onLevelUnload.push_back(bind(&RaceCountdown::detach, this));
}

void RaceCountdown::onLevelPostLoad(LevelChangedEventArgs* eventArgs)
{
	// only run this on race levels!
	if (eventArgs->newLevel.getType() == LevelType::Race && (!eventArgs->request.isMultiplayer))
		//|| get<NetworkManager>()->allConnectionsReady)) 
	{
		start();
	}
}

void RaceCountdown::start()
{
	three = two = one = go = oneSecondAfterGo = false;
	elapsed = 0;

	const std::vector<Player*> players = getG<PlayerManager>()->getPlayers();
	for (auto player : players) 
	{
		// first make sure all of the karts can't be controlled
#if ENABLE_COUNTDOWN
		player->isControlEnabled = false;
#else
		player->isControlEnabled = true;
#endif
	}

	getG<Root>()->addFrameListener(this);
}

void RaceCountdown::detach()
{
	getG<Root>()->removeFrameListener(this);
}

bool RaceCountdown::frameStarted(const Ogre::FrameEvent& evt)
{
	if (!Pauser::isPaused)
	{
		if (!three && elapsed >= INITIAL_DELAY) 
		{
			invoke(RaceCountdownState::Three);
			three = true;
			elapsed = INITIAL_DELAY;
		}
		else if (!two && elapsed >= INITIAL_DELAY + 1) 
		{
			invoke(RaceCountdownState::Two);
			two = true;
			elapsed = INITIAL_DELAY + 1;
		}
		else if (!one && elapsed >= INITIAL_DELAY + 2) 
		{
			invoke(RaceCountdownState::One);
			one = true;
			elapsed = INITIAL_DELAY + 2;
		}
		else if (!go && elapsed >= INITIAL_DELAY + 3) 
		{
#if ENABLE_COUNTDOWN
			for (auto player : get<PlayerManager>()->getPlayers()) 
			{
				// first make sure all of the karts can't be controlled
				player->isControlEnabled = true;
			}
			getG<ItemManager>()->spawning = true;
#endif

			invoke(RaceCountdownState::Go);
			go = true;
			elapsed = INITIAL_DELAY + 3;
		}
		else if (!oneSecondAfterGo && elapsed >= INITIAL_DELAY + 4) 
		{
			invoke(RaceCountdownState::OneSecondAfterGo);
			oneSecondAfterGo = true;

			// don't need to keep checking the time any more
			detach();
		}

		elapsed += evt.timeSinceLastFrame;
	}
	return true;
}

void RaceCountdown::invoke(RaceCountdownState state)
{
	for (unsigned i = 0; i < onCountdown.size(); i++) // DO NOT use iterators, the array can lose elements during the loop
		onCountdown[i].second(state);
}
