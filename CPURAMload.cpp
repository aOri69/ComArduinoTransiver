// TestCpuLoad.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include <iostream>
#include <Windows.h>
#include <Psapi.h>
// Для опр. процессора
#include "TCHAR.h"
#include "pdh.h"

static float CalculateCPULoad(unsigned long long idleTicks, unsigned long long totalTicks)
{
	static unsigned long long _previousTotalTicks = 0;
	static unsigned long long _previousIdleTicks = 0;

	unsigned long long totalTicksSinceLastTime = totalTicks - _previousTotalTicks;
	unsigned long long idleTicksSinceLastTime = idleTicks - _previousIdleTicks;

	float ret = 1.0f - ((totalTicksSinceLastTime > 0) ? ((float)idleTicksSinceLastTime) / totalTicksSinceLastTime : 0);

	_previousTotalTicks = totalTicks;
	_previousIdleTicks = idleTicks;
	return ret;
}
static unsigned long long FileTimeToInt64(const FILETIME & ft) 
{ 
	return (((unsigned long long)(ft.dwHighDateTime)) << 32) | ((unsigned long long)ft.dwLowDateTime); 
}
// Returns 1.0f for "CPU fully pinned", 0.0f for "CPU idle", or somewhere in between
// You'll need to call this at regular intervals, since it measures the load between
// the previous call and the current one.  Returns -1.0 on error.
float GetCPULoad()
{
	FILETIME idleTime, kernelTime, userTime;
	return GetSystemTimes(&idleTime, &kernelTime, &userTime) ? CalculateCPULoad(FileTimeToInt64(idleTime), FileTimeToInt64(kernelTime) + FileTimeToInt64(userTime)) : -1.0f;
}
void TotalMem(DWORDLONG *totalVirtualMem,DWORDLONG *virtualMemUsed,DWORDLONG *totalPhysMem,DWORDLONG *physMemUsed, SIZE_T *virtualMemUsedByMe, SIZE_T *physMemUsedByMe)
{
	MEMORYSTATUSEX memInfo;
	//DWORDLONG totalVirtualMem, virtualMemUsed, totalPhysMem, physMemUsed;
	//PROCESS_MEMORY_COUNTERS_EX pmc;
	PROCESS_MEMORY_COUNTERS pmc;
	//SIZE_T virtualMemUsedByMe, physMemUsedByMe;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	*totalVirtualMem = memInfo.ullTotalPageFile;
	*totalPhysMem = memInfo.ullTotalPhys;
	*virtualMemUsed = memInfo.ullTotalPageFile - memInfo.ullAvailPageFile;
	*physMemUsed = memInfo.ullTotalPhys - memInfo.ullAvailPhys;
	GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
	*virtualMemUsedByMe = pmc.PagefileUsage;
	*physMemUsedByMe = pmc.WorkingSetSize;
}

int main()
{
	DWORDLONG totalVirtualMem, virtualMemUsed, totalPhysMem, physMemUsed;
	SIZE_T virtualMemUsedByMe, physMemUsedByMe;
	do
	{
		system("cls");
		std::cout << "CPU: " << (int)(GetCPULoad()*100) << '%' << std::endl;
		TotalMem(&totalVirtualMem, &virtualMemUsed, &totalPhysMem, &physMemUsed, &virtualMemUsedByMe, &physMemUsedByMe);
		std::cout << "Total Virtual Memory:       " << totalVirtualMem/ 1024 / 1024 << " MB" << std::endl;
		std::cout << "Virtual Memory Used:        " << virtualMemUsed/ 1024 / 1024 << " MB" << std::endl;
		std::cout << "Total Physical Memory:      " << totalPhysMem/ 1024 / 1024 << " MB" << std::endl;
		std::cout << "Physical Memory Used:       " << physMemUsed/ 1024 / 1024 << " MB" << std::endl;
		std::cout << "Virtual Memory Used By Me:  " << virtualMemUsedByMe << " KB" << std::endl;
		std::cout << "Physical Memory Used By Me: " << physMemUsedByMe << " KB" << std::endl;
		Sleep(1000);
	} while (true);
    return 0;
}




//
//static PDH_HQUERY cpuQuery;
//static PDH_HCOUNTER cpuTotal;
//
//void init() {
//	PdhOpenQuery(NULL, NULL, &cpuQuery);
//	// You can also use L"\\Processor(*)\\% Processor Time" and get individual CPU values with PdhGetFormattedCounterArray()
//	PdhAddEnglishCounter(cpuQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &cpuTotal);
//	PdhCollectQueryData(cpuQuery);
//}
//
//double getCurrentValue() {
//	PDH_FMT_COUNTERVALUE counterVal;
//
//	PdhCollectQueryData(cpuQuery);
//	PdhGetFormattedCounterValue(cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
//	return counterVal.doubleValue;
//}
//static ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
//static int numProcessors;
//static HANDLE self;
//
//void init() {
//	SYSTEM_INFO sysInfo;
//	FILETIME ftime, fsys, fuser;
//
//	GetSystemInfo(&sysInfo);
//	numProcessors = sysInfo.dwNumberOfProcessors;
//
//	GetSystemTimeAsFileTime(&ftime);
//	memcpy(&lastCPU, &ftime, sizeof(FILETIME));
//
//	self = GetCurrentProcess();
//	GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
//	memcpy(&lastSysCPU, &fsys, sizeof(FILETIME));
//	memcpy(&lastUserCPU, &fuser, sizeof(FILETIME));
//}
//
//double getCurrentValue() {
//	FILETIME ftime, fsys, fuser;
//	ULARGE_INTEGER now, sys, user;
//	double percent;
//
//	GetSystemTimeAsFileTime(&ftime);
//	memcpy(&now, &ftime, sizeof(FILETIME));
//
//	GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
//	memcpy(&sys, &fsys, sizeof(FILETIME));
//	memcpy(&user, &fuser, sizeof(FILETIME));
//	percent = (sys.QuadPart - lastSysCPU.QuadPart) +
//		(user.QuadPart - lastUserCPU.QuadPart);
//	percent /= (now.QuadPart - lastCPU.QuadPart);
//	percent /= numProcessors;
//	lastCPU = now;
//	lastUserCPU = user;
//	lastSysCPU = sys;
//
//	return percent * 100;
//}
