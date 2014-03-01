#pragma once
#include <vector>
#include <list>
using namespace std ;

class CVMInfo
{
public:
	CVMInfo(void);
	~CVMInfo(void);
public:
	SYSTEM_INFO SysInfo;
	MEMORYSTATUS MemStatus;
	list<MEMORY_BASIC_INFORMATION> MemList;
	VOID VM_GetSysInfo(void);
	VOID VM_GetMemStatus(void);
	BOOL VM_GetMemInfoList(DWORD dwProcessId);
};
