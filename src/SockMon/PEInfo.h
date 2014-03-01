#pragma once

#include <dbghelp.h>
#pragma comment ( lib, "dbghelp.lib" )

#include <list>
using namespace std ;

// �����ڴ�ӳ��ṹ
typedef struct _MAP_ITEM {
	HANDLE	hFile ;				// �ļ��ں˶�����
	HANDLE	hMapFile ;			// �ļ�ӳ�������
	LPVOID	lpMapAddr ;			// ӳ�����ַ
} MAP_ITEM ;
typedef MAP_ITEM* PMAP_ITEM ;

#define MAX_RES_TYPES 64

// ������Դ������Ϣ
typedef struct _RES_ITEM {
	DWORD	dwNameOrId ;		// ��Դ����ƫ��������ԴID
	DWORD	dwOffsetToData ;	// ��Դ����ƫ����
	DWORD	dwDataOffset ;		// ֻ��IMAGE_DATA_DIRECTORY_ENTRY��Ч
	DWORD	dwDataSize ;		// ֻ��IMAGE_DATA_DIRECTORY_ENTRY��Ч
	WORD	wNumOfNamedEntry ;	// ���������
	WORD	wNumOfIdEntry ;		// ID�����
	struct _RES_ITEM () {
		dwNameOrId = dwOffsetToData = dwDataOffset = dwDataSize = wNumOfNamedEntry = wNumOfIdEntry = 0 ;
	}
} RES_ITEM ;
typedef RES_ITEM* PRES_ITEM ;

class CPEInfo
{
public:
	CPEInfo(void);
	~CPEInfo(void);

public:
	BOOL						isReady ;						// ��ʶPE�ļ��Ƿ��Ѿ�ӳ��
	MAP_ITEM					MapItem ;						// ӳ��ṹ
	PIMAGE_DOS_HEADER			pDosHeader ;					// ָ��DOS�ļ�ͷ
	PIMAGE_NT_HEADERS			pNtHeader ;						// ָ��NT�ļ�ͷ
	PIMAGE_OPTIONAL_HEADER32	pOptionalHeader ;				// ָ���ѡ�ļ�ͷ
	LPVOID						lpResBaseRva ;					// ��ԴRVA
	LPVOID						lpResBase ;						// ��ԴVA
	DWORD						dwNamedEntry, dwIdEntry ;		// ��Դ��Ŀ¼
	RES_ITEM					ResDirectory[MAX_RES_TYPES] ;	// ��Դ�������飬��¼�����͵Ļ�����Ϣ
	list<RES_ITEM>				ResList[MAX_RES_TYPES] ;		// ��Դ����ÿһ���͵���Դ��Ӧһ������


	// ʹ���ڴ�ӳ�䷽ʽ��PE�ļ�ӳ�䵽��ǰ����
	BOOL CreateMap(TCHAR* pPathName);
	// ж��ӳ���ļ�
	VOID DeleteMap(void);
	// ��ӳ���ļ���������д���ļ�
	BOOL FlushData(LPVOID lpAddr, UINT nNumToFlush);
	// ȡ����Դ�Ļ�����Ϣ
	void GetResInfo(void);
	// ������Դ�������������ڸ������е�������ȡ����Դ�Ļ�����Ϣ�����浽pResItem
	void GetResItemByIndex(int nRoot, int nSub, PRES_ITEM pResItem);
	// ��ָ������Դд��lpTarFile
	BOOL DumpResource(LPTSTR lpTarFile, LPVOID lpResStart, DWORD pResSize);
};
