#ifndef _PERF_H_
#define _PERF_H_


//NEED TO PORT THIS

#include "windows.h";
#include "psapi.h";
#include "TCHAR.h";
#include "pdh.h";

struct MemInfo
{
	size_t totalMemory, memoryInUse, usedBy, usedByPeak;
};
static MEMORYSTATUSEX mem_;
static PROCESS_MEMORY_COUNTERS pmc_;
static HANDLE process_;
static SYSTEM_INFO info_;
static int procCount_;
static ULARGE_INTEGER cpu_, sys_, user_;

static class Perf
{
public:
	static void Init()
	{
		GlobalMemoryStatusEx(&mem_);
		process_ = GetCurrentProcess();
		GetSystemInfo(&info_);
		procCount_ = info_.dwNumberOfProcessors;

		FILETIME ftime, fsys, fuser;
		GetSystemTimeAsFileTime(&ftime);
		memcpy(&cpu_, &ftime, sizeof(FILETIME));

		GetProcessTimes(process_, &ftime, &ftime, &fsys, &fuser);
		memcpy(&sys_, &fsys, sizeof(FILETIME));
		memcpy(&user_, &fuser, sizeof(FILETIME));
	}
	static MemInfo GetMemoryInfo()
	{
		mem_.dwLength = sizeof(MEMORYSTATUSEX);
		DWORDLONG totalMemory = mem_.ullTotalPhys;
		DWORDLONG memoryInUse = mem_.ullTotalPhys - mem_.ullAvailPhys;
		GetProcessMemoryInfo(process_, &pmc_, sizeof(pmc_));
		DWORDLONG inUsed = pmc_.WorkingSetSize;
		DWORDLONG mostUsed = pmc_.PeakWorkingSetSize;

		return MemInfo{ totalMemory, memoryInUse, inUsed, mostUsed };
	}

	static double GetCPUInfo()
	{
		FILETIME ftime, fsys, fuser;
		ULARGE_INTEGER now, sys, user;
		double percent;

		GetSystemTimeAsFileTime(&ftime);
		memcpy(&now, &ftime, sizeof(FILETIME));

		GetProcessTimes(process_, &ftime, &ftime, &fsys, &fuser);
		memcpy(&sys, &fsys, sizeof(FILETIME));
		memcpy(&user, &fuser, sizeof(FILETIME));
		percent = (sys.QuadPart - sys_.QuadPart) +
			(user.QuadPart - user_.QuadPart);
		percent /= (now.QuadPart - cpu_.QuadPart);
		percent /= procCount_;
		cpu_ = now;
		user_ = user;
		sys_ = sys;

		return percent * 100;
	}
private:
};
#endif