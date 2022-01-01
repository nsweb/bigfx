
#include "../bigfx.h"
#include "camera.h"
#include "controller.h"
#include "../core/json.h"

namespace bigfx
{

CLASS_EQUIP_CPP(Camera);
CLASS_EQUIP_CPP(CameraCtrl_Base);
CLASS_EQUIP_CPP(CameraCtrl_Fly);

CameraView::CameraView() :
	m_transform( dquat(1.), dvec3(0.), 1. )
{
	m_parameters[eCP_FOV] = 55.0f;
	m_parameters[eCP_ASPECT] = 16.0f / 9.0f;
	m_parameters[eCP_NEAR] = 0.01f;
	m_parameters[eCP_FAR] = 100.0f;
}

Camera::Camera()
{

}

Camera::~Camera()
{
}

void Camera::Create( EntityPattern* pattern, class json::Object* proto, Name in_name )
{
	Super::Create(pattern, proto, in_name);

	json::TokenIdx ent_tok = proto->GetToken("entity", json::OBJECT);
	json::TokenIdx cam_tok = proto->GetToken("camera", json::OBJECT, ent_tok);
	if (cam_tok != INDEX_NONE)
	{
		char const* param_names[] = { "fov", "near", "far" };
		for (int32 i = 0; i < COUNT_OF(param_names); ++i)
		{
			json::TokenIdx param_tok = proto->GetToken(param_names[i], json::PRIMITIVE, cam_tok);
			if (param_tok != INDEX_NONE)
				m_view.m_parameters[i] = proto->GetFloatValue(param_tok, m_view.m_parameters[i]);
		}
	}
}
void Camera::Destroy()
{
	Super::Destroy();
}
void Camera::AddToWorld()
{
	Super::AddToWorld();

	Controller::GetStaticInstance()->AddCamera( this );
}
void Camera::RemoveFromWorld()
{
	Controller::GetStaticInstance()->RemoveCamera( this );

	Super::RemoveFromWorld();
}

void Camera::Tick( float DeltaSeconds )
{
}

void Camera::SetPosition( dvec3 position )
{
	m_view.m_transform.SetTranslation( position );
}
void Camera::SetRotation( dquat rotation )
{
	m_view.m_transform.SetRotation( rotation );
}

#if 0


//-------------------------------------------------------------------------------
void Camera::Tick( float DeltaSeconds )
{
	const float fINTERPOLATIONFACTOR = 4.0f;

	float fPrecalc = ( 1.0f - exp( -DeltaSeconds * fINTERPOLATIONFACTOR ) );

	m_mTarget[eReference_CURRENT] = m_mTarget[eReference_DESIRED];
	m_TargetPosition = m_mTarget[eReference_CURRENT][3].xyz;

	// Lerp on targetdist
	m_fTargetDist[eReference_CURRENT] += fPrecalc*(m_fTargetDist[eReference_DESIRED] - m_fTargetDist[eReference_CURRENT]);

	// Lerp sur le yaw et le pitch
	float fYawDist = m_fYaw[eReference_DESIRED] - m_fYaw[eReference_CURRENT];
	if( fYawDist < 0.0f )	fYawDist += F_PI*2.0f;
	// Prendre le sens de rotation le plus court
	if( fYawDist > F_PI )
		fYawDist = fYawDist - F_PI*2.0f;
	m_fYaw[eReference_CURRENT] = fmod( m_fYaw[eReference_CURRENT] + fPrecalc*fYawDist, F_PI*2.0f );
	//if( m_fYaw[eReference_CURRENT] > F_PI )	
	//	m_fYaw[eReference_CURRENT] -= 2.0f*F_PI;
	if( m_fYaw[eReference_CURRENT] < 0.0f )
		m_fYaw[eReference_CURRENT] += 2.0f*F_PI;


	// Lerp sur le yaw et le pitch
	/*float fYawDist = vYaw[EReference_DESIRED] - vYaw[EReference_CURRENT];
	if( fYawDist < 0.0f )	fYawDist += rwPI*2.0f;
	// Prendre le sens de rotation le plus court
	if( fYawDist > rwPI )
		fYawDist = fYawDist - rwPI*2.0f;
	float fYaw = fmodf( vYaw[EReference_CURRENT] + fPrecalc*fYawDist, rwPI*2.0f );
	if( fYaw > rwPI )	fYaw -= 2.0f*rwPI;*/

	float fPitchDist = m_fPitch[eReference_DESIRED] - m_fPitch[eReference_CURRENT];
	if( fPitchDist < 0.0f )	fPitchDist += F_PI_2;
	// Prendre le sens de rotation le plus court
	if( fPitchDist > F_PI )
		fPitchDist = fPitchDist - F_PI_2;
	m_fPitch[eReference_CURRENT] = fmod( m_fPitch[eReference_CURRENT] + fPrecalc*fPitchDist, F_PI*2.0f );
	if( m_fPitch[eReference_CURRENT] > F_PI )	
		m_fPitch[eReference_CURRENT] -= F_PI_2;

	vec3 v3Dir( cosf( m_fYaw[eReference_CURRENT] )*cosf( m_fPitch[eReference_CURRENT] ), cosf( m_fPitch[eReference_CURRENT] )*sinf( m_fYaw[eReference_CURRENT] ), sinf( m_fPitch[eReference_CURRENT] ) );
	m_Position = /*m_TargetPosition*/ - v3Dir*m_fTargetDist[eReference_CURRENT];

	// Passer en coordonn�es world
	m_Position = (m_mTarget[eReference_CURRENT] * vec4( m_Position, 1.0f )).xyz;
	//D3DXVec3TransformCoord( &m_Position, &m_Position, &m_mTarget[eReference_CURRENT] );

	// Mettre � jour les matrices correspondantes
	UpdateMatrix();
}

#endif
    
void CameraCtrl_Base::UpdateView( CameraView& cam_view, float delta_seconds )
{
        
}

bool CameraCtrl_Base::OnControllerInput( Camera* camera, ControllerInput const& input )
{
	return false;
}

void CameraCtrl_Base::TickMouseState(ControllerMouseState const& mouse_state)
{

}

CameraCtrl_Fly::CameraCtrl_Fly() :
	m_strafe_speed(200.0f),
	m_rotation_speed(1000.0f)
{

}

void CameraCtrl_Fly::UpdateView(CameraView& cam_view, float delta_seconds)
{

}

bool CameraCtrl_Fly::OnControllerInput(Camera* camera, ControllerInput const& input)
{
	CameraView& view = camera->GetView();

	if (input.m_type == eCIT_Key)
	{
		mat4 cam_to_world_mat(view.m_transform.GetRotation());
		vec3 right = cam_to_world_mat.v0.xyz;
		vec3 up = cam_to_world_mat.v1.xyz;
		vec3 front = -cam_to_world_mat.v2.xyz;
		view.m_transform.GetTranslation() += (right * input.m_delta.x + up * input.m_delta.z + front * input.m_delta.y) * m_strafe_speed;
	}
	else if (input.m_type == eCIT_Mouse)
	{
		// TODO : arcball rotation
		quat YawPitchRoll(quat::fromeuler_xyz(input.m_delta.y * m_rotation_speed, input.m_delta.x * m_rotation_speed, 0.0f));

		view.m_transform.SetRotation(view.m_transform.GetRotation() * YawPitchRoll);
	}

	return true;
}

} /* namespace bigfx */