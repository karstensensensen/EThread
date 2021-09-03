#pragma once

#include "include/EThread.h"

#include <mutex>
#include <condition_variable>

namespace ETH
{

	// an extended thread that allows the same start and stop functionality as ETHread, but without actually constructing and deconstructing a new thread every time
	// FEATURES:
	// 
	//		allows control over when the thread starts and stops and loops using start(), restart(), stop() for the main thread, and startLoop(), restartLoop() and joinLoop() for the function to be looped.
	//
	//		auto joins on deconstruction unless ETH_NO_AUTO_JOIN is defined
	//
	class LThread: protected EThread
	{
	public:
		LThread();
		LThread(LThread && other) noexcept;
		LThread(const LThread & other);
		~LThread();

		template<typename TFunc, typename ... Args>
		LThread(std::function<TFunc> thread_function, Args ... function_args);

		template<typename TFunc, typename ... Args, std::enable_if_t<std::is_function_v<TFunc>&& std::is_same_v<std::invoke_result_t<TFunc>, void>, bool> = false>
		LThread(TFunc & function_ptr, Args ... function_args) : LThread(std::function(function_ptr), function_args...) {}

		// for class methods
		template<typename TFunc, typename TClass, typename ... Args, std::enable_if_t<std::is_member_function_pointer_v<TFunc> && std::is_class_v<TClass>, bool> = false>
		LThread(TFunc thread_method, TClass* class_ptr, Args ... function_args) : LThread(std::function<void()>(std::bind(thread_method, class_ptr, function_args...))) {}

		template<typename TFunc, typename ... Args>
		void setThrdFunc(std::function<TFunc> thread_fucntion, Args ... function_args);

		template<typename TFunc, typename ... Args, std::enable_if_t<std::is_function_v<TFunc>&& std::is_same_v<std::invoke_result_t<TFunc>, void>, bool> = false>
		void setThrdFunc(TFunc & thread_function, Args ... function_args) { setThrdFunc(std::function(thread_function), function_args...); }

		// for class methods
		template<typename TFunc, typename TClass, typename ... Args, std::enable_if_t<std::is_member_function_pointer_v<TFunc>&& std::is_class_v<TClass>, bool> = false>
		void setThrdFunc(TFunc thread_method, TClass * class_ptr, Args ... function_args) { setThrdFunc(std::function(std::bind(thread_method, class_ptr, function_args...))); }

		void start();
		void restart();
		void stop();

		using EThread::running;

		void startLoop();
		void restartLoop();
		void joinLoop();


		bool runningLoop();

		void swap(LThread & other);

		using EThread::getId;

	protected:

		void loopThread();

		std::function<void()> m_loop_function;
		std::mutex m_mutex;
		std::condition_variable m_cv;

		bool m_loop_is_running = false;
		bool m_stop_joining = false;
		std::atomic<bool> m_stop = true;
	};
}

#include "include/ippsrc/LThread.ipp"
