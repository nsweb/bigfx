

#ifndef BB_DRAWUTILS_H
#define BB_DRAWUTILS_H

#include "../engine/basemanager.h"

namespace bigfx
{

class BIGFX_API Shader;

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
    };
    
    struct InstanceParams
    {
        u8vec4	m_col;
        vec4    m_params;
        vec3    m_eye_to_box;
    };
    
	struct BufferRange
	{
        int m_offset;
        int m_count;
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
	void		PushSegment( vec3 p0, vec3 p1, u8vec4 color0, u8vec4 color1 );
	void		PushSegmentList( Array<vec3> const& segment_list, u8vec4 color);
    void		PushSegmentList( Array<vec3> const& segment_list, Array<u8vec4> const& color_list );
	void		PushOBB( transform const& t, u8vec4 color, float ratio_y = 1.f, float ratio_z = 1.f );
	void		PushAABB( vec3 pos, float scale, u8vec4 color, float ratio_y = 1.f, float ratio_z = 1.f );
    void		PushSphere( vec3 pos, float scale, u8vec4 color );
    void		PushCylinder( transform const& t, u8vec4 color, float ratio_radius = 1.f );

protected:
#if 0 // REBIND
	Shader*                     m_util_seg_shader;
    Shader*                     m_util_shape_shader;
#endif

	Array<Draw::BufferRange>	m_seg_list;
    Array<Draw::Vertex>         m_seg_buffer;
    
    Draw::BufferRange           m_shapes;
    //Array<u8vec4>               m_shape_colors;
    Array<mat4>                 m_shape_matrices;
    Array<Draw::InstanceParams> m_shape_params;
    //Array<vec3>                 m_shape_eye_to_boxes;
    
#if 0 // REBIND
    enum eVAType
    {
        eVASeg = 0,
        eVAShape,
        eVACount
    };
    enum eVBType
    {
        eVBSeg = 0,        /** Dynamic VB used to render segments */
        eVBShapeElt,
        eVBShapePos,
        eVBShapeMat,
        //eVBShapeCol,
        eVBShapeParams,
        //eVBShapeEye,
        eVBCount
    };

    GLuint					m_varrays[eVACount];
    GLuint					m_vbuffers[eVBCount];
#endif // 0

	void		RemoveOldElements( float delta_seconds );
    void		PushGenericShape( transform const& t, u8vec4 color, Draw::ShapeType type, float param0 = 1.f, float param1 = 1.f );
};

} /* namespace bigfx */

#endif // BB_DRAWUTILS_H