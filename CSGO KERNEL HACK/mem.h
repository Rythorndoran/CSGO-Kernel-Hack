#pragma once
#include <ntifs.h>
#include <intrin.h>

namespace memory
{
	template <typename T>
	inline auto read(void* address) -> T
	{
		if(MmIsAddressValid(address))
		{
			return *reinterpret_cast<T*>(address);
		}
		T ZeroVal;
		RtlZeroMemory(&ZeroVal, sizeof(T));
		return ZeroVal;
	}

	template <typename T>
	inline void write(void* address,T val)
	{
		if (MmIsAddressValid(address))
		{
			 *reinterpret_cast<T*>(address) = val;
		}
	}

	template <typename T>
	inline auto read(ULONG64 address) -> T
	{
		if (MmIsAddressValid((void*)address))
		{
			return *reinterpret_cast<T*>(address);
		}
		T ZeroVal;
		RtlZeroMemory(&ZeroVal,sizeof(T));
		return ZeroVal;
	}

	template <typename T>
	inline void write(ULONG64 address, T val)
	{
		if (MmIsAddressValid((void*)address))
		{
			*reinterpret_cast<T*>(address) = val;
		}
	}


	inline bool read(void* address,void* buffer,size_t len)
	{
		if (MmIsAddressValid(address) && MmIsAddressValid(buffer))
		{
			RtlCopyMemory(buffer, address, len);
			return true;
		}
		return false;
	}

	inline bool write(void* address, void* buffer, size_t len)
	{
		if (MmIsAddressValid(address) && MmIsAddressValid(buffer))
		{
			RtlCopyMemory(address, buffer,len);
			return true;
		}
		return false;
	}

}
