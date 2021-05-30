#define _CRT_SECURE_NO_WARNINGS
//#define ENABLE_DEBUG_CONSOLE
//#define OVERLAY_RENDERER

#ifdef ENABLE_DEBUG_CONSOLE
#include <iostream>
#endif //ENABLE_DEBUG_CONSOLE

#include <Windows.h>
#include <direct.h>
#include <float.h>
#include <vector>
#include <time.h>
#include <ShlObj.h>
#include <iomanip>
#include <fstream>
#include <string>

#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

#ifdef OVERLAY_RENDERER
#include <d3d9.h>
#pragma comment (lib, "d3d9.lib")
#include <dwmapi.h>
#pragma comment (lib, "dwmapi.lib")
#include "DXWF/DXWF.h"
#pragma comment (lib, "DXWF/DXWF.lib")
#else
#include <d3d11.h>
#pragma comment(lib, "d3d11")
#endif // OVERLAY_RENDERER

#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_impl_win32.h"

#ifdef OVERLAY_RENDERER
#include "imgui/imgui_impl_dx9.h"
#else
#include "imgui/imgui_impl_dx11.h"
#endif

#include "minhook/minhook.h"
#pragma comment (lib, "minhook/minhook.lib")

#include "math/martix4x4/matrix4x4.h"
#include "math/vector/vector.h"
#include "utils/utils.h"

#include "memory_utils/memory_utils.h"

#include "game_utils/game_utils.h"

#include "globals_vars/globals_vars.h"
#include "vars/vars.h"

#include "draw-list/draw-list.h"
#include "render/render.h"

#include "hook_and_unhook/hook_and_unhook.h"

#ifdef OVERLAY_RENDERER
#include "overlay_renderer/overlay_renderer.h"
#endif

#include "features/features.h"
