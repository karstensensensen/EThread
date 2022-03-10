#include "include/LThread.h"
#include <cassert>

namespace ETH
{
	LThread::LThread()
		: EThread(&LThread::loopThread, this)
	{}

	LThread::LThread(LThread&& other) noexcept
		: EThread(std::move(other)), m_loop_function(std::move(other.m_loop_function)) {}

	LThread::LThread(const LThread& other)
		: EThread(other), m_loop_function(other.m_loop_function) {}

	LThread::~LThread()
	{
	#ifndef ETH_NO_AUTO_JOIN
		if (running())
			join();
	#endif
	}

	bool LThread::runningLoop()
	{
		return m_loop_is_running;
	}

	void LThread::swap(LThread& other)
	{
		EThread::swap(static_cast<EThread&>(other));

		{
			// locks must be initiated when swapping data that might be read from the thread
			std::swap(m_loop_function, other.m_loop_function);
			std::swap(m_loop_is_running, other.m_loop_is_running);
		
			std::lock_guard<std::mutex> lock_this(m_mutex);
			std::lock_guard<std::mutex> lock_other(other.m_mutex);

			bool tmp = m_stop;
			m_stop = other.m_stop.load();
			other.m_stop = tmp;
		}
	}

	void LThread::start()
	{
		m_stop = false;
		EThread::start();
	}

	void LThread::restart()
	{
		join();
		start();
	}

	void LThread::join()
	{
		// simply set the stop variable to false, and notify the condition variable so the loop function can exit.
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			m_stop = true;
		}

		m_cv.notify_one();

		join();
	}

	void LThread::startLoop()
	{
	#ifdef ETH_DEBUG
		assert(running());
	#endif

		// set the loop running variable to true and notify the condition variable that it has changed, so the loop thread can start
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			m_loop_is_running = true;
		}

		m_cv.notify_one();
	}

	void LThread::restartLoop()
	{
		joinLoop();
		startLoop();
	}

	void LThread::joinLoop()
	{
	#ifdef ETH_DEBUG
		assert(running());
	#endif
		
		if(!m_loop_is_running)
		{
			// simply wait until loop is running is set to false
			std::unique_lock<std::mutex> lock(m_mutex);

			m_cv.wait(lock, [this] { return !m_loop_is_running; });
		}
	}

	void LThread::loopThread()
	{
		while (!m_stop)
		{
			{
				// start of by waiting until a loop is started
				std::unique_lock<std::mutex> lock(m_mutex);
				m_cv.wait(lock, [this]() { return m_loop_is_running || m_stop; });

				if (m_loop_is_running)
					m_loop_function();

				m_loop_is_running = false;
			}

			// notify, so functions like joinLoop will know the loop has finished
			m_cv.notify_one();
		}
	}
}
