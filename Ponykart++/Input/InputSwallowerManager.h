#ifndef INPUTSWALLOWER_H_INCLUDED
#define INPUTSWALLOWER_H_INCLUDED

#include <functional>
#include <vector>
#include "Kernel/LKernelObject.h"

namespace Ponykart
{
namespace Input
{

/// "Swallowing" input is when we only give input to one particular thing. For example, if we have WASD as movement keys,
/// if we have a text box come up asking us for our name or something, we don't want to start moving while we're doing that.
/// We would say that text box has "swallowed" the input. This class manages that.
/// 
/// Sexual references ahoy.
/// 
/// Oh and pausing also swallows input. Might want to do something about that later.
class InputSwallowerManager
{
public:
	InputSwallowerManager(){};
	void addSwallower(std::function<bool()> condition, LKernel::LKernelObject* swallower);
	bool isSwallowed(LKernel::LKernelObject* querier);
	bool isSwallowed();

private:
	std::vector<std::pair<std::function<bool()>, LKernel::LKernelObject*>> thingsToCheck;
};


} // Input
} // Ponykart
#endif // INPUTSWALLOWER_H_INCLUDED
