

#ifndef BB_ENGINE_H
#define BB_ENGINE_H

#include "entry/entry.h"

namespace bigfx
{

class BIGFX_API BaseManager;
//BIGBALL_TEMPLATE template class BIGFX_API Array< BaseManager* >;

struct BIGFX_API CommandLine
{
	void Parse( int argc, char* argv[] );
	bool IsCommand( String& cmd_type );
	bool GetTokenValue( String const& key, String& value );

	String			cmd_exec;
	Array<String>	tokens;
	Array<String>	switches;
};

struct BIGFX_API EngineInitParams
{
	bool create_window;
	bool resizable_window;
	bool mouse_capture;
	uint32 default_res_x;
	uint32 default_res_y;
	int32 argc;
	const char* const* argv;
	entry::AppI* app;

	EngineInitParams() :
		create_window(true),
		resizable_window(false),
		mouse_capture(true),
		default_res_x(800),
		default_res_y(600),
		argc(0),
		argv(nullptr),
		app(nullptr)
	{}
};

//////////////////////////////////////////////////////////////////////////
class BIGFX_API Engine
{
public:
					Engine();
	virtual			~Engine();

	virtual bool	Init(EngineInitParams const& init_params);
	virtual void	Shutdown();
	virtual bool	MainLoop();
	virtual bool	RunCommand( String const& cmd_type, Array<String> const& switches, Array<String> const& tokens );
    virtual int     HandleInputs(float delta_seconds);
	virtual void	RegisterInputBindings();

	struct DisplayMode
	{
		uint32	width = 0;
		uint32	height = 0;
	};
	DisplayMode			GetDisplayMode()	{ return m_display_mode; }
	static Engine*		Get()				{ return ms_engine; }
	CommandLine&		GetCommandLine()	{ return m_cmd_line; }


protected:
	static Engine*			ms_engine;
	DisplayMode				m_display_mode;
	CommandLine				m_cmd_line;
	EngineInitParams		m_init_params;

	entry::MouseState		m_mouseState;
	uint32_t				m_debug;
	uint32_t				m_reset;

	Array<BaseManager*>		m_managers;

public:
	uint32					m_frame_count;
	uint32					m_render_mode;
	bool					m_show_culling;

protected:
	virtual void	InitManagers();
	virtual void	DestroyManagers();
	virtual void	DeclareComponentsAndEntities();
	virtual void	CreateGameCameras() = 0;
	//void			InitImGui();
	virtual void	ResizeWindow(int w, int h);
    virtual void    PreTickManagers( struct TickContext& tick_ctxt );

};

} /* namespace bigfx */

#endif  // BB_ENGINE_H
