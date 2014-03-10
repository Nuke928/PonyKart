#ifndef EVENTEXTENSIONS_H_INCLUDED
#define EVENTEXTENSIONS_H_INCLUDED

#include <utility>
#include <atomic>
#include <unordered_map>

namespace Extensions
{

template <typename... TParams> class EventDelegate;

struct EventSubscriptionToken
{
public:
	EventSubscriptionToken ();
	EventSubscriptionToken (const EventSubscriptionToken &other) = default;

	bool operator== (const EventSubscriptionToken other) const;

private:
	static std::atomic_int nextKey;

	static EventSubscriptionToken getNext ();

	int key;


	template <typename... TParams> friend class EventDelegate;
	template <typename T> friend struct std::hash;
};


template <typename... TParams> class EventDelegate
{
public:
	EventDelegate () { }
	EventDelegate (const EventDelegate &other) = default;
	EventDelegate (EventDelegate &&other) = default;


	EventDelegate &operator= (const EventDelegate &other)
	{
		handlers = other.handlers;
	}
	EventDelegate &operator= (EventDelegate &&other)
	{
		handlers = std::move(other.handlers);
	}


	EventSubscriptionToken subscribe (std::function<void (TParams... eventArgs)> handler)
	{
		auto key = EventSubscriptionToken::getNext();
		handlers.insert(std::make_pair(key, handler));
		return key;
	}
	bool unsubscribe (EventSubscriptionToken key)
	{
		return handlers.erase(key);
	}


	void operator() (TParams... eventArgs)
	{
		if (handlers.size())
			for (auto h : handlers)
				h.second(std::forward<TParams>(eventArgs)...);
	}


	bool empty ()
	{
		return handlers.size() > 0;
	}

private:
	std::unordered_map<EventSubscriptionToken, std::function<void (TParams... eventArgs)>> handlers;
};


} // Extensions


namespace std
{
	template <> struct hash<Extensions::EventSubscriptionToken>
	{
	public:
		size_t operator() (Extensions::EventSubscriptionToken token) const {
			return std::hash<int>()(token.key);
		}
	};
} // std

#endif // EVENTEXTENSIONS_H_INCLUDED
