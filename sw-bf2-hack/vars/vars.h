namespace vars
{
	namespace global
	{
		extern bool enable;
	}
	namespace visuals
	{
		extern bool enable;
		extern int player_type;
		extern int box_type;
		extern bool name;
		extern bool health;
		extern bool distance;
		extern bool radar_3d;
		extern float col_enemy_vis[3];
		extern float col_enemy_invis[3];
		extern float col_teammate_vis[3];
		extern float col_teammate_invis[3];
	}
	namespace misc
	{
		extern bool norecoil;
		extern bool fire_rate;
		extern bool fast_reload;
		extern bool smooth_shot;
		extern bool nospread;
		extern bool endless_dash;
	}
	namespace font
	{
		extern int style;
		extern float size;
	}
	extern void load_default_settings();
}
