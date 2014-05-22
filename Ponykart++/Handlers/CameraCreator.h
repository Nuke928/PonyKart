#ifndef CAMERACREATOR_H_INLUDED
#define CAMERACREATOR_H_INLUDED

#include "Kernel/LKernelObject.h"

namespace Ponykart
{
namespace Handlers
{
	/// Makes a player camera at the beginning of a race level.
	class CameraCreatorHandler : public LKernel::LKernelObject
	{
	public:
		CameraCreatorHandler();
	};
}
}

#endif // CAMERACREATOR_H_INLUDED