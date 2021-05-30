enum LINE_STATUS_BAR
{
	LEFT,
	RIGHT,
	UPPER,
	BOTTOM
};

class CFeatures
{
public:
	void run();
	void DataGrabber();
	void ESP(Matrix4x4 matrix, Vector origin, Vector aabb_min_val, Vector aabb_max_val, int team, int my_team, bool is_visible, float health, float max_health, char* name);
	void DrawBox(float x, float y, float w, float h, const ImColor col);
	void DrawName(const char* pcszPlayerName, float x, float y, float w, ImColor col);
	void DrawStatusLine(float x, float y, float w, float h, float status_value, float max_of_status_value, ImColor col, LINE_STATUS_BAR status_side);
	void DrawDistance(float x, float y, float w, float h, float distance);
	void Draw3DCircle(Vector vOrigin, Matrix4x4 matrix, ImColor col);
	ImColor PlayerColor(int iTeam, int iMyTeam, const bool isVisible);
	void DrawScreenshotCounter();
	void IncreaseFireRate(bool is_enable);
	void NoRecoil(bool bIsEnable);
	void FastReload(bool bIsEnable);
	void InfinityJerk();
	void DisableHacks();
private:

};
extern std::unique_ptr<CFeatures> m_pFeatures;