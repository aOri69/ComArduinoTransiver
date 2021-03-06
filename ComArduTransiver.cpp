// ComArduTransiver.cpp: определяет точку входа для консольного приложения.
//
#include "stdafx.h"
#include <conio.h>
#include <iostream>
#include <cstdlib>
// Для опр. процессора
#include <windows.h>
#include <Psapi.h>
#include "TCHAR.h"
#include "pdh.h"
using namespace std;


HANDLE hSerial;
LPCTSTR sPortName = L"COM3";
DCB dcbSerialParams = { 0 };

void SendCOM();
//
void GetLoad();
static float CalculateCPULoad(unsigned long long idleTicks, unsigned long long totalTicks);
static unsigned long long FileTimeToInt64(const FILETIME & ft);
float GetCPULoad();
void TotalMem(DWORDLONG *totalVirtualMem, DWORDLONG *virtualMemUsed, DWORDLONG *totalPhysMem, DWORDLONG *physMemUsed, SIZE_T *virtualMemUsedByMe, SIZE_T *physMemUsedByMe);
//

int main()
{
	do
	{
	hSerial = ::CreateFile(sPortName, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (hSerial == INVALID_HANDLE_VALUE)
	{
		if (GetLastError() == ERROR_FILE_NOT_FOUND) cout << "Serial does not exist\n";
		else cout << "Another f**ing error\n";
	}
	else cout << "Port opened\n";
	dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
	if (!GetCommState(hSerial, &dcbSerialParams)) cout << "State error\n";
	dcbSerialParams.BaudRate = CBR_9600;
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;
	if (!GetCommState(hSerial, &dcbSerialParams)) cout << "Setting COM error\n";
	//
	int i = 0;
	do
	{
		SendCOM();
		Sleep(750);
		i++;
	} while (i!=100);
	//
	CloseHandle(hSerial);
	cout << "Port Closed";
	} while (true);
	//CloseHandle(hSerial);
	return 0;
}
void SendCOM() {
	/*DWORD iSize;
	char sRecievedChar;
	LPCVOID toSend;*/
	LPDWORD numWritten = 0;
	//LPOVERLAPPED numOverlapped;
	char myStr[8];
	int s;
	s = sizeof(myStr);
	_itoa_s((int)(GetCPULoad() * 100), myStr, sizeof(myStr), 10);
	for (int j = 0; j < sizeof(myStr); j++)
	{
		if (myStr[j] == '\0')
		{
			myStr[j + 2] = myStr[j];
			myStr[j + 1] = '\n';
			myStr[j] = '\r';
			break;
		}
	}
	WriteFile(hSerial, myStr, sizeof(myStr), numWritten, NULL);
	//ReadFile(hSerial, &sRecievedChar, 1, &iSize, 0); // получаем один байт
	//if (iSize > 0) cout << sRecievedChar;
	//sRecievedChar != '\n'
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////
void GetLoad() {
	DWORDLONG totalVirtualMem, virtualMemUsed, totalPhysMem, physMemUsed;
	SIZE_T virtualMemUsedByMe, physMemUsedByMe;
	do
	{
		system("cls");
		std::cout << "CPU: " << (int)(GetCPULoad() * 100) << '%' << std::endl;
		TotalMem(&totalVirtualMem, &virtualMemUsed, &totalPhysMem, &physMemUsed, &virtualMemUsedByMe, &physMemUsedByMe);
		std::cout << "Total Virtual Memory:       " << totalVirtualMem / 1024 / 1024 << " MB" << std::endl;
		std::cout << "Virtual Memory Used:        " << virtualMemUsed / 1024 / 1024 << " MB" << std::endl;
		std::cout << "Total Physical Memory:      " << totalPhysMem / 1024 / 1024 << " MB" << std::endl;
		std::cout << "Physical Memory Used:       " << physMemUsed / 1024 / 1024 << " MB" << std::endl;
		std::cout << "Virtual Memory Used By Me:  " << virtualMemUsedByMe << " KB" << std::endl;
		std::cout << "Physical Memory Used By Me: " << physMemUsedByMe << " KB" << std::endl;
		Sleep(1000);
	} while (true);
}
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
void TotalMem(DWORDLONG *totalVirtualMem, DWORDLONG *virtualMemUsed, DWORDLONG *totalPhysMem, DWORDLONG *physMemUsed, SIZE_T *virtualMemUsedByMe, SIZE_T *physMemUsedByMe)
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