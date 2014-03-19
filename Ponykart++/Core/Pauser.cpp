#include "pch.h"
#include "Core/Pauser.h"
#include "Input/InputMain.h"
#include "Kernel/LKernel.h"
#include "Kernel/LKernelOgre.h"
#include "Levels/LevelManager.h"
#include "Input/KeyBindingManager.h"

using namespace Ponykart::Core;
using namespace Ponykart::Input;
using namespace Ponykart::LKernel;
using namespace Ponykart::Levels;
namespace bs2 = boost::signals2;

// Define static members
bool Pauser::isPaused = false;
bs2::signal<void (PausingState)> Pauser::pauseEvent;

Pauser::Pauser()
{
	log("[Loading] Creating Pauser");

	// if we press `, then pause
	LKernel::getG<InputMain>()->onKeyPress.connect(keyInvokePauseEvent);
}

void Pauser::keyInvokePauseEvent(const SDL_KeyboardEvent &ke)
{
	if (ke.keysym.sym == SDLK_BACKQUOTE)
		invokePauseEvent();
}

void Pauser::invokePauseEvent()
{
	if (LKernel::getG<LevelManager>()->isPlayableLevel())
		pauseWithEvent();
}

void Pauser::pauseWithEvent()
{
	log("Pause!");
	isPaused = !isPaused;

	if (isPaused) {
		LKernel::getG<KeyBindingManager>()->suppressInput();
		pauseEvent(PausingState::Pausing);
	} else {
		LKernel::getG<KeyBindingManager>()->allowInput();
		pauseEvent(PausingState::Unpausing);
	}
}
