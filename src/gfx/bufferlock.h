

#ifndef BB_BUFFERLOCK_H
#define BB_BUFFERLOCK_H

// Inspired from apitest on github

#if TOREMOVE

namespace bigfx
{

struct BufferRange
{
	uint32 m_StartOffset;
	uint32 m_Length;

	bool Overlaps(const BufferRange& _rhs) const 
	{
		return m_StartOffset < (_rhs.m_StartOffset + _rhs.m_Length)
			&& _rhs.m_StartOffset < (m_StartOffset + m_Length);
	}
};

struct BufferLockRange
{
	BufferRange m_Range;
	GLsync m_SyncObj;
};

class BufferLock
{
public:
	BufferLock( bool bCPUUpdates );
	~BufferLock();

	void WaitForLockedRange(uint32 _lockBeginBytes, uint32 _lockLength);
	void LockRange(uint32 _lockBeginBytes, uint32 _lockLength);

private:
	void wait(GLsync* _syncObj);
	void cleanup(BufferLockRange* _bufferLock);

	Array<BufferLockRange>		m_BufferLocks;
	/** Whether it's the CPU (true) that updates, or the GPU (false) */
	bool						m_bCPUUpdates;
};

} /* namespace bigfx */

#endif

#endif // BB_BUFFERLOCK_H