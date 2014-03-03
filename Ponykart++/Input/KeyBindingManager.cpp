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

// Define our static members
std::map<LKey, SDL_Keycode> KeyBindingManager::lKeysDict;
std::map<SDL_Keycode, LKey> KeyBindingManager::sdlKeysDict;
std::map<ControllerButtons, LKey> KeyBindingManager::lButtonsDict;
std::map<ControllerAxis, LKey> KeyBindingManager::lAxisDict;
std::map<LKey, std::function<void ()>> KeyBindingManager::pressEventsDict;
std::map<LKey, std::function<void ()>> KeyBindingManager::releaseEventsDict;
std::map<LKey, std::function<void ()>> KeyBindingManager::axisEvents;

KeyBindingManager::KeyBindingManager()
{
	log("[Loading] Creating KeyBindingManager...");

	setupInitialBindings();

	// TODO: Replace the LymphInputEvents with std::functions

	auto input = LKernel::getG<InputMain>();
	input->onKeyboardPress.push_back(onKeyboardPress);
	input->onKeyboardRelease.push_back(onKeyboardRelease);


//	if (Options::getBool("Twh"))
//	{
//		input->onMousePress_Anything.push_back(onMousePress_Anything);
//		input->onMouseRelease_Anything.push_back(onMouseRelease_Anything);
//	}
}

// TODO read these from a file
void KeyBindingManager::setupInitialBindings()
{
	lKeysDict[LKey::Accelerate] = SDLK_w;
	lKeysDict[LKey::TurnLeft] = SDLK_a;
	lKeysDict[LKey::TurnRight] = SDLK_d;
	lKeysDict[LKey::Drift] = SDLK_SPACE;
	lKeysDict[LKey::Reverse] = SDLK_s;
	lKeysDict[LKey::Item] = SDLK_LSHIFT;
	sdlKeysDict[SDLK_w] = LKey::Accelerate;
	sdlKeysDict[SDLK_a] = LKey::TurnLeft;
	sdlKeysDict[SDLK_d] = LKey::TurnRight;
	sdlKeysDict[SDLK_SPACE] = LKey::Drift;
	sdlKeysDict[SDLK_s] = LKey::Reverse;
	sdlKeysDict[SDLK_LSHIFT] = LKey::Item;
	lButtonsDict[ControllerButtons::A] = LKey::Drift;
	lAxisDict[ControllerAxis::LeftX] = LKey::SteeringAxis;

	pressEventsDict.insert(pair<LKey,function<void()>>(LKey::Accelerate, function<void ()>()));
	pressEventsDict.insert(pair<LKey,function<void()>>(LKey::TurnLeft, function<void ()>()));
	pressEventsDict.insert(pair<LKey,function<void()>>(LKey::TurnRight, function<void ()>()));
	pressEventsDict.insert(pair<LKey,function<void()>>(LKey::Drift, function<void ()>()));
	pressEventsDict.insert(pair<LKey,function<void()>>(LKey::Reverse, function<void ()>()));
	pressEventsDict.insert(pair<LKey,function<void()>>(LKey::Item, function<void ()>()));

	releaseEventsDict.insert(pair<LKey,function<void()>>(LKey::Accelerate, function<void ()>()));
	releaseEventsDict.insert(pair<LKey,function<void()>>(LKey::TurnLeft, function<void ()>()));
	releaseEventsDict.insert(pair<LKey,function<void()>>(LKey::TurnRight, function<void ()>()));
	releaseEventsDict.insert(pair<LKey,function<void()>>(LKey::Drift, function<void ()>()));
	releaseEventsDict.insert(pair<LKey,function<void()>>(LKey::Reverse, function<void ()>()));
	releaseEventsDict.insert(pair<LKey,function<void()>>(LKey::Item, function<void ()>()));
}

void KeyBindingManager::onKeyboardPress(const SDL_KeyboardEvent &ke)
{
	// don't do anything if it's swallowed
	if (LKernel::getG<InputSwallowerManager>()->isSwallowed())
		return;

	LKey key;
	if (sdlKeysDict.find(ke.keysym.scancode) != sdlKeysDict.end()) // TODO: Check if this is correct. I'm not sure of the translation.
		invoke(pressEventsDict[key]);
}

void KeyBindingManager::onKeyboardRelease(const SDL_KeyboardEvent &ke)
{
	// don't do anything if it's swallowed
	if (LKernel::getG<InputSwallowerManager>()->isSwallowed())
		return;

	LKey key;
	if (sdlKeysDict.find(ke.keysym.scancode) != sdlKeysDict.end()) // TODO: Check if this is correct. I'm not sure of the translation.
		invoke(releaseEventsDict[key]);
}

void KeyBindingManager::onMousePress(const SDL_MouseButtonEvent &mbe)
{
	if (LKernel::getG<InputSwallowerManager>()->isSwallowed())
		return;

	switch (mbe.button)
	{
		case SDL_BUTTON_LEFT:
			invoke(pressEventsDict[LKey::Accelerate]); break;
		case SDL_BUTTON_X1:
			invoke(pressEventsDict[LKey::TurnLeft]); break;
		case SDL_BUTTON_X2:
			invoke(pressEventsDict[LKey::TurnRight]); break;
		case SDL_BUTTON_MIDDLE:
			invoke(pressEventsDict[LKey::Drift]); break;
		default:;
	}
}

void KeyBindingManager::onMouseRelease(const SDL_MouseButtonEvent &mbe)
{
	switch (mbe.button)
	{
	case SDL_BUTTON_LEFT:
		invoke(releaseEventsDict[LKey::Accelerate]); break;
	case SDL_BUTTON_X1:
		invoke(releaseEventsDict[LKey::TurnLeft]); break;
	case SDL_BUTTON_X2:
		invoke(releaseEventsDict[LKey::TurnRight]); break;
	case SDL_BUTTON_MIDDLE:
		invoke(releaseEventsDict[LKey::Drift]); break;
	default:;
	}
}

void KeyBindingManager::invoke(std::function<void ()> e)
{
	if (e)
		e();
}
