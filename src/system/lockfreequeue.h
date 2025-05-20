
#ifndef BB_LOCKFREEQUEUE_H
#define BB_LOCKFREEQUEUE_H 

#if 0 // REBIND

//#include "threadingutils.h"

namespace bigfx
{

//#include "peTUtils.h"


// reader-writer circular queue.. one thread writes, one thread reads only...
template<class T> 
class BIGFX_API CACHE_ALIGN CircularQueue
{

public:
	CircularQueue()
	{
		m_iReadOffset = 0;
		m_iWriteOffset = 0;
		m_pData = NULL;
		m_nQueueSize = 0;
	}
	~CircularQueue()
	{
		BB_DELETE_ARRAY( m_pData );
	}

	void Resize( int32 _nSize )
	{
		m_iReadOffset = 0;
		m_iWriteOffset = 0;
		m_nQueueSize = _nSize;
		m_pData = new T[_nSize];//
	}

	//push a new element in the circular queue
	bool Push( const T &_rElement )
	{
		int32 iNextElement = (m_iWriteOffset + 1) % m_nQueueSize;
		if( iNextElement != m_iReadOffset )
		{
			m_pData[m_iWriteOffset] = _rElement;

			// barrier cpu & compiler 
			ThreadTools::CompilerBarrier();				// compiler reordering
			ThreadTools::CPUBarrierWriteRelease();		// cpu reordering

			m_iWriteOffset = iNextElement;			// atomic op
			return true;
		}
		else
			return false;	// buffer full
	}
	// remove the next element from the circular queue
	bool Pop( T &_rElement )
	{
		if( m_iReadOffset == m_iWriteOffset )
			return false;

		int32 iNextElement = (m_iReadOffset + 1) % m_nQueueSize;
		_rElement = m_pData[m_iReadOffset];

		// barrier cpu & compiler 
		ThreadTools::CompilerBarrier();	// compiler reordering
		ThreadTools::CPUBarrierReadAcquire();		// cpu reordering

		m_iReadOffset = iNextElement;				// [10/5/2010 serouart] Atomic
		return true;
	}

	bool IsFull() const
	{
		int32 iNextElement = (m_iWriteOffset + 1) % m_nQueueSize;
		if( iNextElement == m_iReadOffset )
			return true;

		return false;
	}

	int32 GetElementCount() const
	{
		return (m_iWriteOffset + m_nQueueSize - m_iReadOffset) % m_nQueueSize;
	}
	
private:
	int32		m_iReadOffset;
	int32		m_iWriteOffset;
	T*			m_pData;
	int32		m_nQueueSize;
};

#if 0

//////////////////////////////////////////////////////////////////////////
class BIGFX_API CACHE_ALIGN LockFreeQueue
{
public:
	LockFreeQueue();
	~LockFreeQueue();

	class NodeCAS
	{
	public:
		NodeCAS();

		TaskNode*	m_pNode;
		int32			m_pCASCounter;
	};

	void Push( TaskNode* _pItem );
	TaskNode* Pop();


private:
	static TaskNode	ms_NodeNull;
	NodeCAS			m_NodeHead;
	NodeCAS			m_NodeTail;

};

#endif


} /*namespace bigfx*/

#endif // 0

#endif // BB_LOCKFREEQUEUE_H