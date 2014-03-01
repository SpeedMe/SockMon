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
	DWORD dwOpenThread;                    // OpenThread ��ַ
	DWORD dwTerminateThread;               // TerminateThread ��ַ
	DWORD dwCid;                           // ��Ҫж�ص��߳�Cid
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
	// ��������
	ThreadParam Param = {0};
    Param.dwFreeLibrary = (DWORD)FreeLibrary;
	Param.dwGetModuleHandle = (DWORD)GetModuleHandleA;
    MoveMemory(Param.szModuleName,szModuleName,16);

	//��ָ���Ľ���
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwPid);
	if (hProcess == NULL)
	{
		OutputDebugString(_T("OpenProcess failed!"));
		return FALSE;
	}
	//����д��
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
	//����д��
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

	//�����߳�
	HANDLE hThread = NULL;
	hThread = CreateRemoteThread(hProcess,0,0,(LPTHREAD_START_ROUTINE)lpThread,lpParam,0,NULL);
    if (hThread == NULL)
    {
		return FALSE;
    }

	//������
	VirtualFreeEx(hProcess,lpThread,0,MEM_RELEASE);
	VirtualFreeEx(hProcess,lpParam,0,MEM_RELEASE);
	CloseHandle(hProcess);

	return TRUE;
}

//Զ���̺߳���
void CMySystem::RemoteThreadFun(PThreadParam PParam)
{
	DWORD dwFreeLibrary;
	DWORD dwGetModuleHandle;
	DWORD dwModuleName;

	dwFreeLibrary = PParam->dwFreeLibrary;
	dwGetModuleHandle = PParam->dwGetModuleHandle;
	dwModuleName = (DWORD)PParam->szModuleName;

	//�ͷŵ���ߴ���
	DWORD dwCount = 100;
	
	// ѭ��Ѱ��ָ��ģ��ľ��������ҵ�����ô����FreeLibrary�ͷ�
	// ֱ����ģ�鱻�ͷ�
	__asm
	{
START:
		push dwModuleName;             // ģ������ѹջ
		call dwGetModuleHandle;        // ����GetModuleHandle
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
