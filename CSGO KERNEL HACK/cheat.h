#pragma once
#include <ntifs.h>
#include <float.h>
#include "mem.h"

constexpr auto dwEntityList = 0x4DDB92C;
constexpr auto dwLocalPlayer = 0xDBF4CC;
constexpr auto dwForceAttack = 0x320BE10;
constexpr auto dwGlowObjectManager = 0x5324588;
constexpr auto m_iHealth = 0x100;
constexpr auto m_iTeamNum = 0xF4;
constexpr auto m_iCrosshairId = 0x11838;
constexpr auto m_iGlowIndex = 0x10488;
constexpr auto m_aimPunchAngle = 0x303C;
constexpr auto dwClientState = 0x58CFDC;
constexpr auto dwClientState_ViewAngles = 0x4D90;

struct Vector3
{
	float x = 0; float y = 0; float z = 0;
	Vector3 operator+(const Vector3& pos)
	{
		Vector3 ret;
		ret.x = x + pos.x;
		ret.y = y + pos.y;
		ret.z = z + pos.z;
		return ret;
	}
	Vector3 operator-(const Vector3& pos)
	{
		Vector3 ret;
		ret.x = x - pos.x;
		ret.y = y - pos.y;
		ret.z = z - pos.z;
		return ret;
	}
	bool operator==(const Vector3& pos)
	{
		if (pos.x == x && pos.y == y && pos.z == z)
			return true;
		return false;
	}

	Vector3() {};

	Vector3(float _x, float _y, float _z) noexcept : x{ _x }, y{ _y }, z{ _z } {}

};

class glow_mananger
{
private:
	ULONG64 base;

public:
	glow_mananger() { base = 0; };

	glow_mananger(ULONG64 _base) :base(_base) {};

	~glow_mananger() {};
public:
	void set_glow();
};

class entity
{

private:
	ULONG64 base;

public:
	entity() { base = 0; };
	entity(ULONG64 entitybase) :base(entitybase) {};
	~entity() {};

public:
	BOOLEAN is_valid_palyer();

	ULONG64 get_base();

	ULONG get_health();

	ULONG get_team();

	ULONG get_crosshair_id();

	Vector3 get_aim_punch();

	glow_mananger get_glow_mananger();
};

class client
{
public:
	entity get_entity(int index);

	entity get_local();

	Vector3 get_view_angles();

	void set_view_angles(Vector3 new_view_angles);

	void set_force_attack(int val = 6);

	int get_force_attack();
};
