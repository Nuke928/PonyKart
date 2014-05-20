#ifndef LKERNELCLEANUP_H_DEFINED
#define LKERNELCLEANUP_H_DEFINED

namespace Ponykart
{
namespace LKernel
{
	void cleanup(); ///< This is run just after the OnLevelUnload event.
	void remakeSceneManager(); ///< Destroys everything in the scene manager so it's as good as new without destroying the scene manager itself
}
}

#endif // LKERNELCLEANUP_H_DEFINED