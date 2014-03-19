#include "pch.h"
#include "Input/InputMain.h"
#include <string>
#include <sstream>
#include <OgreRenderWindow.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include "Kernel/LKernel.h"
#include "Kernel/LKernelOgre.h"
#include "Levels/LevelManager.h"

using namespace std;
using namespace Ponykart;
using namespace Ponykart::Input;
using namespace Ponykart::Levels;
using namespace Ponykart::LKernel;


InputMain::InputMain()
{
	log("[Loading] Initialising SDL2 input system");

	if (SDL_InitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) < 0)
		throw std::string(SDL_GetError());

	log("[Loading] SDL2 input system loaded.");
}


void InputMain::processEvent (const SDL_Event &event)
{
	switch (event.type) {
	case SDL_KEYDOWN:
		onKeyPress(event.key);
		break;
	case SDL_KEYUP:
		onKeyRelease(event.key);
		break;
	case SDL_TEXTINPUT:
		onTextInput(event.text);
		break;
	case SDL_TEXTEDITING:
		onTextEdit(event.edit);
		break;

	case SDL_MOUSEMOTION:
		onMouseMove(event.motion);
		break;
	case SDL_MOUSEWHEEL:
		onMouseWheelMove(event.wheel);
		break;
	case SDL_MOUSEBUTTONDOWN:
		onMouseButtonPress(event.button);
		break;
	case SDL_MOUSEBUTTONUP:
		onMouseButtonRelease(event.button);
		break;

	case SDL_JOYDEVICEADDED:
		onJoystickConnect(event.jdevice);
		break;
	case SDL_JOYDEVICEREMOVED:
		onJoystickRemove(event.jdevice);
		break;
	case SDL_JOYAXISMOTION:
		onJoystickAxisMove(event.jaxis);
		break;
	case SDL_JOYBUTTONDOWN:
		onJoystickButtonPress(event.jbutton);
		break;
	case SDL_JOYBUTTONUP:
		onJoystickButtonRelease(event.jbutton);
		break;

	case SDL_CONTROLLERDEVICEADDED:
	case SDL_CONTROLLERDEVICEREMAPPED:
		onControllerConnect(event.cdevice);
		break;
	case SDL_CONTROLLERDEVICEREMOVED:
		onControllerRemove(event.cdevice);
		break;
	case SDL_CONTROLLERAXISMOTION:
		onControllerAxisMove(event.caxis);
		break;
	case SDL_CONTROLLERBUTTONDOWN:
		onControllerButtonPress(event.cbutton);
		break;
	case SDL_CONTROLLERBUTTONUP:
		onControllerButtonRelease(event.cbutton);
		break;
	}
}


bool receiveTextInput (bool enabled)
{
	if (enabled)
		SDL_StartTextInput();
	else
		SDL_StopTextInput();
}
