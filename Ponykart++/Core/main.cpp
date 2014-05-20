#include "pch.h"
#include "Core/main.h"

#include <string>
#include <fstream>
#include <cstdio>
#include <Ogre.h>
#include <OgreWindowEventUtilities.h>
#include "Core/Options.h"
#include "UI/Splash.h"
#include "Kernel/LKernel.h"
#include "Kernel/LKernelOgre.h"
#include "Input/InputMain.h"
#include "Input/KeyBindingManager.h"
#include "Misc/sdl2Extensions.h"

#define CATCH_STD_STRINGS false

using namespace Ponykart::Launch;
using namespace Ponykart::LKernel;
using namespace Ponykart::Input;
using namespace Ponykart::UI;
using namespace Extensions;
using Ponykart::Core::Options;
using Ponykart::UI::Splash;

Uint32 Ponykart::Launch::tenthOfASecondEvent = 0;
bool Ponykart::Launch::quit = false;

int main (int argc, char *argv[])
{
	try
	{
		initOgreRoot();
	}
	catch (SDL2Exception e)
	{
		std::ofstream logfile;
		auto logStr = std::string("Error initializing SDL2: ") + e.what();
		std::cerr << logStr << std::endl;
		logfile.open("Ponykart.log", std::ios::out);
		if (logfile.is_open())
		{
			logfile.clear();
			logfile << logStr;
			logfile.close();
		}
		abort();
	}
	catch (std::bad_alloc e)
	{
		std::ofstream logfile;
		auto logStr = "Error initializing ogre. Are you using the correct DLLs ?";
		std::cerr << logStr << std::endl;
		logfile.open("Ponykart.log", std::ios::out);
		if (logfile.is_open())
		{
			logfile.clear();
			logfile << logStr << std::endl;
			logfile.close();
		}
		abort();
	}
	catch (Ogre::InternalErrorException e)
	{
		std::ofstream logfile;
		std::cerr << "Error initializing ogre : " << e.getDescription();
		logfile.open("Ponykart.log", std::ios::out);
		if (logfile.is_open())
		{
			logfile.clear();
			logfile  << "Error initializing ogre : " << e.getFullDescription();
			logfile.close();
		}
		abort();
	}
	
#if (CATCH_STD_STRING)
	try
	{
#endif
		Options::initialize();
		initOgreGraphics();

		// Splash screen
		{
			Splash splash;
			loadInitialObjects(splash);
		}
		enterGameLoop();

		log("End of code. Shutting down...");
		shutdownOgre();
		std::printf("Shutdown complete.\n");
		return EXIT_SUCCESS;
#if (CATCH_STD_STRING)
	}
	catch (std::string e) // If you can't guarantee that someone will catch your exceptions, throw a string.
	{
		log("[ERROR][EXCEPTION] " + e);
	}

	log ("[ERROR] Exception thrown! Shutting down...");
	shutdownOgre();
	std::printf("Post-exception shutdown complete.\n");
	return EXIT_FAILURE; // If we're here, we came from a catch
#endif
}


void Ponykart::Launch::enterGameLoop ()
{
	auto ogreRoot = getG<Ogre::Root>();
	auto sdlWindow = getG<SDL_Window>();
	auto ogreWindow = getG<Ogre::RenderWindow>();

	tenthOfASecondEvent = SDL_RegisterEvents(1);
	auto tenthOfASecondTimer = SDL_AddTimer(100, &tenthOfASecondCallback, nullptr);

	auto binder = LKernel::getG<KeyBindingManager>();
	binder->pressEvent.connect([](int playerID, GameInputID input) {
		std::cout << "PRESS" << std::endl;
	});

	while (!quit) {
		ogreRoot->_fireFrameStarted();
		ogreWindow->update(false);
		ogreRoot->_fireFrameRenderingQueued();
		ogreRoot->_fireFrameEnded();
		SDL_GL_SwapWindow(sdlWindow);

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
			case SDL_USEREVENT:
				if (event.user.code == tenthOfASecondEvent)
					onEveryUnpausedTenthOfASecondEvent(nullptr);
				break;
			case SDL_QUIT:
				quit = true;
				break;
			default:
				LKernel::getG<InputMain>()->processEvent(event);
				break;
			}
		}
	}

	SDL_RemoveTimer(tenthOfASecondTimer);
}

unsigned int Ponykart::Launch::tenthOfASecondCallback (Uint32 interval, void *param) {
	SDL_Event event;
	SDL_zero(event);
	event.type = SDL_USEREVENT;
	event.user.code = tenthOfASecondEvent;

	SDL_PushEvent(&event);

	CEGUI::System::getSingleton().injectTimePulse(0.1f);

	return 100;
}
