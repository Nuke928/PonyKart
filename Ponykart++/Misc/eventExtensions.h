#ifndef EVENTEXTENSIONS_H_INCLUDED
#define EVENTEXTENSIONS_H_INCLUDED

namespace Extensions
{

template<typename... T> using EventDelegate = typename std::vector<std::function<void (T... eventArgs)>>;

template<typename... T> void fireEvent(EventDelegate<T...> handler, T... eventArgs)
{
	if (handler.size())
		for (auto fun : handler)
			fun(eventArgs...);
}

} // Extensions

#endif // EVENTEXTENSIONS_H_INCLUDED
