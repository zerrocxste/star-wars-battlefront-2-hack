#include "../includes.h"

std::unique_ptr<CFeatures> m_pFeatures = std::make_unique<CFeatures>();

void CFeatures::run()
{
	this->DrawScreenshotCounter();
	this->DataGrabber();
}

void CFeatures::DataGrabber()
{
	const auto& g_entity_list = game_utils::get_entity_list();

	if (g_entity_list == NULL)
		return;

	const auto& g_local_entity = game_utils::CPseduoEnitity::get_local_entity(g_entity_list);

	const auto& i_my_team = g_local_entity->get_team();

	const auto& g_matrix = game_utils::get_matrix();

	for (int i = 0; i < 64/*40*/; i++)
	{
		const auto& g_entity = game_utils::CPseduoEnitity::get_entity_by_id(g_entity_list, i);

		if (g_entity == NULL)
			continue;

		const auto &csz_name = g_entity->get_name();

		if (csz_name == NULL || csz_name == "" || strlen(csz_name) == NULL)
			continue;

		const auto& g_soldier_entity = g_entity->get_soldier_entity();

		if (g_soldier_entity == NULL)
			continue;

		const auto& g_health_component = g_soldier_entity->get_health_component();

		if (g_health_component == NULL)
			continue;

		const auto& fl_health = g_health_component->get_health();

		if (fl_health <= 0.f)
			continue;

		const auto& fl_max_health = g_health_component->get_max_health();

		const auto& i_team = g_entity->get_team();

		if (vars::visuals::player_type == 0 && i_team == i_my_team)
			continue;

		const auto& g_movement_controller = g_soldier_entity->get_movement_controller();

		if (g_movement_controller == NULL)
			continue;

		const auto& v_origin = g_movement_controller->get_origin();

		const auto& is_visible = g_soldier_entity->is_visible();

		if (v_origin.IsZero())
			continue;
		
		this->ESP(g_matrix, v_origin, g_movement_controller->get_aabb_min(), g_movement_controller->get_aabb_max(), i_team, i_my_team, is_visible, fl_health, fl_max_health, csz_name);
	}
}

void CFeatures::ESP(Matrix4x4 matrix, Vector origin, Vector aabb_min_val, Vector aabb_max_val, int team, int my_team, bool is_visible, float health, float max_health, char* name)
{
	auto color = this->PlayerColor(team, my_team, is_visible);

	auto aabb_min = origin + aabb_min_val;
	auto aabb_max = origin + aabb_max_val;

	float out_min[2], out_max[2];
	if (game_utils::world_to_screen(matrix, aabb_min, out_min) && game_utils::world_to_screen(matrix, aabb_max, out_max))
	{
		float h = out_min[1] - out_max[1];
		float w = h / 2;
		float x = out_min[0] - w / 2;
		float y = out_max[1];

		this->DrawBox(x, y, w, h, color);

		this->DrawName(name, x, y, w, color);

		this->DrawStatusLine(x, y, w, h, health, max_health, ImColor(0.f, 1.f, 0.f), LINE_STATUS_BAR::LEFT);
	}

	this->Draw3DCircle(origin, matrix, color);
}

void CFeatures::DrawBox(float x, float y, float w, float h, const ImColor col)
{
	m_pDrawing->DrawEspBox(vars::visuals::box_type, x, y, w, h, col.Value.x, col.Value.y, col.Value.z, col.Value.w);
}

void CFeatures::DrawName(const char* pcszPlayerName, float x, float y, float w, ImColor col)
{
	if (vars::visuals::name == false)
		return;

	if (pcszPlayerName == NULL)
		return;

	ImFont* Font = ImGui::GetIO().Fonts->Fonts[0];
	ImVec2 text_size = Font->CalcTextSizeA(vars::font::size ? vars::font::size : Font->FontSize, FLT_MAX, 0, "");

	m_pDrawing->AddText(x + w / 2.f, y - text_size.y - 2.f, ImColor(1.f, 1.f, 1.f, col.Value.w), vars::font::size, FL_CENTER_X, u8"%s", pcszPlayerName);
}

void CFeatures::DrawStatusLine(float x, float y, float w, float h, float status_value, float max_of_status_value, ImColor col, LINE_STATUS_BAR status_side)
{
	if (status_value <= 0.f)
		return;

	status_value = ImClamp(status_value, 0.f, max_of_status_value);

	const auto size_h = h / max_of_status_value * status_value;
	const auto size_w = w / max_of_status_value * status_value;

	const auto thickness = 2.f;

	switch (status_side)
	{
	case LINE_STATUS_BAR::LEFT:
		m_pDrawing->DrawFillArea(x - thickness - 1.9f, y + h, thickness, -size_h, col);

		if (vars::visuals::box_type == 2 || vars::visuals::box_type == 4 || vars::visuals::box_type == 6)
			m_pDrawing->DrawBox(x - thickness - 2.9f, y - 1.f, thickness + 2.f, h + 2.f, ImColor(0.f, 0.f, 0.f, col.Value.w));

		break;
	case LINE_STATUS_BAR::RIGHT:
		m_pDrawing->DrawFillArea(x + w - thickness + (2.9f * 2.f), y + h, thickness, -size_h, col);

		if (vars::visuals::box_type == 2 || vars::visuals::box_type == 4 || vars::visuals::box_type == 6)
			m_pDrawing->DrawBox(x + w - thickness + (2.9f * 2.f) - 0.9f, y - 1.f, thickness + 2.f, h + 2.f, ImColor(0.f, 0.f, 0.f, col.Value.w));

		break;
	case LINE_STATUS_BAR::UPPER:
		m_pDrawing->DrawFillArea(x, y - thickness - 1.9f, size_w + 1.9f, thickness, col);

		if (vars::visuals::box_type == 2 || vars::visuals::box_type == 4 || vars::visuals::box_type == 6)
			m_pDrawing->DrawBox(x - 1.9f, y - thickness - 2.9f, w + 3.9f, thickness + 2.f, ImColor(0.f, 0.f, 0.f, col.Value.w));

		break;
	case LINE_STATUS_BAR::BOTTOM:
		m_pDrawing->DrawFillArea(x, y + h + thickness + 0.9f, size_w + 1.9f, thickness, col);

		if (vars::visuals::box_type == 2 || vars::visuals::box_type == 4 || vars::visuals::box_type == 6)
			m_pDrawing->DrawBox(x - 1.9f, y + h + thickness, w + 3.9f, thickness + 2.f, ImColor(0.f, 0.f, 0.f, col.Value.w));

		break;
	default: break;
	}
}

void CFeatures::DrawDistance(float x, float y, float w, float h, float distance)
{
	if (vars::visuals::distance == false)
		return;

	m_pDrawing->AddText(x + w / 2.f, y + h + 4.9f, ImColor(1.f, 1.f, 1.f), vars::font::size, FL_CENTER_X, "%.0f", distance);
}

void CFeatures::Draw3DCircle(Vector vOrigin, Matrix4x4 matrix, ImColor col)
{
	if (vars::visuals::radar_3d == false)
		return;

	m_pDrawing->AddCircle3D(vOrigin, matrix, 18.f, 0.8f, col);
}

ImColor CFeatures::PlayerColor(int iTeam, int iMyTeam, const bool isVisible)
{
	if (iTeam == iMyTeam)
	{
		if (isVisible)
			return ImColor(vars::visuals::col_teammate_vis[0], vars::visuals::col_teammate_vis[1], vars::visuals::col_teammate_vis[2], 1.f);
		else
			return ImColor(vars::visuals::col_teammate_invis[0], vars::visuals::col_teammate_invis[1], vars::visuals::col_teammate_invis[2], 1.f);
	}
	else if (iTeam != iMyTeam)
	{
		if (isVisible)
			return ImColor(vars::visuals::col_enemy_vis[0], vars::visuals::col_enemy_vis[1], vars::visuals::col_enemy_vis[2], 1.f);
		else
			return ImColor(vars::visuals::col_enemy_invis[0], vars::visuals::col_enemy_invis[1], vars::visuals::col_enemy_invis[2], 1.f);
	}
	else
		return ImColor(1.f, 1.f, 1.f);
}

void CFeatures::DrawScreenshotCounter()
{
	static int save_screenshot_count = global_vars::screenshot_counter;

	static float color_fade = 1.f;

	static auto col = ImColor(1.f, 1.f, 1.f);

	if (save_screenshot_count != global_vars::screenshot_counter)
	{
		color_fade = 0.f;
		save_screenshot_count = global_vars::screenshot_counter;
	}

	if (color_fade <= 1.f)
	{
		color_fade += 0.007f;
		col.Value.y = color_fade;
		col.Value.z = color_fade;
	}

	m_pDrawing->AddText(10, 10, col, 20, FL_NONE, "Taked screenshot: %d", global_vars::screenshot_counter);
	m_pDrawing->AddText(10, 30, ImColor(0.5f, 1.f, 0.5f), 20, FL_NONE, "Taked fake screenshot: %d", global_vars::fake_screenshot_counter);
}

void CFeatures::IncreaseFireRate(bool is_enable)
{
	static auto qwFirerateInsructionAddressSpecialGun = memory_utils::find_pattern(memory_utils::get_base(), "\x41\x8B\x46\x08\x89\x86\x6C\x01\x00\x00", "xxxxxxxxxx");
	static auto qwFirerateInsructionAddressOtherWeapon = memory_utils::find_pattern(memory_utils::get_base(), "\x89\x42\x08\x8B\x81\x64\x01\x00\x00", "xxxxxxxxx");
	static auto qwRelativeFirerateInsructionAddress = memory_utils::find_pattern(memory_utils::get_base(), "\xF3\x0F\x11\x83\x6C\x01\x00\x00\xE9\xE0", "xxxxxxxxxx");

#ifdef ENABLE_DEBUG_CONSOLE
	static auto print_once = []()
	{
		std::cout << __FUNCTION__ << ": qdwFireRateAddress = 0x" << std::hex << qwFirerateInsructionAddressSpecialGun << std::endl;
		std::cout << __FUNCTION__ << ": qwFireRateAddress2 = 0x" << std::hex << qwFirerateInsructionAddressOtherWeapon << std::endl;
		std::cout << __FUNCTION__ << ": qwFireRateAddress3 = 0x" << std::hex << qwRelativeFirerateInsructionAddress << std::endl;
		return true;
	}();
#endif // ENABLE_DEBUG_CONSOLE

	if (qwFirerateInsructionAddressSpecialGun == NULL 
		|| qwFirerateInsructionAddressOtherWeapon == NULL 
		|| qwRelativeFirerateInsructionAddress == NULL)
	{
		return;
	}
		
	if (is_enable)
	{
		memory_utils::patch_instruction(qwFirerateInsructionAddressSpecialGun, "\x90\x90\x90\x90", 4);
		memory_utils::patch_instruction(qwFirerateInsructionAddressOtherWeapon, "\x90\x90\x90", 3);
		memory_utils::patch_instruction(qwRelativeFirerateInsructionAddress, "\x90\x90\x90\x90\x90\x90\x90\x90", 8);
	}
	else
	{
		memory_utils::patch_instruction(qwFirerateInsructionAddressSpecialGun, "\x41\x8B\x46\x08", 4);
		memory_utils::patch_instruction(qwFirerateInsructionAddressOtherWeapon, "\x89\x42\x08", 3);
		memory_utils::patch_instruction(qwRelativeFirerateInsructionAddress, "\xF3\x0F\x11\x83\x6C\x01\x00\x00", 8);
	}
}

void CFeatures::NoRecoil(bool bIsEnable)
{
	//vert
	//ptr: F3 0F 11 56 6C EB
	//bytes: starwarsbattlefrontii.exe+7EEE6AF - F3 0F 11 56 6C

	//hor
	//ptr: F9 F3 0F 10 73 04
	//starwarsbattlefrontii.exe+7EEA7A4 - F3 0F 10 73 04  

	static auto qwRecoilInsructionAddress = memory_utils::find_pattern(memory_utils::get_base(), "\xF3\x0F\x58\x56\x6C\xF3", "xxxxxx");
	static auto qwRecoilVerticalAxisInstructionAddress = memory_utils::find_pattern(memory_utils::get_base(), "\xF3\x0F\x11\x56\x6C\xEB", "xxxxxx");
	static auto qwRecoilHorizontalAxisInstructionAddress = (memory_utils::find_pattern(memory_utils::get_base(), "\xF9\xF3\x0F\x10\x73\x04", "xxxxxx") + 1);

#ifdef ENABLE_DEBUG_CONSOLE
	static auto print_once = []()
	{
		std::cout << __FUNCTION__ << ": qwRecoilInsructionAddress = 0x" << std::hex << qwRecoilInsructionAddress << std::endl;
		std::cout << __FUNCTION__ << ": qwRecoilVerticalAxisInstructionAddress = 0x" << std::hex << qwRecoilVerticalAxisInstructionAddress << std::endl;
		std::cout << __FUNCTION__ << ": qwRecoilHorizontalAxisInstructionAddress = 0x" << std::hex << qwRecoilHorizontalAxisInstructionAddress << std::endl;
		return true;
	}();
#endif // ENABLE_DEBUG_CONSOLE

	if (qwRecoilInsructionAddress == NULL
		|| qwRecoilVerticalAxisInstructionAddress == NULL
		|| qwRecoilHorizontalAxisInstructionAddress == NULL)
	{
		return;
	}

	if (bIsEnable)
	{
		memory_utils::patch_instruction(qwRecoilInsructionAddress, "\x90\x90\x90\x90\x90", 5);
		memory_utils::patch_instruction(qwRecoilVerticalAxisInstructionAddress, "\x90\x90\x90\x90\x90", 5);
		memory_utils::patch_instruction(qwRecoilHorizontalAxisInstructionAddress, "\x90\x90\x90\x90\x90", 5);
	}
	else
	{
		memory_utils::patch_instruction(qwRecoilInsructionAddress, "\xF3\x0F\x58\x56\x6C", 5);
		memory_utils::patch_instruction(qwRecoilVerticalAxisInstructionAddress, "\xF3\x0F\x11\x56\x6C", 5);
		memory_utils::patch_instruction(qwRecoilHorizontalAxisInstructionAddress, "\xF3\x0F\x10\x73\x04", 5);
	}
}

void CFeatures::FastReload(bool bIsEnable)
{
	//ptr: 41 8B 46 34 89 86 D0 01 00 00
	//bytes: starwarsbattlefrontii.exe+185CD83 - 41 8B 46 34

	static auto qwReloadInstructionAddress = memory_utils::find_pattern(memory_utils::get_base(), "\xF3\x0F\x10\x4F\x34\x48", "xxxxxx");
	static auto qwRelativeReloadInstructionAddress = memory_utils::find_pattern(memory_utils::get_base(), "\xF3\x0F\x11\x43\x34\x75", "xxxxxx");

#ifdef ENABLE_DEBUG_CONSOLE
	static auto print_once = []()
	{
		std::cout << __FUNCTION__ << ": qwReloadInstruction = 0x" << std::hex << qwReloadInstructionAddress << std::endl;
		std::cout << __FUNCTION__ << ": qwReloadInstruction2 = 0x" << std::hex << qwRelativeReloadInstructionAddress << std::endl;
		return true;
	}();
#endif // ENABLE_DEBUG_CONSOLE

	if (qwReloadInstructionAddress == NULL 
		|| qwRelativeReloadInstructionAddress == NULL)
	{
		return;
	}

	if (bIsEnable)
	{
		memory_utils::patch_instruction(qwReloadInstructionAddress, "\x90\x90\x90\x90\x90", 5);
		memory_utils::patch_instruction(qwRelativeReloadInstructionAddress, "\x90\x90\x90\x90\x90", 5);
	}
	else
	{
		memory_utils::patch_instruction(qwReloadInstructionAddress, "\xF3\x0F\x10\x4F\x34", 5);
		memory_utils::patch_instruction(qwRelativeReloadInstructionAddress, "\xF3\x0F\x11\x43\x34", 5);
	}
}

void CFeatures::InfinityJerk()
{
	//ptr: F3 0F 11 83 88 00 00 00 EB 0A
	//bytes: starwarsbattlefrontii.exe+8612E87 - F3 0F 11 83 88 00 00 00  

	static auto qwResetHeroesSpecEnergy = memory_utils::find_pattern(memory_utils::get_base(), "\xF3\x0F\x11\x83\x88\x00\x00\x00\xEB\x0A", "xxxxxxxxxx");

#ifdef ENABLE_DEBUG_CONSOLE
	static auto print_once = []()
	{
		std::cout << __FUNCTION__ << ": qwResetHeroesSpecEnergy = 0x" << std::hex << qwResetHeroesSpecEnergy << std::endl;
		return true;
	}();
#endif // ENABLE_DEBUG_CONSOLE

	if (qwResetHeroesSpecEnergy == NULL)
	{
		return;
	}

	if (vars::misc::endless_dash == false)
		return;
	
	if (GetAsyncKeyState(VK_RBUTTON))
	{
		memory_utils::patch_instruction(qwResetHeroesSpecEnergy, "\x90\x90\x90\x90\x90\x90\x90\x90", 8);
	}
	else
	{
		memory_utils::patch_instruction(qwResetHeroesSpecEnergy, "\xF3\x0F\x11\x83\x88\x00\x00\x00", 8);
	}
}

//lightsaber stamina
//starwarsbattlefrontii.exe+6C8AE80 - 89 03                
//38 89 03 48 8B 5C 24 30

//ptr: 49 8B 40 38 0F 2E 00 74 0D
//starwarsbattlefrontii.exe+86EAF86 - 0F2E 00            

//F3 0F 10 30 E8 FE 92 D1 F8
//starwarsbattlefrontii.exe+74B7039 - F3 0F10 30

void CFeatures::DisableHacks()
{
	if (vars::misc::fast_reload)
		this->FastReload(false);
	if (vars::misc::fire_rate)
		this->IncreaseFireRate(false);
	if (vars::misc::norecoil)
		this->NoRecoil(false);

}