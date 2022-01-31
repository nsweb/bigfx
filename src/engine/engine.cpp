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

#include "imgui/imgui.h"
#include "entry/input.h"
#include "entry/cmd.h"

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
    Args args(init_params.argc, init_params.argv);

	m_init_params = init_params;

    uint32 res_x = init_params.default_res_x;
    uint32 res_y = init_params.default_res_y;
    String str_value;
    if (m_cmd_line.GetTokenValue("res_x", str_value))
        res_x = std::atoi(str_value.c_str());
    if (m_cmd_line.GetTokenValue("res_y", str_value))
        res_y = std::atoi(str_value.c_str());

    m_display_mode.width = res_x;
    m_display_mode.height = res_y;

    m_debug = BGFX_DEBUG_NONE;
    m_reset = BGFX_RESET_VSYNC;

    bgfx::Init init;
    init.type = args.m_type;
    init.vendorId = args.m_pciId;
    init.resolution.width = res_x;
    init.resolution.height = res_y;
    init.resolution.reset = m_reset;
    bgfx::init(init);

    entry::setCurrentDir("../data/");

    // Enable debug text.
    bgfx::setDebug(m_debug);

    // Set view 0 clear state.
    bgfx::setViewClear(0
        , BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH
        , 0x303030ff
        , 1.0f
        , 0
    );

    imguiCreate();

	//// Allow FPS style mouse movement if needed (mouse capture)
	//SDL_SetRelativeMouseMode(init_params.mouse_capture ? SDL_TRUE : SDL_FALSE);
	//SDL_SetWindowGrab(m_main_window, init_params.mouse_capture ? SDL_TRUE : SDL_FALSE);

	// Ready to init our managers
	InitManagers();

	// Create drawutils manager
	DrawUtils* draw_utils = new DrawUtils();
	draw_utils->Create();

	// Create UI manager
	UIManager* ui_manager = new UIManager();
	ui_manager->Create();

    // Register input shortcuts
    RegisterInputBindings();

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

    imguiDestroy();

    // Shutdown bgfx.
    bgfx::shutdown();
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
bool Engine::MainLoop()
{
    if (!entry::processEvents(m_display_mode.width, m_display_mode.height, m_debug, m_reset, &m_mouseState))
    {
        // Set view 0 default viewport.
        bgfx::setViewRect(0, 0, 0, uint16_t(m_display_mode.width), uint16_t(m_display_mode.height));

        // Set view 1 default viewport.
        //bgfx::setViewRect(1, 0, 0, uint16_t(m_width), uint16_t(m_height) );

        // This dummy draw call is here to make sure that view 0 is cleared
        // if no other draw calls are submitted to viewZ 0.
        bgfx::touch(0);

        int64 now = bx::getHPCounter();
        static int64 last = now;
        const int64 frameTime = now - last;
        last = now;
        const float delta_seconds = float(frameTime / double(bx::getHPFrequency()));
        TickContext tick_ctxt(delta_seconds, m_frame_count);

        PreTickManagers(tick_ctxt);
        for (int32 i = 0; i < m_managers.size(); ++i)
        {
            m_managers[i]->Tick(tick_ctxt);
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
        for (int32 i = 0; i < m_managers.size(); ++i)
        {
            m_managers[i]->_Render(render_ctxt);
        }

        DrawUtils::GetStaticInstance()->_Render(render_ctxt);

        // Render UI
        imguiBeginFrame(m_mouseState.m_mx
            , m_mouseState.m_my
            , (m_mouseState.m_buttons[entry::MouseButton::Left] ? IMGUI_MBUT_LEFT : 0)
            | (m_mouseState.m_buttons[entry::MouseButton::Right] ? IMGUI_MBUT_RIGHT : 0)
            | (m_mouseState.m_buttons[entry::MouseButton::Middle] ? IMGUI_MBUT_MIDDLE : 0)
            , m_mouseState.m_mz
            , uint16_t(m_display_mode.width)
            , uint16_t(m_display_mode.height)
        );

        showExampleDialog(m_init_params.app);

        UIManager::GetStaticInstance()->_Render(render_ctxt);

        imguiEndFrame();


        // Advance to next frame. Rendering thread will be kicked to
        // process submitted rendering primitives.
        bgfx::frame();

        return true;
    }

    return false;
}
    
void Engine::PreTickManagers( struct TickContext& tick_ctxt )
{
        
}

static int ImputCmdEngine(CmdContext* /*_context*/, void* /*_userData*/, int _argc, char const* const* _argv)
{
	if (_argc > 1)
	{
        if (0 == bx::strCmp(_argv[1], "debugmenu"))
        {
            UIManager::GetStaticInstance()->ToggleDebugMenu();
        }
        else if (0 == bx::strCmp(_argv[1], "editor"))
        {
            UIManager::GetStaticInstance()->ToggleEditor();
        }
        else if (0 == bx::strCmp(_argv[1], "culling"))
        {
            Engine::Get()->m_show_culling = !Engine::Get()->m_show_culling;
            if (Engine::Get()->m_show_culling)
                g_SavedFrustumView = Controller::GetStaticInstance()->GetRenderView();
        }
	}

	return bx::kExitFailure;
}

void Engine::RegisterInputBindings()
{
    static const InputBinding s_input_bindings[] =
    {
        { entry::Key::F5,           entry::Modifier::None,      1, NULL, "engine debugmenu"                  },
        { entry::Key::KeyE,         entry::Modifier::LeftCtrl,  1, NULL, "engine editor"                     },
        { entry::Key::KeyC,         entry::Modifier::LeftCtrl,  1, NULL, "engine culling"                    },

        INPUT_BINDING_END
    };

	cmdAdd("engine", ImputCmdEngine);
	inputAddBindings("bindings_engine", s_input_bindings);
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
