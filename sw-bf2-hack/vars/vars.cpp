#include "../includes.h"

namespace vars
{
	namespace global
	{
		bool enable;
	}
	namespace visuals
	{
		int player_type = 0;
		int box_type = 0;
		bool name = false;
		bool health = false;
		bool distance = false;
		bool radar_3d = false;
		float col_enemy_vis[3];
		float col_enemy_invis[3];
		float col_teammate_vis[3];
		float col_teammate_invis[3];
		bool vehicle_esp = false;
		float col_enemy_horse[3];
		float col_teammate_horse[3];
		float col_enemy_vehicle[3];
		float col_teammate_vehicle[3];
	}
	namespace misc
	{
		bool norecoil = false;
		bool fire_rate = false;
		bool fast_reload = false;
		bool smooth_shot = false;
		bool nospread = false;
		bool endless_dash = false;
	}
	namespace font
	{
		int style = 0;
		float size = 0;
	}
	void load_default_settings() {
		global::enable = true;

		visuals::player_type = 0;
		visuals::box_type = 4;
		visuals::name = true;
		visuals::health = true;
		visuals::distance = true;
		visuals::radar_3d = true;

		visuals::col_enemy_vis[0] = 1.f;
		visuals::col_enemy_vis[1] = 1.f;
		visuals::col_enemy_vis[2] = 0.f;
		visuals::col_enemy_invis[0] = 1.f;
		visuals::col_enemy_invis[1] = 0.f;
		visuals::col_enemy_invis[2] = 0.f;

		visuals::col_teammate_vis[0] = 0.f;
		visuals::col_teammate_vis[1] = 1.f;
		visuals::col_teammate_vis[2] = 1.f;
		visuals::col_teammate_invis[0] = 0.f;
		visuals::col_teammate_invis[1] = 0.f;
		visuals::col_teammate_invis[2] = 1.f;

		font::style = 1;
	}
}
