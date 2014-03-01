#include "StdAfx.h"
#include "PEInfo.h"

CPEInfo::CPEInfo(void)
{
	this->isReady = FALSE ;
	this->pDosHeader = NULL ;
	this->pNtHeader = NULL ;
	this->pOptionalHeader = NULL ;
	this->lpResBase = NULL ;
	memset ( &MapItem, 0, sizeof(MapItem) ) ;
}

CPEInfo::~CPEInfo(void)
{
}

// ʹ���ڴ�ӳ�䷽ʽ��PE�ļ�ӳ�䵽��ǰ����
BOOL CPEInfo::CreateMap(TCHAR* pPathName)
{
	MapItem.hFile = CreateFile(
		pPathName,
		GENERIC_READ,
		FILE_SHARE_READ|FILE_SHARE_WRITE,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);
	if (MapItem.hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	//�����ļ�ӳ���ں˶���
    MapItem.lpMapAddr = (PBYTE)MapViewOfFile(MapItem.hMapFile,FILE_MAP_READ,0,0,0);
    if (MapItem.hMapFile == NULL)
    {
		DWORD dwErrorCode = GetLastError();
		CloseHandle(MapItem.hMapFile);
		CloseHandle(MapItem.hFile);
		return FALSE;
    }
	this->isReady = TRUE;
	return TRUE;
}

// ж��ӳ���ļ�
VOID CPEInfo::DeleteMap(void)
{
	UnmapViewOfFile(MapItem.lpMapAddr);
	CloseHandle(MapItem.hMapFile);
	CloseHandle(MapItem.hFile);
	this->isReady = FALSE;
}

// ��ӳ���ļ���������д���ļ�
BOOL CPEInfo::FlushData(LPVOID lpAddr, UINT nNumToFlush)
{

	return FlushViewOfFile(lpAddr,nNumToFlush);
}

// ȡ����Դ�Ļ�����Ϣ
void CPEInfo::GetResInfo(void)
{
	memset(&ResDirectory,0,sizeof(ResDirectory));
	for(int i = 0 ; i < MAX_RES_TYPES ; i++)
		this->ResList[i].clear();
	this->pDosHeader = (PIMAGE_DOS_HEADER)MapItem.lpMapAddr;
	this->pNtHeader = (PIMAGE_NT_HEADERS)((DWORD)MapItem.lpMapAddr+this->pDosHeader->e_lfanew);
    this->pOptionalHeader = &(this->pNtHeader->OptionalHeader);
	this->lpResBaseRva = (LPVOID)this->pOptionalHeader->DataDirectory[2].VirtualAddress;
	this->lpResBase = ImageRvaToVa(this->pNtHeader,this->MapItem.lpMapAddr,\
		this->pOptionalHeader->DataDirectory[2].VirtualAddress,NULL);

	int i,j;
	PIMAGE_RESOURCE_DIRECTORY pSubDir,pCurDir,pRootDir = (PIMAGE_RESOURCE_DIRECTORY)this->lpResBase;
    PIMAGE_RESOURCE_DIRECTORY_ENTRY pSubEntry,pCurEntry,pRootEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)\
		((DWORD)pRootDir+sizeof(IMAGE_RESOURCE_DIRECTORY));

	this->dwNamedEntry = pRootDir->NumberOfNamedEntries;
	this->dwIdEntry = pRootDir->NumberOfIdEntries;
	DWORD dwTypeNum = pRootDir->NumberOfNamedEntries + pRootDir->NumberOfIdEntries;
	for (j = 0 ; j < dwTypeNum ; j++)
	{
		this->ResDirectory[i].dwNameOrId     = pRootEntry[i].Name;
		this->ResDirectory[i].dwOffsetToData = pRootEntry[i].OffsetToData;

		pSubDir = (PIMAGE_RESOURCE_DIRECTORY)((DWORD)pRootDir + (this->ResDirectory[i].dwOffsetToData & 0x7fffffff));

		this->ResDirectory[i].wNumOfNamedEntry = pSubDir->NumberOfNamedEntries;
		this->ResDirectory[i].wNumOfIdEntry = pSubDir->NumberOfIdEntries;

		DWORD dwResNum = pSubDir->NumberOfNamedEntries + pSubDir->NumberOfIdEntries;
		pSubEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)pSubDir + sizeof(IMAGE_RESOURCE_DIRECTORY));

		for (j = 0 ; j < dwResNum; j++)
		{
			RES_ITEM  CurResItem;
			CurResItem.dwNameOrId      = pSubEntry[i].Name;
			CurResItem.dwOffsetToData  = pCurEntry[j].OffsetToData;

			pCurEntry = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)pCurDir+sizeof(IMAGE_RESOURCE_DIRECTORY));

			PIMAGE_RESOURCE_DATA_ENTRY pDataEntry = (PIMAGE_RESOURCE_DATA_ENTRY)((DWORD)pRootDir+pCurEntry->OffsetToData);
            CurResItem.dwDataOffset = pDataEntry->OffsetToData;
			CurResItem.dwDataSize = pDataEntry->Size;

			ResList[i].push_back(CurResItem);
		}
	}



}

// ������Դ�������������ڸ������е�������ȡ����Դ�Ļ�����Ϣ�����浽pResItem
void CPEInfo::GetResItemByIndex(int nRoot, int nSub, PRES_ITEM pResItem)
{
	list<RES_ITEM>::iterator p = this->ResList[nRoot-1].begin();
	for(int i = 2 ; i <= nSub ; i++)
		p++;
	pResItem->dwDataOffset = (*p).dwDataOffset;
	pResItem->dwDataSize   = (*p).dwDataSize;
}

// ��ָ������Դд��lpTarFile
BOOL CPEInfo::DumpResource(LPTSTR lpTarFile, LPVOID lpResStart, DWORD pResSize)
{
	HANDLE hFile = CreateFile(
		lpTarFile,
		GENERIC_READ|GENERIC_WRITE,
		FILE_SHARE_READ|FILE_SHARE_WRITE,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL
		);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

	//�����ļ�ӳ���ں˶���
	HANDLE hMapFile = CreateFileMapping(hFile,NULL,PAGE_READWRITE,0,pResSize,NULL);
	if (hMapFile == NULL)
	{
		CloseHandle(hFile);
		return FALSE;
	}

	//�����ļ���ͼ
	LPVOID lpMapAddr = (PBYTE)MapViewOfFile(hMapFile,FILE_MAP_READ|FILE_MAP_WRITE,0,0,pResSize);
    if (lpMapAddr == NULL)
    {
		DWORD dwErrorCode = GetLastError();
		CloseHandle(hMapFile);
		CloseHandle(hFile);
		return FALSE;
    }

	memcpy(lpMapAddr,lpResStart,pResSize);
	this->FlushData(lpMapAddr,pResSize);
	UnmapViewOfFile(lpMapAddr);
	CloseHandle(hMapFile);
	CloseHandle(hFile);
	return TRUE;
}
