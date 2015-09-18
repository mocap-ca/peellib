#ifndef ___PEELLIB_WORKER_COMMAND___
#define ___PEELLIB_WORKER_COMMAND___

#ifdef _WIN32
#pragma warning ( disable : 4996 )
#endif


namespace peel
{

//! A command used by peel::Worker
/*! Subclass this with different types of commands, each with a unique integer code */
class WorkerCommand
{
public:
	//! Command object with the specified code (must be unique).
	WorkerCommand(int code) : m_code(code) {};

	virtual ~WorkerCommand() {};

	//! Get the code for this command
	int getCode() const { return m_code; };

	//! returns true if both m_code's match
	bool operator==(const WorkerCommand &other) { return m_code == other.getCode(); }

protected:
	int m_code;
};

}

#endif
