

#include "../bigfx.h"
#include "bufferlock.h"

#if TOREMOVE

namespace bigfx
{

BufferLock::BufferLock(bool bCPUUpdates) :
	m_bCPUUpdates(bCPUUpdates)
{

}

BufferLock::~BufferLock()
{
	for( int i = 0; i < m_BufferLocks.size(); ++i) 
	{
		cleanup( &m_BufferLocks[i] );
	}

	m_BufferLocks.clear();
}

void BufferLock::WaitForLockedRange(uint32 _lockBeginBytes, uint32 _lockLength)
{
	BufferRange testRange = { _lockBeginBytes, _lockLength };
	Array<BufferLockRange> swapLocks;
	for( int i = 0; i < m_BufferLocks.size(); ++i) 
	{
		if( testRange.Overlaps( m_BufferLocks[i].m_Range ) ) 
		{
			wait( &m_BufferLocks[i].m_SyncObj );
			cleanup( &m_BufferLocks[i] );
		} 
		else 
		{
			swapLocks.push_back( m_BufferLocks[i] );
		}
	}

	m_BufferLocks = swapLocks;
}

void BufferLock::LockRange(uint32 _lockBeginBytes, uint32 _lockLength)
{
	BufferRange newRange = { _lockBeginBytes, _lockLength };
	GLsync syncName = glFenceSync( GL_SYNC_GPU_COMMANDS_COMPLETE, 0 );
	BufferLockRange newLock = { newRange, syncName };

	m_BufferLocks.push_back(newLock);
}

void BufferLock::wait(GLsync* _syncObj)
{
	const GLuint64 kOneSecondInNanoSeconds = 1000000000;
	if( m_bCPUUpdates ) 
	{
		GLbitfield waitFlags = 0;
		GLuint64 waitDuration = 0;
		while (1) 
		{
			GLenum waitRet = glClientWaitSync(*_syncObj, waitFlags, waitDuration);
			if (waitRet == GL_ALREADY_SIGNALED || waitRet == GL_CONDITION_SATISFIED) {
				return;
			}

			if (waitRet == GL_WAIT_FAILED) 
			{
				BB_ASSERT(!"Not sure what to do here. Probably raise an exception or something.");
				return;
			}

			// After the first time, need to start flushing, and wait for a looong time.
			waitFlags = GL_SYNC_FLUSH_COMMANDS_BIT;
			waitDuration = kOneSecondInNanoSeconds;
		}
	} 
	else 
	{
		glWaitSync(*_syncObj, 0, GL_TIMEOUT_IGNORED);
	}
}

void BufferLock::cleanup(BufferLockRange* _bufferLock)
{
	glDeleteSync(_bufferLock->m_SyncObj);
}

} /*namespace bigfx*/

#endif