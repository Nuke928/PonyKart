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
using namespace Extensions;


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
		fireEvent<const SDL_KeyboardEvent &>(onKeyPress, event.key);
		break;
	case SDL_KEYUP:
		fireEvent<const SDL_KeyboardEvent &>(onKeyRelease, event.key);
		break;
	case SDL_TEXTINPUT:
		fireEvent<const SDL_TextInputEvent &>(onTextInput, event.text);
		break;
	case SDL_TEXTEDITING:
		fireEvent<const SDL_TextEditingEvent &>(onTextEdit, event.edit);
		break;

	case SDL_MOUSEMOTION:
		fireEvent<const SDL_MouseMotionEvent &>(onMouseMove, event.motion);
		break;
	case SDL_MOUSEWHEEL:
		fireEvent<const SDL_MouseWheelEvent &>(onMouseWheelMove, event.wheel);
		break;
	case SDL_MOUSEBUTTONDOWN:
		fireEvent<const SDL_MouseButtonEvent &>(onMouseButtonPress, event.button);
		break;
	case SDL_MOUSEBUTTONUP:
		fireEvent<const SDL_MouseButtonEvent &>(onMouseButtonRelease, event.button);
		break;

	case SDL_JOYDEVICEADDED:
		fireEvent<const SDL_JoyDeviceEvent &>(onJoystickConnect, event.jdevice);
		break;
	case SDL_JOYDEVICEREMOVED:
		fireEvent<const SDL_JoyDeviceEvent &>(onJoystickRemove, event.jdevice);
		break;
	case SDL_JOYAXISMOTION:
		fireEvent<const SDL_JoyAxisEvent &>(onJoystickAxisMove, event.jaxis);
		break;
	case SDL_JOYBUTTONDOWN:
		fireEvent<const SDL_JoyButtonEvent &>(onJoystickButtonPress, event.jbutton);
		break;
	case SDL_JOYBUTTONUP:
		fireEvent<const SDL_JoyButtonEvent &>(onJoystickButtonRelease, event.jbutton);
		break;

	case SDL_CONTROLLERDEVICEADDED:
	case SDL_CONTROLLERDEVICEREMAPPED:
		fireEvent<const SDL_ControllerDeviceEvent &>(onControllerConnect, event.cdevice);
		break;
	case SDL_CONTROLLERDEVICEREMOVED:
		fireEvent<const SDL_ControllerDeviceEvent &>(onControllerRemove, event.cdevice);
		break;
	case SDL_CONTROLLERAXISMOTION:
		fireEvent<const SDL_ControllerAxisEvent &>(onControllerAxisMove, event.caxis);
		break;
	case SDL_CONTROLLERBUTTONDOWN:
		fireEvent<const SDL_ControllerButtonEvent &>(onControllerButtonPress, event.cbutton);
		break;
	case SDL_CONTROLLERBUTTONUP:
		fireEvent<const SDL_ControllerButtonEvent &>(onControllerButtonRelease, event.cbutton);
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
