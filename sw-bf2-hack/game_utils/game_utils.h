namespace game_utils
{
	inline bool world_to_screen(const Matrix4x4 &matrix, const Vector& vIn, float* flOut)
	{
		const auto &view_projection = matrix;

		float w = view_projection.m[0][3] * vIn.x + view_projection.m[1][3] * vIn.y + view_projection.m[2][3] * vIn.z + view_projection.m[3][3];

		if (w < 0.01)
			return false;

		flOut[0] = view_projection.m[0][0] * vIn.x + view_projection.m[1][0] * vIn.y + view_projection.m[2][0] * vIn.z + view_projection.m[3][0];
		flOut[1] = view_projection.m[0][1] * vIn.x + view_projection.m[1][1] * vIn.y + view_projection.m[2][1] * vIn.z + view_projection.m[3][1];

		float invw = 1.0f / w;

		flOut[0] *= invw;
		flOut[1] *= invw;

		int width, height;

		auto io = ImGui::GetIO();
		width = io.DisplaySize.x;
		height = io.DisplaySize.y;

		float x = (float)width / 2;
		float y = (float)height / 2;

		x += 0.5 * flOut[0] * (float)width + 0.5;
		y -= 0.5 * flOut[1] * (float)height + 0.5;

		flOut[0] = x;
		flOut[1] = y;

		return true;
	}

	inline Matrix4x4 get_matrix()
	{
		return memory_utils::read<Matrix4x4>({ memory_utils::get_base_address(), 0x3FFBE10, 0xC8, 0x0, 0x20, 0x210, 0x248, 0x9E0, 0x430 });
	}

	inline DWORD64 get_entity_list()
	{
		return memory_utils::read<DWORD64>({ memory_utils::get_base_address(), 0x3EFAA20, 0x8 });
	}

	class CMovementController
	{
	public:
		CMovementController* get__class()
		{
			return this;
		}

		Vector get_origin()
		{
			return memory_utils::read<Vector>({ (DWORD64)this, 0x20 });
		}

		Vector get_aabb_min()
		{
			return Vector(0.f, -0.1f, 0.f);
		}

		Vector get_aabb_max()
		{
			return Vector(0.f, 1.7f, 0.f);
		}
	};

	class CHealthComponent
	{
	public:
		CHealthComponent* get__class()
		{
			return this;
		}

		float get_health()
		{
			return memory_utils::read<float>({ (DWORD64)this, 0x20 });
		}

		float get_max_health()
		{
			return memory_utils::read<float>({ (DWORD64)this, 0x24 });
		}
	};

	class CWeapon
	{
	public:

	};

	class CSoldierEntity
	{
	public:
		CSoldierEntity* get__class()
		{
			return this;
		}

		CHealthComponent* get_health_component()
		{
			return memory_utils::read<CHealthComponent*>({ (DWORD64)this, 0x2C8 });
		}

		bool is_visible()
		{
			return (memory_utils::read<bool>({ (DWORD64)this, 0xA58 }) == false);
		}

		CMovementController* get_movement_controller()
		{
			return memory_utils::read<CMovementController*>({ (DWORD64)this, 0x758 });
		}

		CWeapon* get_weapon_soldier()
		{
			return memory_utils::read<CWeapon*>({ (DWORD64)this, 0xA28 });
		}
	};

	class CVehicleEntity
	{
	public:
		CVehicleEntity* get__class()
		{
			return this;
		}
	};

	class CPseduoEnitity
	{
	public:
		CPseduoEnitity* get__class()
		{
			return this;
		}

		static CPseduoEnitity* get_local_entity(DWORD64 entity_list_ptr)
		{
			return memory_utils::read<CPseduoEnitity*>({ entity_list_ptr, 0x80 });
		}

		static CPseduoEnitity* get_entity_by_id(DWORD64 entity_list_ptr, int id)
		{
			return memory_utils::read<CPseduoEnitity*>( { entity_list_ptr, (DWORD64)((id * 0x8) + 0x88) } );
		}

		char* get_name()
		{
			return memory_utils::read_string({ (DWORD64)this, 0x68 });
		}

		int get_team()
		{
			return memory_utils::read<int>({ (DWORD64)this, 0x58 });
		}

		CSoldierEntity* get_soldier_entity()
		{
			return memory_utils::read<CSoldierEntity*>({ (DWORD64)this, 0x210 });
		}
	};
}