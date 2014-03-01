#include "StdAfx.h"
#include "VMInfo.h"

CVMInfo::CVMInfo(void)
{
}

CVMInfo::~CVMInfo(void)
{
}

VOID CVMInfo::VM_GetSysInfo(void)
{
	GetSystemInfo(&this->SysInfo);
}

VOID CVMInfo::VM_GetMemStatus(void)
{
	GlobalMemoryStatus(&this->MemStatus);
}

BOOL CVMInfo::VM_GetMemInfoList(DWORD dwProcessId)
{
	//��������ڴ���Ϣ����
	this->MemList.clear();

	HANDLE hProcess = INVALID_HANDLE_VALUE;
	if (dwProcessId != GetCurrentProcessId())
	{
		//�򿪽���Ŀ��
		hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,dwProcessId);
        if(hProcess == NULL)
			return FALSE;
	}
	MEMORY_BASIC_INFORMATION MemBaseInfo;
	DWORD dwInfoSize = sizeof(MEMORY_BASIC_INFORMATION);
	DWORD dwCurPos = (DWORD)this->SysInfo.lpMinimumApplicationAddress;
	while(dwCurPos < (DWORD)this->SysInfo.lpMaximumApplicationAddress)
	{
		//��ѯָ�����̵�ָ����ַ״̬��Ϣ
		VirtualQueryEx(hProcess,(LPVOID)dwCurPos,&MemBaseInfo,dwInfoSize);
		//��״̬��Ϣ��ӵ�����
		this->MemList.push_back(MemBaseInfo);
		//��λ����һ������
		dwCurPos = (DWORD)MemBaseInfo.BaseAddress + MemBaseInfo.RegionSize;
	}
	return TRUE;
}
