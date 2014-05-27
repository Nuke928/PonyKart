#include "pch.h"
#include "Core/IDs.h"
#include "Handlers/BackgroundMusicHandler.h"
#include "Kernel/LKernel.h"
#include "Levels/LevelChangedEventArgs.h"
#include "Levels/LevelManager.h"
#include "Muffin/MuffinDefinition.h"

using namespace Ponykart::Core;
using namespace Ponykart::Handlers;
using namespace Ponykart::Levels;
using namespace Ponykart::LKernel;
using namespace Ponykart::Sound;
using namespace PonykartParsers;
using namespace std;

BackgroundMusicHandler::BackgroundMusicHandler()
{
	eventId = IDs::incremental();
	LevelManager::onLevelLoad.push_back(bind(&BackgroundMusicHandler::onLevelLoad, this, placeholders::_1));
	LevelManager::onLevelUnload.push_back(bind(&BackgroundMusicHandler::onLevelUnload, this, placeholders::_1));
	RaceCountdown::onCountdown.push_back({ eventId, bind(&BackgroundMusicHandler::onCountdown, this, placeholders::_1) });
}


void BackgroundMusicHandler::onLevelLoad(LevelChangedEventArgs* eventArgs)
{
	// only want to load this on nonempty levels
	if (eventArgs->newLevel.getType() != LevelType::EmptyLevel) 
	{
		// get the property from the .muffin file, if it has one
		string musicFile = eventArgs->newLevel.getDefinition()->getStringProperty("Music", "");
		if (musicFile != "") 
		{
			// if it's a race level, don't play the music until we need it
			bgMusic = getG<SoundMain>()->playMusic(musicFile, eventArgs->newLevel.getType() == LevelType::Race);
		}
	}
}

void BackgroundMusicHandler::onLevelUnload(LevelChangedEventArgs* eventArgs)
{
	bgMusic.release();
}

void BackgroundMusicHandler::onCountdown(RaceCountdownState state)
{
	if (state == RaceCountdownState::Go && bgMusic)// && getG<SoundMain>()->isMusicEnabled)
		bgMusic->play();
}
