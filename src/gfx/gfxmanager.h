


#ifndef BB_GFXMANAGER_H
#define BB_GFXMANAGER_H

#if TOREMOVE

#include "../engine/basemanager.h"

namespace bigfx
{

class BIGFX_API Shader;
//BIGBALL_TEMPLATE template class BIGFX_API Map<String,Shader*>;

////////////////////////////////////////////////////////////
class BIGFX_API GfxManager : public BaseManager 
{
	STATIC_MANAGER_H(GfxManager)

public:
						GfxManager();
	virtual				~GfxManager();

	virtual void		Create();
	virtual void		Destroy();	
	virtual void		Tick( struct TickContext& tick_ctxt );

	Shader*				LoadShader( String const& shader_name );
    Shader*				LoadShaderFromMemory( String const& shader_name, const char** src_buffers );

protected:
	Map<String,Shader*>		m_shaders;

};

} /* namespace bigfx */

#endif

#endif // BB_GFXMANAGER_H