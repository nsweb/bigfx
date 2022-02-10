

#ifndef BB_DRAWUTILS_H
#define BB_DRAWUTILS_H

#include "../engine/basemanager.h"

namespace bigfx
{
namespace Draw
{
    enum ShapeType : uint32
	{
		Box = 0,
        Cylinder,
        Sphere,
        Count
	};

    struct Vertex
    {
        vec3	m_pos;
        u8vec4	m_col;

        static void init()
        {
            ms_layout
                .begin()
                .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
                .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
                .end();
        }

        static bgfx::VertexLayout ms_layout;
    };

    struct CubeVertex
    {
        vec3	m_pos;

        static void init()
        {
            ms_layout
                .begin()
                .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
                .end();
        };
        static bgfx::VertexLayout ms_layout;
    };
    
    struct InstanceParams
    {
        mat4    m_shape_mat;
        vec4	m_col;
        //vec3    m_eye_to_box;
        //vec4    m_params;
    };
}

////////////////////////////////////////////////////////////
class BIGFX_API DrawUtils
{
	STATIC_MANAGER_H(DrawUtils)

public:
						DrawUtils();
	virtual				~DrawUtils();

	virtual void		Create();
	virtual void		Destroy();	
	virtual void		Tick( struct TickContext& tick_ctxt );
	virtual void		_Render( struct RenderContext& render_ctxt );

	/* Functions for drawing utility shapes
	 */
	void		    PushSegment( vec3 p0, vec3 p1, u8vec4 color0, u8vec4 color1 );
	void		    PushSegmentList( Array<vec3> const& segment_list, u8vec4 color);
    void		    PushSegmentList( Array<vec3> const& segment_list, Array<u8vec4> const& color_list );
	void	    	PushOBB( transform const& t, vec4 color, float ratio_y = 1.f, float ratio_z = 1.f );
	void	    	PushAABB( vec3 pos, float scale, vec4 color, float ratio_y = 1.f, float ratio_z = 1.f );
    void		    PushSphere( vec3 pos, float scale, vec4 color );
    void		    PushCylinder( transform const& t, vec4 color, float ratio_radius = 1.f );

    void			SetCubeBuffers();

protected:

    bgfx::ProgramHandle         m_util_seg_program;
    bgfx::ProgramHandle         m_util_shape_program;

    Array<Draw::Vertex>         m_seg_buffer;
    Array<uint16>               m_seg_index_buffer;
 
    Array<Draw::InstanceParams> m_shape_params;

    bgfx::VertexBufferHandle	m_vbh_cube;
    bgfx::IndexBufferHandle		m_ibh_cube;

	void		RemoveOldElements( float delta_seconds );
    void		PushGenericShape( transform const& t, vec4 color, Draw::ShapeType type, float param0 = 1.f, float param1 = 1.f );
};

} /* namespace bigfx */

#endif // BB_DRAWUTILS_H