#ifndef BB_UIMANAGER_H
#define BB_UIMANAGER_H

#include "../gfx/dynamicvb.h"

namespace bigfx
{

typedef void (*DrawEditorCB)( bool*, struct RenderContext& );
typedef void (*ToggleEditorCB)( bool );
typedef void (*DrawCustomMenuBarCB)(struct RenderContext&);
    
    
class BIGFX_API Shader;

struct UIVertex
{
	vec2	m_os;
	vec2	m_tex;
	u8vec4	m_col;
};

//////////////////////////////////////////////////////////////////////////
class BIGFX_API UIManager
{

public:

						UIManager();
	virtual				~UIManager();

	virtual void		Create();
	virtual void		Destroy();	
	virtual void		Tick( struct TickContext& tick_ctxt );
	virtual void		_Render( struct RenderContext& render_ctxt );
	static UIManager*	GetStaticInstance()		{ return m_static_instance; }
    //void                NewFrame(); //ZZZ

	void				RenderDrawLists(struct ImDrawData* data);
	void				ToggleDebugMenu();
    void                ToggleEditor();
    void                SetDrawEditorFn( DrawEditorCB fn )          { m_draw_editor_fn = fn; }
    void                SetToggleEditorFn( ToggleEditorCB fn )      { m_toggle_editor_fn = fn; }
	void                SetDrawCustomMenuFn(DrawCustomMenuBarCB fn) { m_draw_custom_menu_fn = fn; }

protected:

	void				DrawDebugMenu();
	void				DrawProfiler();

	static UIManager*		m_static_instance;

public:
    DrawEditorCB            m_draw_editor_fn;
    ToggleEditorCB          m_toggle_editor_fn;
	DrawCustomMenuBarCB		m_draw_custom_menu_fn;

	bool					m_show_debug_menu;
	bool					m_show_profiler;
    bool                    m_show_editor;
};

} /* namespace bigfx */

#endif // BB_UIMANAGER_H