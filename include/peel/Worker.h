#ifndef ___PEELLIB_WORKER_H__
#define ___PEELLIB_WORKER_H__

#ifdef _WIN32
#pragma warning ( disable : 4996 )
#endif

//#include "boost/thread.hpp"
#include "Lock.h"
#include <deque>

#include "WorkerCommand.h"

namespace peel
{



//! A queue based worker thread
/*!
	The worker thread keeps a queue of commands and processes them in turn.  If the queue empties, 
	the worker will block until a new entry is added.  This makes it easier to syncronize events
	and avoids having to scatter blocking calls.  Any common memory should only be read or modified
	by commands, to ensure the reading and writing is syncronized correctly.  
	<P>
	Subclass WorkerCommand with each command type and implement the function execute(WorkerCommand*)
	in the subclass of Worker.  The execute method will recieve the commands in the order they are
	added.
	<P>
	If two threads add a series of commands, it is possible that the two commands are interwoven,
	so do not assume that commands will be run exactly one after the other, i.e. each execution
	should be atomic.
*/

class Worker : public Condition
{
public:
	typedef enum { INIT, RUNNING, TERM, PAUSE, END } TStatus;
	TStatus mStatus;

protected:

	std::deque<WorkerCommand*> mQueue;        //!< The queue of commands to execute

	//!< Used to maintain concurrency 
	/* Keeps calls to accessing the queue for adding, executing or removing seperated */
	peel::LockMutex      mListMutex; 


public:

	//! Create a new worker object. Necessary mutexes are created by this construtor.
	Worker( ) : mStatus(INIT),  mListMutex() {};

	//! Virtual destuctor 
	virtual ~Worker() {}; 

	//! Execute a command
	/*! This method should be subclassed and provides the execution
		functionality for each command. */
	virtual void execute(WorkerCommand *) = 0;

	//! Queue a new command to execute
	void enqueue(WorkerCommand* command)
	{
		if(command == NULL) return;
		peel::Lock lock(&mListMutex);
		mQueue.push_back(command);
		notify();
	}

	void enqueue_unique(WorkerCommand* command)
	{
		peel::Lock lock(&mListMutex);
		for(size_t i =0; i < mQueue.size(); i++)
		{
			if( *(mQueue[i]) == *command)  return;
		}
		mQueue.push_back(command);
		notify();
	}


	//! Finish all tasks
	void terminate()
	{
		peel::Lock lock(&mListMutex);
		mStatus = TERM;
		notify();
	}


	//! Quit without finishing
	void stop()
	{
		peel::Lock lock(&mListMutex);
		mStatus = END;
		notify();
	}  

	//! Delete all the items in the list
	void clear() 
	{
		peel::Lock lock(&mListMutex);

		std::deque<WorkerCommand*>::iterator it = mQueue.begin();
		while (it != mQueue.end())
		{
			delete (*it);
			++it;
		}
		mQueue.clear();
	}

	//! Don't run any new items, will cause the loop to block
	void pause()
	{
		peel::Lock lock(&mListMutex);
		mStatus = PAUSE;
	}

	//! Unblock the list and resume processing
	void resume()
	{
		peel::Lock lock(&mListMutex);
		mStatus = RUNNING;
		notify();
	}

	void run()
	{
		mStatus = RUNNING;
		while (mStatus == RUNNING || mStatus == PAUSE)
		{
			runone();
		}
		while (mStatus == TERM && mQueue.size())
		{
			runone();
		}
		
		clear();
	}


	void runone()
	{
		if(mStatus == PAUSE)
		{
			wait();
			return;
		}

		WorkerCommand* command = get();

		if(command == NULL)
		{
			// Break from the loop if the list is empty and mRunning is false
		
			if(mStatus == TERM)
			{
				mStatus = END;
				return;
			}

			// Block for another task
			wait();
			return;
		}

		execute(command);
		
		delete command;
		return;
	}

protected:

	WorkerCommand* get()
	{
		WorkerCommand* command = NULL;
	
		peel::Lock lock(&mListMutex);
	
		if (mQueue.size())
		{
			command = mQueue.front();
			mQueue.pop_front();
		}

		return command;
	}

	bool isEmpty()
	{
		peel::Lock lock(&mListMutex);
		bool ret = mQueue.empty();
		return ret;
	}


};

}// namespace peel

#endif
