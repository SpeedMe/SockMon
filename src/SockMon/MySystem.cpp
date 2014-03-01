#include "StdAfx.h"
#include "MySystem.h"



CMySystem::CMySystem(void)
{
}

CMySystem::~CMySystem(void)
{
}

void CMySystem::KillProcessFun(PKillThreadParam PParam)
{
	DWORD dwOpenThread;                    // OpenThread 地址
	DWORD dwTerminateThread;               // TerminateThread 地址
	DWORD dwCid;                           // 需要卸载的线程Cid
	HANDLE hThreadHandle=0;
	dwOpenThread = PParam->dwOpenThread;
	dwTerminateThread = PParam->dwTerminateThread;
	dwCid = (DWORD)PParam->dwCid;
	__asm{
		push dwCid
			push 0
			push THREAD_ALL_ACCESS
			mov eax,dwOpenThread
			call eax
			mov hThreadHandle,eax
			push 0
			push hThreadHandle
			mov eax,dwTerminateThread
			call eax
	}



	return;
}

BOOL CMySystem::FreeModuleByPid(TCHAR* szModuleName, DWORD dwPid)
{
	// 参数构造
	ThreadParam Param = {0};
    Param.dwFreeLibrary = (DWORD)FreeLibrary;
	Param.dwGetModuleHandle = (DWORD)GetModuleHandleA;
    MoveMemory(Param.szModuleName,szModuleName,16);

	//打开指定的进程
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwPid);
	if (hProcess == NULL)
	{
		OutputDebugString(_T("OpenProcess failed!"));
		return FALSE;
	}
	//参数写入
	LPVOID lpParam = NULL;
	lpParam = VirtualAllocEx(hProcess,NULL,sizeof(Param),MEM_COMMIT,PAGE_READWRITE);
	if (lpParam == NULL)
	{
		return FALSE;
	}
    if (!WriteProcessMemory(hProcess,lpParam,&Param,sizeof(Param),0))
    {
		VirtualFreeEx(hProcess,lpParam,0,MEM_RELEASE);
		return FALSE;
    }
	//函数写入
	LPVOID lpThread = NULL;
	lpThread = VirtualAllocEx(hProcess,NULL,0x100,MEM_COMMIT,PAGE_EXECUTE_READWRITE);
    if (lpThread == NULL)
    {
		return FALSE;
    }
	if (!WriteProcessMemory(hProcess,lpThread,(LPCVOID)*CMySystem::RemoteThreadFun,0x100,0))
	{
		VirtualFreeEx(hProcess,lpThread,0,MEM_RELEASE);
		return FALSE;
	}

	//创建线程
	HANDLE hThread = NULL;
	hThread = CreateRemoteThread(hProcess,0,0,(LPTHREAD_START_ROUTINE)lpThread,lpParam,0,NULL);
    if (hThread == NULL)
    {
		return FALSE;
    }

	//清理工作
	VirtualFreeEx(hProcess,lpThread,0,MEM_RELEASE);
	VirtualFreeEx(hProcess,lpParam,0,MEM_RELEASE);
	CloseHandle(hProcess);

	return TRUE;
}

//远程线程函数
void CMySystem::RemoteThreadFun(PThreadParam PParam)
{
	DWORD dwFreeLibrary;
	DWORD dwGetModuleHandle;
	DWORD dwModuleName;

	dwFreeLibrary = PParam->dwFreeLibrary;
	dwGetModuleHandle = PParam->dwGetModuleHandle;
	dwModuleName = (DWORD)PParam->szModuleName;

	//释放的最高次数
	DWORD dwCount = 100;
	
	// 循环寻找指定模块的句柄，如果找到，那么调用FreeLibrary释放
	// 直到该模块被释放
	__asm
	{
START:
		push dwModuleName;             // 模块名称压栈
		call dwGetModuleHandle;        // 调用GetModuleHandle
		test eax,eax;
		jz OVER;
		dec dwCount;
		jz OVER;
		push eax;
		call dwFreeLibrary;
		test eax,eax;
		jnz START;

OVER:
	}
	return;


}
