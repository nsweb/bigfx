

#include "../bigfx.h"
#include "shader.h"
#include "drawutils.h"
#include "rendercontext.h"
#include "gfxmanager.h"

namespace bigfx
{

STATIC_MANAGER_CPP( DrawUtils )


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
#if 0 // REBIND
    m_util_seg_shader = GfxManager::GetStaticInstance()->LoadShader( "utilseg" );
    m_util_shape_shader = GfxManager::GetStaticInstance()->LoadShader( "utilshape" );
    
    glGenVertexArrays( eVACount, m_varrays );
    glGenBuffers( eVBCount, m_vbuffers );
    
    glBindVertexArray( m_varrays[eVASeg] );
    glBindBuffer( GL_ARRAY_BUFFER, m_vbuffers[eVBSeg] );

    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Draw::Vertex) /*stride*/, (void*)0 /*offset*/	);
    glVertexAttribPointer( 1, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Draw::Vertex) /*stride*/, (void*)12 /*offset*/	);
    
    glBindVertexArray(0);
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    
    //////////////////////////////////////////////////////////
    // Shape
    const vec3 cube_data[] = { vec3(-1.f, -1.f, -1.f), vec3(-1.f, 1.f, -1.f), vec3(1.f, 1.f, -1.f), vec3(1.f, -1.f, -1.f),
                              vec3(-1.f, -1.f, 1.f), vec3(-1.f, 1.f, 1.f), vec3(1.f, 1.f, 1.f), vec3(1.f, -1.f, 1.f) };
    
    GLuint idx_data[] = {
            0,1,2, 0,2,3,
            1,0,4, 1,4,5,
            2,1,5, 2,5,6,
            3,2,6, 3,6,7,
            0,3,7, 0,7,4,
            5,4,7, 5,7,6
    };
    
    glBindVertexArray( m_varrays[eVAShape] );

    glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, m_vbuffers[eVBShapeElt] );
    glBufferData( GL_ELEMENT_ARRAY_BUFFER, COUNT_OF(idx_data) * sizeof(GLuint), idx_data, GL_STATIC_DRAW );
    
    glBindBuffer( GL_ARRAY_BUFFER, m_vbuffers[eVBShapePos] );
    glBufferData( GL_ARRAY_BUFFER, COUNT_OF(cube_data) * sizeof(vec3), cube_data, GL_STATIC_DRAW );
    glEnableVertexAttribArray(0);
    glVertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3) /*stride*/, (void*)0 /*offset*/ );
    
    glBindBuffer( GL_ARRAY_BUFFER, m_vbuffers[eVBShapeMat] );
    for (int i = 0; i < 4; i++ )
    {
        glEnableVertexAttribArray( 1 + i );
        glVertexAttribPointer( 1 + i, 4, GL_FLOAT, GL_FALSE, sizeof(mat4) /*stride*/, (void*)(sizeof(vec4) * i) /*offset*/ );
        glVertexAttribDivisor( 1 + i, 1 );
    }

	uintptr_t offset_params = 0;
    glBindBuffer( GL_ARRAY_BUFFER, m_vbuffers[eVBShapeParams] );
    glEnableVertexAttribArray(5);
    glVertexAttribPointer( 5, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Draw::InstanceParams) /*stride*/, (void*)offset_params );
    glVertexAttribDivisor( 5, 1 );
    offset_params += sizeof(u8vec4);
    
    //glBindBuffer( GL_ARRAY_BUFFER, m_vbuffers[eVBShapeParams] );
    glEnableVertexAttribArray(6);
    glVertexAttribPointer( 6, 4, GL_FLOAT, GL_FALSE, sizeof(Draw::InstanceParams) /*stride*/, (void*)offset_params );
    glVertexAttribDivisor( 6, 1 );
    offset_params += sizeof(vec4);
    
    //glBindBuffer( GL_ARRAY_BUFFER, m_vbuffers[eVBShapeEye] );
    glEnableVertexAttribArray(7);
    glVertexAttribPointer( 7, 3, GL_FLOAT, GL_FALSE, sizeof(Draw::InstanceParams) /*stride*/, (void*)offset_params);
    glVertexAttribDivisor( 7, 1 );
    
    glBindVertexArray(0);
    for( int attrib_idx = 0; attrib_idx < 8; attrib_idx++)
        glDisableVertexAttribArray( attrib_idx);
#endif // 0
}
    
void DrawUtils::Destroy()
{
#if 0 // REBIND
    glDeleteBuffers( eVBCount, m_vbuffers );
    glDeleteVertexArrays( eVACount, m_varrays );
#endif // 0
}

void DrawUtils::Tick( TickContext& tick_ctxt )
{
	
}

void DrawUtils::_Render( struct RenderContext& render_ctxt )
{
	mat4 cam_to_world_mat(render_ctxt.m_view.m_transform.GetRotation(), render_ctxt.m_view.m_transform.GetTranslation(), (float)render_ctxt.m_view.m_transform.GetScale());
    mat4 view_mat = bigfx::inverse(cam_to_world_mat);

#if 0 // REBIND
    
    // Transform camera eye pos to box frame
    for( int32 shape_idx = 0; shape_idx < m_shape_params.size(); shape_idx++ )
    {
        Draw::InstanceParams& params = m_shape_params[shape_idx];
        mat4 const& box_to_world = m_shape_matrices[shape_idx];
        mat4 box_to_view = view_mat * box_to_world;
        params.m_eye_to_box = (inverse( box_to_view ) * vec4(0.f, 0.f, 0.f, 1.f)).xyz;
    }
    
    // Render Segments
    m_util_seg_shader->Bind();
    {
        ShaderUniform uni_proj = m_util_seg_shader->GetUniformLocation("proj_mat");
        m_util_seg_shader->SetUniform( uni_proj, render_ctxt.m_proj_mat );
        ShaderUniform uni_view = m_util_seg_shader->GetUniformLocation("view_mat");
        m_util_seg_shader->SetUniform( uni_view, view_mat );
    
        glBindVertexArray( m_varrays[eVASeg] );
    
        glBindBuffer(GL_ARRAY_BUFFER, m_vbuffers[eVBSeg]);
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)m_seg_buffer.size() * sizeof(Draw::Vertex), (GLvoid*)m_seg_buffer.Data(), GL_DYNAMIC_DRAW );
        
        for( int iSL = 0; iSL < m_seg_list.size(); iSL++)
        {
            Draw::BufferRange const& seg_list = m_seg_list[iSL];
            glDrawArrays(/*GL_TRIANGLE_STRIP*/GL_LINE_STRIP, seg_list.m_offset, seg_list.m_count );
        }
    
        glBindVertexArray(0);
    }
    m_util_seg_shader->Unbind();
    
    // Render Shapes
    m_util_shape_shader->Bind();
    {
        const float z_near = render_ctxt.m_view.m_parameters[eCP_NEAR];
        const float z_far = render_ctxt.m_view.m_parameters[eCP_FAR];
        vec2 z_var;
        z_var.x = (z_far + z_near) / (z_far - z_near);
        z_var.y = 2.0f*z_far*z_near / (z_far - z_near);
        
        ShaderUniform uni_proj = m_util_shape_shader->GetUniformLocation("proj_mat");
        m_util_shape_shader->SetUniform( uni_proj, render_ctxt.m_proj_mat );
        ShaderUniform uni_view = m_util_shape_shader->GetUniformLocation("view_mat");
        m_util_shape_shader->SetUniform( uni_view, view_mat );
        ShaderUniform uni_zvar = m_util_shape_shader->GetUniformLocation("z_var");
        m_util_shape_shader->SetUniform( uni_zvar, z_var );
    
        glBindVertexArray( m_varrays[eVAShape] );
    
        glBindBuffer(GL_ARRAY_BUFFER, m_vbuffers[eVBShapeMat]);
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)m_shape_matrices.size() * sizeof(mat4), (GLvoid*)m_shape_matrices.Data(), GL_DYNAMIC_DRAW );

        glBindBuffer(GL_ARRAY_BUFFER, m_vbuffers[eVBShapeParams]);
        glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)m_shape_params.size() * sizeof(Draw::InstanceParams), (GLvoid*)m_shape_params.Data(), GL_DYNAMIC_DRAW );
        
        glDrawElementsInstanced( GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0, m_shape_matrices.size() );
    
        glBindVertexArray(0);
    }
    m_util_shape_shader->Unbind();

#endif // 0
    
	// Purge old elements
	RemoveOldElements( render_ctxt.m_delta_seconds );
}

void DrawUtils::RemoveOldElements( float delta_seconds )
{
    m_seg_list.clear();
    m_seg_buffer.clear();
    
    m_shapes.m_offset = 0;
    m_shapes.m_count = 0;
    m_shape_matrices.clear();
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
	Draw::BufferRange seg_list = { offset, 2 };
    m_seg_list.push_back( seg_list );
}
void DrawUtils::PushSegmentList( Array<vec3> const& segment_list, u8vec4 color )
{
    const int offset = m_seg_buffer.size();
    for( int i =0; i < segment_list.size(); i++ )
    {
		Draw::Vertex v = { segment_list[i], color };
        m_seg_buffer.push_back( v );
    }
	Draw::BufferRange seg_list = { offset, segment_list.size() };
	m_seg_list.push_back( seg_list );
}
void DrawUtils::PushSegmentList( Array<vec3> const& segment_list, Array<u8vec4> const& color_list )
{
    const int offset = m_seg_buffer.size();
    for( int i =0; i < segment_list.size(); i++ )
    {
        Draw::Vertex v = { segment_list[i], color_list[i] };
        m_seg_buffer.push_back( v );
    }
    Draw::BufferRange seg_list = { offset, segment_list.size() };
    m_seg_list.push_back( seg_list );
}
void DrawUtils::PushOBB( transform const& t, u8vec4 color, float ratio_y, float ratio_z )
{
    PushGenericShape( t, color, Draw::Box, ratio_y, ratio_z);
}
void DrawUtils::PushAABB( vec3 pos, float scale, u8vec4 color, float ratio_y, float ratio_z )
{
    PushGenericShape( transform( quat(1.f,0.f,0.f,0.f), pos, scale ), color, Draw::Box, ratio_y, ratio_z);
}
void DrawUtils::PushSphere( vec3 pos, float scale, u8vec4 color )
{
    PushGenericShape( transform( quat(1.f,0.f,0.f,0.f), pos, scale ), color, Draw::Sphere );
}
void DrawUtils::PushCylinder( transform const& t, u8vec4 color, float ratio_radius )
{
    PushGenericShape( t, color, Draw::Cylinder, ratio_radius );
}
void DrawUtils::PushGenericShape( transform const& t, u8vec4 color, Draw::ShapeType type, float param0, float param1 )
{
    mat4 m( t.GetRotation(), t.GetTranslation(), t.GetScale() );
    m_shape_matrices.push_back( m );
    
    Draw::InstanceParams params = { color, vec4((float)type, param0, param1, t.GetScale()), t.GetTranslation() };
    m_shape_params.push_back(params);
    
    m_shapes.m_offset = 0;
    m_shapes.m_count = m_shape_matrices.size();
}

} /*namespace bigfx*/