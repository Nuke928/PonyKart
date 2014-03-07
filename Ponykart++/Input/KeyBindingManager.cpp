#include "pch.h"
#include "Kernel/LKernel.h"
#include "Kernel/LKernelOgre.h"
#include "Input/KeyBindingManager.h"
#include "Input/InputMain.h"
#include "Core/Options.h"
#include "Input/InputSwallowerManager.h"

using namespace std;
using namespace Ponykart;
using namespace Ponykart::Input;
using namespace Ponykart::LKernel;
using namespace Extensions;


// SDLInputID

SDLInputID::SDLInputID (const SDLInputID &other)
	: inputType(other.inputType), input(other.input)
{
}
SDLInputID::SDLInputID (SDL_Scancode scancode)
{
	*this = scancode;
}
SDLInputID SDLInputID::ofMouseAxis (Uint8 axis)
{
	auto id = SDLInputID();
	id.inputType = SDL_MOUSEMOTION;
	id.input.maxis = axis;
}
SDLInputID SDLInputID::ofMouseWheelAxis (Uint8 axis)
{
	auto id = SDLInputID();
	id.inputType = SDL_MOUSEWHEEL;
	id.input.maxis = axis;
}
SDLInputID::SDLInputID (SDL_GameControllerAxis axis)
{
	*this = axis;
}
SDLInputID::SDLInputID (SDL_GameControllerButton button)
{
	*this = button;
}
SDLInputID::SDLInputID (const SDL_KeyboardEvent &ke)
	: inputType(SDL_KEYDOWN)
{
	input.scancode = ke.keysym.scancode;
}
SDLInputID::SDLInputID (const SDL_MouseButtonEvent &mbe)
	: inputType(SDL_MOUSEBUTTONDOWN)
{
	input.mbutton = mbe.button;
}
SDLInputID::SDLInputID (const SDL_ControllerAxisEvent &cae)
	: inputType(SDL_CONTROLLERAXISMOTION)
{
	input.caxis = (SDL_GameControllerAxis)cae.axis;
}
SDLInputID::SDLInputID (const SDL_ControllerButtonEvent &cbe)
	: inputType(SDL_CONTROLLERBUTTONDOWN)
{
	input.cbutton = (SDL_GameControllerButton)cbe.button;
}

SDLInputID &SDLInputID::operator= (SDL_Scancode scancode)
{
	this->inputType = SDL_KEYDOWN;
	this->input.scancode = scancode;
	return *this;
}
SDLInputID &SDLInputID::operator= (SDL_GameControllerAxis axis)
{
	this->inputType = SDL_CONTROLLERAXISMOTION;
	this->input.caxis = axis;
	return *this;
}
SDLInputID &SDLInputID::operator= (SDL_GameControllerButton button)
{
	this->inputType = SDL_CONTROLLERBUTTONDOWN;
	this->input.cbutton = button;
	return *this;
}


bool SDLInputID::operator== (SDLInputID other) const
{
	if (other.inputType != inputType)
		return false;
	switch (inputType) {
	case SDL_KEYDOWN:
		return other.input.scancode == input.scancode;
	case SDL_MOUSEMOTION:
		return other.input.maxis == input.maxis;
	case SDL_MOUSEWHEEL:
		return true;
	case SDL_MOUSEBUTTONDOWN:
		return other.input.mbutton == input.mbutton;
	case SDL_CONTROLLERAXISMOTION:
		return other.input.caxis == input.caxis;
	case SDL_CONTROLLERBUTTONDOWN:
		return other.input.cbutton == input.cbutton;
	default:
		return false;
	}
}


size_t SDLInputID::Hash::operator() (SDLInputID value) const
{
	return std::hash<typename std::underlying_type<SDL_EventType>::type>()(value.inputType)
		+ std::hash<int>()(value.input.dummy);
}


// KeyBindingManager

KeyBindingManager::KeyBindingManager()
{
	log("[Loading] Creating KeyBindingManager...");

	setDefaultBindings();

	auto input = LKernel::getG<InputMain>();
	input->onKeyPress.push_back(bind(&KeyBindingManager::onKeyboardPress, this, placeholders::_1));
	input->onKeyRelease.push_back(bind(&KeyBindingManager::onKeyboardRelease, this, placeholders::_1));

//	if (Options::getBool("Twh"))
//	{
//		input->onMousePress_Anything.push_back(onMousePress_Anything);
//		input->onMouseRelease_Anything.push_back(onMouseRelease_Anything);
//	}
}


void KeyBindingManager::setControllerPlayer (SDL_JoystickID controllerID, int playerID)
{
	clearControllerPlayer(controllerID);
	playersMapByController.insert(make_pair(controllerID, playerID));
	controllersMapByPlayer.insert(make_pair(playerID, controllerID));
}
void KeyBindingManager::clearControllerPlayer (SDL_JoystickID controllerID)
{
	auto entry = playersMapByController.find(controllerID);
	if (entry != playersMapByController.end()) {
		controllersMapByPlayer.erase(entry->second);
		playersMapByController.erase(entry);
	}
}
void KeyBindingManager::clearPlayerController (int playerID)
{
	auto entry = controllersMapByPlayer.find(playerID);
	if (entry != controllersMapByPlayer.end()) {
		playersMapByController.erase(entry->second);
		controllersMapByPlayer.erase(entry);
	}
}
void KeyBindingManager::clearControllerPlayers ()
{
	playersMapByController.clear();
	controllersMapByPlayer.clear();
}


void KeyBindingManager::setKeyBinding (SDLInputID realInput, LInputID gameInput)
{
	clearKeyBinding(realInput);
	gameInputsMapByReal.insert(make_pair(realInput, gameInput));
	realInputsMapByGame.insert(make_pair(gameInput, realInput));
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
void KeyBindingManager::clearKeyBinding (LInputID gameInput)
{
	auto range = realInputsMapByGame.equal_range(gameInput);
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


void KeyBindingManager::setDefaultBindings ()
{
	clearKeyBindings();
	setKeyBinding(SDL_SCANCODE_W, LInputID::Accelerate);
	setKeyBinding(SDL_SCANCODE_S, LInputID::Reverse);
	setKeyBinding(SDL_SCANCODE_A, LInputID::TurnLeft);
	setKeyBinding(SDL_SCANCODE_D, LInputID::TurnRight);
	setKeyBinding(SDL_SCANCODE_SPACE, LInputID::Drift);
	setKeyBinding(SDL_SCANCODE_LSHIFT, LInputID::Item);
}


void KeyBindingManager::onKeyboardPress (const SDL_KeyboardEvent &ke)
{
	// don't do anything if it's swallowed
	if (LKernel::getG<InputSwallowerManager>()->isSwallowed())
		return;

	auto binding = gameInputsMapByReal.find(SDLInputID(ke));
	if (binding != gameInputsMapByReal.end())
		fireEvent(pressEvent, 0, binding->second);
}
void KeyBindingManager::onKeyboardRelease (const SDL_KeyboardEvent &ke)
{
	auto binding = gameInputsMapByReal.find(SDLInputID(ke));
	if (binding != gameInputsMapByReal.end())
		fireEvent(releaseEvent, 0, binding->second);
}


void KeyBindingManager::onMouseMove (const SDL_MouseMotionEvent &mme)
{
	// don't do anything if it's swallowed
	if (LKernel::getG<InputSwallowerManager>()->isSwallowed())
		return;

	// TODO: Mouse sensetivity
	if (mme.xrel != 0) {
		auto binding = gameInputsMapByReal.find(SDLInputID::ofMouseAxis(0));
		if (binding != gameInputsMapByReal.end())
			fireEvent(axisMoveEvent, 0, binding->second, static_cast<float>(mme.xrel) / 32768.0f);
	}
	if (mme.yrel != 0) {
		auto binding = gameInputsMapByReal.find(SDLInputID::ofMouseAxis(1));
		if (binding != gameInputsMapByReal.end())
			fireEvent(axisMoveEvent, 0, binding->second, static_cast<float>(mme.yrel) / 32768.0f);
	}
}


void KeyBindingManager::onMouseWheelMove (const SDL_MouseWheelEvent &mwe)
{
	// don't do anything if it's swallowed
	if (LKernel::getG<InputSwallowerManager>()->isSwallowed())
		return;

	if (mwe.x != 0) {
		auto binding = gameInputsMapByReal.find(SDLInputID::ofMouseWheelAxis(0));
		if (binding != gameInputsMapByReal.end())
			fireEvent(axisMoveEvent, 0, binding->second, static_cast<float>(mwe.x) / 3.0f);
	}
	if (mwe.y != 0) {
		auto binding = gameInputsMapByReal.find(SDLInputID::ofMouseWheelAxis(1));
		if (binding != gameInputsMapByReal.end())
			fireEvent(axisMoveEvent, 0, binding->second, static_cast<float>(mwe.y) / 3.0f);
	}
}


void KeyBindingManager::onMousePress (const SDL_MouseButtonEvent &mbe)
{
	if (LKernel::getG<InputSwallowerManager>()->isSwallowed())
		return;

	auto binding = gameInputsMapByReal.find(SDLInputID(mbe));
	if (binding != gameInputsMapByReal.end())
		fireEvent(pressEvent, 0, binding->second);
}
void KeyBindingManager::onMouseRelease (const SDL_MouseButtonEvent &mbe)
{
	auto binding = gameInputsMapByReal.find(SDLInputID(mbe));
	if (binding != gameInputsMapByReal.end())
		fireEvent(releaseEvent, 0, binding->second);
}


void KeyBindingManager::onControllerAxisMove (const SDL_ControllerAxisEvent &cae)
{
	// don't do anything if it's swallowed
	if (LKernel::getG<InputSwallowerManager>()->isSwallowed())
		return;

	auto binding = gameInputsMapByReal.find(SDLInputID(cae));
	if (binding != gameInputsMapByReal.end())
		fireEvent(axisMoveEvent, 0, binding->second, static_cast<float>(cae.value) / 32768.0f);
}


void KeyBindingManager::onControllerButtonPress (const SDL_ControllerButtonEvent &cbe)
{
	if (LKernel::getG<InputSwallowerManager>()->isSwallowed())
		return;

	auto binding = gameInputsMapByReal.find(SDLInputID(cbe));
	if (binding != gameInputsMapByReal.end())
		fireEvent(pressEvent, 0, binding->second);
}
void KeyBindingManager::onControllerButtonRelease (const SDL_ControllerButtonEvent &cbe)
{
	auto binding = gameInputsMapByReal.find(SDLInputID(cbe));
	if (binding != gameInputsMapByReal.end())
		fireEvent(releaseEvent, 0, binding->second);
}
