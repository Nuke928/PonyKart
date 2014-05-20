#ifndef LKERNELHANDLER_H_DEFINED
#define LKERNELHANDLER_H_DEFINED

#include "Levels/Level.h"

namespace Ponykart
{
namespace LKernel
{
	void loadLevelHandlers(Levels::Level& newLevel); ///< Load handlers for each level
	void unloadLevelHandlers(); ///< Unload and dispose of the special per-level handlers. This is run before the regular OnLevelUnload event.
	void loadGlobalHandlers(); ///< Load global handlers
}
}

#endif // LKERNELHANDLER_H_DEFINED