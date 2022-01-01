


#ifndef BB_TICKCONTEXT_H
#define BB_TICKCONTEXT_H


namespace bigfx
{


struct BIGFX_API TickContext
{
	TickContext( float delta_seconds = 0.f, uint32 frame_idx = 0 ) :
			m_delta_seconds(delta_seconds),
			m_frame_idx(frame_idx)
			{

			}

	float		m_delta_seconds;
	uint32		m_frame_idx;
};

} /* namespace bigfx */

#endif // BB_TICKCONTEXT_H