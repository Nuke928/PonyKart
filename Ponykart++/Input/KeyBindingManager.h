#ifndef KEYBINDINGMANAGER_H_INCLUDED
#define KEYBINDINGMANAGER_H_INCLUDED

#include <map>
#include <functional>
#include <SDL.h>
#include "Kernel/LKernelObject.h"
#include "Input/ControllerManager.h"


namespace Ponykart
{
namespace Input
{

/// Our key commands - these are for things that need to be polled.
/** If you want to just respond to events, use the ones in InputMain. */
enum LKey
{
	Accelerate,
	TurnLeft,
	TurnRight,
	Drift,
	Reverse,
	SteeringAxis,
	AccelerateAxis,
	BrakeAxis,
	Item
};

/// This class provides an interface between game commands (accelerate, etc) and key presses (WASD, etc).
/** This way we can change which keys do things at runtime */
class KeyBindingManager : public LKernel::LKernelObject
{
public:
	KeyBindingManager();
	void setupInitialBindings(); ///< Set up some initial key bindings
	static void onKeyboardPress(const SDL_KeyboardEvent &ke);
	static void onKeyboardRelease(const SDL_KeyboardEvent &ke);
	static void onMousePress(const SDL_MouseButtonEvent &mbe);
	static void onMouseRelease(const SDL_MouseButtonEvent &mbe);
	static void invoke(std::function<void ()> e);
	// Getters
	const std::map<LKey, std::function<void ()>>& getPressEventsDict();
	const std::map<LKey, std::function<void ()>>& getReleaseEventsDict();
	const std::map<LKey, std::function<void ()>>& getAxisEvents();

private:
	// The maps that converts our key commands into SDL keys
	static std::map<LKey, SDL_Keycode> lKeysDict;
	static std::map<SDL_Keycode, LKey> sdlKeysDict;
	static std::map<ControllerButtons, LKey> lButtonsDict;
	static std::map<ControllerAxis, LKey> lAxisDict;

	static std::map<LKey, std::function<void ()>> pressEventsDict;
	static std::map<LKey, std::function<void ()>> releaseEventsDict;
	static std::map<LKey, std::function<void ()>> axisEvents;
};

} // Input
} // Ponykart

#endif // KEYBINDINGMANAGER_H_INCLUDED
