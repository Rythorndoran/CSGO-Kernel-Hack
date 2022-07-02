#include "Headers.h"
#include "cheat.h"

HANDLE game_process_id = 0;
PEPROCESS game_eprocess = NULL;

PVOID m_client = 0;
PVOID m_engine = 0;

KEVENT driver_unload_event;

HANDLE g_work_thread;

client g_client;

void reduce_recoil()
{
	auto clamp_angle = [](Vector3 qaAng) noexcept -> Vector3
	{
		if (qaAng.x > 89.0f)
			qaAng.x = 89.0f;
		if (qaAng.x < -89.0f)
			qaAng.x = -89.0f;
		while (qaAng.y > 180.0f)
			qaAng.y -= 360.0f;
		while (qaAng.y < -180.0f)
			qaAng.y += 360.0f;
		qaAng.z = 0;
		return qaAng;
	};

	static Vector3 old_punch_angle;
	Vector3 aim_punch_angle = g_client.get_local().get_aim_punch();
	Vector3 view_angles = g_client.get_view_angles();

	float totalPunch = aim_punch_angle.x + aim_punch_angle.y;
	if (totalPunch != 0.f)
	{
		auto compensatedAngle = Vector3{
			((view_angles.x + old_punch_angle.x) - (aim_punch_angle.x * 2.f)),
			((view_angles.y + old_punch_angle.y) - (aim_punch_angle.y * 2.f)),0.f
		};
		auto newAngle = clamp_angle(compensatedAngle);
		old_punch_angle = Vector3{ aim_punch_angle.x * 2.f, aim_punch_angle.y * 2.f, 0.f };
		g_client.set_view_angles(newAngle);
	}
	else
	{
		old_punch_angle = Vector3{ 0.f,0.f,0.f };
	}
}


void trigger_bot()
{
	ULONG crosshair_id = g_client.get_local().get_crosshair_id();
	if (crosshair_id <= 0 || crosshair_id > 65)
		return;

	crosshair_id -= 1;

	entity target = g_client.get_entity(crosshair_id);

	if (!target.is_valid_palyer())
		return;

	if (target.get_team() == g_client.get_local().get_team())
		return;

	if (target.get_health() <= NULL)
		return;

	if (g_client.get_force_attack() == 4)
		g_client.set_force_attack(6);

}

void glow_hack()
{
	entity local = g_client.get_local();

	if (!local.is_valid_palyer())
		return;

	for (ULONG n = 0; n <= 32; n++)
	{
		entity player = g_client.get_entity(n);

		if (!player.is_valid_palyer())
			continue;

		if (player.get_team() == local.get_team())
			continue;

		if (player.get_health() <= NULL)
			continue;

		player.get_glow_mananger().set_glow();
	}

}




VOID work_thread(void* context [[maybe_unused]] )
{
	while (true)
	{
		kernel_sleep(2);
		if (KeReadStateEvent(&driver_unload_event))
			break;
		if (!game_eprocess)
			continue;
		if (!m_client || !m_engine)
			continue;
		KAPC_STATE apc_state;
		KeStackAttachProcess(game_eprocess, &apc_state);

		glow_hack();

		trigger_bot();

		reduce_recoil();

		KeUnstackDetachProcess(&apc_state);
	}
	PsTerminateSystemThread(0);
}


VOID load_image_notify([[maybe_unused]] PUNICODE_STRING FullImageName, [[maybe_unused]] HANDLE ProcessId, [[maybe_unused]] PIMAGE_INFO ImageInfo)
{
	if (game_process_id == ProcessId)
	{
		if (wcsstr(FullImageName->Buffer, L"\\csgo\\bin\\client.dll")) {
			DbgPrintEx(77, 0, "[+] Loaded Name: %ws \n", FullImageName->Buffer);
			DbgPrintEx(77, 0, "[+] Loaded Base: 0x%llx \n", ImageInfo->ImageBase);
			DbgPrintEx(77, 0, "[+] Loaded To Process: %d \n", ProcessId);
			m_client = ImageInfo->ImageBase;
		}
		if (wcsstr(FullImageName->Buffer, L"\\bin\\engine.dll")) {
			DbgPrintEx(77, 0, "[+] Loaded Name: %ws \n", FullImageName->Buffer);
			DbgPrintEx(77, 0, "[+] Loaded Base: 0x%llx \n", ImageInfo->ImageBase);
			DbgPrintEx(77, 0, "[+] Loaded To Process: %d \n", ProcessId);
			m_engine = ImageInfo->ImageBase;
		}

	}
}

VOID create_process_notify(PEPROCESS process, HANDLE process_id, PPS_CREATE_NOTIFY_INFO create_info)
{
	if (create_info)
	{
		PCHAR process_name = PsGetProcessImageFileName(process);
		if (0 == _stricmp(process_name, "csgo.exe"))
		{
			DbgPrintEx(77, 0, "[+] game_process_id: %d \n", process_id);
			DbgPrintEx(77, 0, "[+] game_eprocess: 0x%llx \n", process);
			game_process_id = process_id;
			game_eprocess = process;
		}
	}
	else
	{
		if (game_process_id == process_id)
		{
			game_process_id = 0;
			game_eprocess = NULL;
			m_client = 0;
			m_engine = 0;
			DbgPrintEx(77, 0, "[+] game client exit! \n");
		}
	}
}

EXTERN_C NTSTATUS DriverEntry([[maybe_unused]] PDRIVER_OBJECT drv_obj, [[maybe_unused]] PUNICODE_STRING registry_path)
{
	drv_obj->DriverUnload = []([[maybe_unused]] PDRIVER_OBJECT drv_obj) noexcept -> VOID
	{

		KeSetEvent(&driver_unload_event, IO_NO_INCREMENT, FALSE);

		ZwWaitForSingleObject(g_work_thread, FALSE, NULL);

		ZwClose(g_work_thread);

		PsRemoveLoadImageNotifyRoutine(&load_image_notify);

		PsSetCreateProcessNotifyRoutineEx(create_process_notify, TRUE);

		DbgPrintEx(77, 0, "[+] Driver Unload!\n");

		return;
	};
	*((ULONG*)drv_obj->DriverSection + 26) |= 0x20ul;

	KeInitializeEvent(&driver_unload_event, NotificationEvent, FALSE);

	PsCreateSystemThread(&g_work_thread, THREAD_ALL_ACCESS, NULL, NULL, NULL, work_thread, NULL);

	PsSetCreateProcessNotifyRoutineEx(create_process_notify, FALSE);

	PsSetLoadImageNotifyRoutine(&load_image_notify);

	DbgPrintEx(77, 0, "[+] Driver Load!\n");

	return STATUS_SUCCESS;
}