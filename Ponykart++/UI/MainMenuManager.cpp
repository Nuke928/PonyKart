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
		mainWindow->hide();
		MainMenuUIHandler* menuUiHandler = getG<MainMenuUIHandler>();
		MainMenuSinglePlayerHandler* menuSpHandler = getG<MainMenuSinglePlayerHandler>();
		menuUiHandler->onGameType_SelectSinglePlayer();
		menuSpHandler->onLevelSelect("SweetAppleAcres");  
		menuSpHandler->onCharacterSelect("Applejack");
		return true; 
	};
	playButton->subscribeEvent(PushButton::EventClicked, Event::Subscriber(playLambda));
	playmenu->addChild(playButton);

	Window* hostButton = WindowManager::getSingleton().createWindow("Ponykart/Button", "hostButton");
	hostButton->setYPosition({ 0.40f, 0 });
	hostButton->setText("Host Networked Game");
	playmenu->addChild(hostButton);

	Window* joinButton = WindowManager::getSingleton().createWindow("Ponykart/Button", "joinButton");
	joinButton->setYPosition({ 0.50f, 0 });
	joinButton->setText("Join Networked Game");
	playmenu->addChild(joinButton);

	Window* optionsButton = WindowManager::getSingleton().createWindow("Ponykart/Button", "optionsButton");
	optionsButton->setYPosition({ 0.60f, 0 });
	optionsButton->setText("Options");
	playmenu->addChild(optionsButton);

	Window* quitButton = WindowManager::getSingleton().createWindow("Ponykart/Button", "quitButton");
	quitButton->setYPosition({ 0.70f, 0 });
	quitButton->setText("Quit");
	auto quitLambda = [](const EventArgs& e){Launch::quit = true; return true; };
	quitButton->subscribeEvent(PushButton::EventClicked, Event::Subscriber(quitLambda));
	playmenu->addChild(quitButton);
}
