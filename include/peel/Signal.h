#ifndef ___PEELSIGNAL_H_
#define ___PEELSIGNAL_H_

#include <vector>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#pragma warning ( disable: 4996 )
#else
#include <pthread.h>
#include <errno.h>
#endif


namespace peel
{

//! Simple blocking function
/*! This can be used to block one thread which is then unblocked (singalled) by another thread
 * On windows it uses CreateEvent and WaitForSingleObject 
 * On Unit it uses prthread_cont and pthread_mutex functions
 */
class Signal
{
public:
	Signal();  //!< Creates internal mutex object (handle)
	~Signal(); //!< Destroys internal mutex object (handle)

	bool block(); //!< Stop execution until another thread calls raise
	bool raise(); //!< Tell the block elsewhere to continue

	//! confirm the mutex was created okay
	inline bool okay() { return m_okay; };
private:

	bool m_okay;  //!< false if the mutex was not able to be created
#ifdef _WIN32
	bool block(DWORD millisec);  //!< block for a numer of milliseconds, or INFINITE
	HANDLE m_handle;             //!< mutex handle
#else
	bool block(unsigned int milisec);
	pthread_cond_t  m_cond;
	pthread_mutex_t m_mutex;
#endif
};


} // namespace peel

#endif



