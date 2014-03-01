#pragma once

// ���������Ϣ�ṹ
typedef struct _PROCESS_INFO {
	DWORD	dwProcessId ;					// ����ID
	WCHAR	lpPath[MAX_PATH_LENGTH] ;		// ����·��ȫ��
} PROCESS_INFO, *LPPROCESS_INFO ;
		
// �����׽�����Ϣ�ṹ
typedef struct _SOCKET_INFO {
	SOCKET		s ;							// �׽��־��
	int			protocol ;					// Э������
	DWORD		dwProcessId ;				// ����ID
	SYSTEMTIME	CreateTime ;				// ����ʱ��
} SOCKET_INFO, *LPSOCKET_INFO ;

// ���������Ϣ�ṹ
typedef struct _OPERATE_INFO {
	SOCKET		s ;							// �׽��־��
	WCHAR		szOperate[64] ;				// �������ͣ��������õĺ�����
	DWORD		dwProcessId ;				// ����ID
	DWORD		dwThreadId ;				// �߳�ID
	sockaddr_in	LocalAddr ;					// ���ص�ַ��Ϣ
	sockaddr_in RemoteAddr ;				// Զ�̵�ַ��Ϣ
	int			nErrorCode ;				// �������صĴ�����
	DWORD		dwDataSize ;				// �������ݵĴ�С
	LPBYTE		lpData ;					// ָ��������
} OPERATE_INFO, *LPOPERATE_INFO ;



class CData
{
public:
	CData(void);
public:
	~CData(void);

private:
	HANDLE				hFile ;
	HANDLE				hMapFile ;
	LPBYTE				lpData ;
	UINT				nCount ;
	CRITICAL_SECTION	cs ;

public:
	VOID	AddProcessInfo ( LPPROCESS_INFO lpProcessInfo ) ;
	VOID	AddSocketInfo ( LPSOCKET_INFO lpSockInfo ) ;
	VOID	AddOperateInfo ( LPOPERATE_INFO lpOperateInfo ) ;

private:
	VOID	SendDataToClient ( LPBYTE lpData, DWORD dwSize ) ;
	VOID	SendNotifyMessage ( WPARAM wParam, LPARAM lParam ) ;
	UINT	GetIndex () ;

private:
	BOOL	SetupShareMemeory () ;
	VOID	ReleaseShareMemory () ;
};
