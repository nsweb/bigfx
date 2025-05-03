#include "../bigfx.h"
#include "drawutils.h"
#include "rendercontext.h"

namespace bigfx
{

STATIC_MANAGER_CPP( DrawUtils )

bgfx::VertexLayout Draw::Vertex::ms_layout;
bgfx::VertexLayout Draw::QuadVertex::ms_layout;
bgfx::VertexLayout Draw::CubeVertex::ms_layout;

DrawUtils::DrawUtils()
{
	m_pStaticInstance = this;
}

DrawUtils::~DrawUtils()
{
	m_pStaticInstance = nullptr;
}

void DrawUtils::Create()
{
    Draw::Vertex::init();
    Draw::QuadVertex::init();
    Draw::CubeVertex::init();

    m_util_seg_program = loadProgram("utilseg_vs", "utilseg_fs");
    m_util_shape_program = loadProgram("utilshape_vs", "utilshape_fs");

    // CUBE
    static Draw::CubeVertex cube_data[] = { vec3(-1.f, -1.f, -1.f), vec3(-1.f, 1.f, -1.f), vec3(1.f, 1.f, -1.f), vec3(1.f, -1.f, -1.f),
                                    vec3(-1.f, -1.f, 1.f), vec3(-1.f, 1.f, 1.f), vec3(1.f, 1.f, 1.f), vec3(1.f, -1.f, 1.f) };

    static uint16 idx_data[] = {
        0,1,2, 0,2,3,
        1,0,4, 1,4,5,
        2,1,5, 2,5,6,
        3,2,6, 3,6,7,
        0,3,7, 0,7,4,
        5,4,7, 5,7,6
    };

    m_vbh_cube = bgfx::createVertexBuffer(bgfx::makeRef(cube_data, sizeof(cube_data)),
        Draw::CubeVertex::ms_layout);

    m_ibh_cube = bgfx::createIndexBuffer(
        // Static data can be passed with bgfx::makeRef
        bgfx::makeRef(idx_data, sizeof(idx_data))
    );
}
    
void DrawUtils::Destroy()
{
    bgfx::destroy(m_util_seg_program);
    bgfx::destroy(m_util_shape_program);

    bgfx::destroy(m_vbh_cube);
    bgfx::destroy(m_ibh_cube);
}

void DrawUtils::Tick( TickContext& tick_ctxt )
{
	
}

void DrawUtils::_Render( struct RenderContext& render_ctxt )
{
    // Set view and projection matrix for view 0
    bgfx::setViewTransform(0, &render_ctxt.m_view_mat.v0, &render_ctxt.m_proj_mat.v0);

    // Render Segments
    if (m_seg_buffer.size() > 0 &&
        checkAvailTransientBuffers(m_seg_buffer.size(), Draw::Vertex::ms_layout, m_seg_index_buffer.size()))
    {
        bgfx::TransientVertexBuffer tvb;
        bgfx::allocTransientVertexBuffer(&tvb, m_seg_buffer.size(), Draw::Vertex::ms_layout);
        bx::memCopy(tvb.data, m_seg_buffer.Data(), m_seg_buffer.size() * Draw::Vertex::ms_layout.m_stride);

        bgfx::TransientIndexBuffer tib;
        bgfx::allocTransientIndexBuffer(&tib, m_seg_index_buffer.size());
        bx::memCopy(tib.data, m_seg_index_buffer.Data(), m_seg_index_buffer.size() * sizeof(uint16));

        bgfx::setState(0
            | BGFX_STATE_WRITE_RGB
            | BGFX_STATE_PT_LINES
            | BGFX_STATE_DEPTH_TEST_LEQUAL
            //| BGFX_STATE_LINEAA
            | BGFX_STATE_BLEND_ALPHA);
        bgfx::setIndexBuffer(&tib);
        bgfx::setVertexBuffer(0, &tvb);
        bgfx::submit(0, m_util_seg_program);
    }

    const uint32 instance_stride = sizeof(Draw::InstanceParams);
    uint32 num_shapes = m_shape_params.size();
    if (num_shapes > 0)
    {
        uint32 avail_shapes = bgfx::getAvailInstanceDataBuffer(num_shapes, instance_stride);
        if (num_shapes <= avail_shapes)
        {
            bgfx::InstanceDataBuffer idb;
            bgfx::allocInstanceDataBuffer(&idb, num_shapes, instance_stride);

            Draw::InstanceParams* data = (Draw::InstanceParams*)idb.data;

            for (uint32 ii = 0; ii < num_shapes; ++ii)
            {
                data[ii] = m_shape_params[ii];
            }

            bgfx::setViewTransform(0, &render_ctxt.m_view_mat.v0, &render_ctxt.m_proj_mat.v0);

            // Set vertex and index buffer.
            SetCubeBuffers();

            // Set instance data buffer.
            bgfx::setInstanceDataBuffer(&idb);

            bgfx::setState(BGFX_STATE_DEFAULT);
            bgfx::submit(0, m_util_shape_program);
        }
    }

	// Purge old elements
	RemoveOldElements( render_ctxt.m_delta_seconds );
}

void DrawUtils::RemoveOldElements( float delta_seconds )
{
    m_seg_index_buffer.clear();
    m_seg_buffer.clear();
    
    m_shape_params.clear();

#if 0
    for( int i = m_Segments.size() - 1; i >= 0 ; i++ )
    {
        if( m_Segments[i].persist_time < 0.f )
            continue;
        
        m_Segments[i].persist_time = bigfx::max( 0.f, m_Segments[i].persist_time - DeltaSeconds );
        if( m_Segments[i].persist_time == 0.f )
            m_Segments.erase(i);
    }
    
    for( int i = m_SegmentList.size() - 1; i >= 0 ; i++ )
    {
        if( m_SegmentList[i].persist_time < 0.f )
            continue;
        
        m_SegmentList[i].persist_time = bigfx::max( 0.f, m_SegmentList[i].persist_time - DeltaSeconds );
        if( m_SegmentList[i].persist_time == 0.f )
            m_SegmentList.erase(i);
    }
    
    for( int i = m_Shapes.size() - 1; i >= 0 ; i++ )
    {
        if( m_Shapes[i].persist_time < 0.f )
            continue;
        
        m_Shapes[i].persist_time = bigfx::max( 0.f, m_Shapes[i].persist_time - DeltaSeconds );
        if( m_Shapes[i].persist_time == 0.f )
            m_Shapes.erase(i);
    }
#endif
}

void DrawUtils::PushSegment( vec3 p0, vec3 p1, u8vec4 color0, u8vec4 color1 )
{
    const int offset = m_seg_buffer.size();
	Draw::Vertex v0 = { p0, color0 };
	Draw::Vertex v1 = { p1, color1 };
    m_seg_buffer.push_back( v0 );
    m_seg_buffer.push_back( v1 );
    m_seg_index_buffer.push_back(offset);
    m_seg_index_buffer.push_back(offset + 1);
}
void DrawUtils::PushSegmentList( Array<vec3> const& segment_list, u8vec4 color )
{
    const int offset = m_seg_buffer.size();
    const int seg_count = segment_list.size();
    for( int i = 0; i < seg_count; i++ )
    {
		Draw::Vertex v = { segment_list[i], color };
        m_seg_buffer.push_back( v );
    }
    for (int i = 0; i < seg_count - 1; i++)
    {
        m_seg_index_buffer.push_back(offset + i);
        m_seg_index_buffer.push_back(offset + i + 1);
    }
}
void DrawUtils::PushSegmentList( Array<vec3> const& segment_list, Array<u8vec4> const& color_list )
{
    const int offset = m_seg_buffer.size();
    const int seg_count = segment_list.size();
    for( int i = 0; i < seg_count; i++ )
    {
        Draw::Vertex v = { segment_list[i], color_list[i] };
        m_seg_buffer.push_back( v );
    }
    for (int i = 0; i < seg_count-1; i++)
    {
        m_seg_index_buffer.push_back(offset + i);
        m_seg_index_buffer.push_back(offset + i + 1);
    }
}
void DrawUtils::PushOBB( transform const& t, vec4 color, float ratio_y, float ratio_z )
{
    PushGenericShape( t, color, Draw::Box, ratio_y, ratio_z);
}
void DrawUtils::PushAABB( vec3 pos, float scale, vec4 color, float ratio_y, float ratio_z )
{
    PushGenericShape( transform( quat(1.f,0.f,0.f,0.f), pos, scale ), color, Draw::Box, ratio_y, ratio_z);
}
void DrawUtils::PushSphere( vec3 pos, float scale, vec4 color )
{
    PushGenericShape( transform( quat(1.f,0.f,0.f,0.f), pos, scale ), color, Draw::Sphere );
}
void DrawUtils::PushCylinder( transform const& t, vec4 color, float ratio_radius )
{
    PushGenericShape( t, color, Draw::Cylinder, ratio_radius );
}
void DrawUtils::PushGenericShape(transform const& t, vec4 color, Draw::ShapeType type, float param0, float param1)
{
    mat4 m(t.GetRotation(), t.GetTranslation(), t.GetScale());

    Draw::InstanceParams params = { m, color };// , vec4((float)type, param0, param1, t.GetScale())

    m_shape_params.push_back(params);
}


void DrawUtils::SetCubeBuffers()
{
    // Set vertex and index buffer.
    bgfx::setVertexBuffer(0, m_vbh_cube);
    bgfx::setIndexBuffer(m_ibh_cube);
}

} /*namespace bigfx*/