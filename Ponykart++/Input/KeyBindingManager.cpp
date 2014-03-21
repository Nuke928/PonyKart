#include "pch.h"
#include "Kernel/LKernel.h"
#include "Kernel/LKernelOgre.h"
#include "Input/KeyBindingManager.h"
#include "Input/InputMain.h"
#include "Core/Options.h"
#include "Players/PlayerManager.h"

using namespace std;
using namespace Ponykart;
using namespace Ponykart::Core;
using namespace Ponykart::Input;
using namespace Ponykart::LKernel;
using namespace Extensions;


KeyBindingManager::KeyBindingManager()
{
	log("[Loading] Creating KeyBindingManager...");

	setDefaultBindings();
	clearKeyboardPlayer();
	clearMousePlayer();

	auto input = LKernel::getG<InputMain>();
	input->onKeyPress.connect(bind(&KeyBindingManager::onKeyPress, this, placeholders::_1));
	input->onKeyRelease.connect(bind(&KeyBindingManager::onKeyRelease, this, placeholders::_1));
	input->onMouseMove.connect(bind(&KeyBindingManager::onMouseMove, this, placeholders::_1));
	input->onMouseWheelMove.connect(bind(&KeyBindingManager::onMouseWheelMove, this, placeholders::_1));
	input->onMouseButtonPress.connect(bind(&KeyBindingManager::onMouseButtonPress, this, placeholders::_1));
	input->onMouseButtonRelease.connect(bind(&KeyBindingManager::onMouseButtonRelease, this, placeholders::_1));
	input->onControllerAxisMove.connect(bind(&KeyBindingManager::onControllerAxisMove, this, placeholders::_1));
	input->onControllerButtonPress.connect(bind(&KeyBindingManager::onControllerButtonPress, this, placeholders::_1));
	input->onControllerButtonRelease.connect(bind(&KeyBindingManager::onControllerButtonRelease, this, placeholders::_1));

	try {
		mouseSens = std::strtof(Options::get("Mouse Sensitivity").c_str(), nullptr);
	} catch (...) {
		log("WARNING: Invalid mouse sensitivity value in ponykart.cfg!");
		mouseSens = 1.0f;
	}

	inputSuppressedSem = 0;

//	if (Options::getBool("Twh"))
//	{
//		input->onMousePress_Anything.push_back(onMousePress_Anything);
//		input->onMouseRelease_Anything.push_back(onMouseRelease_Anything);
//	}
}


void KeyBindingManager::setKeyboardPlayer (int playerID)
{
	keyboardPlayerID = playerID;
}
void KeyBindingManager::clearKeyboardPlayer ()
{
	keyboardPlayerID = -1;
}
void KeyBindingManager::setMousePlayer (int playerID)
{
	mousePlayerID = playerID;
}
void KeyBindingManager::clearMousePlayer ()
{
	mousePlayerID = -1;
}

void KeyBindingManager::setControllerPlayer (SDL_JoystickID controller, int playerID)
{
	clearControllerPlayer(controller);
	playersMapByControllerID.insert(make_pair(controller, playerID));
	controllerIDsMapByPlayer.insert(make_pair(playerID, controller));
}
void KeyBindingManager::clearControllerPlayer (SDL_JoystickID controller)
{
	auto entry = playersMapByControllerID.find(controller);
	if (entry != playersMapByControllerID.end()) {
		controllerIDsMapByPlayer.erase(entry->second);
		playersMapByControllerID.erase(entry);
	}
}
void KeyBindingManager::clearPlayerController (int playerID)
{
	auto entry = controllerIDsMapByPlayer.find(playerID);
	if (entry != controllerIDsMapByPlayer.end()) {
		playersMapByControllerID.erase(entry->second);
		controllerIDsMapByPlayer.erase(entry);
	}
}
void KeyBindingManager::clearControllerPlayers ()
{
	playersMapByControllerID.clear();
	controllerIDsMapByPlayer.clear();
}


void KeyBindingManager::setKeyBinding (SDLInputID realInputID, GameInputID gameInputID)
{
	clearKeyBinding(realInputID);
	gameInputsMapByReal.insert(make_pair(realInputID, gameInputID));
	realInputsMapByGame.insert(make_pair(gameInputID, realInputID));
}
void KeyBindingManager::clearKeyBinding (SDLInputID realInput)
{
	auto entry = gameInputsMapByReal.find(realInput);
	if (entry != gameInputsMapByReal.end()) {
		auto range = realInputsMapByGame.equal_range(entry->second);
		for (auto it = range.first; it != range.second; it++) {
			if (it->second == realInput) {
				realInputsMapByGame.erase(it);
				break;
			}
		}
		gameInputsMapByReal.erase(entry);
	}
}
void KeyBindingManager::clearKeyBinding (GameInputID gameInputID)
{
	auto range = realInputsMapByGame.equal_range(gameInputID);
	if (range.first != realInputsMapByGame.end()) {
		for (auto it = range.first; it != range.second; )
			gameInputsMapByReal.erase(it->second);
		realInputsMapByGame.erase(range.first, range.second);
	}
}
void KeyBindingManager::clearKeyBindings ()
{
	gameInputsMapByReal.clear();
	realInputsMapByGame.clear();
}


void KeyBindingManager::suppressInput ()
{
	if (inputSuppressedSem == 0) {
		// Zero-out all control state while input is suppressed
		axisMoveEvent(0, GameInputID::AccelerateAxis, 0.0f);
		axisMoveEvent(0, GameInputID::BrakeAxis, 0.0f);
		axisMoveEvent(0, GameInputID::SteeringAxis, 0.0f);
		releaseEvent(0, GameInputID::Accelerate);
		releaseEvent(0, GameInputID::Reverse);
		releaseEvent(0, GameInputID::TurnLeft);
		releaseEvent(0, GameInputID::TurnRight);
		releaseEvent(0, GameInputID::Drift);
		releaseEvent(0, GameInputID::Item);

		for (auto entry : controllerIDsMapByPlayer) {
			if (entry.first == 0)
				continue;
			axisMoveEvent(entry.first, GameInputID::AccelerateAxis, 0.0f);
			axisMoveEvent(entry.first, GameInputID::BrakeAxis, 0.0f);
			axisMoveEvent(entry.first, GameInputID::SteeringAxis, 0.0f);
			releaseEvent(entry.first, GameInputID::Accelerate);
			releaseEvent(entry.first, GameInputID::Reverse);
			releaseEvent(entry.first, GameInputID::TurnLeft);
			releaseEvent(entry.first, GameInputID::TurnRight);
			releaseEvent(entry.first, GameInputID::Drift);
			releaseEvent(entry.first, GameInputID::Item);
		}
	}
	inputSuppressedSem++;
}
void KeyBindingManager::allowInput ()
{
	inputSuppressedSem--;
	if (inputSuppressedSem < 0)
		inputSuppressedSem = 0;
}


void KeyBindingManager::setDefaultBindings ()
{
	clearKeyBindings();
	setKeyBinding(SDL_SCANCODE_W, GameInputID::Accelerate);
	setKeyBinding(SDL_SCANCODE_S, GameInputID::Reverse);
	setKeyBinding(SDL_SCANCODE_A, GameInputID::TurnLeft);
	setKeyBinding(SDL_SCANCODE_D, GameInputID::TurnRight);
	setKeyBinding(SDL_SCANCODE_SPACE, GameInputID::Drift);
	setKeyBinding(SDL_SCANCODE_LSHIFT, GameInputID::Item);
}


void KeyBindingManager::onKeyPress (const SDL_KeyboardEvent &ke)
{
	if (inputSuppressedSem > 0 || keyboardPlayerID < 0)
		return;

	auto binding = gameInputsMapByReal.find(SDLInputID(ke));
	if (binding != gameInputsMapByReal.end())
		pressEvent(keyboardPlayerID, binding->second);
}
void KeyBindingManager::onKeyRelease (const SDL_KeyboardEvent &ke)
{
	if (inputSuppressedSem > 0 || keyboardPlayerID < 0)
		return;

	auto binding = gameInputsMapByReal.find(SDLInputID(ke));
	if (binding != gameInputsMapByReal.end())
		releaseEvent(keyboardPlayerID, binding->second);
}


void KeyBindingManager::onMouseMove (const SDL_MouseMotionEvent &mme)
{
	if (inputSuppressedSem > 0 || mousePlayerID < 0)
		return;

	if (mme.xrel != 0) {
		auto binding = gameInputsMapByReal.find(SDLInputID::ofMouseAxis(0));
		if (binding != gameInputsMapByReal.end()) {
			axisMoveEvent(mousePlayerID, binding->second, static_cast<float>(mme.xrel) / 10 * mouseSens);
			// Reset value to zero since mice don't auto-center like an analog stick
			axisMoveEvent(mousePlayerID, binding->second, 0.0f);
		}
	}
	if (mme.yrel != 0) {
		auto binding = gameInputsMapByReal.find(SDLInputID::ofMouseAxis(1));
		if (binding != gameInputsMapByReal.end()) {
			axisMoveEvent(mousePlayerID, binding->second, static_cast<float>(mme.yrel) / 10 *  mouseSens);
			// Reset value to zero since mice don't auto-center like an analog stick
			axisMoveEvent(mousePlayerID, binding->second, 0.0f);
		}
	}
}


void KeyBindingManager::onMouseWheelMove (const SDL_MouseWheelEvent &mwe)
{
	if (inputSuppressedSem > 0 || mousePlayerID < 0)
		return;

	if (mwe.x != 0) {
		auto binding = gameInputsMapByReal.find(SDLInputID::ofMouseWheelAxis(0));
		if (binding != gameInputsMapByReal.end()) {
			axisMoveEvent(mousePlayerID, binding->second, static_cast<float>(mwe.x));
			// Reset value to zero since mice don't auto-center like an analog stick
			axisMoveEvent(mousePlayerID, binding->second, 0.0f);
		}
	}
	if (mwe.y != 0) {
		auto binding = gameInputsMapByReal.find(SDLInputID::ofMouseWheelAxis(1));
		if (binding != gameInputsMapByReal.end()) {
			axisMoveEvent(mousePlayerID, binding->second, static_cast<float>(mwe.y));
			// Reset value to zero since mice don't auto-center like an analog stick
			axisMoveEvent(mousePlayerID, binding->second, 0.0f);
		}
	}
}


void KeyBindingManager::onMouseButtonPress (const SDL_MouseButtonEvent &mbe)
{
	if (inputSuppressedSem > 0 || mousePlayerID < 0)
		return;

	auto binding = gameInputsMapByReal.find(SDLInputID(mbe));
	if (binding != gameInputsMapByReal.end())
		pressEvent(mousePlayerID, binding->second);
}
void KeyBindingManager::onMouseButtonRelease (const SDL_MouseButtonEvent &mbe)
{
	if (inputSuppressedSem > 0 || mousePlayerID < 0)
		return;

	auto binding = gameInputsMapByReal.find(SDLInputID(mbe));
	if (binding != gameInputsMapByReal.end())
		releaseEvent(mousePlayerID, binding->second);
}


void KeyBindingManager::onControllerAxisMove (const SDL_ControllerAxisEvent &cae)
{
	if (inputSuppressedSem > 0)
		return;
	auto entry = playersMapByControllerID.find(cae.which);
	if (entry == playersMapByControllerID.end())
		return;

	auto binding = gameInputsMapByReal.find(SDLInputID(cae));
	if (binding != gameInputsMapByReal.end()) {
		if (cae.axis == SDL_CONTROLLER_AXIS_TRIGGERLEFT || cae.axis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT)
			axisMoveEvent(entry->second, binding->second, static_cast<float>(cae.value) / 65536.f + 0.5f);
		else
			axisMoveEvent(entry->second, binding->second, static_cast<float>(cae.value) / 32768.f);
	}
}


void KeyBindingManager::onControllerButtonPress (const SDL_ControllerButtonEvent &cbe)
{
	if (inputSuppressedSem > 0)
		return;
	auto entry = playersMapByControllerID.find(cbe.which);
	if (entry == playersMapByControllerID.end())
		return;

	auto binding = gameInputsMapByReal.find(SDLInputID(cbe));
	if (binding != gameInputsMapByReal.end())
		pressEvent(entry->second, binding->second);
}
void KeyBindingManager::onControllerButtonRelease (const SDL_ControllerButtonEvent &cbe)
{
	if (inputSuppressedSem > 0)
		return;
	auto entry = playersMapByControllerID.find(cbe.which);
	if (entry == playersMapByControllerID.end())
		return;

	auto binding = gameInputsMapByReal.find(SDLInputID(cbe));
	if (binding != gameInputsMapByReal.end())
		releaseEvent(entry->second, binding->second);
}


bool KeyBindingManager::pollKey (int playerID, GameInputID gameInputID)
{
	auto range = realInputsMapByGame.equal_range(gameInputID);
	for (auto it = range.first; it != range.second; it++) {
		auto realInputID = it->second;
		if (realInputID.inputType == SDL_KEYDOWN || realInputID.inputType == SDL_MOUSEBUTTONDOWN) {
			if (getG<InputMain>()->pollButton(realInputID))
				return true;
		} else {
			if (getG<InputMain>()->pollButton(realInputID, controllerIDsMapByPlayer[playerID]))
				return true;
		}
	}

	return false;
}
float KeyBindingManager::pollAxis (int playerID, GameInputID gameInputID)
{
	auto range = realInputsMapByGame.equal_range(gameInputID);
	for (auto it = range.first; it != range.second; it++) {
		auto realInputID = it->second;
		int value;
		switch (realInputID.inputType) {
		case SDL_MOUSEMOTION:
			value = getG<InputMain>()->pollAxis(realInputID);
			if (value != 0)
				return (float)value / 10 * mouseSens;
		case SDL_MOUSEWHEEL:
			value = getG<InputMain>()->pollAxis(realInputID);
			if (value != 0)
				return (float)value;
		default:
			value = getG<InputMain>()->pollAxis(realInputID, controllerIDsMapByPlayer[playerID]);
			if (value != 0) {
				if (realInputID.input.caxis == SDL_CONTROLLER_AXIS_TRIGGERLEFT || realInputID.input.caxis == SDL_CONTROLLER_AXIS_TRIGGERRIGHT)
					return (float)value / 65536.f + 0.5f;
				else
					return (float)value / 32768.f;
			}
		}
	}

	return 0.f;
}
