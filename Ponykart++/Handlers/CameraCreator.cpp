#include "pch.h"
#include "Handlers/CameraCreator.h"
#include "Core/Cameras/PlayerCamera.h"

using namespace Ponykart::Core;
using namespace Ponykart::Handlers;

CameraCreatorHandler::CameraCreatorHandler()
{
	PlayerCamera* pcam = new PlayerCamera("PlayerCamera");
	pcam->registerCam();
	pcam->makeActiveCamera();
}
