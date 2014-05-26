#include "pch.h"
#include "Core/main.h"
#include "Handlers/UI/MainMenuUIHandler.h"
#include "Handlers/MainMenuSinglePlayerHandler.h"
#include "Kernel/LKernel.h"
#include "Kernel/LKernelOgre.h"
#include "UI/MainMenuManager.h"

using namespace CEGUI;
using namespace Ponykart;
using namespace Ponykart::Handlers;
using namespace Ponykart::UI;
using namespace Ponykart::LKernel;

MainMenuManager::MainMenuManager()
{
	// TODO: Implement correctly
	// TODO: Add the onLevelLoad and onLevelUnload event handlers

	// Add a quick menu placeholder as a test
	Window* mainWindow = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();

	// Loading screen
	Window* loadingWindow = WindowManager::getSingleton().createWindow("Generic/Image", "loadingWindow");
	loadingWindow->setSize(USize(UDim(1, 0), UDim(1, 0)));
	loadingWindow->setProperty("Image", "loading/background");

	// Background
	Window* dashbackground = WindowManager::getSingleton().createWindow("Generic/Image", "dashbackground");
	dashbackground->setSize(USize(UDim(1, 0), UDim(1, 0)));
	dashbackground->setProperty("Image", "dashbackground/dashbackground");
	mainWindow->addChild(dashbackground);

	Window* checkers = WindowManager::getSingleton().createWindow("Generic/Image", "checkers");
	checkers->setSize(USize(UDim(0.5, 0), UDim(1, 0)));
	checkers->setProperty("Image", "checkers/checkers");
	mainWindow->addChild(checkers);

	Window* playmenu = WindowManager::getSingleton().createWindow("Generic/Image", "playmenu");
	playmenu->setPosition({ { 0.20f, 0 }, { 0.15f, 0 } });
	playmenu->setSize(USize(UDim(0.60f, 0), UDim(0.85f, 0)));
	playmenu->setProperty("Image", "playmenu/playmenu");
	mainWindow->addChild(playmenu);
	
	// Menu buttons
	Window* playButton = WindowManager::getSingleton().createWindow("Ponykart/Button", "playButton");
	playButton->setYPosition({ 0.30f, 0 });
	playButton->setText("Single Player");
	auto playLambda = [=](const EventArgs& e)
	{
		auto ogreRoot = getG<Ogre::Root>();
		auto sdlWindow = getG<SDL_Window>();
		auto ogreWindow = getG<Ogre::RenderWindow>();

		// Show the loading screen and redraw
		System::getSingleton().getDefaultGUIContext().setRootWindow(loadingWindow);
		ogreRoot->_fireFrameStarted();
		getG<Ogre::RenderWindow>()->update(true);
		ogreRoot->_fireFrameRenderingQueued();
		SDL_GL_SwapWindow(getG<SDL_Window>());

		// Load a level
		MainMenuUIHandler* menuUiHandler = getG<MainMenuUIHandler>();
		MainMenuSinglePlayerHandler* menuSpHandler = getG<MainMenuSinglePlayerHandler>();
		menuUiHandler->onGameType_SelectSinglePlayer();
		//menuSpHandler->onLevelSelect("WhitetailWoods");
		menuSpHandler->onLevelSelect("SweetAppleAcres");
		//menuSpHandler->onCharacterSelect("Rarity");
		menuSpHandler->onCharacterSelect("Applejack");

		// Hide the loading screen so we can see the level
		loadingWindow->hide();
		return true; 
	};
	playButton->subscribeEvent(PushButton::EventClicked, Event::Subscriber(playLambda));
	playmenu->addChild(playButton);

	Window* hostButton = WindowManager::getSingleton().createWindow("Ponykart/Button", "hostButton");
	hostButton->setYPosition({ 0.40f, 0 });
	hostButton->setText("Host Networked Game");
	hostButton->setEnabled(false);
	playmenu->addChild(hostButton);

	Window* joinButton = WindowManager::getSingleton().createWindow("Ponykart/Button", "joinButton");
	joinButton->setYPosition({ 0.50f, 0 });
	joinButton->setText("Join Networked Game");
	joinButton->setEnabled(false);
	playmenu->addChild(joinButton);

	Window* optionsButton = WindowManager::getSingleton().createWindow("Ponykart/Button", "optionsButton");
	optionsButton->setYPosition({ 0.60f, 0 });
	optionsButton->setText("Options");
	optionsButton->setEnabled(false);
	playmenu->addChild(optionsButton);

	Window* quitButton = WindowManager::getSingleton().createWindow("Ponykart/Button", "quitButton");
	quitButton->setYPosition({ 0.70f, 0 });
	quitButton->setText("Quit");
	auto quitLambda = [](const EventArgs& e){Launch::quit = true; return true; };
	quitButton->subscribeEvent(PushButton::EventClicked, Event::Subscriber(quitLambda));
	playmenu->addChild(quitButton);
}
