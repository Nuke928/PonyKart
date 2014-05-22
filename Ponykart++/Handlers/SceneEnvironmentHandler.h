#ifndef SCENEENVIRONMENTHANDLER_H_INCLUDED
#define SCENEENVIRONMENTHANDLER_H_INCLUDED

#include "Kernel/LKernelObject.h"

namespace Ponykart
{
namespace Levels{ class LevelChangedEventArgs; }
namespace Handlers
{
	/// Just a bunch of extra stuff that needs to go in the scene, such as ambient light, shadow info, a sunlight, etc.
	class SceneEnvironmentHandler : public LKernel::LKernelObject
	{
	public:
		SceneEnvironmentHandler();

	private:
		void onLevelPostLoad(Levels::LevelChangedEventArgs* eventArgs);
	};
}
}

#endif // SCENEENVIRONMENTHANDLER_H_INCLUDED