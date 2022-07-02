#pragma once
#include <ntifs.h>
#include "imports.h"

inline void kernel_sleep(int ms)
{
    LARGE_INTEGER time;
    time.QuadPart = -(ms) * 10 * 1000;
    KeDelayExecutionThread(KernelMode, TRUE, &time);
}