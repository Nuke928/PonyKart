#ifndef BACKGROUNDMUSICHANDLER_H_INCLUDED
#define BACKGROUNDMUSICHANDLER_H_INCLUDED

#include "Core/RaceCountdown.h"
#include "Kernel/LKernelObject.h"
#include "Sound/SoundMain.h"

namespace Ponykart
{
namespace Levels{ class LevelChangedEventArgs; }
namespace Handlers
{
	class BackgroundMusicHandler : public LKernel::LKernelObject
	{
	public:
		BackgroundMusicHandler();
	
	private:
		void onLevelLoad(Levels::LevelChangedEventArgs* eventArgs); ///< Runs whenever a new level is loaded. We create the background music now so it loads, then pause it until we need to play it.
		void onLevelUnload(Levels::LevelChangedEventArgs* eventArgs); ///< Delete all of the sound sources
		void onCountdown(Core::RaceCountdownState state);

	private:
		unsigned eventId;
		Sound::SoundMain::MusicSourcePtr bgMusic;
	};
}
}

#endif // BACKGROUNDMUSICHANDLER_H_INCLUDED