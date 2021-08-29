#include "include/EThread.h"

namespace ETH
{
	LoopThread::LoopThread()
		: EThread(&LoopThread::lThread, this)
	{}

	LoopThread::LoopThread(LoopThread&& other) noexcept
		: EThread(std::move(other)), m_loop_function(std::move(other.m_loop_function)) {}

	LoopThread::LoopThread(const LoopThread& other)
		: EThread(other), m_loop_function(other.m_loop_function) {}

	LoopThread::~LoopThread()
	{
		if (started())
			stop();
	}

	void LoopThread::swap(LoopThread& other)
	{
		EThread::swap(static_cast<EThread>(other));

		std::swap(m_loop_function, other.m_loop_function);
		std::swap(m_ready_to_run_loop, other.m_ready_to_run_loop);
		std::swap(m_stop_joining, other.m_stop_joining);
		
		bool tmp = m_stop;
		m_stop = other.m_stop.load();
		other.m_stop = tmp;
	}

	void LoopThread::start()
	{
		m_stop = false;
		EThread::start();
	}

	void LoopThread::restart()
	{
		stop();
		start();
	}

	void LoopThread::stop()
	{
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			m_stop = true;
		}

		m_cv.notify_one();

		join();
	}

	void LoopThread::startLoop()
	{
		{
			std::lock_guard<std::mutex> lock(m_mutex);

			m_ready_to_run_loop = true;
		}

		m_cv.notify_one();
	}

	void LoopThread::restartLoop()
	{
		joinLoop();
		startLoop();
	}

	void LoopThread::joinLoop()
	{
		if (m_ready_to_run_loop)
		{
			std::unique_lock<std::mutex> lock(m_mutex);

			m_cv.wait(lock, [this] { return m_stop_joining; });
			m_stop_joining = false;
		}
	}

	void LoopThread::lThread()
	{
		std::unique_lock<std::mutex> lock(m_mutex);

		m_cv.notify_one();
		while (!m_stop)
		{
			m_cv.wait(lock, [this]() { return m_ready_to_run_loop || m_stop; });

			if (m_ready_to_run_loop)
				m_loop_function();

			m_ready_to_run_loop = false;
			m_stop_joining = true;
			m_cv.notify_one();
		}
	}
}
