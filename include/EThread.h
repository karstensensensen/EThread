#pragma once

#include <thread>
#include <functional>

namespace ETH
{

	typedef std::thread::id ETHID;

	// A simple wrapper for a std::tread instance
	// FEATURES:
	// 
	//		allows control over when the thread starts and stops outside of class initialization using start(), restart() and stop functions
	//
	//		auto joins on deconstruction unless ETH_NO_AUTO_JOIN is defined
	//
	// also acts as a base class for all other extended thread types
	//
	class EThread
	{
	public:

		EThread() = default;
		EThread(EThread&& other) noexcept;
		EThread(const EThread& other);
		~EThread();

		template<typename TFunc, typename ... Args>
		EThread(std::function<TFunc> thread_function, Args ... function_args);

		template<typename TFunc, typename ... Args, std::enable_if_t<std::is_function_v<TFunc> && std::is_same_v<std::invoke_result_t<TFunc>, void>, bool> = false>
		EThread(TFunc& function_ptr, Args ... function_args) : EThread(std::function(function_ptr), function_args...) {}

		// for class methods
		template<typename TFunc, typename TClass, typename ... Args, std::enable_if_t<std::is_member_function_pointer_v<TFunc> && std::is_class_v<TClass>, bool> = false>
		EThread(TFunc thread_method, TClass* class_ptr, Args ... function_args) : EThread(std::function<void()>(std::bind(thread_method, class_ptr, function_args...))) {}

		template<typename TFunc, typename ... Args>
		void setThrdFunc(std::function<TFunc> thread_fucntion, Args ... function_args);

		template<typename TFunc, typename ... Args, std::enable_if_t<std::is_function_v<TFunc>&& std::is_same_v<std::invoke_result_t<TFunc>, void>, bool> = false>
		void setThrdFunc(TFunc& thread_fucntion, Args ... function_args) { setThrdFunc(std::function(thread_fucntion), function_args...); }

		// for class methods
		template<typename TFunc, typename TClass, typename ... Args, std::enable_if_t<std::is_member_function_pointer_v<TFunc>&& std::is_class_v<TClass>, bool> = false>
		void setThrdFunc(TFunc thread_method, TClass* class_ptr, Args ... function_args) { setThrdFunc(std::function(std::bind(thread_method, class_ptr, function_args...))); }

		void start();
		void restart();
		void join();

		bool running() { return m_is_running; };
		bool joinable() { return m_thread.joinable(); }

		void detach();
		bool detached() { return m_is_detached; }

		void swap(EThread& other);

		ETHID getId() { return m_thread.get_id(); }

	protected:

		std::thread m_thread;
		std::function<void()> m_function;
		bool m_is_running = false;
		bool m_is_detached = false;
	};
}

#include "include/ippsrc/EThread.ipp"
