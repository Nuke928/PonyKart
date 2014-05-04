#include "pch.h"
#include "sdl2Extensions.h"

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
	return id;
}
SDLInputID SDLInputID::ofMouseWheelAxis (Uint8 axis)
{
	auto id = SDLInputID();
	id.inputType = SDL_MOUSEWHEEL;
	id.input.maxis = axis;
	return id;
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
	case SDL_MOUSEWHEEL:
		return other.input.maxis == input.maxis;
		return other.input.maxis == input.maxis;
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
