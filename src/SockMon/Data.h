#pragma once



typedef struct _OPERATE_INFO {
	SOCKET		s ;
	WCHAR		szOperate[64] ;
	DWORD		dwProcessId ;
	DWORD		dwThreadId ;
	sockaddr_in	LocalAddr ;
	sockaddr_in RemoteAddr ;
	int			nErrorCode ;
	DWORD		dwDataSize ;
	LPBYTE		lpData ;
} OPERATE_INFO, *LPOPERATE_INFO ;

typedef struct _OPERATE_ITEM {
	UINT			nOperateIndex ;				// �������׽��ֵĲ�������
	OPERATE_INFO	OperateInfo ;				// ������Ϣ�ṹ
} OPERATE_ITEM, *LPOPERATE_ITEM ;

typedef struct _SOCKET_INFO {
	SOCKET		s ;
	int			protocol ;
	DWORD		dwProcessId ;
	SYSTEMTIME	CreateTime ;
} SOCKET_INFO, *LPSOCKET_INFO ;

typedef struct _SOCKET_ITEM {
	UINT					nSocketIndex ;		// ���������̵��׽�������
	UINT					nOperateCount ;		// ���׽����������Ĳ�����
	SOCKET_INFO				SocketInfo ;		// �׽�����Ϣ
	vector<OPERATE_ITEM>	OperateTable ;		// ������Ϣ��
	HTREEITEM				hTreeItem ;			// ���οؼ�������
} SOCKET_ITEM, *LPSOCKET_ITEM ;

typedef struct _PROCESS_INFO {
	DWORD	dwProcessId ;
	WCHAR	lpPath[MAX_PATH_LENGTH] ;
} PROCESS_INFO, *LPPROCESS_INFO ;

typedef struct _PROCESS_ITEM {
	UINT					nProcessIndex ;		// ��������
	UINT					nSocketCount ;		// �ý������������׽�����
	PROCESS_INFO			ProcessInfo ;		// ������Ϣ
	vector<SOCKET_ITEM>		SocketTable ;		// �׽�����Ϣ��
	HTREEITEM				hTreeItem ;			// ���οؼ�������
} PROCESS_ITEM, *LPPROCESS_ITEM ;

typedef struct _SOCKLINE{
	int       type;                 // ��������
	DWORD     Length;               // ���ݰ���С
}SOCKLINE,*LPSOCKLINE;

class CData
{
public:
	CData(void);
public:
	~CData(void);

private:
	//volatile LONG			nCount ;
	LONG					nCount ;
	HANDLE					hFile ;
	HANDLE					hMapFile ;
	LPBYTE					lpData ;
	CTreeCtrl*				pSockTree ;
	HTREEITEM				hRootTree ;

	vector<PROCESS_ITEM>	ProcessTable ;
public:
	//�����������OnDraw��ʹ��
	vector<SOCKLINE>             SockLine;     


public:
	VOID SetTreeCtrl ( CTreeCtrl* pTree ) ;
	VOID UpdateInfo () ;
	VOID ResetSockData () ;
	VOID FlushSockData () ;
	BOOL IsFull () ;

private:
	VOID AddProcessItem( LPPROCESS_INFO lpProcessInfo ) ;
	VOID AddSocketItem( LPSOCKET_INFO lpSocketInfo ) ;
	VOID AddOperateItem( LPOPERATE_INFO lpOperateInfo ) ;

private:
	UINT GetProcessIndexById ( UINT nProcessId ) ;
	UINT GetSocketIndexByHandle ( UINT nProcessIndex,  SOCKET s ) ;

private:
	BOOL SetupShareMemeory () ;
	VOID ReleaseShareMemory () ;
};