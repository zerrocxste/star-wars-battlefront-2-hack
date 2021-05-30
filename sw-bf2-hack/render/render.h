namespace render
{
	extern void shutdown_imgui();
#ifndef OVERLAY_RENDERER
	extern void begin_scene(ID3D11Device* device, ID3D11DeviceContext* context);
#else
	extern void begin_scene();
#endif // !OVERLAY_RENDERER
	extern void reset();

	static ImFont* font_Main;
	static ImFont* font_Credits;

	namespace menu
	{
		extern void run();
	}
}

extern bool HDCToFile(const char* FilePath, HDC Context, RECT Area, bool save_in_file, uint16_t BitsPerPixel = 24);