#include "include/LThread.h"

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
			stop();
	#endif
	}

	void LThread::swap(LThread& other)
	{
		EThread::swap(*(EThread*)&other);

		std::swap(m_loop_function, other.m_loop_function);
		std::swap(m_loop_is_running, other.m_loop_is_running);
		std::swap(m_stop_joining, other.m_stop_joining);
		
		{
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
		stop();
		start();
	}

	void LThread::stop()
	{
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			m_stop = true;
		}

		m_cv.notify_one();

		join();
	}

	void LThread::startLoop()
	{
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
		if (m_loop_is_running)
		{
			std::unique_lock<std::mutex> lock(m_mutex);

			m_cv.wait(lock, [this] { return m_stop_joining; });
			m_stop_joining = false;
		}
	}

	void LThread::loopThread()
	{
		std::unique_lock<std::mutex> lock(m_mutex);

		m_cv.notify_one();
		while (!m_stop)
		{
			m_cv.wait(lock, [this]() { return m_loop_is_running || m_stop; });

			if (m_loop_is_running)
				m_loop_function();

			m_loop_is_running = false;
			m_stop_joining = true;
			m_cv.notify_one();
		}
	}
}
