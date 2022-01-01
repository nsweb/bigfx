


#ifndef BB_GFXSHADER_H
#define BB_GFXSHADER_H

#if TOREMOVE

#include "../core/map_rh.h"

namespace bigfx
{

struct BIGFX_API ShaderUniform
{
	friend class Shader;

	inline ShaderUniform() : m_index(INDEX_NONE) {}

protected:
    GLint	m_index;
	//uint32	m_flags;
};
    
struct BIGFX_API ShaderUniformDetail : public ShaderUniform
{
    friend class Shader;
    
    ShaderUniformDetail() {}
    
    Name    m_name;
    GLenum  m_type;
};

class BIGFX_API Shader
{
public:

	enum eType
	{
		Vertex = 0,
		Fragment,
		TesselationControl,
		TesselationEvaluation,
		Geometry,
		Compute,
		MAX
	};

						Shader();
	virtual				~Shader();

	bool				Create( String const& shader_name );
    /** Create shaders from memory, src_buffers should hold at least MAX elements */
    bool				CreateFromMemory( String const& shader_name, const char** src_buffers );
	ShaderUniform		GetUniformLocation( char const* uniform_name ) const;
    int                 GetActiveUniforms( Array<ShaderUniformDetail>& uniforms ) const;
	void				SetUniform( ShaderUniform const &uni, int i);
	void				SetUniform( ShaderUniform const &uni, ivec2 const &v);
	void				SetUniform( ShaderUniform const &uni, ivec3 const &v);
	void				SetUniform( ShaderUniform const &uni, ivec4 const &v);
	void				SetUniform( ShaderUniform const &uni, float f);
	void				SetUniform( ShaderUniform const &uni, vec2 const &v);
	void				SetUniform( ShaderUniform const &uni, vec3 const &v);
	void				SetUniform( ShaderUniform const &uni, vec4 const &v);
	void				SetUniform( ShaderUniform const &uni, mat2 const &m);
	void				SetUniform( ShaderUniform const &uni, mat3 const &m);
	void				SetUniform( ShaderUniform const &uni, mat4 const &m);
	//void				SetUniform( ShaderUniform const &uni, ShaderTexture tex, int index);
	void				SetUniform( ShaderUniform const &uni, Array<float> const &v);
	void				SetUniform( ShaderUniform const &uni, Array<vec2> const &v);
	void				SetUniform( ShaderUniform const &uni, Array<vec3> const &v);
	void				SetUniform( ShaderUniform const &uni, Array<vec4> const &v);

    void				GetUniform( ShaderUniform const &uni, int& i) const;
    void				GetUniform( ShaderUniform const &uni, ivec2& v) const;
    void				GetUniform( ShaderUniform const &uni, ivec3& v) const;
    void				GetUniform( ShaderUniform const &uni, ivec4& v) const;
    void				GetUniform( ShaderUniform const &uni, float& f) const;
    void				GetUniform( ShaderUniform const &uni, vec2& v) const;
    void				GetUniform( ShaderUniform const &uni, vec3& v) const;
    void				GetUniform( ShaderUniform const &uni, vec4& v) const;
    
	void				Bind() const;
	void				Unbind() const;

protected:
	String					m_name;
	GLuint					m_shader_ids[MAX];
	GLuint					m_program_id;

	MapRH<String,String>		m_include_files;

	bool				ParseIncludeFiles( String& shader_src );
	void				DeleteShaders();
    bool                CreateAndCompileShader( char const* shader_src, GLenum shader_type, GLuint& shader_id );
    bool                LinkProgram();
};

} /* namespace bigfx */

#endif

#endif // BB_GFXSHADER_H