

#include "../bigfx.h"
#include "lockfreequeue.h"

#if 0
TaskNode LockFreeQueue::ms_NodeNull;

LockFreeQueue::LockFreeQueue()
{
	ms_NodeNull.m_pNextNode = NULL;
	m_NodeTail.m_pNode = &ms_NodeNull;
	m_NodeHead.m_pNode = &ms_NodeNull;

}
LockFreeQueue::~LockFreeQueue()
{

}

LockFreeQueue::NodeCAS::NodeCAS()
{
	m_pNode = NULL;
	m_pCASCounter = 0;
}

void LockFreeQueue::Push( TaskNode* _pItem )
{
	NodeCAS oTail, oNewTail;
	//
	_pItem->m_pNextNode = NULL;
	//
	while( 1 )
	{
		ThreadTools::InterlockedExchange( &oTail , *(s64*)&m_NodeTail );

		// if tail next if 0 replace it with new item
		if( ThreadTools::InterlockedCompareExchange( &oTail.m_pNode->m_pNextNode, (s32)_pItem, 0 ) != 0 ) 
			break;

		// else push tail back until it reaches end
		oNewTail.m_pCASCounter = oTail.m_pCASCounter+1;
		oNewTail.m_pNode = oTail.m_pNode->m_pNextNode;
		ThreadTools::InterlockedCompareExchange2( &m_NodeTail, (s32)oTail.m_pNode->m_pNextNode, oTail.m_pCASCounter+1, (s32)oTail.m_pNode, oTail.m_pCASCounter );

		ThreadTools::SpinLoop();
	}

	// try and change tail pointer (it is also fixed in Pop)
	oNewTail.m_pCASCounter = oTail.m_pCASCounter+1;
	oNewTail.m_pNode = _pItem;
	ThreadTools::InterlockedCompareExchange2( &m_NodeTail, (s32)_pItem, oTail.m_pCASCounter+1, (s32)oTail.m_pNode, oTail.m_pCASCounter );
}

TaskNode* LockFreeQueue::Pop()
{
	NodeCAS oHead, oTail;
	//
	while( 1 )
	{
		ThreadTools::InterlockedExchange( &oHead , *(s64*)&m_NodeHead );
		ThreadTools::InterlockedExchange( &oTail , *(s64*)&m_NodeTail );
		//
		TaskNode* pNodeNext = oHead.m_pNode->m_pNextNode;

		// is queue empty
		if( oHead.m_pNode == oTail.m_pNode )
		{
			// is it really empty
			if( !pNodeNext ) 
				return 0;

			// or is just tail falling behind...
			ThreadTools::InterlockedCompareExchange2( &m_NodeTail, (s32)pNodeNext, oTail.m_pCASCounter+1 , (int)oTail.m_pNode, oTail.m_pCASCounter );
		}
		else
		{
			if( ThreadTools::InterlockedCompareExchange2( &m_NodeHead, (s32)pNodeNext, oHead.m_pCASCounter+1 , (int)oHead.m_pNode, oHead.m_pCASCounter ) ) 
				return pNodeNext;
		}
		//
		ThreadTools::SpinLoop();
	}
}
#endif