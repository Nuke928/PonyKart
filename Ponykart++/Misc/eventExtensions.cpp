#include "eventExtensions.h"

using namespace std;
using namespace Extensions;


atomic_int EventSubscriptionToken::nextKey(0);


EventSubscriptionToken::EventSubscriptionToken ()
{
	key = -1;
}


bool EventSubscriptionToken::operator== (const EventSubscriptionToken other) const
{
	return key == other.key;
}


EventSubscriptionToken EventSubscriptionToken::getNext ()
{
	auto token = EventSubscriptionToken();
	token.key = nextKey++;
}

