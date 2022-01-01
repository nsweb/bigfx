

#ifndef BB_CAMERA_H
#define BB_CAMERA_H

#include "entity.h"

namespace bigfx
{

struct ControllerMouseState;

namespace json
{
	class Object;
};

enum BIGFX_API eCameraParam
{
	eCP_FOV = 0,
	eCP_NEAR,
	eCP_FAR,
	eCP_ASPECT,
	eCP_MAX
};

class BIGFX_API CameraView
{
public:
					CameraView();

	dtransform	m_transform;
	float		m_parameters[eCP_MAX];
};

class BIGFX_API Camera : public Entity
{
	CLASS_EQUIP_H(Camera, Entity)

public:
					Camera();
					~Camera();

	// Begin : Entity interface
	static Entity*	NewEntity()		{ return new Camera();	}
	virtual void	Create( EntityPattern* pattern, class json::Object* proto = nullptr, Name in_name = Name() ) override;
	virtual void	Destroy() override;
	virtual void	AddToWorld() override;
	virtual void	RemoveFromWorld() override;
	virtual void	Tick( float DeltaSeconds ) override;
	// End : Entity interface

	void			SetPosition( dvec3 Position );
	dvec3			GetPosition()		{ return m_view.m_transform.GetTranslation(); }
	void			SetRotation( dquat Rotation );
	dquat			GetRotation()		{ return m_view.m_transform.GetRotation(); }
	CameraView&		GetView()			{ return m_view; }

protected:
	CameraView			m_view;
};

//////////////////////////////////////////////////////////////////////////
class BIGFX_API CameraCtrl_Base
{
	CLASS_EQUIP_BASE_H(CameraCtrl_Base)

public:
	virtual void		UpdateView( CameraView& cam_view, float delta_seconds );
	virtual bool		OnControllerInput( Camera* camera, struct ControllerInput const& input );
	virtual void		TickMouseState(ControllerMouseState const& mouse_state);
};

class BIGFX_API CameraCtrl_Fly : public CameraCtrl_Base
{
	CLASS_EQUIP_H(CameraCtrl_Fly, CameraCtrl_Base)

public:
						CameraCtrl_Fly();

	float			m_strafe_speed;
	float			m_rotation_speed;

public:
	virtual void		UpdateView(CameraView& cam_view, float delta_seconds) override;
	virtual bool		OnControllerInput(Camera* camera, struct ControllerInput const& input) override;
};

} /* namespace bigfx */

#endif // BB_CAMERA_H