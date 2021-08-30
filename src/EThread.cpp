#include "include/EThread.h"

namespace ETH
{
	EThread::EThread(EThread&& other)
		: m_thread(std::move(other.m_thread)), m_function(std::move(other.m_function)), m_is_running(other.m_is_running)
	{}

	EThread::EThread(const EThread& other)
		: m_function(other.m_function)
	{}

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

		m_thread.join();
	}

	void EThread::swap(EThread& other)
	{
		std::swap(m_function, other.m_function);
	}
}
