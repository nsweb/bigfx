

#include "../bigfx.h"
#include "uimanager.h"
#include "../engine/engine.h"
#include "../engine/controller.h"
//#include "../gfx/gfxmanager.h"
//#include "../gfx/shader.h"
#include "../gfx/rendercontext.h"
//#include "../gfx/bufferlock.h"
//#include "../system/profiler.h"

// Needed for loading png
//#define STB_IMAGE_IMPLEMENTATION
//#include "stb/stb_image.h"

namespace bigfx
{

UIManager* UIManager::m_static_instance = nullptr;

UIManager::UIManager() :
    m_draw_editor_fn(nullptr),
    m_toggle_editor_fn(nullptr),
	m_draw_custom_menu_fn(nullptr),
	m_show_debug_menu(false),
	m_show_profiler(false),
    m_show_editor(false)
{
	m_static_instance = this;
}

UIManager::~UIManager()
{
	m_static_instance = nullptr;
}

void UIManager::Create()
{

}
void UIManager::Destroy()
{

}


#if 0 //ZZZ
void UIManager::InitImGui()
{
	int wx, wy;
	SDL_GetWindowSize( g_pEngine->GetDisplayWindow(), &wx, &wy );
	//float mousePosScalex = (float)g_pEngine->GetDisplayMode().w / wx;                  // Some screens e.g. Retina display have framebuffer size != from window size, and mouse inputs are given in window/screen coordinates.
	//float mousePosScaley = (float)g_pEngine->GetDisplayMode().h / wy;
    
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO();
	io.IniFilename = "../data/imgui.ini";
	io.LogFilename = "../data/log/imgui.log";
	io.DisplaySize = ImVec2((float)g_pEngine->GetDisplayMode().w, (float)g_pEngine->GetDisplayMode().h);  // Display size, in pixels. For clamping windows positions.
	io.DeltaTime = 1.0f/60.0f;                          // Time elapsed since last frame, in seconds (in this sample app we'll override this every frame because our timestep is variable)
	//io.PixelCenterOffset = 0.0f;                        // Align OpenGL texels
	io.KeyMap[ImGuiKey_Tab] = SDLK_TAB;                     // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
	io.KeyMap[ImGuiKey_LeftArrow] = SDL_SCANCODE_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow] = SDL_SCANCODE_UP;
	io.KeyMap[ImGuiKey_DownArrow] = SDL_SCANCODE_DOWN;
	io.KeyMap[ImGuiKey_PageUp] = SDL_SCANCODE_PAGEUP;
	io.KeyMap[ImGuiKey_PageDown] = SDL_SCANCODE_PAGEDOWN;
	io.KeyMap[ImGuiKey_Home] = SDL_SCANCODE_HOME;
	io.KeyMap[ImGuiKey_End] = SDL_SCANCODE_END;
	io.KeyMap[ImGuiKey_Delete] = SDLK_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = SDLK_BACKSPACE;
	io.KeyMap[ImGuiKey_Enter] = SDLK_RETURN;
	io.KeyMap[ImGuiKey_Escape] = SDLK_ESCAPE;
	io.KeyMap[ImGuiKey_A] = SDLK_a;
	io.KeyMap[ImGuiKey_C] = SDLK_c;
	io.KeyMap[ImGuiKey_V] = SDLK_v;
	io.KeyMap[ImGuiKey_X] = SDLK_x;
	io.KeyMap[ImGuiKey_Y] = SDLK_y;
	io.KeyMap[ImGuiKey_Z] = SDLK_z;

	// TODO io.RenderDrawListsFn = ImImpl_RenderDrawLists;
	io.SetClipboardTextFn = ImImpl_SetClipboardTextFn;
	io.GetClipboardTextFn = ImImpl_GetClipboardTextFn;
#ifdef _MSC_VER
	io.ImeSetInputScreenPosFn = ImImpl_ImeSetInputScreenPosFn;
#endif

	// Load font texture
	glGenTextures(1, &m_debug_font_tex_id);
	glBindTexture(GL_TEXTURE_2D, m_debug_font_tex_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Build texture atlas
	uint8* pixels;
	int width, height, bytes_per_pixel;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height, &bytes_per_pixel);
	//const int data_size = width * bytes_per_pixel * height;

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	// Store our identifier
	io.Fonts->TexID = (void *)(intptr_t)m_debug_font_tex_id;

	// Cleanup (don't clear the input data if you want to append new fonts later)
	io.Fonts->ClearInputData();
	io.Fonts->ClearTexData();
}
#endif //ZZZ

void UIManager::Tick( TickContext& tick_ctxt )
{

}
    
void UIManager::_Render( struct RenderContext& render_ctxt )
{
	if( m_show_debug_menu )
		DrawDebugMenu();

	if( m_show_profiler )
		DrawProfiler();
    
    if( m_show_editor && m_draw_editor_fn )
        (*m_draw_editor_fn)( &m_show_editor, render_ctxt );

	if (m_draw_custom_menu_fn)
		(*m_draw_custom_menu_fn)(render_ctxt);
        
	//static bool show_test_window = false;//true;
	//static bool show_another_window = true;

	//// 1. Show a simple window
	//// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
	//{
	//	static float f;
	//	ImGui::Text("Hello, world!");
	//	ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
	//	show_test_window ^= ImGui::Button("Test Window");
	//	show_another_window ^= ImGui::Button("Another Window");

	//	// Calculate and show framerate
	//	static float ms_per_frame[120] = { 0 };
	//	static int ms_per_frame_idx = 0;
	//	static float ms_per_frame_accum = 0.0f;
	//	ms_per_frame_accum -= ms_per_frame[ms_per_frame_idx];
	//	ms_per_frame[ms_per_frame_idx] = ImGui::GetIO().DeltaTime * 1000.0f;
	//	ms_per_frame_accum += ms_per_frame[ms_per_frame_idx];
	//	ms_per_frame_idx = (ms_per_frame_idx + 1) % 120;
	//	const float ms_per_frame_avg = ms_per_frame_accum / 120;
	//	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", ms_per_frame_avg, 1000.0f / ms_per_frame_avg);
	//}

	//// 2. Show another simple window, this time using an explicit Begin/End pair
	//if (show_another_window)
	//{
	//	ImGui::Begin("Another Window", &show_another_window, ImVec2(200,100));
	//	ImGui::Text("Hello");
	//	ImGui::End();
	//}

	//// 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
	//if (show_test_window)
	//{
	//	ImGui::SetNewWindowDefaultPos(ImVec2(650, 20));        // Normally user code doesn't need/want to call this, because positions are saved in .ini file. Here we just want to make the demo initial state a bit more friendly!
	//	ImGui::ShowTestWindow(&show_test_window);
	//}

	//ZZZ
	//ImGui::Render();
}

void UIManager::DrawDebugMenu()
{
	bool show_debug_menu = m_show_debug_menu;

#if 0 //ZZZ
	// TODO
	ImGui::Begin("Debug menu", &show_debug_menu);// , ImVec2(10, 10));
	ImGui::Text("Hello");
	if (ImGui::CollapsingHeader("Profiling"))
	{
		ImGui::Checkbox("enable profiling", &m_show_profiler);
	}
	if (ImGui::CollapsingHeader("Style Editor"))
	{
		ImGui::ShowStyleEditor();
	}

	if (ImGui::CollapsingHeader("Logging"))
	{
		ImGui::LogButtons();
	}
	ImGui::End();
#endif //ZZZ

	// Release mouse if window was closed
	if( show_debug_menu != m_show_debug_menu )
		ToggleDebugMenu();
}

void UIManager::DrawProfiler()
{
#if 0 //ZZZ
	// TODO
	ImGui::Begin("Profiler", &m_show_debug_menu, /*ImVec2(200, 400), -1.f,*/ ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar);
	Profiler::BuildGui();	
	ImGui::End();
#endif //ZZZ
}

void UIManager::ToggleDebugMenu()
{
	bool new_show_debug_menu = !m_show_debug_menu;

#if 0 // REBIND
	if( new_show_debug_menu )
	{
		// Allow menu interaction
		SDL_SetRelativeMouseMode(SDL_FALSE);
		SDL_SetWindowGrab( g_pEngine->GetDisplayWindow(), SDL_FALSE );
	}
	else
	{
		// Allow FPS style mouse movement
		SDL_SetRelativeMouseMode(SDL_TRUE);
		SDL_SetWindowGrab( g_pEngine->GetDisplayWindow(), SDL_TRUE );
	}
#endif

	m_show_debug_menu = new_show_debug_menu;
}
void UIManager::ToggleEditor()
{
    if( !m_draw_editor_fn )
        return;
    
    bool new_show_editor = !m_show_editor;
    
    if( m_toggle_editor_fn )
        (*m_toggle_editor_fn)( new_show_editor );
    
#if 0 // REBIND
    if( new_show_editor )
    {
        // Allow menu interaction
        SDL_SetRelativeMouseMode(SDL_FALSE);
        SDL_SetWindowGrab( g_pEngine->GetDisplayWindow(), SDL_FALSE );
    }
    else
    {
        // Allow FPS style mouse movement
        SDL_SetRelativeMouseMode(SDL_TRUE);
        SDL_SetWindowGrab( g_pEngine->GetDisplayWindow(), SDL_TRUE );
    }
#endif
    
    m_show_editor = new_show_editor;
}

} /* namespace bigfx */
