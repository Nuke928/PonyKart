#include "pch.h"
#include "Input/InputSwallowerManager.h"
#include "Kernel/LKernel.h"

using namespace std;
using namespace Ponykart::LKernel;
using namespace Ponykart::Input;

/// Add an object that has something that can be swallowed. If it's part of a UI, this should be the class that manages that UI thingy.
/// The function should be read as "if this is true, then swallow the input".
/// @param condition A condition for when the input should be swallowed. If this is true, it is swallowed.
/// @param swallower The object that "manages" the thing doing the swallowing. E.g if the swallower is part of the UI, this should be that UI's manager.
void InputSwallowerManager::addSwallower(function<bool()> condition, LKernelObject* swallower)
{
	thingsToCheck.push_back(pair<function<bool()>, LKernelObject* >(condition, swallower));
}

/// Is the current input swallowed or not, with respect to me?
/// @param querier For the most part, just use the keyword "this". The thing asking whether the input is swallowed or not. This is needed because otherwise nothing would be able to do anything if the input was swallowed. So when we're checking the conditions, we excude any that are managed by the querier.
/// @return Returns true if the input is swallowed by something else, false otherwise.
bool InputSwallowerManager::isSwallowed(LKernelObject* querier)
{
	bool result = false;
	// we go through our conditions to check
	for (pair<function<bool()>, LKernelObject*> thing : thingsToCheck)
	{
		// and we don't count things that are managed by the querier
		if (thing.second == querier)
			continue;
		// We OR the conditions with the result. If any of the conditions are true, the input is swallowed.
		result |= thing.first();
	}
	return result;
}

/// Is the current input swallowed or not? This method is the same as IsSwallowed(object) but without the extra condition.
/// This should be used by objects that do not swallow input themselves.
/// @return Returns true if the input is swallowed by something, false otherwise.
bool InputSwallowerManager::isSwallowed()
{
	bool result = false;
	// we go through our conditions to check
	for (pair<function<bool()>, LKernelObject*> thing : thingsToCheck)
		result |= thing.first(); // If any of the conditions are true, the input is swallowed.
	return result;
}
