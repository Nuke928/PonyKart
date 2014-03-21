#include "pch.h"
#include "Input/InputSwallower.h"
#include "Kernel/LKernel.h"
#include "Input/KeyBindingManager.h"

using namespace std;
using namespace Ponykart::Input;


InputSwallower::InputSwallower ()
{
	LKernel::getG<KeyBindingManager>()->suppressInput();
}
InputSwallower::InputSwallower (const InputSwallower &other)
{
	LKernel::getG<KeyBindingManager>()->suppressInput();
}
InputSwallower::~InputSwallower ()
{
	LKernel::getG<KeyBindingManager>()->allowInput();
}

