


#include "../bigfx.h"
#include "profiler.h"

#if 0 // REBIND
#include "threadingutils.h"

namespace bigfx
{

namespace Profiler
{

static ProfilerData g_ProfilerData = ProfilerData();
DECL_THREAD CallNode* g_ThreadCallRoot = nullptr;
DECL_THREAD ThreadState CallNode::m_ThreadState = { {0}, nullptr };


/** Start profiling on all threads */
void Start()
{

}
void Stop()
{

}
void FrameReset()
{
	g_ProfilerData.m_ThreadLock.Lock();

	for( int32 RootIdx = 0; RootIdx < g_ProfilerData.m_ThreadRoots.size(); ++RootIdx )
	{
		CallRoot& TRoot = g_ProfilerData.m_ThreadRoots[RootIdx];
		//if( TRoot.m_pThreadState->m_pActiveNode )
		{
			TRoot.m_pRoot->FrameReset();
		}
	}
	g_ProfilerData.m_ThreadLock.Unlock();	
}

void ThreadEntry( const char* szName )
{
	CallNode* newRoot = new CallNode( szName );

	g_ProfilerData.m_ThreadLock.Lock();
	g_ProfilerData.m_ThreadRoots.push_back( CallRoot(newRoot, &CallNode::m_ThreadState ) );
	
	CallNode::m_ThreadState.m_ThreadNodeLock.Lock();
	CallNode::m_ThreadState.m_pActiveNode = newRoot;
	newRoot->Start();
	newRoot->SetActive( true );
	g_ThreadCallRoot = newRoot;
	CallNode::m_ThreadState.m_ThreadNodeLock.Unlock();

	g_ProfilerData.m_ThreadLock.Unlock();	
}

void ThreadExit()
{
	g_ProfilerData.m_ThreadLock.Lock();

    CallNode::m_ThreadState.m_ThreadNodeLock.Lock();
    g_ThreadCallRoot->Stop();
    g_ThreadCallRoot->SetActive( false );
	CallNode::m_ThreadState.m_pActiveNode = nullptr;
    CallNode::m_ThreadState.m_ThreadNodeLock.Unlock();

    g_ProfilerData.m_ThreadLock.Unlock();	
}

void NodeEntry( const char* szName ) 
{
	CallNode* pParent = CallNode::m_ThreadState.m_pActiveNode;
	//if ( !parent )
	//	return;

	CallNode* pActive = pParent->FindOrCreateChildNode( szName );
	pActive->Start();
	CallNode::m_ThreadState.m_pActiveNode = pActive;
}

void NodeExit() 
{
	CallNode* pActive = CallNode::m_ThreadState.m_pActiveNode;
	//if ( !active )
	//	return;

	pActive->Stop();
	CallNode::m_ThreadState.m_pActiveNode = pActive->m_pParent;
}

void BuildGui()
{
	g_ProfilerData.m_ThreadLock.Lock();

	for( int32 RootIdx = 0; RootIdx < g_ProfilerData.m_ThreadRoots.size(); ++RootIdx )
	{
		CallRoot& TRoot = g_ProfilerData.m_ThreadRoots[RootIdx];
		//if( ImGui::TreeNode(TRoot.m_pRoot->m_Name) )
		{
			TRoot.m_pRoot->BuildGui();
			//ImGui::TreePop();
		}
	}
	g_ProfilerData.m_ThreadLock.Unlock();
}

void BuildLog()
{
	g_ProfilerData.m_ThreadLock.Lock();

	for( int32 RootIdx = 0; RootIdx < g_ProfilerData.m_ThreadRoots.size(); ++RootIdx )
	{
		CallRoot& TRoot = g_ProfilerData.m_ThreadRoots[RootIdx];
		//if( ImGui::TreeNode(TRoot.m_pRoot->m_Name) )
		{
			TRoot.m_pRoot->BuildLog();
			//ImGui::TreePop();
		}
	}
	g_ProfilerData.m_ThreadLock.Unlock();
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

CallNode* CallNode::FindOrCreateChildNode( const char* szName ) 
{
	for( int32 i = 0; i < m_ChildrenArray.size(); ++i )
	{
		if( m_ChildrenArray[i]->m_Name == szName )
			return m_ChildrenArray[i];
	}

	// didn't find the caller, lock this thread and mutate
	CallNode::m_ThreadState.m_ThreadNodeLock.Lock();
	CallNode* newChild = new CallNode( szName, this );
	m_ChildrenArray.push_back( newChild );
	CallNode::m_ThreadState.m_ThreadNodeLock.Unlock();

	return newChild;
}

void CallNode::Start()
{
	m_StartTime = SDL_GetPerformanceCounter();
	m_CallCount++;
}

void CallNode::Stop()
{
	uint64 StopTime = SDL_GetPerformanceCounter();
	float fTimeSpent = (StopTime - m_StartTime) / (float)SDL_GetPerformanceFrequency();
	m_fTimeSpent += fTimeSpent;
	m_fMaxTimeSpent = bigfx::max( m_fTimeSpent, m_fMaxTimeSpent );
	m_MaxCallCount = bigfx::max( m_CallCount, m_MaxCallCount );

	//BB_LOG( Profiler, Log, "[STAT] %s = %f ms - Max = %f ms", m_Name, m_fTimeSpent*100.0f, m_fMaxTimeSpent*100.0f );
}

void CallNode::SetActive( bool bActive )
{

}

void CallNode::FrameReset()
{
	m_StartTime = 0; 
	m_LastCallCount = m_CallCount;
	m_CallCount = 0; 
	m_fLastTimeSpent = m_fTimeSpent;
	m_fTimeSpent = 0;

	// Recursively call each child node
	for( int32 ChildIdx = 0; ChildIdx < m_ChildrenArray.size(); ++ChildIdx )
	{
		m_ChildrenArray[ChildIdx]->FrameReset();
	}
}

void CallNode::HardReset()
{
	m_StartTime = 0; 
	m_CallCount = 0; 
	m_MaxCallCount = 0;
	m_fTimeSpent = 0; 
	m_fMaxTimeSpent = 0;
}

void CallNode::BuildGui()
{
	//ZZZ
	//bool bTreeNode = ImGui::TreeNode(m_Name);
	//ImGui::SameLine(0.f, 100.f);
	//ImGui::Text( "[%d / %d] %.2f / %.2f ms", m_LastCallCount, m_MaxCallCount, m_fLastTimeSpent*100.0f, m_fMaxTimeSpent*100.0f );

	//if( bTreeNode )
	//{
	//	for( int32 ChildIdx = 0; ChildIdx < m_ChildrenArray.size(); ++ChildIdx )
	//	{
	//		m_ChildrenArray[ChildIdx]->BuildGui();
	//	}
	//	ImGui::TreePop();
	//}
}

void CallNode::BuildLog( int indent_level )
{
	//bool bTreeNode = ImGui::TreeNode(m_Name);
	//ImGui::SameLine(0, 100);
	String strIndent;
	for( int i = 0; i < indent_level; i++ )
	{
		if( i == indent_level-1 )
		{
			strIndent += " ";
			strIndent += (char)192;
			strIndent += (char)196;
		}
		else
			strIndent += "  ";
	}
	
	BB_LOG( Profiler, Log, "%s%s (%d) - %.2f ms", strIndent.c_str(), m_Name, m_MaxCallCount, m_fMaxTimeSpent*100.0f );

	//if( bTreeNode )
	{
		for( int32 ChildIdx = 0; ChildIdx < m_ChildrenArray.size(); ++ChildIdx )
		{
			m_ChildrenArray[ChildIdx]->BuildLog( indent_level+1 );
		}
		//ImGui::TreePop();
	}
}




} /* namespace Profiler */

} /* namespace bigfx */

#endif // 0