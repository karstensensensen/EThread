#include "include/LThread.h"

namespace ETH
{
	template<typename TFunc, typename ...Args>
	LThread::LThread(std::function<TFunc> thread_function, Args ... function_args)
		: EThread(&LThread::loopThread, this), m_loop_function(std::bind(thread_function, function_args...))
	{}

	template<typename TFunc, typename ...Args>
	void LThread::setThrdFunc(std::function<TFunc> thread_fucntion, Args ... function_args)
	{
		m_loop_function = std::bind(thread_fucntion, function_args...);
	}
}
