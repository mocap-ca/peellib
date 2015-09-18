#ifndef PEELLIB_NTSERVICE_LOCK_H__
#define PEELLIB_NTSERVICE_LOCK_H__

#include <stdlib.h>
//#include <time.h>
#ifndef _WIN32
#include <sys/time.h>
#endif
//! Implements a local locking mechinism

// LockMutex       - provides the locking mechinism
// Lock            - Locks a thread using aa LockMutex object
// Condition mutex - provides a locking mechinism that can be triggered as a condition
// Condition       - provides a wait for a specified condition

namespace peel
{
#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
	
	/* WINDOWS */

	//! Provides a local lock, using Critical section
	class LockMutex
	{
	public:
		LockMutex()  { InitializeCriticalSection(&m_cs); }
		~LockMutex() { DeleteCriticalSection (&m_cs);    }
		CRITICAL_SECTION m_cs; 
	};

	//! Provides a local lock
	class Lock 
	{
	public:
		Lock(LockMutex *m) : m_mutex(m) { EnterCriticalSection(&m->m_cs); }
		~Lock()                         { LeaveCriticalSection(&m_mutex->m_cs); };
		LockMutex *m_mutex;
	};

	//! Provides a lock that can be turned off and on
	class LockSwitch
	{
	public:
		LockSwitch(LockMutex *m) : m_mutex(m), isOn(false) {}
		~LockSwitch() { if(isOn) off(); }
		void on() { EnterCriticalSection(&m_mutex->m_cs); isOn=true; }
		void off() { isOn=false; LeaveCriticalSection(&m_mutex->m_cs); }
		LockMutex *m_mutex;
		bool isOn;
	};


	//! Creates a handle which can be signalled
	class ConditionMutex
	{
	public:
		ConditionMutex() { m_handle = CreateEvent( NULL, FALSE, FALSE, NULL); }
		~ConditionMutex() { if(m_handle) CloseHandle(m_handle); }
		HANDLE m_handle;
	};


	class Condition 
	{
	public: 
		Condition(ConditionMutex *m) : m_myMutex(NULL), m_mutex(m) {};
		Condition()
		{
			m_myMutex = new ConditionMutex();
			m_mutex = m_myMutex;
		}
		~Condition() { if(m_myMutex) delete m_myMutex; }

		void wait()
		{
			WaitForSingleObject(m_mutex->m_handle, INFINITE);
			ResetEvent(m_mutex->m_handle);
		};

		void wait(DWORD milliseconds)
		{
			WaitForSingleObject(m_mutex->m_handle, milliseconds);
			ResetEvent(m_mutex->m_handle);
		}


		ConditionMutex *m_myMutex;
		ConditionMutex *m_mutex;

		void notify()
		{
			SetEvent(m_mutex->m_handle);
		};
	};
#else

	/* UNIX */

	#include <pthread.h>

	//! Provides a local lock, using Critical section
	class LockMutex
	{
	public:
		LockMutex()  { pthread_mutex_init(&m_mt, NULL); }
		~LockMutex() { pthread_mutex_destroy(&m_mt); }
		pthread_mutex_t m_mt;
	};

	//! Provides a local lock
	class Lock 
	{
	public:
		Lock(LockMutex *m) : m_mutex(m) { pthread_mutex_lock(&m->m_mt); };
		~Lock()                         { pthread_mutex_unlock(&m_mutex->m_mt); };
		LockMutex *m_mutex;
	};

	//! Provides a lock that can be turned off and on
	class LockSwitch
	{
	public:
		LockSwitch(LockMutex *m) : m_mutex(m) {};
		~LockSwitch() { if(isOn) off(); }
		void on() { pthread_mutex_lock(&m_mutex->m_mt); isOn = true; }
		void off() { isOn=false; pthread_mutex_unlock(&m_mutex->m_mt); }
		LockMutex *m_mutex;
		bool isOn;
	};


	//! Creates a handle which can be signalled
	class ConditionMutex
	{
	public:
		ConditionMutex()
		{
			pthread_mutex_init(&m_mt, NULL);
			pthread_cond_init(&m_cond, NULL);
		}
		~ConditionMutex()
		{
			pthread_mutex_destroy(&m_mt);
			pthread_cond_destroy(&m_cond);
		}
		pthread_mutex_t m_mt;
		pthread_cond_t  m_cond;

	};


	//! Creates a wait condition that can be blocked on a signal
	class Condition
	{
	public:
		Condition(ConditionMutex *m) : m_myMutex(NULL), m_mutex(m) {};
		Condition()
		{
			m_myMutex = new ConditionMutex();
			m_mutex = m_myMutex;
		}
		~Condition() { if(m_myMutex) delete m_myMutex; }

		void wait()
		{
			pthread_mutex_lock(&m_mutex->m_mt);
			pthread_cond_wait(&m_mutex->m_cond, &m_mutex->m_mt);
			pthread_mutex_unlock(&m_mutex->m_mt);
		};

		void wait(const struct timespec *t)
		{
			pthread_mutex_lock(&m_mutex->m_mt);
			pthread_cond_timedwait(&m_mutex->m_cond, &m_mutex->m_mt, t);
			pthread_mutex_unlock(&m_mutex->m_mt);
		}

		void wait(int milliseconds)
		{
			struct timespec t;
			struct timeval  tv;
			gettimeofday(&tv, NULL);
			// removed on osx - clock_gettime(CLOCK_REALTIME, &t);
			t.tv_sec = tv.tv_sec + milliseconds / 1000;
			t.tv_nsec = milliseconds % 1000 * 1000000;
			this->wait(&t);
		}

		ConditionMutex* m_myMutex;
		ConditionMutex* m_mutex;

		void notify()
		{
			// Set the condition
			pthread_mutex_lock(&m_mutex->m_mt);
			pthread_cond_signal(&m_mutex->m_cond);
			pthread_mutex_unlock(&m_mutex->m_mt);
		};
	};


#endif // Windows or not

} // namespace peel

#endif  //PEELLIB_NTSERVICE_LOCK_H__
