#include "include/EThread.h"
#include <cassert>

namespace ETH
{
	EThread::EThread(EThread&& other) noexcept
		: m_function(std::move(other.m_function)), m_thread(std::move(other.m_thread)), m_is_running(other.m_is_running)
	{}

	EThread::EThread(const EThread& other)
		: m_function(other.m_function)
	{}

	EThread::~EThread()
	{
	#ifndef ETH_NO_AUTO_JOIN
		if (running() && !detached())
			join();
	#endif
	}

	void EThread::start()
	{
	#ifdef ETH_DEBUG
		assert(running());
	#endif

		m_is_running = true;
		m_thread = std::thread(m_function);
	}

	void EThread::restart()
	{
	#ifdef ETH_DEBUG
		assert(running());
	#endif
		
		if(joinable())
			join();
		
		start();
	}

	void EThread::join()
	{
	#ifdef ETH_DEBUG
		assert(joinable() && running());
	#endif

		m_is_running = false;
		m_thread.join();
	}

	bool EThread::running()
	{
		return m_is_running;
	}

	bool EThread::joinable()
	{
		return m_thread.joinable();
	}

	void EThread::detach()
	{
	#ifdef ETH_DEBUG
		assert(running());
	#endif

		m_is_detached = true;
		m_thread.detach();
	}

	void EThread::swap(EThread& other)
	{
		std::swap(m_function, other.m_function);
	}

	ETHID EThread::getId()
	{
		return m_thread.get_id();
	}
}
