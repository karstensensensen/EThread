#pragma once

#include "include/EThread.h"

#include <atomic>
#include <mutex>
#include <condition_variable>

namespace ETH
{

	/// @brief an extended thread that allows the same start and stop functionality as ETHread, but without actually constructing and deconstructing a new thread every time
	/// FEATURES:
	/// 
	///		allows control over when the thread starts and stops and loops using start(), restart(), stop() for the main thread, and startLoop(), restartLoop() and joinLoop() for the function to be looped.
	///
	///		auto joins on deconstruction unless ETH_NO_AUTO_JOIN is defined
	///
	class LThread: protected EThread
	{
	public:
		LThread();
		LThread(LThread && other) noexcept;
		LThread(const LThread & other);
		~LThread();

		/// @see EThread(std::function<TFunc>, Args ...)
		template<typename TFunc, typename ... Args>
		LThread(std::function<TFunc> thread_function, Args ... function_args);

		/// @see EThread(TFunc&, Args ...)
		template<typename TFunc, typename ... Args, std::enable_if_t<std::is_pointer_v<TFunc> && std::is_function_v<typename std::remove_pointer_t<TFunc>> && std::is_same_v<std::invoke_result_t<TFunc>, void>, bool> = false>
		LThread(TFunc function_ptr, Args ... function_args) : LThread(std::function(function_ptr), function_args...) {}
		/// @brief specialization for static methods
		template<typename TFunc, typename TClass, typename ... Args, std::enable_if_t<std::is_pointer_v<typename TClass::TFunc> && std::is_function_v<typename std::remove_pointer_t<typename TClass::TFunc>> && std::is_same_v<std::invoke_result_t<typename TClass::TFunc, TClass>, void>, bool> = false>
		LThread(typename TClass::TFunc function_ptr, Args ... function_args) : LThread(std::function(function_ptr), function_args...) {}

		/// @see EThread(TFunc&, TClass*, Args ...)
		template<typename TFunc, typename TClass, typename ... Args, std::enable_if_t<std::is_member_function_pointer_v<TFunc> && std::is_class_v<TClass>, bool> = false>
		LThread(TFunc thread_method, TClass* class_ptr, Args ... function_args) : LThread(std::function<void()>(std::bind(thread_method, class_ptr, function_args...))) {}

		
		/// @see EThread::setThrdFunc(std::function<TFunc>, Args ...)
		template<typename TFunc, typename ... Args>
		void setThrdFunc(std::function<TFunc> thread_fucntion, Args ... function_args);
		
		/// @see EThread::setThrdFunc(TFunc&, Args ...)
		template<typename TFunc, typename ... Args, std::enable_if_t<std::is_pointer_v<TFunc> && std::is_function_v<typename std::remove_pointer_t<TFunc>> && std::is_same_v<std::invoke_result_t<TFunc>, void>, bool> = false>
		void setThrdFunc(TFunc thread_function, Args ... function_args) { setThrdFunc(std::function(thread_function), function_args...); }
		
		/// @see EThread::setThrdFunc(TFunc&, TClass*, Args ...)
		template<typename TFunc, typename TClass, typename ... Args, std::enable_if_t<std::is_member_function_pointer_v<TFunc>&& std::is_class_v<TClass>, bool> = false>
		void setThrdFunc(TFunc thread_method, TClass * class_ptr, Args ... function_args) { setThrdFunc(std::function(std::bind(thread_method, class_ptr, function_args...))); }

		/// @brief initializes the loop thread, but does not call the loop function.
		/// this function must be called before the loop is attempted to be started, as this controls the underlyng thread that runs the loop function.
		/// @see EThread::start()
		void start();
		/// @see EThread::restart()
		void restart();
		/// @see EThread::join()
		void join();

		using EThread::running;
		using EThread::joinable;

		/// @brief calls the current thread function in the already running thread.
		/// requirements: is running
		void startLoop();
		/// @brief waits until the current loop is finished, and then start a new one.
		void restartLoop();
		/// @brief wait until the current loop is finished.  
		/// in contrast to join(), this function can be called even when the loop has finished.
		void joinLoop();

		/// @brief returns wether a loop is currently being processed
		bool runningLoop();

		/// @see EThread::swap
		void swap(LThread & other);

		using EThread::getId;

	protected:

		/// @brief the underlying thread function that runs the loop function
		void loopThread();

		std::function<void()> m_loop_function;
		std::mutex m_mutex;
		std::condition_variable m_cv;

		bool m_loop_is_running = false;
		std::atomic<bool> m_stop = true;
	};
}

#include "include/ippsrc/LThread.ipp"
