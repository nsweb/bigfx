

#include "../bigfx.h"

#if TOREMOVE

#include "shader.h"
#include "gfxmanager.h"

namespace bigfx
{

STATIC_MANAGER_CPP( GfxManager )


GfxManager::GfxManager()
{
	m_pStaticInstance = this;
}

GfxManager::~GfxManager()
{
	m_pStaticInstance = nullptr;
}

void GfxManager::Create()
{

}
void GfxManager::Destroy()
{
	uint32 ShaderCount = m_shaders.GetPairCount();
	for( uint32 i = 0; i < ShaderCount; ++i )
	{
		auto pPair = m_shaders.GetPairAt( i );
		BB_DELETE( pPair->Value );
	}
}

void GfxManager::Tick( TickContext& tick_ctxt )
{
	
}

Shader* GfxManager::LoadShader( String const& shader_name )
{
	auto pair = m_shaders.Find( shader_name );
	if( pair )
		return pair->Value;

	Shader* shader = new Shader();
	if( !shader->Create( shader_name ) )
	{
		BB_DELETE( shader );
		return nullptr;
	}

	m_shaders.Add( shader_name, shader );
	return shader;
}
    
Shader*	GfxManager::LoadShaderFromMemory( String const& shader_name, const char** src_buffers )
{
    auto pair = m_shaders.Find( shader_name );
    if( pair )
        return pair->Value;
    
    Shader* shader = new Shader();
    if( !shader->CreateFromMemory( shader_name, src_buffers ) )
    {
        BB_DELETE( shader );
        return nullptr;
    }
    
    m_shaders.Add( shader_name, shader );
    return shader;
}


} /*namespace bigfx*/

#endif