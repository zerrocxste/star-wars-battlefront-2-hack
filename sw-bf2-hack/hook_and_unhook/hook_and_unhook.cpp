#include "../includes.h"

std::unique_ptr<CHook> m_pHook = std::make_unique<CHook>();

using fSetCursorPos = BOOL(WINAPI*)(int, int);

#ifndef OVERLAY_RENDERER
using fPresent = HRESULT(__fastcall*)(IDXGISwapChain*, UINT, UINT);
using fResizeBuffers = HRESULT(__fastcall*)(IDXGISwapChain*, UINT, UINT, UINT, DXGI_FORMAT, UINT);
#endif // !OVERLAY_RENDERER

using fBitBlt = BOOL(WINAPI*)(HDC, int, int, int, int, HDC, int, int, DWORD);

fSetCursorPos pfSetCursorPos = NULL;
#ifndef OVERLAY_RENDERER
fPresent pfPresent = NULL;
fResizeBuffers pfResizeBuffers = NULL;
#endif // !OVERLAY_RENDERER
fBitBlt pfBitBlt = NULL;

#ifndef OVERLAY_RENDERER
IDXGISwapChain* swapchain = nullptr;
ID3D11Device* device = nullptr;
ID3D11DeviceContext* context = nullptr;
ID3D11RenderTargetView* render_view = nullptr;

bool timer_make_fake_screenshot()
{
	static DWORD timer = 0;
	static int is_not_death_next_frame_counter = 0;

	auto entity_list = game_utils::get_entity_list();

	if (entity_list == NULL)
		return false;

	auto local_entity = game_utils::CPseduoEnitity::get_local_entity(entity_list);

	if (local_entity == NULL)
		return false;

	auto entity_soldier = local_entity->get_soldier_entity();

	if (entity_soldier == NULL)
		return false;

	auto health_component = entity_soldier->get_health_component();

	if (health_component == NULL)
		return false;

	if (health_component->get_health() <= 0.1f && is_not_death_next_frame_counter == 0)
		is_not_death_next_frame_counter = 2500;

	if (is_not_death_next_frame_counter != 0)
	{
		is_not_death_next_frame_counter--;
		return false;
	}

	if (global_vars::killed_focus)
		return false;

	if (GetTickCount() - timer < 300000)
	{

	}
	else {
		timer = GetTickCount();
		return true;
	}

	return false;
}

void make_fake_screenshot()
{
	HWND win = global_vars::hwnd_game;
	HDC dc = GetDC(win);
	HDCToFile("FakeScreenshot.bmp", dc, { 0, 0, 1920, 1080 }, true);
	ReleaseDC(win, dc);
#ifdef ENABLE_DEBUG_CONSOLE
	std::cout << "created fake screenshot\n";
#endif //ENABLE_DEBUG_CONSOLE
}

HRESULT __fastcall Present_hooked(IDXGISwapChain* g_swapchain, UINT sync_interval, UINT flags)
{
	constexpr auto max_skiped_frames_screenshot_bypass = 3;

	if (m_pHook->renderview_lost)
	{
		if (SUCCEEDED(g_swapchain->GetDevice(__uuidof(ID3D11Device), (void**)&device)))
		{
			device->GetImmediateContext(&context);

			ID3D11Texture2D* backbuffer;

			g_swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backbuffer);
			device->CreateRenderTargetView(backbuffer, NULL, &render_view);
			backbuffer->Release();

#ifdef ENABLE_DEBUG_CONSOLE
			std::cout << __FUNCTION__ << ": renderview successfully received!" << std::endl;
#endif //ENABLE_DEBUG_CONSOLE

			m_pHook->renderview_lost = false;
		}
	}

	if (timer_make_fake_screenshot() || GetAsyncKeyState(VK_DELETE))
		global_vars::screen_bypass_active = max_skiped_frames_screenshot_bypass;

	context->OMSetRenderTargets(1, &render_view, NULL);

	if (global_vars::screen_bypass_active == FALSE)
		render::begin_scene(device, context);

	if (global_vars::screen_bypass_active > 0)
	{
		if (global_vars::screen_bypass_active == 1)
			make_fake_screenshot();
		global_vars::screen_bypass_active--;
	}

	return pfPresent(g_swapchain, sync_interval, flags);
}

HRESULT __fastcall ResizeBuffers_hooked(IDXGISwapChain* g_swapchain, UINT buffer_count, UINT width, UINT height, DXGI_FORMAT new_format, UINT flags)
{
#ifdef ENABLE_DEBUG_CONSOLE
	static auto once = []()
	{
		std::cout << __FUNCTION__ << ": first called!" << std::endl;
		return true;
	}();
#endif //ENABLE_DEBUG_CONSOLE

	m_pHook->clear_renderview();
	m_pHook->renderview_lost = true;

	render::reset();

	return pfResizeBuffers(g_swapchain, buffer_count, width, height, new_format, flags);
}
#endif // !OVERLAY_RENDERER

LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc_hooked(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
#ifdef ENABLE_DEBUG_CONSOLE
	static auto once = []()
	{
		std::cout << __FUNCTION__ << ": first called!" << std::endl;
		return true;
	}();
#endif //ENABLE_DEBUG_CONSOLE

	if (uMsg == WM_KEYDOWN && wParam == VK_INSERT)
	{
		global_vars::menu_open = !global_vars::menu_open;
		return FALSE;
	}

	switch (uMsg)
	{
	case WM_SETFOCUS:
		global_vars::killed_focus = false;
		//ShowWindow(DXWFGetHWND(), SW_SHOW);
		break;
	case WM_KILLFOCUS:
		global_vars::killed_focus = true;
		//ShowWindow(DXWFGetHWND(), SW_HIDE);
		break;
	default:
		break;
	}

	if (global_vars::menu_open)
	{
		ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam);
		return TRUE;
	}

	return CallWindowProc(m_pHook->get_wndproc(), hwnd, uMsg, wParam, lParam);
}

BOOL WINAPI SetCursorPos_hooked(int x, int y)
{
#ifdef ENABLE_DEBUG_CONSOLE
	static auto once = []()
	{
		std::cout << __FUNCTION__ << ": first called!" << std::endl;
		return true;
	}();
#endif //ENABLE_DEBUG_CONSOLE

	if (global_vars::menu_open)
		return TRUE;

	return pfSetCursorPos(x, y);
}

std::string GetFileName()
{
	std::string path("screenshots\\");
	CreateDirectory(path.c_str(), NULL);

	time_t time_;
	time(&time_);

	return path + std::to_string(time_) + ".bmp";
}

void SaveBitBltResult(HDC hdcDst, HDC hdcSrc, int width, int height)
{
	HBITMAP hbitmapsrc = CreateCompatibleBitmap(hdcDst, width, height);
	HBITMAP hbitmapdst = (HBITMAP)SelectObject(hdcDst, hbitmapsrc);
	
	BITMAPINFOHEADER bi{ 0 };
	BYTE* bmp{ nullptr };
	DWORD dwBmpSize;

	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = width;
	bi.biHeight = height;
	bi.biPlanes = 1;
	bi.biBitCount = 32;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	dwBmpSize = ((width * bi.biBitCount + 31) / 32) * 4 * height;
	bmp = new BYTE[dwBmpSize];

	BITMAPFILEHEADER bmfHeader{ 0 };
	bmfHeader.bfSize = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);
	bmfHeader.bfType = 0x4D42;

	GetDIBits(hdcSrc, hbitmapdst, 0, (UINT)height, bmp, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

	auto path = GetFileName();

	HANDLE hFile = CreateFile(path.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	DWORD dwBytesWritten = 0;
	WriteFile(hFile, (void*)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (void*)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
	WriteFile(hFile, (void*)bmp, dwBmpSize, &dwBytesWritten, NULL);

	CloseHandle(hFile);
}

#ifndef OVERLAY_RENDERER
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}
#endif

BOOL WINAPI BitBlt_hooked(HDC hdc, int x, int y, int cx, int cy, HDC hdcSrc, int x1, int y1, DWORD rop)
{
#ifdef OVERLAY_RENDERER
	global_vars::screenshot_counter++;

	const auto& hwnd = DXWFGetHWND();

	ShowWindow(hwnd, SW_HIDE);
#else
	if (global_vars::screen_bypass_active > 0)
	{
		global_vars::fake_screenshot_counter++;
		return pfBitBlt(hdc, x, y, cx, cy, hdcSrc, x1, x1, rop);
	}

	global_vars::screenshot_counter++;

	HWND h_fake_window = nullptr;

	constexpr const char* classname = "Frostbite";
	constexpr const char* windowname = "STAR WARS Battlefront II";

	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = memory_utils::get_base();
	wc.lpfnWndProc = WndProc;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)RGB(0, 0, 0);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = classname;

	RegisterClass(&wc);

	h_fake_window = CreateWindowExA
	(
		WS_EX_TOPMOST,
		classname, 
		windowname,
		WS_POPUP,
		0, 0,
		1920, 1080,
		NULL, 
		NULL,
		NULL, 
		NULL
	);

	if (!h_fake_window) {
		TerminateProcess(GetCurrentProcess(), 0);
	}

	ShowWindow(h_fake_window, SW_SHOWDEFAULT);
	UpdateWindow(h_fake_window);

	HBITMAP bitmap = (HBITMAP)LoadImage(NULL, "FakeScreenshot.bmp", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

	if (bitmap == NULL)
		TerminateProcess(GetCurrentProcess(), 0);

	auto dc = GetDC(h_fake_window);

	auto comp_dc = CreateCompatibleDC(NULL);

	SelectObject(comp_dc, bitmap);

	StretchBlt(dc, 0, 0, 1920, 1080, comp_dc, 0, 0, 1920, 1080, SRCCOPY);

	//ShowWindow(global_vars::hwnd_game, SW_HIDE);
#endif

	Sleep(100);

	auto ret = pfBitBlt(hdc, x, y, cx, cy, comp_dc, x1, x1, rop);

	SaveBitBltResult(hdc, hdcSrc, cx, cy);

	Sleep(100);

#ifdef OVERLAY_RENDERER
	ShowWindow(hwnd, SW_SHOW);
#else
	DeleteDC(comp_dc);
	ReleaseDC(h_fake_window, dc);

	DestroyWindow(h_fake_window);
	UnregisterClass(classname, memory_utils::get_base());
	//ShowWindow(global_vars::hwnd_game, SW_SHOW);
#endif

	return ret;
}

void CHook::setup(HWND hWnd)
{
	this->game_hwnd = hWnd;

	MH_Initialize();

#ifndef OVERLAY_RENDERER
	this->create_dx11_ctx();

	this->hook_dx11_func();
	utils::raise_error((DWORD64)m_pHook->present_address, "Error hook DX11", "present: ");
	utils::raise_error((DWORD64)m_pHook->resizebuffers_address, "Error hook DX11", "resizebuffers: ");
#endif // !OVERLAY_RENDERER

	this->hook_wndproc();
	utils::raise_error((DWORD64)m_pHook->wnd_proc, "Error hook wndproc", "wndproc: ");

	this->hook_setcursorpos();
	utils::raise_error((DWORD64)m_pHook->setcursorpos_address, "Error hook setcursorpos", "setcursorpos: ");

	this->hook_bitblt();
	utils::raise_error((DWORD64)m_pHook->bitblt_address, "Error hook bitblt", "bitblt: ");
}

void CHook::unhook()
{
	render::shutdown_imgui();

#ifndef OVERLAY_RENDERER
	this->unhook_dx11_func();
#endif // !OVERLAY_RENDERER
	this->unhook_wndproc();
	this->unhook_setcursorpos();
	this->unhook_bitblt();

	MH_Uninitialize();

#ifndef OVERLAY_RENDERER
	this->clear_renderview();
#endif // !OVERLAY_RENDERER
}

#ifndef OVERLAY_RENDERER
void CHook::create_dx11_ctx()
{
	D3D_FEATURE_LEVEL feature_levels[] = { D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_11_0 };
	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(scd));
	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	scd.OutputWindow = this->game_hwnd;
	scd.SampleDesc.Count = 1;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Windowed = TRUE;
	scd.BufferDesc.Width = 100;
	scd.BufferDesc.Height = 100;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;

	if (FAILED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, NULL, feature_levels, 1, D3D11_SDK_VERSION, &scd, &swapchain, &device, NULL, &context)))
	{
#ifdef ENABLE_DEBUG_CONSOLE
		std::cout << __FUNCTION__ << ": failed to create device\n";
#endif //ENABLE_DEBUG_CONSOLE
		return;
	}
}

void CHook::hook_dx11_func()
{
	this->present_address = this->get_address_func_from_vtable<IDXGISwapChain*>(swapchain, IDXGISwapChainvTable::PRESENT);
	this->resizebuffers_address = this->get_address_func_from_vtable<IDXGISwapChain*>(swapchain, IDXGISwapChainvTable::RESIZE_BUFFERS);

	if (MH_CreateHook(this->present_address, &Present_hooked, (LPVOID*)&pfPresent) != MH_OK
		|| MH_EnableHook(this->present_address) != MH_OK)
	{
#ifdef ENABLE_DEBUG_CONSOLE
		std::cout << __FUNCTION__ << ": failed create hook present\n";
#endif //ENABLE_DEBUG_CONSOLE
		return;
	}

	if (MH_CreateHook(this->resizebuffers_address, &ResizeBuffers_hooked, (LPVOID*)&pfResizeBuffers) != MH_OK
		|| MH_EnableHook(this->resizebuffers_address) != MH_OK)
	{
#ifdef ENABLE_DEBUG_CONSOLE
		std::cout << __FUNCTION__ << ": failed create hook resizebuffers\n";
#endif //ENABLE_DEBUG_CONSOLE
		return;
	}
}
#endif // !OVERLAY_RENDERER

void CHook::hook_wndproc()
{
	this->wnd_proc = (WNDPROC)SetWindowLongPtr(this->game_hwnd, GWLP_WNDPROC, (LONG_PTR)WndProc_hooked);
}

void CHook::hook_setcursorpos()
{
	this->setcursorpos_address = (LPVOID)SetCursorPos;

	if (MH_CreateHook(setcursorpos_address, &SetCursorPos_hooked, (PVOID*)&pfSetCursorPos) != MH_OK
		|| MH_EnableHook(setcursorpos_address) != MH_OK)
	{
#ifdef ENABLE_DEBUG_CONSOLE
		std::cout << __FUNCTION__ << ": failed create hook setcursorpos\n";
#endif //ENABLE_DEBUG_CONSOLE
		return;
	}
}

void CHook::hook_bitblt()
{
	this->bitblt_address = (LPVOID)BitBlt;

	if (MH_CreateHook(bitblt_address, &BitBlt_hooked, (LPVOID*)&pfBitBlt) != MH_OK
		|| MH_EnableHook(bitblt_address) != MH_OK)
	{
#ifdef ENABLE_DEBUG_CONSOLE
		std::cout << __FUNCTION__ << ": failed create hook bitblt\n";
#endif //ENABLE_DEBUG_CONSOLE	
		return;
	}
}

#ifndef OVERLAY_RENDERER
void CHook::unhook_dx11_func()
{
	MH_DisableHook(m_pHook->present_address);
	MH_RemoveHook(m_pHook->present_address);
	Sleep(100);

	MH_DisableHook(m_pHook->resizebuffers_address);
	MH_RemoveHook(m_pHook->resizebuffers_address);
	Sleep(100);
}
#endif // !OVERLAY_RENDERER

void CHook::unhook_wndproc()
{
	SetWindowLongPtr(this->game_hwnd, GWLP_WNDPROC, (LONG_PTR)this->wnd_proc);
	Sleep(100);
}

void CHook::unhook_setcursorpos()
{
	MH_DisableHook(this->setcursorpos_address);
	MH_RemoveHook(this->setcursorpos_address);
	Sleep(100);
}

void CHook::unhook_bitblt()
{
	MH_DisableHook(this->bitblt_address);
	MH_RemoveHook(this->bitblt_address);
	Sleep(100);
}

#ifndef OVERLAY_RENDERER
void CHook::clear_renderview()
{
	render_view->Release();
	render_view = nullptr;
}
#endif // !OVERLAY_RENDERER





