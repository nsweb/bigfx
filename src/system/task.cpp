
#include "../bigfx.h"
#include "task.h"
//#include "workerthreadmanager.h"
//#include "thread.h"
//#include "threadingutils.h"
#include "lockfreequeue.h"


namespace bigfx
{


#if 0 
TaskProxy::TaskProxy()
{
	m_pTaskQueue = NULL;
}
TaskProxy::~TaskProxy()
{

}

void TaskProxy::SetTaskQueue( LockFreeQueue* _pTaskQueue )
{
	m_pTaskQueue = _pTaskQueue;
}
void TaskProxy::PushTask( Task* _pItem )
{
	m_pTaskQueue->Push( _pItem );
}
Task* TaskProxy::PopTask()
{
	return (Task*)m_pTaskQueue->Pop();
}

void TaskProxy::ExecuteTasks( bool _bUseCallerThread, const uint32 _cnWaitTask )
{
	const Array<WorkerThread*>& rvpThreadPool = WorkerThreadManager::s_GetInstance()->_GetPool();
	uint32 i, nThread = rvpThreadPool.size();
	uint32 iFinishedTask = 0;

	// Define proxy for threads
	for( i = 0; i < nThread; i++ )
		rvpThreadPool[i]->SetTaskProxy( this );

	if( _bUseCallerThread )
	{
		Task* pTask = NULL;
		while( (pTask = PopTask()) != NULL )
		{
			pTask->Execute();
			iFinishedTask++;
		}
		
	}

	// Wait other threads completion
	while( iFinishedTask != _cnWaitTask )
	{
		for( i = 0; i < nThread; i++ )
		{
			if( rvpThreadPool[i]->PopFinishedTask() )
				iFinishedTask++;
		}
	}

	// Remove proxy once finished
	for( i = 0; i < nThread; i++ )
		rvpThreadPool[i]->SetTaskProxy( NULL );

}

void TaskProxy::ExecuteTaskGroups( bool _bUseCallerThread, TaskGroup* _aGroup, const uint32 _cnGroup )
{
	const Array<WorkerThread*>& rvpThreadPool = WorkerThreadManager::s_GetInstance()->_GetPool();
	uint32 i, j, nThread = rvpThreadPool.size();

	// Define proxy for threads
	for( i = 0; i < nThread; i++ )
		rvpThreadPool[i]->SetTaskProxy( this );

	for( j = 0; j < _cnGroup; j++ )
	{
		const Array<Task*>& rvpTask = _aGroup[j].m_vpTask;
		uint32 k, nTask = rvpTask.size();
		for( k = 0; k < nTask; k++ )
			PushTask( rvpTask[k] );

		// Signal threads
		for( i = 0; i < nThread; i++ )
			rvpThreadPool[i]->ForceBusy();

		uint32 iFinishedTask = 0;
		if( _bUseCallerThread )
		{
			Task* pTask = NULL;
			while( (pTask = PopTask()) != NULL )
			{
				pTask->Execute();
				iFinishedTask++;
			}
		}

		// Wait other threads completion
		while( iFinishedTask != nTask )
		{
			for( i = 0; i < nThread; i++ )
			{
				if( rvpThreadPool[i]->PopFinishedTask() )
					iFinishedTask++;
			}
		}
	}

	// [Remove proxy once finished
	for( i = 0; i < nThread; i++ )
		rvpThreadPool[i]->SetTaskProxy( NULL );

}

#endif

} /*namespace bigfx*/