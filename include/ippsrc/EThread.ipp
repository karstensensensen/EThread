
namespace ETH
{
	template<typename TFunc, typename ...Args>
	EThread::EThread(std::function<TFunc> thread_function, Args ...function_args)
		: m_function(std::bind(thread_function, function_args...))
	{

	}
}
