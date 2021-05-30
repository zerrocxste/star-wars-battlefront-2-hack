class CHook
{
public:
	void setup(HWND hWnd);
	void unhook();

#ifndef OVERLAY_RENDERER
	void clear_renderview();
#endif // !OVERLAY_RENDERER

	bool renderview_lost = true;

	WNDPROC get_wndproc()
	{
		return wnd_proc;
	}
private:
	template <class T>
	void* get_address_func_from_vtable(T addresses_array, const int index)
	{
		void** array_addresses = *(void***)addresses_array;
		return array_addresses[index];
	}

#ifndef OVERLAY_RENDERER
	enum IDXGISwapChainvTable //for dx10 / dx11
	{
		QUERY_INTERFACE,
		ADD_REF,
		RELEASE,
		SET_PRIVATE_DATA,
		SET_PRIVATE_DATA_INTERFACE,
		GET_PRIVATE_DATA,
		GET_PARENT,
		GET_DEVICE,
		PRESENT,
		GET_BUFFER,
		SET_FULLSCREEN_STATE,
		GET_FULLSCREEN_STATE,
		GET_DESC,
		RESIZE_BUFFERS,
		RESIZE_TARGET,
		GET_CONTAINING_OUTPUT,
		GET_FRAME_STATISTICS,
		GET_LAST_PRESENT_COUNT
	};
#endif // !OVERLAY_RENDERER

	//void** swapchain_vtable;

#ifndef OVERLAY_RENDERER
	PVOID present_address;
	PVOID resizebuffers_address;
#endif // !OVERLAY_RENDERER
	PVOID setcursorpos_address;
	PVOID bitblt_address;

	WNDPROC wnd_proc;

	HWND game_hwnd;

#ifndef OVERLAY_RENDERER
	void create_dx11_ctx();
#endif // !OVERLAY_RENDERER

#ifndef OVERLAY_RENDERER
	void hook_dx11_func();
#endif // !OVERLAY_RENDERER
	void hook_wndproc();
	void hook_setcursorpos();
	void hook_bitblt();

#ifndef OVERLAY_RENDERER
	void unhook_dx11_func();
#endif // !OVERLAY_RENDERER
	void unhook_wndproc();
	void unhook_setcursorpos();
	void unhook_bitblt();
};
extern std::unique_ptr<CHook> m_pHook;

