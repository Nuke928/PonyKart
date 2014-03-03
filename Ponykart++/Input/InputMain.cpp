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
		throw string(SDL_GetError());


//	// Create all devices (except joystick, as most people have Keyboard/Mouse) using buffered input.
//	inputController = new Core::ControllerManager();

	LKernel::gRoot->addFrameListener(this);  // TODO: ASAP! Implement this frame listener! It's crashing without it.

	createEventHandlers(); // TODO: Implement

	log("[Loading] SDL2 input system loaded.");
}

void InputMain::createEventHandlers()
{
}

template<typename T> void InputMain::fireEvent(LymphInputEvent1<T> handler, const T &eventArgs)
{
	if (handler.size())
		for (auto fun : handler)
			fun(eventArgs);
}

//template<typename T, typename U> void InputMain::fireEvent(LymphInputEvent2<T,U> handler, const T &eventArg1, const U &eventArg2)
//{
//	if (handler.size())
//		for (auto fun : handler)
//			fun(eventArg1, eventArg2);
//}

bool InputMain::keyPressed(const SDL_KeyboardEvent &ke)
{
	fireEvent<SDL_KeyboardEvent>(onKeyboardPress, ke);
	return true;
}

bool InputMain::keyReleased(const SDL_KeyboardEvent &ke)
{
	fireEvent<SDL_KeyboardEvent>(onKeyboardRelease, ke);
	return true;
}

bool InputMain::mousePressed(const SDL_MouseButtonEvent &mbe)
{
	fireEvent<SDL_MouseButtonEvent>(onMousePress, mbe);
	return true;
}

bool InputMain::mouseReleased(const SDL_MouseButtonEvent &mbe)
{
	fireEvent<SDL_MouseButtonEvent>(onMouseRelease, mbe);
	return true;
}

bool InputMain::mouseMoved(const SDL_MouseMotionEvent &mme)
{
	// you can use mme.xrel/yrel for relative position, and mme.x/y for absolute
	fireEvent<SDL_MouseMotionEvent>(onMouseMove, mme);
	return true;
}

bool InputMain::mouseWheelMoved(const SDL_MouseWheelEvent &mwe)
{
	fireEvent<SDL_MouseWheelEvent>(onMouseWheelMove, mwe);
	return true;
}

bool InputMain::frameStarted(const Ogre::FrameEvent &evt)
{
	if (!LKernel::getG<LevelManager>()->getIsValidLevel())
		return true;
	
	//timeSinceLastFrame += e.timeSinceLastFrame;
	//if (timeSinceLastFrame >= _inputCaptureRate) {
	// Capture all key presses since last check.
	//inputKeyboard->capture();
	// Capture all mouse movements and button presses since last check.
	//inputMouse->capture();
	//	timeSinceLastFrame -= _inputCaptureRate;
	//}

	return true;
}
