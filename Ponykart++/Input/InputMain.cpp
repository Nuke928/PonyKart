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

	int nJoysticks = SDL_NumJoysticks();
	for (int i = 0; i < nJoysticks; i++)
		if (SDL_IsGameController(i))
			openController(i);

	log("[Loading] SDL2 input system loaded.");
}


void InputMain::openController (int deviceIndex)
{
	auto controller = SDL_GameControllerOpen(deviceIndex);
	controllersMapByJoystickID.insert(make_pair(SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller)) , controller));
	log(string("[Input] Opened controller: ") + SDL_GameControllerName(controller));
}
void InputMain::closeController (int controllerID)
{
	auto entry = controllersMapByJoystickID.find(controllerID);
	if (entry != controllersMapByJoystickID.end()) {
		log(string("[Input] Closed controller: ") + SDL_GameControllerName(entry->second));
		SDL_GameControllerClose(entry->second);
		controllersMapByJoystickID.erase(entry);
	}
}


SDL_JoystickID InputMain::getControllerID (SDL_GameController *controller)
{
	return SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(controller));
}
SDL_GameController *InputMain::getController (SDL_JoystickID joystickID)
{
	return controllersMapByJoystickID.find(joystickID)->second;
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
		openController(event.cdevice.which);
		// fallthrough
	case SDL_CONTROLLERDEVICEREMAPPED:
		onControllerConnect(event.cdevice);
		break;
	case SDL_CONTROLLERDEVICEREMOVED:
		closeController(event.cdevice.which);
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


void InputMain::receiveTextInput (bool enabled)
{
	if (enabled)
		SDL_StartTextInput();
	else
		SDL_StopTextInput();
}


bool InputMain::pollButton (Extensions::SDLInputID inputID, SDL_GameController *controller)
{
	switch (inputID.inputType) {
	case SDL_KEYDOWN:
		{
			int nKeys;
			auto keys = SDL_GetKeyboardState(&nKeys);
			return keys[inputID.input.scancode];
		}
	case SDL_MOUSEBUTTONDOWN:
		{
			int x, y;
			auto buttonState = SDL_GetMouseState(&x, &y);
			return SDL_BUTTON(inputID.input.mbutton);
		}
	case SDL_CONTROLLERBUTTONDOWN:
		return SDL_GameControllerGetButton(controller, inputID.input.cbutton);
	default:
		return false;
	}
}
bool InputMain::pollButton (Extensions::SDLInputID inputID, SDL_JoystickID controllerID)
{
	return pollButton(inputID, getController(controllerID));
}
int InputMain::pollAxis (Extensions::SDLInputID inputID, SDL_GameController *controller)
{
	switch (inputID.inputType) {
	case SDL_MOUSEMOTION:
		{
			int x, y;
			auto buttonState = SDL_GetMouseState(&x, &y);
			return inputID.input.maxis ? y : x;
		}
	case SDL_MOUSEWHEEL:
		return 0;
	case SDL_CONTROLLERAXISMOTION:
		return SDL_GameControllerGetAxis(controller, inputID.input.caxis);
	default:
		return 0;
	}
}
int InputMain::pollAxis (Extensions::SDLInputID inputID, SDL_JoystickID controllerID)
{
	return pollAxis(inputID, getController(controllerID));
}

