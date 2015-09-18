#ifndef ___PEEL_THREAD_H___
#define ___PEEL_THREAD_H___

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <pthread.h>
#endif

namespace peel
{
	//! Cross Platform thread class.  
	/*! Subclass this and implement the operator() function with the work that needs to be done.
 	    Thread is determined to be running so long as m_handle is not null.  When the thread
	    completes m_running is set to null.
	*/
	class Thread
	{
	public:
		virtual ~Thread() {};

#ifdef _WIN32
		//! Create the thread object with default (null) values
		Thread() : m_threadId(0), m_handle(NULL)
		{	
		};

		//! Start the thread
		virtual bool start()
		{
			m_handle =  CreateThread(NULL, 0, Thread::Proc, this, 0, &m_threadId);
			return m_handle != NULL;
		}

		//! Terminate the thread.
		bool terminate()
		{
			TerminateThread(m_handle, 0);
			return true;
		}

		//! Wait for the thread to finish
		bool join()
		{
			WaitForSingleObject(m_handle, INFINITE);
			return true;
		}

		//! returns true if the thread is still running.
		bool isRunning()
		{
			return m_handle != NULL;
		}

	private:
		DWORD  m_threadId;  //!< Windows Id for this thread.
		HANDLE m_handle;    //!< Handle to thread object

		//! Internal function used to start thread
		static DWORD WINAPI Proc( LPVOID lpParam )
		{
			Thread *t = (Thread*)lpParam;
			t->operator ()();
			CloseHandle(t->m_handle);
			t->m_handle=NULL;
			return 0;
		}

#else
		//! Create the thread object with default (null) values
		Thread() :  m_err(0), m_handle(0)
		{};

		// Start the thread
		bool start()
		{
			m_err = pthread_create( &m_handle, NULL, Thread::Proc, (void*) this);
			return m_err == 0;
		};

		//! Block till thread completes
		bool join()
		{
			return pthread_join( m_handle, NULL) == 0;
		}

		//! see pthread_detach - unix only
		bool detach()
		{
			return pthread_detach( m_handle ) == 0;
		}

		//! Terminate the thread - threads running state is left undetermined.
		bool terminate()
		{
			if(m_handle == 0) return false;
			pthread_cancel(m_handle);
			return true;
		};

		//! returns true if the thread is still running - don't call after terminate()
		bool isRunning()
		{
			return m_handle != 0;
		}

		bool      m_err;

	private:

		//! Internal function used for starting thread
		static void* Proc(void *param)
		{
			if(param == NULL) return NULL;
			Thread *t = (Thread*)param;
			t->operator ()();
			t->m_handle = 0;
			return NULL;
		}

		pthread_t m_handle;  //!< pthread handle to thread object
#endif

	protected:

		//! Virtual execute funtion.  Subclass this with the work that needs to be done.
		virtual void operator ()(void) = 0;


	};

}
#endif
