#include "includes.h"

namespace console
{
	FILE* output_stream = nullptr;

	void attach(const char* name)
	{
		if (AllocConsole())
		{
			freopen_s(&output_stream, "conout$", "w", stdout);
		}
		SetConsoleTitle(name);
	}

	void detach()
	{
		if (output_stream)
		{
			fclose(output_stream);
		}
		FreeConsole();
	}
}

struct find_window_s
{
	char* window_name;
	HWND hWnd;
};

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	auto* window_inform_s = (find_window_s*)lParam;

	if ((!GetWindow(hwnd, GW_OWNER)) && IsWindow(hwnd))
	{
		DWORD process_id = NULL;
		GetWindowThreadProcessId(hwnd, &process_id);

		char* text_window = new char[255];

		GetWindowText(hwnd, text_window, 255);

		if (GetCurrentProcessId() == process_id && strstr(text_window, window_inform_s->window_name) && !strstr(text_window, ".exe"))
		{
#ifdef ENABLE_DEBUG_CONSOLE
			std::cout << "Window name: " << text_window << std::endl;
#endif // ENABLE_DEBUG_CONSOLE
			window_inform_s->hWnd = hwnd;
			return 0;
		}
	}

	return 1;
}

HWND find_game_window(const char* psz_part_of_word_window_name)
{
	find_window_s window_inform_struct{};

	window_inform_struct.window_name = new char[strlen(psz_part_of_word_window_name)];

	strcpy(window_inform_struct.window_name, psz_part_of_word_window_name);

	EnumWindows(EnumWindowsProc, (LPARAM)&window_inform_struct);

	delete[] window_inform_struct.window_name;

	return window_inform_struct.hWnd;
}

void hack_thread()
{
	global_vars::hwnd_game = find_game_window("STAR WARS");
	utils::raise_error((DWORD64)global_vars::hwnd_game, "Error find window", "Window handle: ");

	vars::load_default_settings();

	m_pHook->setup(global_vars::hwnd_game);

#ifdef OVERLAY_RENDERER
	auto overlay_ret = m_pOverlayRenderer->Initialize();

	if (overlay_ret == FALSE)
	{
		MessageBox(NULL, "Error initialize DXWF", "Star Wars Battlefront II Cheat", MB_OK | MB_ICONERROR);
	}
#endif // OVERLAY_RENDERER

#ifdef OVERLAY_RENDERER
	DXWFSetFramerateLimit(30);
	DXWFRenderLoop();
#else
	while (global_vars::unload == false)
	{
		Sleep(800);
	}
#endif // OVERLAY_RENDERER

	m_pHook->unhook();

	m_pFeatures->DisableHacks();
	Sleep(100);

#ifdef OVERLAY_RENDERER
	m_pOverlayRenderer->Terminate();
#endif // OVERLAY_RENDERER

#ifdef ENABLE_DEBUG_CONSOLE
	std::cout << "free library...\n";
#endif // ENABLE_DEBUG_CONSOLE

	FreeLibraryAndExitThread(global_vars::dll_module, 0);
}

BOOL APIENTRY DllMain( HMODULE hModule,
					   DWORD  ul_reason_for_call,
					   LPVOID lpReserved
					 )
{
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		global_vars::dll_module = hModule;

		auto main_thread_handle = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)hack_thread, NULL, NULL, NULL);

#ifdef ENABLE_DEBUG_CONSOLE
		console::attach("sw battlefront 2 hack");

		if (main_thread_handle == INVALID_HANDLE_VALUE)
			std::cout << __FUNCTION__ << ": failed create main thread\n";
#endif // ENABLE_DEBUG_CONSOLE

		CloseHandle(main_thread_handle);

		return TRUE;
	}
}

