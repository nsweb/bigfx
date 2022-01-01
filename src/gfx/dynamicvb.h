


#ifndef BB_DYNAMICVB_H
#define BB_DYNAMICVB_H

#if TOREMOVE

//#include "../engine/basemanager.h"
#include "bufferlock.h"

namespace bigfx
{

class BIGFX_API SubDataVBO
{
public:
	SubDataVBO();
	~SubDataVBO();

	void				Init( uint32 MaxVertex, uint32 VertSizeInBytes );
	void				Cleanup();
	void				Bind();
	void				Unbind();

	GLuint		m_VB_ID;
	uint32		m_DestHead;
	uint32		m_BufferSize;
	//uint8*		m_VertexDataPtr;
};

class BIGFX_API UnsynchronizedVBO
{
public:
						UnsynchronizedVBO();
						~UnsynchronizedVBO();

	void				Init( uint32 MaxVertex, uint32 VertSizeInBytes );
	void				Cleanup();
	void				Bind();
	void				Unbind();

	BufferLock  m_LockManager;
	GLuint		m_VB_ID;
	uint32		m_DestHead;
	uint32		m_BufferSize;
	//uint8*		m_VertexDataPtr;
};

class BIGFX_API PersistentMappedVBO
{
public:
						PersistentMappedVBO();
						~PersistentMappedVBO();

	void				Init( uint32 MaxVertex, uint32 VertSizeInBytes );
	void				Cleanup();
	void				Bind();
	void				Unbind();

	BufferLock  m_LockManager;
	GLuint		m_VB_ID;
	uint32		m_DestHead;
	uint32		m_BufferSize;
	uint8*		m_VertexDataPtr;
};


} /* namespace bigfx */

#endif

#endif // BB_DYNAMICVB_H