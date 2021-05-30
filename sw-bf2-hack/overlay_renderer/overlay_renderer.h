#ifdef OVERLAY_RENDERER
class COverlayRenderer
{
public:
	BOOL Initialize();
	void Terminate();
private:

};
extern std::unique_ptr<COverlayRenderer> m_pOverlayRenderer;
#endif

