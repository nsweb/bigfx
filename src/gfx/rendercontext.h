


#ifndef BB_GFXRENDERCONTEXT_H
#define BB_GFXRENDERCONTEXT_H

#include "../engine/camera.h"

namespace bigfx
{

class BIGFX_API Shader;
//BIGBALL_TEMPLATE template class BIGFX_API Map<String,Shader*>;

struct BIGFX_API RenderContext
{
	enum eRenderMode
	{
		eRM_Lit,
		eRM_Wireframe,
		eRM_ColorCell,
		eRM_DataCell,
	};

	CameraView	m_view;
	CameraView*	m_pfrustum_view;
	mat4		m_view_mat;
    mat4		m_proj_mat;
	float		m_delta_seconds;
	uint32		m_frame_idx;
	uint32		m_render_mode;
};

} /* namespace bigfx */

#endif // BB_GFXRENDERCONTEXT_H
