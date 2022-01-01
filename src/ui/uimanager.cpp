

#include "../bigfx.h"
#include "uimanager.h"
#include "../engine/engine.h"
#include "../engine/controller.h"
#include "../gfx/gfxmanager.h"
#include "../gfx/shader.h"
#include "../gfx/rendercontext.h"
//#include "../gfx/bufferlock.h"
#include "../system/profiler.h"

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
#if 0 // REBIND
	m_debug_font_tex_id(0),
	m_ui_shader(nullptr),
	m_UI_VAO(0),
#endif
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
#if 0 // REBIND
	InitImGui();

	m_ui_shader = GfxManager::GetStaticInstance()->LoadShader( "ui" );

	glGenVertexArrays( 1, &m_UI_VAO);
	glBindVertexArray( m_UI_VAO);

	glGenBuffers(1, &m_UI_VBO);
	glGenBuffers(1, &m_UI_EBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_UI_VBO);
	//const uint32 MaxUIVertex = 10000;
	//m_UI_VBO.Init( MaxUIVertex, sizeof(UIVertex) );
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);

	glVertexAttribPointer( 0, 2, GL_FLOAT, GL_FALSE, sizeof(UIVertex) /*stride*/, (void*)0 /*offset*/	);
	glVertexAttribPointer( 1, 2, GL_FLOAT, GL_FALSE, sizeof(UIVertex) /*stride*/, (void*)8 /*offset*/	);
	glVertexAttribPointer( 2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(UIVertex) /*stride*/, (void*)16 /*offset*/	);

	glBindVertexArray(0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
#endif
}
void UIManager::Destroy()
{
#if 0 // REBIND
	m_ui_shader = nullptr;

	//m_UI_VBO.Cleanup();
	glDeleteBuffers( 1, &m_UI_VBO );
	glDeleteBuffers( 1, &m_UI_EBO );

	glDeleteVertexArrays( 1, &m_UI_VAO );

	//ImGui::Shutdown();

	//ImGui::DestroyContext();
#endif
}

#if 0 //ZZZ
// This is the main rendering function that you have to implement and provide to ImGui (via setting up 'RenderDrawListsFn' in the ImGuiIO structure)
static void ImImpl_RenderDrawLists(ImDrawData* draw_data)
{
	UIManager* manager = UIManager::GetStaticInstance();
	manager->RenderDrawLists( draw_data );
}

void UIManager::RenderDrawLists(ImDrawData* draw_data)
{
	PROFILE_SCOPE( __FUNCTION__ );

	if( draw_data->CmdListsCount == 0 )
		return;	

#if 0
	// Backup GL state
	GLint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, &last_program);
	GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
	GLint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
	GLint last_element_array_buffer; glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &last_element_array_buffer);
	GLint last_vertex_array; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);
	GLint last_blend_src; glGetIntegerv(GL_BLEND_SRC, &last_blend_src);
	GLint last_blend_dst; glGetIntegerv(GL_BLEND_DST, &last_blend_dst);
	GLint last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, &last_blend_equation_rgb);
	GLint last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &last_blend_equation_alpha);
	GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
	GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
	GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
	GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
	GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);
#endif

	// Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled
	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_SCISSOR_TEST);
	glActiveTexture(GL_TEXTURE0);


	// Handle cases of screen coordinates != from framebuffer coordinates (e.g. retina displays)
	ImGuiIO& io = ImGui::GetIO();
	float fb_height = io.DisplaySize.y * io.DisplayFramebufferScale.y;
	draw_data->ScaleClipRects(io.DisplayFramebufferScale);

	// Setup orthographic projection matrix
	//glViewport(0, 0, (GLsizei)io.DisplaySize.x, (GLsizei)io.DisplaySize.y);
	const float width = ImGui::GetIO().DisplaySize.x;
	const float height = ImGui::GetIO().DisplaySize.y;
	mat4 UIProjMatrix = mat4::ortho( 0.f, width, height, 0.f, 0.f, 1.f );

	m_ui_shader->Bind();
	ShaderUniform UniProj = m_ui_shader->GetUniformLocation("proj_mat");
	ShaderUniform UniSampler0 =  m_ui_shader->GetUniformLocation("textureUnit0");
	m_ui_shader->SetUniform( UniSampler0, 0 );
	m_ui_shader->SetUniform( UniProj, UIProjMatrix );

	glBindVertexArray( m_UI_VAO );

	for (int n = 0; n < draw_data->CmdListsCount; n++)
	{
		const ImDrawList* cmd_list = draw_data->CmdLists[n];
		const ImDrawIdx* idx_buffer_offset = 0;

		glBindBuffer(GL_ARRAY_BUFFER, m_UI_VBO);
		glBufferData(GL_ARRAY_BUFFER, (GLsizeiptr)cmd_list->VtxBuffer.size() * sizeof(ImDrawVert), (GLvoid*)&cmd_list->VtxBuffer.front(), GL_STREAM_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_UI_EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)cmd_list->IdxBuffer.size() * sizeof(ImDrawIdx), (GLvoid*)&cmd_list->IdxBuffer.front(), GL_STREAM_DRAW);

		for (const ImDrawCmd* pcmd = cmd_list->CmdBuffer.begin(); pcmd != cmd_list->CmdBuffer.end(); pcmd++)
		{
			if (pcmd->UserCallback)
			{
				pcmd->UserCallback(cmd_list, pcmd);
			}
			else
			{
				glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
				glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
				glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, idx_buffer_offset);
			}
			idx_buffer_offset += pcmd->ElemCount;
		}
	}

#if 0
	// Restore modified GL state
	glUseProgram(last_program);
	glBindTexture(GL_TEXTURE_2D, last_texture);
	glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, last_element_array_buffer);
	glBindVertexArray(last_vertex_array);
	glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
	glBlendFunc(last_blend_src, last_blend_dst);
	if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
	if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
	if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
	if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
	glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
#endif

	glEnable(GL_CULL_FACE);
	glDisable(GL_SCISSOR_TEST);
	glEnable(GL_DEPTH_TEST);


	// glFenceSync() / glClientWaitSync() ?
	//WriteGeometry( data, ... );
	//data += dataSize;


	//glDisableClientState(GL_COLOR_ARRAY);
	//glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	//glDisableClientState(GL_VERTEX_ARRAY);

	m_ui_shader->Unbind();
}

#endif //ZZZ

#if 0 // REBIND
// NB: ImGui already provide OS clipboard support for Windows so this isn't needed if you are using Windows only.
static const char* ImImpl_GetClipboardTextFn(void* user_data)
{
	return SDL_GetClipboardText();
}

static void ImImpl_SetClipboardTextFn(void* user_data, const char* text)
{
	SDL_SetClipboardText(text);
}
#endif

#ifdef _MSC_VER
// Notify OS Input Method Editor of text input position (e.g. when using Japanese/Chinese inputs, otherwise this isn't needed)
static void ImImpl_ImeSetInputScreenPosFn(int x, int y)
{
	//HWND hwnd = glfwGetWin32Window(window);
	//if (HIMC himc = ImmGetContext(hwnd))
	//{
	//	COMPOSITIONFORM cf;
	//	cf.ptCurrentPos.x = x;
	//	cf.ptCurrentPos.y = y;
	//	cf.dwStyle = CFS_FORCE_POSITION;
	//	ImmSetCompositionWindow(himc, &cf);
	//}
}
#endif

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

#if 0 //ZZZ
void UIManager::NewFrame()
{
    // Start the frame
    ImGui::NewFrame();
}
#endif //ZZZ
    
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
