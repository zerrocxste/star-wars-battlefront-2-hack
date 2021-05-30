#include "../includes.h"

#ifdef OVERLAY_RENDERER

std::unique_ptr<COverlayRenderer> m_pOverlayRenderer;

void RenderCallback();

void BeginSceneCallback();

void ResetCallback();

BOOL COverlayRenderer::Initialize()
{
	if (DXWFInitialization(memory_utils::get_base()) == FALSE)
		return FALSE;

	DXWFRendererCallbacks(DXWF_RENDERER_RESET_, ResetCallback);
	DXWFRendererCallbacks(DXWF_RENDERER_LOOP_, RenderCallback);
	DXWFRendererCallbacks(DXWF_RENDERER_BEGIN_SCENE_LOOP_, BeginSceneCallback);

	return DXWFCreateWindow("Star Wars Battlefront II cheat",
		0, 0,
		1920, 1080,
		WS_POPUP,
		WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_COMPOSITED | WS_EX_TRANSPARENT,
		user_dxwf_flags::ENABLE_WINDOW_ALPHA,
		my_color(0, 0, 0, 1),
		NULL);
}

void COverlayRenderer::Terminate()
{
	DXWFTerminate();
}

void RenderCallback()
{
	render::begin_scene();
}

void BeginSceneCallback()
{
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

void ResetCallback()
{
	ImGui_ImplDX9_InvalidateDeviceObjects();
}

#endif