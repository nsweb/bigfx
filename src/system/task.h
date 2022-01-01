
#ifndef BB_TASK_H
#define BB_TASK_H


namespace bigfx
{

//class BIGFX_API LockFreeQueue;

struct BIGFX_API TaskNode
{
	TaskNode*	m_pNextNode;
};


/** A sync task is executed in one frame by a worker thread */
/** And async task may take multiple frames and executed by a worker thread */
class BIGFX_API Task : public TaskNode
{
public:
	Task() : 
			m_bIsAsync(false),
			m_bCancelled(false),
			m_pUserData(0),
			m_iUserId(0)
	  {
	  }

	  virtual void Execute() = 0;
	  virtual void OnFinished()								{}
	  void SetUserData( void* _pUserData, uint32 _iUserId )	{ m_pUserData = _pUserData; m_iUserId = _iUserId; }
	  uint32 IsCancelled() const							{ return m_bCancelled;			}
	  void Cancel()											{ m_bCancelled = true;			}
	  void SetAsync( bool bAsync)							{ m_bIsAsync = bAsync;	} 
	  bool IsAsync()										{ return m_bIsAsync;			} 

protected:

	uint32				m_bIsAsync		: 1;
	uint32				m_bCancelled	: 1;

	void*				m_pUserData;
	uint32              m_iUserId;
};

#if 0
//////////////////////////////////////////////////////////////////////////
class BIGFX_API TaskGroup
{
public:
	TaskGroup()		{}
	~TaskGroup()	{}

	Array<Task*>	m_vTask;
};

//////////////////////////////////////////////////////////////////////////
class BIGFX_API TaskProxy
{
public:
				TaskProxy();
	virtual		~TaskProxy();

	void		SetTaskQueue( LockFreeQueue* _pTaskQueue );
	void		PushTask( Task* _pItem );
	Task*		PopTask();

	void		ExecuteTasks( bool _bUseCallerThread, const uint32 _cnWaitTask );
	void		ExecuteTaskGroups( bool _bUseCallerThread, TaskGroup* _aGroup, const uint32 _cnGroup );

private:
	LockFreeQueue*	m_pTaskQueue;
};
#endif

} /*namespace bigfx*/

#endif // BB_TASK_H