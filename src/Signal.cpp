#include "peel/Signal.h"

using namespace peel;


#ifdef _WIN32
Signal::Signal()
{
	m_handle = CreateEvent( 
        NULL,   // default security attributes
        FALSE,  // auto-reset event object
        FALSE,  // initial state is nonsignaled
        NULL);  // unnamed object

	if(m_handle==NULL)
		m_okay = false;

}

Signal::~Signal()
{
	if(m_handle) CloseHandle(m_handle);
	m_handle=NULL;
}

bool Signal::block(DWORD milisecs)
{
	WaitForSingleObject(m_handle, milisecs);
	ResetEvent(m_handle);
	return true;

}

bool Signal::block()
{
	block(INFINITE);
	return true;

}

bool Signal::raise()
{
	SetEvent(m_handle);
	return true;
}

#else

Signal::Signal()
{
	pthread_cond_init(&m_cond, NULL);
	pthread_mutex_init(&m_mutex, NULL); 
	m_okay=true;
}

Signal::~Signal()
{
	pthread_cond_destroy(&m_cond);
}

bool Signal::block(unsigned int millisec)
{
	struct timespec t;
	t.tv_sec = (int)millisec/1000;
	t.tv_nsec = millisec * 1000000;

	pthread_mutex_lock(&m_mutex);
	int ret = pthread_cond_timedwait(&m_cond, &m_mutex, &t);
	pthread_mutex_unlock(&m_mutex);
	if(ret== 0 || ret == ETIMEDOUT)
		return true;
	else
		return false;
}

bool Signal::block()
{

	pthread_mutex_lock(&m_mutex);
	int ret = pthread_cond_wait(&m_cond, &m_mutex);
	pthread_mutex_unlock(&m_mutex);
	if(ret== 0 || ret == ETIMEDOUT)
		return true;
	else
		return false;
}

bool Signal::raise()
{
	if(pthread_cond_broadcast(&m_cond)==0)
		return true;
	else
		return false;
}
#endif

