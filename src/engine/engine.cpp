// engine.cpp
//

#include "../bigfx.h"
#include "engine.h"
#include <bx/timer.h>

#include "controller.h"
#include "entitymanager.h"
#include "../gfx/gfxmanager.h"
#include "../gfx/rendercontext.h"
#include "../gfx/drawutils.h"
#include "../system/profiler.h"
#include "../ui/uimanager.h"
#include "coposition.h"
#include "camera.h"
#include "tickcontext.h"

// HACK
#include "../system/file.h"

namespace bigfx
{

Engine* Engine::ms_engine = nullptr;

static CameraView g_SavedFrustumView;

Engine::Engine() :
	//m_main_window(nullptr),
	m_frame_count(0),
	m_render_mode(RenderContext::eRM_Lit),
	m_show_culling(false)
{
    ms_engine = this;
}

Engine::~Engine()
{
    ms_engine = nullptr;
}

bool Engine::Init(EngineInitParams const& init_params)
{
	m_init_params = init_params;

 //   if (SDL_Init(SDL_INIT_VIDEO) < 0) /* Initialize SDL's Video subsystem */
	//{
 //       //sdldie("Unable to initialize SDL"); /* Or die on error */
	//	return false;
	//}

 //   /* Request opengl 3.2 context.
 //    * SDL doesn't have the ability to choose which profile at this time of writing,
 //    * but it should default to the core profile */
 //   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
 //   SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
 //   SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);//SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);//SDL_GL_CONTEXT_PROFILE_CORE);

 //   /* Turn on double buffering with a 24bit Z buffer.
 //    * You may need to change this to 16 or 32 for your system */
 //   SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
 //   SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
 //   
 //   SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
 //   SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    /* Create our window centered at 512x512 resolution */
	int res_x = init_params.default_res_x;
	int res_y = init_params.default_res_y;
	String str_value;
	if( m_cmd_line.GetTokenValue( "res_x", str_value ) )
		res_x = std::atoi( str_value.c_str() );
	if( m_cmd_line.GetTokenValue( "res_y", str_value ) )
		res_y = std::atoi( str_value.c_str() );

	//uint32 window_flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
	//if (init_params.resizable_window)
	//	window_flags |= SDL_WINDOW_RESIZABLE;
 //   m_main_window = SDL_CreateWindow("GL Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
	//	res_x, res_y, window_flags);
 //   if( !m_main_window ) /* Die if creation failed */
	//{
 //     //  sdldie("Unable to create window");
	//	return false;
	//}

    /* Create our opengl context and attach it to our window */
    //m_gl_context = SDL_GL_CreateContext( m_main_window );
    //checkSDLError(__LINE__);

	// Init GLEW
	//glewExperimental = GL_TRUE; 
	//glewInit();

	// Show version info
	/*int num_drivers = SDL_GetNumVideoDrivers();
	for (int i = 0; i < num_drivers; i++)
	{
		const char* driver_id = SDL_GetVideoDriver(i);
		BB_LOG(EngineInit, Log, "Driver<%d>: %s", i, driver_id);
	}*/
	
	//const char* driver_id = SDL_GetCurrentVideoDriver();
	//const char* renderer_id = (const char*)glGetString (GL_RENDERER);	// get renderer string
	//const char* version_id = (const char*)glGetString (GL_VERSION);		// version as a string
	//BB_LOG(EngineInit, Log, "Driver: %s", driver_id);
	//BB_LOG(EngineInit, Log, "Renderer: %s", renderer_id);
	//BB_LOG(EngineInit, Log, "OpenGL version supported %s", version_id);

    /* This makes our buffer swap syncronized with the monitor's vertical refresh */
 //   SDL_GL_SetSwapInterval(1);

 //   /* Clear our buffer with a blue background */
 //   glClearColor( 0.1f, 0.1f, 0.3f, 1.0f );
 //   glClear ( GL_COLOR_BUFFER_BIT );
 //   /* Swap our back buffer to the front */
 //   SDL_GL_SwapWindow( m_main_window );

	//SDL_GetWindowDisplayMode( m_main_window, &m_display_mode );

	//// Allow FPS style mouse movement if needed (mouse capture)
	//SDL_SetRelativeMouseMode(init_params.mouse_capture ? SDL_TRUE : SDL_FALSE);
	//SDL_SetWindowGrab(m_main_window, init_params.mouse_capture ? SDL_TRUE : SDL_FALSE);
 //   
 //   // Work around a bug on Mac OS X where window is not displayed correctly
 //   glViewport(0, 0, m_display_mode.w, m_display_mode.h);
 //   SDL_SetWindowSize(m_main_window, m_display_mode.w, m_display_mode.h);

    m_display_mode.width = res_x;
    m_display_mode.height = res_y;

	// Ready to init our managers
	InitManagers();

	// Create drawutils manager
	DrawUtils* draw_utils = new DrawUtils();
	draw_utils->Create();

	// Create UI manager
	UIManager* ui_manager = new UIManager();
	ui_manager->Create();

	// Declare available components and entities
	DeclareComponentsAndEntities();

	// Setup available cameractrl
	CreateGameCameras();
 
	return true;
}

void Engine::Shutdown()
{
	UIManager::GetStaticInstance()->Destroy();
	delete UIManager::GetStaticInstance();

	DrawUtils::GetStaticInstance()->Destroy();
	delete DrawUtils::GetStaticInstance();

	DestroyManagers();

	/* Delete our opengl context, destroy our window, and shutdown SDL */
	//SDL_GL_DeleteContext( m_gl_context );
	//SDL_DestroyWindow( m_main_window );
	//SDL_Quit();
}

//////////////////////////////////////////////////////////////////////////

void Engine::InitManagers()
{
	//WorkerThreadManager* worker_thread_manager = new WorkerThreadManager();
	//worker_thread_manager->Create();
	//m_managers.push_back( worker_thread_manager );

	Controller* controller = new Controller();
	controller->Create();
	m_managers.push_back( controller );

	EntityManager* entity_manager = new EntityManager();
	entity_manager->Create();
	m_managers.push_back( entity_manager );

	//GfxManager* gfx_manager = new GfxManager();
	//gfx_manager->Create();
	//m_managers.push_back( gfx_manager );
}

void Engine::DestroyManagers()
{
	for( int32 i = m_managers.size() - 1; i >=0 ; --i )
	{
		m_managers[i]->Destroy();
		BB_DELETE( m_managers[i] );
	}
	m_managers.clear();
}

void Engine::DeclareComponentsAndEntities()
{
	DECLARE_COMPONENT( CoPosition );
	DECLARE_ENTITYPATTERN( Camera, Camera, (0), (0) );
}

void Engine::CreateGameCameras()
{

}

//////////////////////////////////////////////////////////////////////////
void Engine::MainLoop()
{
	//uint64 old_time, current_time = SDL_GetPerformanceCounter();

	while( 1 )
	{
		//old_time = current_time;
		//current_time = SDL_GetPerformanceCounter();
		//float delta_seconds = (current_time - old_time) / (float)SDL_GetPerformanceFrequency(); // / 1000.0f;

        int64 now = bx::getHPCounter();
        static int64 last = now;
        const int64 frameTime = now - last;
        last = now;
        const float delta_seconds = float(frameTime / double(bx::getHPFrequency()));
        
        int loop_status = 1; // HandleEvents(delta_seconds);

		//glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
		//// tell GL to only draw onto a pixel if the shape is closer to the viewer
		//glEnable( GL_DEPTH_TEST ); // enable depth-testing
		//glDepthFunc( GL_LESS ); // depth-testing interprets a smaller value as "closer"
  //      //glDepthRange(0.f, 1.f);
		//glEnable( GL_CULL_FACE );
		//glCullFace( GL_BACK );

		//if( m_render_mode == RenderContext::eRM_Wireframe )
		//	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		//else
		//	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

		TickContext tick_ctxt( delta_seconds, m_frame_count );
        
        PreTickManagers(tick_ctxt);
		for( int32 i = 0; i < m_managers.size(); ++i )
		{
			m_managers[i]->Tick( tick_ctxt );
		}

		// Prepare rendering
		RenderContext render_ctxt;
        CameraView view = Controller::GetStaticInstance()->GetRenderView();
		render_ctxt.m_view = view;
		render_ctxt.m_pfrustum_view = (m_show_culling ? &g_SavedFrustumView : nullptr);
        mat4 cam_to_world_mat(view.m_transform.GetRotation(), view.m_transform.GetTranslation(), (float)view.m_transform.GetScale());
        render_ctxt.m_view_mat = bigfx::inverse(cam_to_world_mat);
		render_ctxt.m_proj_mat = Controller::GetStaticInstance()->GetRenderProjMatrix();
		render_ctxt.m_delta_seconds = delta_seconds;
		render_ctxt.m_frame_idx = m_frame_count++;
		render_ctxt.m_render_mode = m_render_mode;
		for( int32 i = 0; i < m_managers.size(); ++i )
		{
			m_managers[i]->_Render( render_ctxt );
		}

		DrawUtils::GetStaticInstance()->_Render(render_ctxt);

		//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		UIManager::GetStaticInstance()->_Render(render_ctxt);

		//SDL_GL_SwapWindow( m_main_window );

		//// Reset profiler data for next frame
		//PROFILE_THREAD_FRAMERESET()

		if( loop_status == 1 ) // if received instruction to quit
			break;
	}
}
    
void Engine::PreTickManagers( struct TickContext& tick_ctxt )
{
        
}

#if 0
int Engine::HandleEvents(float delta_seconds)
{
    int loop_status = 0;
    
    // Handle SDL events & inputs
    SDL_Event event;
    const uint8* keys = SDL_GetKeyboardState( nullptr );
    uint32 modifiers = 0;
    if( keys[SDL_SCANCODE_LCTRL] || keys[SDL_SCANCODE_RCTRL] )
        modifiers |= eIM_Ctrl;
    if( keys[SDL_SCANCODE_LSHIFT] || keys[SDL_SCANCODE_RSHIFT] )
        modifiers |= eIM_Shift;
    if( keys[SDL_SCANCODE_LALT] || keys[SDL_SCANCODE_RALT] )
        modifiers |= eIM_Alt;
    
    if( keys[SDL_SCANCODE_LEFT] )
        Controller::GetStaticInstance()->OnInputX( modifiers, -delta_seconds );
    if( keys[SDL_SCANCODE_RIGHT] )
        Controller::GetStaticInstance()->OnInputX( modifiers, delta_seconds );
    if( keys[SDL_SCANCODE_UP] )
        Controller::GetStaticInstance()->OnInputY( modifiers, delta_seconds );
    if( keys[SDL_SCANCODE_DOWN] )
        Controller::GetStaticInstance()->OnInputY( modifiers, -delta_seconds );
    if( keys[SDL_SCANCODE_PAGEUP] || keys[SDL_SCANCODE_U]  )
        Controller::GetStaticInstance()->OnInputZ( modifiers, delta_seconds );
    if( keys[SDL_SCANCODE_PAGEDOWN] || keys[SDL_SCANCODE_D]  )
        Controller::GetStaticInstance()->OnInputZ( modifiers, -delta_seconds );
    
    //ZZZ
    //ImGuiIO& io = ImGui::GetIO();
    //// Setup inputs for imgui
    //{
    //    io.MouseWheel = 0;
    //    io.DeltaTime = bigfx::max(0.000001f, delta_seconds);
    //    
    //    int mouse_x, mouse_y;
    //    uint32 mouse_state = SDL_GetMouseState(&mouse_x, &mouse_y);
    //    bool left_down = (mouse_state & SDL_BUTTON_LMASK ? true : false);
    //    bool right_down = (mouse_state & SDL_BUTTON_RMASK ? true : false);
    //    bool middle_down = (mouse_state & SDL_BUTTON_MMASK ? true : false);
    //    io.MousePos = ImVec2((float)mouse_x, (float)mouse_y);
    //    io.MouseDown[0] = left_down;
    //    io.MouseDown[1] = right_down;
    //    io.MouseDown[2] = middle_down;
    //}
    
    while( SDL_PollEvent( &event ) )
    {
        switch( event.type )
        {
            case SDL_KEYDOWN:
            {
                // Warn ImGui
                //ZZZ
                //int key = event.key.keysym.sym & ~SDLK_SCANCODE_MASK;
                //io.KeysDown[key] = (event.type == SDL_KEYDOWN);
                //io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
                //io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
                //io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
            }
                break;
            case SDL_KEYUP:
            {
                // if escape is pressed, quit
                if( event.key.keysym.sym == SDLK_ESCAPE )
                    loop_status = 1; // set status to 1 to exit main loop
                else if( event.key.keysym.sym >= SDLK_F1 && event.key.keysym.sym <= SDLK_F4 )
                {
                    m_render_mode = event.key.keysym.sym - SDLK_F1;
                }
                else if( event.key.keysym.sym == SDLK_F5 )
                {
                    UIManager::GetStaticInstance()->ToggleDebugMenu();
                }
                else if( event.key.keysym.sym == SDLK_e )
                {
                    UIManager::GetStaticInstance()->ToggleEditor();
                }
                else if( event.key.keysym.sym == SDLK_F9 || event.key.keysym.sym == SDLK_F10 )
                {
                    // HACK
                    bool is_write = event.key.keysym.sym == SDLK_F9 ? true : false;
                    
                    File out_file;
                    if( out_file.Open("../save/camera_debug.bin", is_write) )
                    {
                        Camera* current_cam = Controller::GetStaticInstance()->GetActiveCamera();
                        if( current_cam )
                        {
                            CameraView& cam_view = current_cam->GetView();
                            out_file.Serialize(&cam_view.m_transform, sizeof(cam_view.m_transform));
                            out_file.Serialize( &cam_view.m_parameters, sizeof(cam_view.m_parameters) );
                        }
                    }
                }
                else if( event.key.keysym.sym == SDLK_c )
                {
                    m_show_culling = !m_show_culling;
                    if( m_show_culling )
                        g_SavedFrustumView = Controller::GetStaticInstance()->GetRenderView();
                }
                
                // Warn ImGui
                //ZZZ
                //int key = event.key.keysym.sym & ~SDLK_SCANCODE_MASK;
                //io.KeysDown[key] = (event.type == SDL_KEYDOWN);
                //io.KeyShift = ((SDL_GetModState() & KMOD_SHIFT) != 0);
                //io.KeyCtrl = ((SDL_GetModState() & KMOD_CTRL) != 0);
                //io.KeyAlt = ((SDL_GetModState() & KMOD_ALT) != 0);
            }
                break;
            case SDL_TEXTINPUT:
            {
                //ZZZ
                //io.AddInputCharactersUTF8(event.text.text);
            }
                break;
            case SDL_MOUSEMOTION:
            {
                if( !UIManager::GetStaticInstance()->m_show_debug_menu )
                {
                    vec3 mouse_delta(	-(float)event.motion.xrel / (float)m_display_mode.w,
                                     -(float)event.motion.yrel / (float)m_display_mode.h,
                                     0.f );
                    //BB_LOG( Inputs, Log, "MouseDelta x=%f y=%f Mouse x=%d y=%d xrel=%d yrel=%d mod=%d", mouse_delta, mouse_delta, event.motion.x, event.motion.y, event.motion.xrel, event.motion.yrel, event.key.keysym.mod );
                    Controller::GetStaticInstance()->OnMouseMove( modifiers, mouse_delta * delta_seconds );
                }
            }
                break;
            case SDL_MOUSEBUTTONDOWN:
                break;
            case SDL_MOUSEBUTTONUP:
                break;
                //case SDL_RENDER_TARGETS_RESET:
                //break;
            case SDL_WINDOWEVENT:
            {
                switch (event.window.event)
                {
                    case SDL_WINDOWEVENT_RESIZED:
                        SDL_Log("Window %d resized to %dx%d",
                                event.window.windowID, event.window.data1,
                                event.window.data2);
                        ResizeWindow(event.window.data1, event.window.data2);
                        break;
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        SDL_Log("Window %d size changed to %dx%d",
                                event.window.windowID, event.window.data1,
                                event.window.data2);
                        break;
                }
                break;
            }
            case SDL_QUIT:
                loop_status = 1;
                break;
        }
    }
    
    // Inputs have been set up, proceed with a new frame for UI
    //ZZZ
    //UIManager::GetStaticInstance()->NewFrame();
    
    // Set mouse state
    //ZZZ
    //bool controller_left_down = io.MouseDown[0] && !io.MouseDownOwned[0];
    //bool controller_right_down = io.MouseDown[1] && !io.MouseDownOwned[1];
    //bool controller_middle_down = io.MouseDown[2] && !io.MouseDownOwned[2];
	//Controller::GetStaticInstance()->SetMouseState(modifiers, controller_left_down, controller_right_down, controller_middle_down, (int32)io.MousePos.x, (int32)io.MousePos.y);
    
    
    return loop_status;
}
#endif //0
    
bool Engine::RunCommand( String const& cmd_type, Array<String> const& switches, Array<String> const& tokens )
{
    return false;
}

void Engine::ResizeWindow(int w, int h)
{
	m_display_mode.width = w;
	m_display_mode.height = h;
	//SDL_SetWindowDisplayMode(m_main_window, &m_display_mode);
 //   
 //   glViewport(0,0,w,h);
    
    //ZZZ
    //ImGuiIO& io = ImGui::GetIO();
    //io.DisplaySize = ImVec2((float)w, (float)h);  // Display size, in pixels. For clamping windows positions.
    
    /*
    SDL_CreateRenderer();
    SDL_Rect topLeftViewport;
    topLeftViewport.x = 0;
    topLeftViewport.y = 0;
    topLeftViewport.w = SCREEN_WIDTH / 2;
    topLeftViewport.h = SCREEN_HEIGHT / 2;
    SDL_RenderSetViewport( gRenderer, &topLeftViewport );*/
}

//////////////////////////////////////////////////////////////////////////
void CommandLine::Parse( int argc, char* argv[] )
{
	if (argc)
		cmd_exec = argv[0];
	else
		cmd_exec = "";

	String str_arg;
	for( int32 arg_idx = 1; arg_idx < argc; ++arg_idx )
	{
		str_arg = argv[arg_idx];
		if( str_arg[0] == '-' )
		{
			// switch
			switches.push_back( str_arg.c_str() + 1 );
		}
		else
		{
			// token
			tokens.push_back( str_arg );
		}
	}
}

bool CommandLine::IsCommand( String& cmd_type )
{
	if( tokens.size() > 0 && tokens[0].StartsWith( "cmd=" ) )
	{
		cmd_type = tokens[0].Sub( 4, tokens[0].Len() - 4 );
		return true;
	}
	return false;
}

bool CommandLine::GetTokenValue( String const& key, String& value )
{
	for( int tok_idx = 0; tok_idx < tokens.size(); tok_idx++)
	{
		String const& token = tokens[tok_idx];
		int sep_idx = token.IndexOf( "=" );
		if( sep_idx > 0 )
		{
			String keyword = token.Sub( 0, sep_idx );
			if( keyword == key )
			{
				value = token.Sub(sep_idx+1, token.Len() - sep_idx - 1 );
				return true;
			}
		}
	}
	return false;
}

}
