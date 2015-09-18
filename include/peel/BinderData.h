#ifndef ___PEELLIB_BINDER_DATA___
#define ___PEELLIB_BINDER_DATA___

#include "chartype.h"
#include "peel/File.h"

#include <string>

namespace peel
{


	//! Abstract base class for a block of data in a Binder.  
	/*!
 	Subclass should implment parseBlock(), write() and clear() methods
	a size_t id is provided and should be unique for each subclass.  The subclass
	should populate the id when calling this class' constrctor.
	*/
	class BinderData
	{
	public:
		BinderData(pl_sizet id) : m_id(id) {};
		~BinderData() {};

		pl_uint32 getId()  { return m_id;    }

		//! Parse a block of data in the binder
		virtual void     parseBlock(File *file, pl_sizet offset, bool ptrs64) = 0;
		virtual pl_sizet write(File*) = 0 ;  //!< returns number of bytes written 
		virtual void     clear() = 0;            //!< should clear contents of data block
		pl_uint32        m_id;                   //!< unique id for this block
	};

}


#endif
