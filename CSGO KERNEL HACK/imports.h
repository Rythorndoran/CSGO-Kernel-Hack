#pragma once
#include<ntifs.h>
EXTERN_C NTKERNELAPI
PIMAGE_NT_HEADERS
RtlImageNtHeader(
	PVOID Base);

EXTERN_C NTKERNELAPI
NTSTATUS
MmCopyVirtualMemory(
	PEPROCESS SourceProcess,
	PVOID SourceAddress,
	PEPROCESS TargetProcess,
	PVOID TargetAddress,
	SIZE_T BufferSize,
	KPROCESSOR_MODE PreviousMode,
	PSIZE_T ReturnSize);

EXTERN_C NTKERNELAPI
PVOID
PsGetProcessWow64Process(_In_ PEPROCESS Process);

EXTERN_C NTKERNELAPI
PVOID
PsGetProcessPeb(_In_ PEPROCESS Process);

EXTERN_C NTKERNELAPI
PVOID
RtlFindExportedRoutineByName(
	_In_ PVOID ImageBase,
	_In_ PCCH RoutineName);

EXTERN_C NTKERNELAPI
PCHAR
PsGetProcessImageFileName(PEPROCESS Process);