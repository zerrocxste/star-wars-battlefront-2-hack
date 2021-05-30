#include "../includes.h"

namespace global_vars
{
	HMODULE dll_module;
	HWND hwnd_game;

	bool unload;

	bool menu_open;

	int screenshot_counter;

	int fake_screenshot_counter;

	int screen_bypass_active = 0;

	bool make_screenshot_post_frame = false;

	int overlay_max_fps = 30;

	bool killed_focus = true;

	void* fake_screenshot_bmp;

	bool enable_fake_screenshot = false;
}