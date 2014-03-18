#include "pch.h"
#include "Core/Cameras/CameraManager.h"
#include "Kernel/LKernel.h"
#include "Levels/LevelManager.h"

using namespace std;
using namespace Ponykart::Core;
using namespace Ponykart::Levels;

CameraManager::CameraManager()
{
	LevelManager::onLevelLoad.push_back(bind(&CameraManager::onLevelLoad,this,placeholders::_1));
	LevelManager::onLevelUnload.push_back(bind(&CameraManager::onLevelUnload,this,placeholders::_1));
}

void CameraManager::onLevelLoad(LevelChangedEventArgs *args)
{
}

void CameraManager::onLevelUnload(LevelChangedEventArgs *args)
{
}

const LCamera* const CameraManager::getCurrentCamera()
{
	return currentCamera;
}
