#include "include/EThread.h"

namespace ETH
{
	template<typename TFunc, typename ...Args>
	EThread::EThread(std::function<TFunc> thread_function, Args ...function_args)
		: m_function(std::bind(thread_function, function_args...))
	{

	}
	
	template<typename TFunc, typename ...Args>
	void EThread::setThrdFunc(std::function<TFunc> thread_fucntion, Args ...function_args)
	{
		m_functoin = std::bind(thread_functoin, functoin_args...);
	}
}
