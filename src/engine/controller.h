


#ifndef BB_CONTROLLER_H
#define BB_CONTROLLER_H

#include "basemanager.h"
#include "camera.h"

namespace bigfx
{

enum eControllerInputModifier
{
	eIM_Ctrl = 0x00000001,
	eIM_Alt = 0x00000002,
	eIM_Shift = 0x00000004,
};

enum eControllerInputType
{
	eCIT_Key = 0,
	eCIT_KeyCtrl,
    eCIT_KeyAlt,
    eCIT_KeyShift,
	eCIT_Mouse,
	eCIT_MouseCtrl,
};

struct ControllerInput
{
	vec3				m_delta;
	eControllerInputType			m_type;

	bool operator == ( eControllerInputType other_type ) const		{ return m_type == other_type; }
};

struct ControllerMouseState
{
	uint32	m_modifier_flags;
	bool	m_left_down;
	bool	m_right_down;
	bool	m_middle_down;
	int32	m_mouse_x;
	int32	m_mouse_y;
};


//////////////////////////////////////////////////////////////////////////
class BIGFX_API Controller : public BaseManager 
{
	STATIC_MANAGER_H(Controller)

public:

						Controller();
	virtual				~Controller();

	virtual void		Create();
	virtual void		Destroy();	
	virtual void		Tick( struct TickContext& tick_ctxt );

	void				RegisterCameraCtrl( CameraCtrl_Base* cam_ctrl );
	void				SetActiveCameraCtrl( Name const& cam_ctrl_name );
	CameraCtrl_Base*	GetActiveCameraCtrl()			{ return m_active_cam_ctrl;			}
	CameraCtrl_Base*	GetCameraCtrl( Name const& cam_ctrl_name );
	void				AddCamera( Camera* camera );
	void				RemoveCamera( Camera* camera );
	void				UpdateRenderCamera( float delta_seconds );
	Camera*				GetActiveCamera()				{ return m_active_cam;				}
    void                SetActiveCamera(Camera* cam)    { m_active_cam = cam;               }

	void				OnInputX( uint32 modifier_flags, float delta );
	void				OnInputY( uint32 modifier_flags, float delta );
	void				OnInputZ( uint32 modifier_flags, float delta );
	void				OnMouseMove( uint32 modifier_flags, vec3 delta );
	void				SetMouseState(uint32 modifier_flags, bool left_down, bool right_down, bool middle_down, int32 mouse_x, int32 mouse_y);
	CameraView const&	GetRenderView()					{ return m_render_view;				}
	mat4 const&			GetRenderProjMatrix()			{ return m_render_proj_matrix;		}
	ControllerMouseState const&	GetMouseState()			{ return m_frame_mouse_state;		}

protected:
	Array<ControllerInput>		m_frame_inputs;
	Array<Camera*>				m_cameras;
	Array<CameraCtrl_Base*>		m_cam_ctrls;
	ControllerMouseState		m_frame_mouse_state;
	Camera*						m_active_cam;
	CameraCtrl_Base*			m_active_cam_ctrl;
	CameraView					m_render_view;
	mat4						m_render_proj_matrix;

	void				OnInputXYZ( uint32 modifier_flags, vec3 delta );

};

} /* namespace bigfx */

#endif // BB_CONTROLLER_H
