#pragma once

#include <thread>
#include <functional>

namespace ETH
{
	// A simple wrapper for a std::tread instance
	// allows control over when the thread starts and stops outside of class initialization
	// also acts as a base class for all other extended thread types
	class EThread
	{
	public:

		EThread() = default;
		EThread(EThread&& other);
		EThread(const EThread& other);

		template<typename TFunc, typename ... Args>
		EThread(std::function<TFunc> thread_function, Args ... function_args);

		template<typename TFunc, typename ... Args, std::enable_if_t<std::is_function_v<TFunc> && std::is_same_v<std::invoke_result_t<TFunc>, void>, bool> = false>
		EThread(TFunc& function_ptr, Args ... args) : EThread(std::function(function_ptr), args...) {}

		// for class methods
		template<typename TFunc, typename TClass, typename ... Args, std::enable_if_t<std::is_member_function_pointer_v<TFunc> && std::is_class_v<TClass>, bool> = false>
		EThread(TFunc thread_method, TClass* class_ptr, Args ... args) : EThread(std::function(std::bind(thread_method, class_ptr, args...))) {}


		void start();
		void restart();
		void join();

		bool started();

		void swap(EThread& other);

	protected:
		
		std::thread m_thread;
		std::function<void()> m_function;
		bool m_is_running = false;
	};
}

#include "include/ippsrc/EThread.ipp"
