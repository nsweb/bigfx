

#include "../bigfx.h"
#include "controller.h"
#include "camera.h"
#include "engine.h"
#include "tickcontext.h"

namespace bigfx
{

STATIC_MANAGER_CPP( Controller )

Controller::Controller()
{
	m_pStaticInstance = this;
	m_active_cam_ctrl = nullptr;
	m_active_cam = nullptr;
}

Controller::~Controller()
{
	m_pStaticInstance = nullptr;
}

void Controller::Create()
{
	// Create set of camera Ctrls

}
void Controller::Destroy()
{
	for( int32 i = 0; i < m_cam_ctrls.size(); ++i )
		BB_DELETE( m_cam_ctrls[i] );
	m_cam_ctrls.clear();
}

void Controller::Tick( TickContext& tick_ctxt )
{
	// Manage inputs
	if( m_active_cam_ctrl && m_active_cam )
	{
		for( int32 i = 0; i < m_frame_inputs.size(); ++i )
		{
			m_active_cam_ctrl->OnControllerInput( m_active_cam, m_frame_inputs[i] );
		}
	}
	m_frame_inputs.clear();

    if (m_active_cam_ctrl)
        m_active_cam_ctrl->TickMouseState(m_frame_mouse_state);

	UpdateRenderCamera( tick_ctxt.m_delta_seconds );
}

void Controller::RegisterCameraCtrl( CameraCtrl_Base* cam_ctrl )
{
	m_cam_ctrls.push_back( cam_ctrl );
}

void Controller::SetActiveCameraCtrl( Name const& cam_ctrl_name )
{
	for( int32 i = 0; i < m_cam_ctrls.size(); ++i )
	{
		if( m_cam_ctrls[i]->GetClassName() == cam_ctrl_name )
		{
			m_active_cam_ctrl = m_cam_ctrls[i];
			break;
		}
	}
}

CameraCtrl_Base* Controller::GetCameraCtrl( Name const& cam_ctrl_name )
{
	for( int32 i = 0; i < m_cam_ctrls.size(); ++i )
	{
		if( m_cam_ctrls[i]->GetClassName() == cam_ctrl_name )
		{
			return m_cam_ctrls[i];
		}
	}
	return nullptr;
}

void Controller::AddCamera( Camera* camera )
{
	if( INDEX_NONE == m_cameras.find( camera ) )
		m_cameras.push_back( camera );
}

void Controller::RemoveCamera( Camera* camera )
{
	int camera_idx = m_cameras.find( camera );
	if( INDEX_NONE != camera_idx )
		m_cameras.remove( camera );
}

void Controller::UpdateRenderCamera( float delta_seconds )
{
	if( !m_active_cam && m_cameras.size() )
	{
		m_active_cam = m_cameras[0];
	}

	CameraView* current_view = m_active_cam ? &m_active_cam->GetView() : &m_render_view;
	if( m_active_cam_ctrl )
		m_active_cam_ctrl->UpdateView( *current_view, delta_seconds );

	m_render_view = *current_view;

	// Compute proj matrix
	Engine::DisplayMode display_mode = Engine::Get()->GetDisplayMode();
	m_render_view.m_parameters[eCP_ASPECT] = (float)display_mode.width / (float)display_mode.height;
	m_render_proj_matrix = mat4::perspective(m_render_view.m_parameters[eCP_FOV], (float)display_mode.width / (float)display_mode.height, m_render_view.m_parameters[eCP_NEAR], m_render_view.m_parameters[eCP_FAR]);
}

void Controller::OnInputX( uint32 modifier_flags, float delta )
{
	OnInputXYZ( modifier_flags, vec3( delta, 0.f, 0.f ) );
}

void Controller::OnInputY( uint32 modifier_flags, float delta )
{
	OnInputXYZ( modifier_flags, vec3( 0.f, delta, 0.f ) );
}

void Controller::OnInputZ( uint32 modifier_flags, float delta )
{
	OnInputXYZ( modifier_flags, vec3( 0.f, 0.f, delta ) );
}

void Controller::OnInputXYZ( uint32 modifier_flags, vec3 delta )
{
    eControllerInputType input_type = (modifier_flags & eIM_Ctrl ? eCIT_KeyCtrl : (modifier_flags & eIM_Alt ? eCIT_KeyAlt : (modifier_flags & eIM_Shift ? eCIT_KeyShift : eCIT_Key)));
    
	int Idx = m_frame_inputs.FindByKey( input_type );
	if( Idx == INDEX_NONE )
	{
		ControllerInput input;
		input.m_delta = delta;
		input.m_type = input_type;
		m_frame_inputs.push_back( input );
	}
	else
	{
		ControllerInput& input = m_frame_inputs[Idx];
		input.m_delta += delta;
	}
}

void Controller::OnMouseMove( uint32 modifier_flags, vec3 delta )
{
	eControllerInputType input_type = (modifier_flags & eIM_Ctrl ? eCIT_MouseCtrl : eCIT_Mouse);
	//BB_LOG( Inputs, Log, "----MouseDelta x=%f y=%f CTRL=%d", Delta.x, Delta.y, ModifierFlags & KMOD_CTRL ? 1 : 0 );
	int Idx = m_frame_inputs.FindByKey( input_type );
	if( Idx == INDEX_NONE )
	{
		ControllerInput input;
		input.m_delta = delta;
		input.m_type = input_type;
		m_frame_inputs.push_back( input );
	}
	else
	{
		ControllerInput& input = m_frame_inputs[Idx];
		input.m_delta += delta;
	}
}

void Controller::SetMouseState(uint32 modifier_flags, bool left_down, bool right_down, bool middle_down, int32 mouse_x, int32 mouse_y)
{
	m_frame_mouse_state.m_modifier_flags = modifier_flags;
	m_frame_mouse_state.m_left_down = left_down;
	m_frame_mouse_state.m_right_down = right_down;
	m_frame_mouse_state.m_middle_down = middle_down;
	m_frame_mouse_state.m_mouse_x = mouse_x;
	m_frame_mouse_state.m_mouse_y = mouse_y;
}

}