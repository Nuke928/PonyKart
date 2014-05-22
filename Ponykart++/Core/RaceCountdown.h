#ifndef RACECOUNTDOWN_H_INCLUDED
#define RACECOUNTDOWN_H_INCLUDED

#include <functional>
#include <vector>
#include <OgreFrameListener.h>
#include "Kernel/LKernelObject.h"

namespace Ponykart
{
namespace Levels{ class LevelChangedEventArgs; }
namespace Core
{
	enum class RaceCountdownState
	{
		Three = 3,
		Two = 2,
		One = 1,
		Go = 0,
		OneSecondAfterGo = -1
	};

	using RaceCountdownEvent = std::vector<std::pair<int,std::function<void(RaceCountdownState)>>>;

	class RaceCountdown : Ogre::FrameListener, public LKernel::LKernelObject
	{
		// TODO: Implement RaceCountdown with all its members
	public:
		RaceCountdown(); ///< Hook up to the level un/load events
		void start();
		void detach();
	private:
		void onLevelPostLoad(Levels::LevelChangedEventArgs* eventArgs); ///< Reset the elapsed time, reset the bools, disable control of all of the players, and hook up to the frame started event.
		bool frameStarted(const Ogre::FrameEvent& evt) override; ///< Countdown
		void invoke(RaceCountdownState state); ///< Helper method

	public:
		static RaceCountdownEvent onCountdown;
	private:
		bool three, two, one, go, oneSecondAfterGo;
		float elapsed;
		const static int INITIAL_DELAY = 1;
	};
}
}
#endif // RACECOUNTDOWN_H_INCLUDED