#ifndef INPUTMAIN_H_INCLUDED
#define INPUTMAIN_H_INCLUDED

#include <vector>
#include <functional>
#include <unordered_map>
#include <SDL.h>
#include <OgreFrameListener.h>
#include <boost/signals2.hpp>
#include "Kernel/LKernelObject.h"
#include "Misc/sdl2Extensions.h"

namespace Ponykart
{
namespace Input
{

/// Class that handles all of the input.
/**
 * This class takes input event objects from the message loop and dispatches them to interested
 * subscribers.
 *
 * NOTE: This class provides SDL's notion of "game controllers" as a specific kind of joystick.
 * For a joystick to considered a game controller, it must have an input mapping that specifies
 * which buttons and axes on the generic joystick have what position on the device, using the 360
 * pad as a template.
 */
class InputMain : public LKernel::LKernelObject
{
public:
	InputMain();

	SDL_JoystickID getControllerID(SDL_GameController *controller);
	SDL_GameController *getController(SDL_JoystickID joystickID);

	// Event handlers
	void processEvent (const SDL_Event &event); ///< Process SDL input events
	void receiveTextInput (bool enabled);

	// Events
	boost::signals2::signal<void (const SDL_KeyboardEvent&)> onKeyPress;
	boost::signals2::signal<void (const SDL_KeyboardEvent&)> onKeyRelease;
	boost::signals2::signal<void (const SDL_TextInputEvent&)> onTextInput;
	boost::signals2::signal<void (const SDL_TextEditingEvent&)> onTextEdit;

	boost::signals2::signal<void (const SDL_MouseMotionEvent&)> onMouseMove;
	boost::signals2::signal<void (const SDL_MouseButtonEvent&)> onMouseButtonPress;
	boost::signals2::signal<void (const SDL_MouseButtonEvent&)> onMouseButtonRelease;
	boost::signals2::signal<void (const SDL_MouseWheelEvent&)> onMouseWheelMove;

	boost::signals2::signal<void (const SDL_JoyDeviceEvent&)> onJoystickConnect;
	boost::signals2::signal<void (const SDL_JoyDeviceEvent&)> onJoystickRemove;
	boost::signals2::signal<void (const SDL_JoyAxisEvent&)> onJoystickAxisMove;
	boost::signals2::signal<void (const SDL_JoyButtonEvent&)> onJoystickButtonPress;
	boost::signals2::signal<void (const SDL_JoyButtonEvent&)> onJoystickButtonRelease;

	boost::signals2::signal<void (const SDL_ControllerDeviceEvent&)> onControllerConnect;
	boost::signals2::signal<void (const SDL_ControllerDeviceEvent&)> onControllerRemove;
	boost::signals2::signal<void (const SDL_ControllerAxisEvent&)> onControllerAxisMove;
	boost::signals2::signal<void (const SDL_ControllerButtonEvent&)> onControllerButtonPress;
	boost::signals2::signal<void (const SDL_ControllerButtonEvent&)> onControllerButtonRelease;

	// Polling
	bool pollButton (Extensions::SDLInputID inputID, SDL_GameController *controller = nullptr);
	bool pollButton (Extensions::SDLInputID inputID, SDL_JoystickID controllerID);
	int pollAxis (Extensions::SDLInputID inputID, SDL_GameController *controller = nullptr);
	int pollAxis (Extensions::SDLInputID inputID, SDL_JoystickID controllerID);

private:
	void openController (int deviceIndex);
	void closeController (int controllerID);

	std::unordered_map<SDL_JoystickID, SDL_GameController *> controllersMapByJoystickID;
};

} // Input
} // Ponykart
#endif // INPUTMAIN_H_INCLUDED
