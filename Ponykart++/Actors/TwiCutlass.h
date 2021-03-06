#ifndef TWICUTLASS_H_INCLUDED
#define TWICUTLASS_H_INCLUDED

#include "Misc/alExtensions.h"
#include "Actors/Kart.h"

namespace PonykartParsers
{
	class ThingBlock;
	class ThingDefinition;
} // PonykartParsers

namespace Ponykart
{
namespace Sound
{
	class SoundMain;
} // Sound

namespace Actors
{
	class TwiCutlass : public Kart, public Ogre::FrameListener
	{
	public:
		TwiCutlass(PonykartParsers::ThingBlock* block, PonykartParsers::ThingDefinition* def);
		bool frameStarted(const Ogre::FrameEvent& evt) override; ///< Change the width of the jet engine based on our current speed
		// Getters

	private:
        const float topSpeedKmHour;
        Sound::SoundMain* soundMain;
		Extensions::ALSource idleSound, fullSound;
        bool idleState;
	};
} // Actors
} // Ponykart

#endif // TWICUTLASS_H_INCLUDED
