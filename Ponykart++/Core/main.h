#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <string>
#include <SDL.h>

namespace Ponykart
{
namespace Launch
{
	extern Uint32 tenthOfASecondEvent;

	void enterGameLoop ();
	unsigned int tenthOfASecondCallback (Uint32 interval, void *param);

//	static bool quit;
};
}

#endif // MAIN_H_INCLUDED
