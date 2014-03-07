#ifndef INPUTMAIN_H_INCLUDED
#define INPUTMAIN_H_INCLUDED

#include <vector>
#include <functional>
#include <SDL.h>
#include <OgreFrameListener.h>
#include "Kernel/LKernelObject.h"
#include "Misc/eventExtensions.h"


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

	// Event handlers
	void processEvent (const SDL_Event &event); ///< Process SDL input events
	void receiveTextInput (bool enabled);

	// Events
	Extensions::EventDelegate<const SDL_KeyboardEvent&> onKeyPress;
	Extensions::EventDelegate<const SDL_KeyboardEvent&> onKeyRelease;
	Extensions::EventDelegate<const SDL_TextInputEvent&> onTextInput;
	Extensions::EventDelegate<const SDL_TextEditingEvent&> onTextEdit;

	Extensions::EventDelegate<const SDL_MouseMotionEvent&> onMouseMove;
	Extensions::EventDelegate<const SDL_MouseButtonEvent&> onMouseButtonPress;
	Extensions::EventDelegate<const SDL_MouseButtonEvent&> onMouseButtonRelease;
	Extensions::EventDelegate<const SDL_MouseWheelEvent&> onMouseWheelMove;

	Extensions::EventDelegate<const SDL_JoyDeviceEvent&> onJoystickConnect;
	Extensions::EventDelegate<const SDL_JoyDeviceEvent&> onJoystickRemove;
	Extensions::EventDelegate<const SDL_JoyAxisEvent&> onJoystickAxisMove;
	Extensions::EventDelegate<const SDL_JoyButtonEvent&> onJoystickButtonPress;
	Extensions::EventDelegate<const SDL_JoyButtonEvent&> onJoystickButtonRelease;

	Extensions::EventDelegate<const SDL_ControllerDeviceEvent&> onControllerConnect;
	Extensions::EventDelegate<const SDL_ControllerDeviceEvent&> onControllerRemove;
	Extensions::EventDelegate<const SDL_ControllerAxisEvent&> onControllerAxisMove;
	Extensions::EventDelegate<const SDL_ControllerButtonEvent&> onControllerButtonPress;
	Extensions::EventDelegate<const SDL_ControllerButtonEvent&> onControllerButtonRelease;
};

} // Input
} // Ponykart
#endif // INPUTMAIN_H_INCLUDED
