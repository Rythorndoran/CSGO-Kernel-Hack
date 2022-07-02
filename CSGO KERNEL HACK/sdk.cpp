#include "cheat.h"
extern PVOID m_client;
extern PVOID m_engine;

void glow_mananger::set_glow()
{
	if (!base) return;
	memory::write<float>((base + 0xC), 1.f);
	memory::write<float>((base + 0x14), 1.f);
	memory::write<bool>((base + 0x28), true);
	memory::write<bool>((base + 0x29), false);
}

BOOLEAN entity::is_valid_palyer()
{
	return (base != NULL);
}

ULONG64 entity::get_base() {
	return base;
}

ULONG entity::get_health() {
	if (!base) return NULL;
	return memory::read<ULONG>(base + m_iHealth);
}

ULONG entity::get_team() {
	if (!base) return NULL;
	return memory::read<ULONG>(base + m_iTeamNum);
}

ULONG entity::get_crosshair_id() {
	if (!base) return NULL;
	return memory::read<ULONG>(base + m_iCrosshairId);
}

Vector3 entity::get_aim_punch() {
	if (!base) return {0,0,0};
	Vector3 aimPunch = memory::read<Vector3>(base + m_aimPunchAngle);
	return aimPunch;
}

glow_mananger entity::get_glow_mananger()
{
	if (!base) glow_mananger(NULL);
	ULONG GLPOINTR = memory::read<ULONG>(ULONG64(m_client) + (DWORD64)dwGlowObjectManager);
	ULONG GLcpg = memory::read<ULONG>(base + (DWORD64)m_iGlowIndex);
	ULONG64 addr = GLPOINTR + (GLcpg * 0x38);
	return glow_mananger(addr);
}

entity client::get_entity(int index)
{
	auto base = memory::read<ULONG>(ULONG64(m_client) + dwEntityList + index * 0x10);
	return entity(base);
}

entity client::get_local()
{
	auto base = memory::read<ULONG>(ULONG64(m_client) + dwLocalPlayer);
	return entity(base);
}

Vector3 client::get_view_angles() {
	ULONG addr = memory::read<ULONG>(ULONG64(m_engine) + dwClientState) + dwClientState_ViewAngles;
	return memory::read<Vector3>(addr);
}

void client::set_view_angles(Vector3 new_view_angles) {
	if (!_isnan(new_view_angles.y) && !_isnan(new_view_angles.x))
	{
		ULONG addr = memory::read<ULONG>(ULONG64(m_engine) + dwClientState) + dwClientState_ViewAngles;
		memory::write<Vector3>(addr, new_view_angles);
	}
}

void client::set_force_attack(int val)
{
	memory::write<int>(ULONG64(m_client) + dwForceAttack, val);
}

int client::get_force_attack()
{
	return memory::read<int>(ULONG64(m_client) + dwForceAttack);
}