#include "pch.h"
#include "UI/MainMenuManager.h"

using namespace CEGUI;
using namespace Ponykart::UI;

MainMenuManager::MainMenuManager()
{
	// TODO: Implement correctly
	// TODO: Add the onLevelLoad and onLevelUnload event handlers

	// Add a quick menu placeholder as a test
	Window* mainWindow = CEGUI::System::getSingleton().getDefaultGUIContext().getRootWindow();

	Window* dashbackground = WindowManager::getSingleton().createWindow("Generic/Image", "dashbackground");
	dashbackground->setSize(USize(UDim(1, 0), UDim(1, 0)));
	dashbackground->setProperty("Image", "dashbackground/dashbackground");
	mainWindow->addChild(dashbackground);

	Window* checkers = WindowManager::getSingleton().createWindow("Generic/Image", "checkers");
	checkers->setSize(USize(UDim(1, 0), UDim(1, 0)));
	checkers->setProperty("Image", "checkers/checkers");
	mainWindow->addChild(checkers);
}
