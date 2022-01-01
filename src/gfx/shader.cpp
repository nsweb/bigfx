

#include "../bigfx.h"
#include "shader.h"
#include "../system/file.h"

#if TOREMOVE


namespace bigfx
{


Shader::Shader() :
	m_program_id(0)
{
	Memory::Memzero( m_shader_ids, sizeof(m_shader_ids) );
}

Shader::~Shader()
{
	DeleteShaders();
}

bool Shader::Create( String const& shader_name )
{
	m_name = shader_name;
	m_program_id = glCreateProgram();

	GLenum shader_types[Shader::MAX] = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER, GL_GEOMETRY_SHADER, GL_COMPUTE_SHADER };
	char const* shader_exts[Shader::MAX] = { "vs", "fs", "tcs", "tes", "gs", "cs" };
	File shader_file;
	String shader_file_name, shader_src;
    int shader_processed = 0;
    
	for( int32 i = 0; i < Shader::MAX; ++i )
	{
		shader_file_name = String::Printf( "../data/shader/%s.%s.glsl", shader_name.c_str(), shader_exts[i] );
		if( !shader_file.Open( shader_file_name.c_str(), false /*bWrite*/) )
			continue;

        shader_processed++;
		shader_file.SerializeString( shader_src );

		ParseIncludeFiles( shader_src );
        
        bool shader_success = CreateAndCompileShader( shader_src.c_str(), shader_types[i], m_shader_ids[i] );
        if( !shader_success )
        {
            DeleteShaders();
            return false;
        }
        
        BB_LOG(Shader, Log, "<%s.%s> Shader compilation SUCCESSFUL", shader_name.c_str(), shader_exts[i]);

		glAttachShader( m_program_id, m_shader_ids[i] );
	}

#ifdef GL_VERSION_4_1
	if ( GLEW_VERSION_4_1 ) 
	{
		// glProgramParameteri( program, GL_PROGRAM_SEPARABLE, GL_TRUE );
	}
#endif /* GL_VERSION_4_1 */

    if( !shader_processed || !LinkProgram() )
    {
        DeleteShaders();
        return false;
    }

	return true;
}
    
bool Shader::CreateFromMemory( String const& shader_name, const char** src_buffers )
{
	m_name = shader_name;
    m_program_id = glCreateProgram();
    
    GLenum shader_types[Shader::MAX] = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_TESS_CONTROL_SHADER, GL_TESS_EVALUATION_SHADER, GL_GEOMETRY_SHADER, GL_COMPUTE_SHADER };
	String str_shader;
    int shader_processed = 0;

    for( int32 i = 0; i < Shader::MAX; ++i )
    {
        if( !src_buffers[i] )
            continue;

        shader_processed++;
		str_shader = src_buffers[i];
		ParseIncludeFiles( str_shader );

        bool shader_success = CreateAndCompileShader( str_shader.c_str(), shader_types[i], m_shader_ids[i] );
        if( !shader_success )
        {
            DeleteShaders();
            return false;
        }
        
        glAttachShader( m_program_id, m_shader_ids[i] );
    }
    
#ifdef GL_VERSION_4_1
    if ( GLEW_VERSION_4_1 )
    {
        // glProgramParameteri( program, GL_PROGRAM_SEPARABLE, GL_TRUE );
    }
#endif /* GL_VERSION_4_1 */
    
    if( !shader_processed || !LinkProgram() )
    {
        DeleteShaders();
        return false;
    }
    
    return true;
}
    
bool Shader::CreateAndCompileShader( char const* shader_src, GLenum shader_type, GLuint& shader_id )
{
    shader_id = glCreateShader( shader_type );
    
    char const* shader_define =
        "#version 330	\n#define SHADER_SECTION	\n" \
        "#define UNIFORM_LEVEL(type, lvl_name, value) uniform type lvl_name = value;\n";

	char const* shader_strings[2] = { shader_define, shader_src };
    glShaderSource( shader_id, 2, shader_strings, nullptr );
    glCompileShader( shader_id );
    
    GLint compiled_status;
    glGetShaderiv( shader_id, GL_COMPILE_STATUS, &compiled_status );
    if ( !compiled_status )
    {
#if BB_BUILD_DEBUG
        GLsizei len;
        glGetShaderiv( shader_id, GL_INFO_LOG_LENGTH, &len );
        
        String log_str;
        log_str.resize( len );
        glGetShaderInfoLog( shader_id, len, &len, log_str.c_str() );
        BB_LOG(Shader, Log, "<%s> Shader compilation failed: %s", m_name.c_str(), log_str.c_str() );
        //BB_LOG(Shader, Log, shader_src);
#endif /* BB_BUILD_DEBUG */
        
        return false;
    }
    
    return true;
}

bool Shader::LinkProgram()
{
    glLinkProgram( m_program_id );
    
    GLint linked_status;
    glGetProgramiv( m_program_id, GL_LINK_STATUS, &linked_status );
    if ( !linked_status )
    {
#if BB_BUILD_DEBUG
        GLsizei len;
        glGetProgramiv( m_program_id, GL_INFO_LOG_LENGTH, &len );
        
        String log_str;
        log_str.resize( len );
        glGetProgramInfoLog( m_program_id, len, &len, log_str.c_str() );
        BB_LOG( Shader, Log, "<%s> Shader linking failed: %s", m_name.c_str(), log_str.c_str() );
#endif /* BB_BUILD_DEBUG */
        
        return false;
    }
    
    return true;
}

void Shader::DeleteShaders()
{
	for( int32 i = 0; i < Shader::MAX; ++i )
	{
		if( m_shader_ids[i] != 0 )
			glDeleteShader( m_shader_ids[i] );
		m_shader_ids[i] = 0;
	}
	if( m_program_id != 0 )
		glDeleteProgram( m_program_id );
	m_program_id = 0;
}

/** Search for #include directive and substitute with proper include file */
bool Shader::ParseIncludeFiles( String& shader_src )
{
	int last_inc_idx = 0;
	while( 1 )
	{
		int inc_idx = shader_src.IndexOf("#include", last_inc_idx);
		if( inc_idx == INDEX_NONE )
			break;
		int quote_start_idx = shader_src.IndexOf("\"", inc_idx);
		if( quote_start_idx == INDEX_NONE )
		{
			BB_LOG( Shader, Error, "<%s> Invalid include directive", m_name.c_str() );
			return false;
		}
		int quote_end_idx = shader_src.IndexOf("\"", quote_start_idx + 1);
		if( quote_end_idx == INDEX_NONE )
		{
			BB_LOG( Shader, Error, "<%s> Invalid include directive", m_name.c_str() );
			return false;
		}

		String inc_file = shader_src.Sub( quote_start_idx + 1, quote_end_idx - quote_start_idx - 1 );
		auto pair = m_include_files.Find( inc_file );
		if( !pair )
		{
			// include file not found, load it
			File shader_file;
			String inc_file_name = String::Printf( "../data/shader/%s", inc_file.c_str() );
			if( !shader_file.Open( inc_file_name.c_str(), false /*bWrite*/) )
			{
				BB_LOG( Shader, Error, "<%s> Could not find include file <%s>", m_name.c_str(), inc_file.c_str() );
				return false;
			}

			String inc_src;
			shader_file.SerializeString( inc_src );
			pair = m_include_files.Add( inc_file, inc_src );
		}

		// Substitute include directive with include file
		shader_src.erase( inc_idx, quote_end_idx - inc_idx + 1 );
		shader_src.insert( pair->Value, inc_idx );
	}
	return true;
}

ShaderUniform Shader::GetUniformLocation( char const* uniform_name ) const
{
    ShaderUniform ret;

    ret.m_index = glGetUniformLocation( m_program_id, uniform_name );

    return ret;
}
    
int Shader::GetActiveUniforms( Array<ShaderUniformDetail>& uniforms ) const
{
    GLsizei uniform_count = 0;
    glGetProgramiv( m_program_id, GL_ACTIVE_UNIFORMS, &uniform_count );
    uniforms.resize(uniform_count);
    
    for( int i = 0; i < uniform_count; ++i )
    {
        GLsizei buf_size = 256;
        GLsizei length = 0;
        GLint size = 0;
        GLenum type = GL_ZERO;
        GLchar name[256];
     
        glGetActiveUniform( m_program_id, i, buf_size, &length, &size, &type, name );
        uniforms[i].m_index = glGetUniformLocation( m_program_id, name );
        uniforms[i].m_name = name;
        uniforms[i].m_type = type;
    }
    
    return uniform_count;
}

/*
 * Uniform setters for scalars
 */

void Shader::SetUniform( ShaderUniform const &uni, int i )
{
    glUniform1i( uni.m_index, i );
}

void Shader::SetUniform( ShaderUniform const &uni, ivec2 const &v )
{
    glUniform2i( uni.m_index, v.x, v.y);
}

void Shader::SetUniform( ShaderUniform const &uni, ivec3 const &v )
{
    glUniform3i( uni.m_index, v.x, v.y, v.z);
}

void Shader::SetUniform( ShaderUniform const &uni, ivec4 const &v )
{
    glUniform4i( uni.m_index, v.x, v.y, v.z, v.w);
}

void Shader::SetUniform( ShaderUniform const &uni, float f )
{
    glUniform1f( uni.m_index, f);
}

void Shader::SetUniform( ShaderUniform const &uni, vec2 const &v )
{
    glUniform2fv( uni.m_index, 1, &v[0] );
}

void Shader::SetUniform( ShaderUniform const &uni, vec3 const &v )
{
    glUniform3fv( uni.m_index, 1, &v[0] );
}

void Shader::SetUniform( ShaderUniform const &uni, vec4 const &v )
{
    glUniform4fv( uni.m_index, 1, &v[0] );
}

void Shader::SetUniform( ShaderUniform const &uni, mat2 const &m )
{
    glUniformMatrix2fv( uni.m_index, 1, GL_FALSE, &m[0][0] );
}

void Shader::SetUniform( ShaderUniform const &uni, mat3 const &m )
{
    glUniformMatrix3fv( uni.m_index, 1, GL_FALSE, &m[0][0] );
}

void Shader::SetUniform( ShaderUniform const &uni, mat4 const &m )
{
    glUniformMatrix4fv( uni.m_index, 1, GL_FALSE, &m[0][0] );
}

//void Shader::SetUniform( ShaderUniform const &uni, ShaderTexture tex, int index)
//{
//    glActiveTexture(GL_TEXTURE0 + index);
//    //glEnable(GL_TEXTURE_2D);
//    glBindTexture(GL_TEXTURE_2D, (int)tex.m_flags);
//    SetUniform(uni, index);
//}

/*
 * Uniform setters for arrays
 */

void Shader::SetUniform( ShaderUniform const &uni, Array<float> const &v )
{
    glUniform1fv( uni.m_index, v.size(), &v[0] );
}

void Shader::SetUniform( ShaderUniform const &uni, Array<vec2> const &v )
{
    glUniform2fv( uni.m_index, v.size(), &v[0][0] );
}

void Shader::SetUniform( ShaderUniform const &uni, Array<vec3> const &v )
{
    glUniform3fv( uni.m_index, v.size(), &v[0][0] );
}

void Shader::SetUniform( ShaderUniform const &uni, Array<vec4> const &v )
{
    glUniform4fv( uni.m_index, v.size(), &v[0][0] );
}
    
void Shader::GetUniform( ShaderUniform const &uni, int& i) const
{
    glGetUniformiv(	m_program_id, uni.m_index, &i );
}
    
void Shader::GetUniform( ShaderUniform const &uni, ivec2& v) const
{
    glGetUniformiv(	m_program_id, uni.m_index, &v[0] );
}

void Shader::GetUniform( ShaderUniform const &uni, ivec3& v) const
{
    glGetUniformiv(	m_program_id, uni.m_index, &v[0] );
}

void Shader::GetUniform( ShaderUniform const &uni, ivec4& v) const
{
    glGetUniformiv(	m_program_id, uni.m_index, &v[0] );
}
    
void Shader::GetUniform( ShaderUniform const &uni, float& f) const
{
    glGetUniformfv(	m_program_id, uni.m_index, &f );
}

void Shader::GetUniform( ShaderUniform const &uni, vec2& v) const
{
    glGetUniformfv(	m_program_id, uni.m_index, &v[0] );
}

void Shader::GetUniform( ShaderUniform const &uni, vec3& v) const
{
    glGetUniformfv(	m_program_id, uni.m_index, &v[0] );
}

void Shader::GetUniform( ShaderUniform const &uni, vec4& v) const
{
    glGetUniformfv(	m_program_id, uni.m_index, &v[0] );
}

void Shader::Bind() const
{
    glUseProgram( m_program_id );
}

void Shader::Unbind() const
{
    glUseProgram(0);
}



} /*namespace bigfx*/

#endif