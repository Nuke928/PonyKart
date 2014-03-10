#ifndef SDL2EXTENSIONS_H_INCLUDED
#define SDL2EXTENSIONS_H_INCLUDED

#include <string>
#include <SDL.h>


namespace Extensions
{
	class SDL2Exception {
	public:
		const char *what () const { return SDL_GetError(); }
	};

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

}

#endif // SDL2EXTENSIONS_H_INCLUDED
