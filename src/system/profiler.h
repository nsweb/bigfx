


#ifndef BB_PROFILER_H
#define BB_PROFILER_H

#if 0 // REBIND

#include "threadingutils.h"

namespace bigfx
{
#define TOKENPASTE(x, y) x ## y
#define TOKENPASTE2(x, y) TOKENPASTE(x, y)

#if BB_BUILD_DEBUG || BB_BUILD_RELEASE
	
	#define PROFILE_THREAD_SCOPE( static_text )		Profiler::ScopeThread TOKENPASTE2(TPScope,__LINE__) ( static_text );
	#define PROFILE_THREAD_START( static_text )		Profiler::ThreadEntry( static_text )
	#define PROFILE_THREAD_STOP()					Profiler::ThreadExit();
	#define PROFILE_THREAD_FRAMERESET()				Profiler::FrameReset();

	//#define PROFILE_PAUSE()             Profiler::pause();
	//#define PROFILE_UNPAUSE()           Profiler::unpause();
	//#define PROFILE_PAUSE_SCOPED()      Profiler::ScopedPause profilerpause##__LINE__;

	#define PROFILE_SCOPE( static_text )            Profiler::ScopeNode TOKENPASTE2(TPScope,__LINE__) ( static_text );
	#define PROFILE_START( static_text )            Profiler::NodeEntry( static_text );
	#define PROFILE_STOP()							Profiler::NodeExit();
#else
	#define PROFILE_THREAD_SCOPE( static_text )		
	#define PROFILE_THREAD_START( static_text )		
	#define PROFILE_THREAD_STOP()					

	#define PROFILE_SCOPE( static_text )            
	#define PROFILE_START( static_text )            
	#define PROFILE_STOP()							
#endif


namespace Profiler
{
	typedef uint32 NameHandle;
	typedef Map<String,NameHandle> NameMap;

	struct CallNode;

	struct ThreadState 
	{
		ThreadTools::CASLock m_ThreadNodeLock;
		CallNode* m_pActiveNode;
	};

	struct CallNode
	{
		CallNode( const char* szName, CallNode* pParent = nullptr ) : 
			m_Name(szName), 
			m_pParent(pParent), 
			m_StartTime(0), 
			m_CallCount(0), 
			m_LastCallCount(0), 
			m_MaxCallCount(0),
			m_fTimeSpent(0), 
			m_fLastTimeSpent(0), 
			m_fMaxTimeSpent(0)	{}

		void Start();
		void Stop();
		void SetActive( bool bActive );
		void FrameReset();
		void HardReset();
		void BuildGui();
		void BuildLog( int indent_level = 0 );
		CallNode* FindOrCreateChildNode( const char* szName );

		const char*			m_Name;
		CallNode*			m_pParent;
		uint64				m_StartTime;
		uint32				m_CallCount;
		uint32				m_LastCallCount;
		uint32				m_MaxCallCount;
		float				m_fTimeSpent;
		float				m_fLastTimeSpent;
		float				m_fMaxTimeSpent;
		Array<CallNode*>	m_ChildrenArray;

        static DECL_THREAD ThreadState m_ThreadState;
	};

	struct CallRoot
	{
		CallRoot( CallNode* pRoot, ThreadState* pThreadState ) : m_pRoot(pRoot), m_pThreadState(pThreadState)	{}
		CallNode* m_pRoot;
		ThreadState* m_pThreadState;
	};

	struct ProfilerData
	{
		ProfilerData()	{}
		Array<CallRoot>			m_ThreadRoots;
		ThreadTools::CASLock	m_ThreadLock;
	};

	void ThreadEntry( const char* szName );
	void ThreadExit();
	void NodeEntry( const char* szName );
	void NodeExit();

	/** Defines a scope for thread */
	class ScopeThread
	{
	public:
		ScopeThread( const char* szName )	{ ThreadEntry( szName );	}
		~ScopeThread()						{ ThreadExit();				}
	};

	/** Defines a scope for call nodes */
	class ScopeNode
	{
	public:
		ScopeNode( const char* szName )		{ NodeEntry( szName );	}
		~ScopeNode()						{ NodeExit();				}
	};

	/** Start profiling on all threads */
	void Start();
	void Stop();
	/** Should be called at the end of each frame, to reset stats */
	void FrameReset();
	void HardReset();

	void BuildGui();
	void BuildLog();


} /* namespace Profiler */

} /* namespace bigfx */

#endif

#endif // BB_PROFILER_H

