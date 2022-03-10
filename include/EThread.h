#pragma once

#include <thread>
#include <functional>

namespace ETH
{

	typedef std::thread::id ETHID;

	/// @brief A simple wrapper for a std::thread instance
	/// FEATURES:
	/// 
	///		allows control over when the thread starts and stops outside of class initialization using start(), restart() and stop functions
	///
	///		auto joins on deconstruction unless ETH_NO_AUTO_JOIN is defined
	///
	/// also acts as a base class for all other extended thread types
	///
	class EThread
	{
	public:

		EThread() = default;
		EThread(EThread&& other) noexcept;
		EThread(const EThread& other);
		~EThread();

		/// @brief constructor, takes the function and its arguments that should be started in a new thread
		template<typename TFunc, typename ... Args>
		EThread(std::function<TFunc> thread_function, Args ... function_args);

		/// @brief constructor, same as EThread(std::function<TFunc>, Args ...), but the function argument is a function pointer
		template<typename TRet, typename ... TFArgs, typename ... Args, std::enable_if_t<std::is_pointer_v<TRet(*)(TFArgs...)>&& std::is_function_v<typename std::remove_pointer_t<TRet(*)(TFArgs...)>>&& std::is_same_v<TRet, void>, bool> = false>
		EThread(TRet(*function_ptr)(TFArgs...), Args ... function_args) : EThread(std::function(function_ptr), function_args...) {}

		/// @brief ETHread(TFunc&, Args ...) for class methods
		template<typename TFunc, typename TClass, typename ... Args, std::enable_if_t<std::is_member_function_pointer_v<TFunc> && std::is_class_v<TClass>, bool> = false>
		EThread(TFunc thread_method, TClass* class_ptr, Args ... function_args) : EThread(std::function<void()>(std::bind(thread_method, class_ptr, function_args...))) {}

		/// @brief sets the function that will be started as a seperate thread
		template<typename TFunc, typename ... Args>
		void setThrdFunc(std::function<TFunc> thread_fucntion, Args ... function_args);
		/// @brief setThrdFunc(std::function<TFunc>, Args ...) but for function pointers
		template<typename TFunc, typename ... Args, std::enable_if_t<std::is_pointer_v<TFunc> && std::is_function_v<typename std::remove_pointer_t<TFunc>> && std::is_same_v<std::invoke_result_t<TFunc>, void>, bool> = false>
		void setThrdFunc(TFunc thread_fucntion, Args ... function_args) { setThrdFunc(std::function(thread_fucntion), function_args...); }
		/// @brief setThrdFunc(TFunc&, Args ...) but for methods
		template<typename TFunc, typename TClass, typename ... Args, std::enable_if_t<std::is_member_function_pointer_v<TFunc>&& std::is_class_v<TClass>, bool> = false>
		void setThrdFunc(TFunc thread_method, TClass* class_ptr, Args ... function_args) { setThrdFunc(std::function(std::bind(thread_method, class_ptr, function_args...))); }

		/// @brief starts the current thread function as a seperate thread
		/// requirements: must not have a thread currently running
		void start();
		/// @brief joins and starts the current thread function as a seperate thread
		/// requirements: must be running
		void restart();
		/// @see [std::thread::join](https://en.cppreference.com/w/cpp/thread/thread/join)
		void join();

		/// @brief returns wether the EThread is currently running.
		/// this is purely based on wether the thread is started and not wether it has finished processing.
		/// For this functionality instead, see joinable().
		/// @return 
		bool running();
		/// @see [std::thread::joinable](https://en.cppreference.com/w/cpp/thread/thread/joinable)
		bool joinable();

		/// @see [std::thread::detach](https://en.cppreference.com/w/cpp/thread/thread/detach)
		void detach();
		/// @brief returns wether the thread is detached or not.
		/// @see detach
		bool detached() { return m_is_detached; }

		/// @brief swaps the current thread function with the passed EThreads thread function.
		void swap(EThread& other);

		/// @see [std::thread::get_id](https://en.cppreference.com/w/cpp/thread/thread/get_id)
		ETHID getId();

	protected:

		std::function<void()> m_function;
		std::thread m_thread;

		bool m_is_running = false;
		bool m_is_detached = false;
	};
}

#include "include/ippsrc/EThread.ipp"
