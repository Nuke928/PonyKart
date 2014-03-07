#ifndef KEYBINDINGMANAGER_H_INCLUDED
#define KEYBINDINGMANAGER_H_INCLUDED

#include <functional>
#include <vector>
#include <map>
#include <unordered_map>
#include <SDL.h>
#include "Kernel/LKernelObject.h"
#include "Misc/eventExtensions.h"

namespace Ponykart
{
namespace Input
{

template<typename... T> using LInputEvent = typename std::vector<std::function<void (const T &...eventArgs)>>;

/// Struct representing any possible input button or axis SDL can provide.
/** Represented as a pair of an SDL event and a specific named input.
 *  Axes are indicated with MOTION events, buttons are indicated by DOWN events.
 */
struct SDLInputID
{
	SDL_EventType inputType;
	union
	{
		SDL_Scancode scancode;
		Uint8 maxis; // 0: X, 1: Y
		Uint8 mbutton;
		SDL_GameControllerAxis caxis;
		SDL_GameControllerButton cbutton;
		Uint8 dummy;
	} input;

	SDLInputID (const SDLInputID &other);
	SDLInputID (SDL_Scancode scancode);
	static SDLInputID ofMouseAxis (Uint8 axis);
	static SDLInputID ofMouseWheelAxis (Uint8 axis);
	SDLInputID (SDL_GameControllerAxis axis);
	SDLInputID (SDL_GameControllerButton button);
	SDLInputID (const SDL_KeyboardEvent &ke);
	SDLInputID (const SDL_MouseButtonEvent &mbe);
	SDLInputID (const SDL_ControllerAxisEvent &cae);
	SDLInputID (const SDL_ControllerButtonEvent &cbe);

	SDLInputID &operator= (SDL_Scancode scancode);
	SDLInputID &operator= (SDL_GameControllerAxis axis);
	SDLInputID &operator= (SDL_GameControllerButton button);

	bool operator== (SDLInputID other) const;

	struct Hash
	{
		size_t operator() (SDLInputID value) const;
	};

private:
	SDLInputID () { }
};

/// Our key commands - these are for things that need to be polled.
/** If you want to just respond to events, use the ones in InputMain. */
enum class LInputID
{
	SteeringAxis,
	AccelerateAxis,
	BrakeAxis,

	Accelerate,
	Reverse,
	TurnLeft,
	TurnRight,
	Drift,
	Item
};

/// This class provides an interface between game commands (accelerate, etc) and key presses (WASD, etc).
/** This way we can change which keys do things at runtime */
class KeyBindingManager : public LKernel::LKernelObject
{
public:
	KeyBindingManager();

	void setControllerPlayer (SDL_JoystickID controllerID, int playerID);
	void clearControllerPlayer (SDL_JoystickID controllerID);
	void clearPlayerController (int playerID);
	void clearControllerPlayers ();

	void setKeyBinding (SDLInputID realInput, LInputID gameInput);
	void clearKeyBinding (SDLInputID realInput);
	void clearKeyBinding (LInputID gameInput);
	void clearKeyBindings ();

	// Events
	// First argument is local player ID (0 based).
	Extensions::EventDelegate<int, LInputID> pressEvent;
	Extensions::EventDelegate<int, LInputID> releaseEvent;
	Extensions::EventDelegate<int, LInputID, float> axisMoveEvent;

private:
	// Mapping of controllers to players and vice-versa.
	// Keyboard and mouse are always associated with playerID 0.
	std::unordered_multimap<SDL_JoystickID, int> playersMapByController;
	std::map<int, SDL_JoystickID> controllersMapByPlayer;

	// One-to-one mapping of real inputs to game inputs (any given button can only be bound to one action).
	std::unordered_map<SDLInputID, LInputID, typename SDLInputID::Hash> gameInputsMapByReal;
	// One-to-many mapping of game inputs to real inputs (you can bind multiple buttons to the same action)
	std::multimap<LInputID, SDLInputID> realInputsMapByGame;

	void setDefaultBindings(); ///< Set up some initial key bindings
	void onKeyboardPress (const SDL_KeyboardEvent &ke);
	void onKeyboardRelease (const SDL_KeyboardEvent &ke);
	void onMouseMove (const SDL_MouseMotionEvent &mme);
	void onMouseWheelMove (const SDL_MouseWheelEvent &mwe);
	void onMousePress (const SDL_MouseButtonEvent &mbe);
	void onMouseRelease (const SDL_MouseButtonEvent &mbe);
	void onControllerAxisMove (const SDL_ControllerAxisEvent &cae);
	void onControllerButtonPress (const SDL_ControllerButtonEvent &cbe);
	void onControllerButtonRelease (const SDL_ControllerButtonEvent &cbe);

public:
	// Accessors
	decltype(playersMapByController) &getPlayersMapByController() { return playersMapByController; }
	decltype(controllersMapByPlayer) &getControllersMapByPlayer() { return controllersMapByPlayer; }
	decltype(gameInputsMapByReal) &getGameInputMapByReal() { return gameInputsMapByReal; }
	decltype(realInputsMapByGame) &getRealInputMapByGame() { return realInputsMapByGame; }
};

} // Input
} // Ponykart

#endif // KEYBINDINGMANAGER_H_INCLUDED
