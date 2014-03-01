// LSP.cpp : ���� DLL �ĳ�ʼ�����̡�
//

#include "stdafx.h"
#include "LSP.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "Data.h"

//
//TODO: ����� DLL ����� MFC DLL �Ƕ�̬���ӵģ�
//		��Ӵ� DLL �������κε���
//		MFC �ĺ������뽫 AFX_MANAGE_STATE ����ӵ�
//		�ú�������ǰ�档
//
//		����:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// �˴�Ϊ��ͨ������
//		}
//
//		�˺������κ� MFC ����
//		������ÿ��������ʮ����Ҫ������ζ��
//		��������Ϊ�����еĵ�һ�����
//		���֣������������ж������������
//		������Ϊ���ǵĹ��캯���������� MFC
//		DLL ���á�
//
//		�й�������ϸ��Ϣ��
//		����� MFC ����˵�� 33 �� 58��
//


// CLSPApp

BEGIN_MESSAGE_MAP(CLSPApp, CWinApp)
END_MESSAGE_MAP()


// CLSPApp ����

CLSPApp::CLSPApp()
{
	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CLSPApp ����

//////////////////////////////////////////////////////////////////////////////////////
CLSPApp theApp;
char	szLogBuf[MAX_BUF_SIZE] = {0} ;
WCHAR	szLogBufW[MAX_BUF_SIZE] = {0} ;

CData			data ;
CProvider		Provider ;
WSPPROC_TABLE	ProcTable ;	// �²㺯���б�


// CLSPApp ��ʼ��

BOOL CLSPApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}


BOOL CLSPApp::ExitInstance ()
{
	return CWinApp::ExitInstance () ;
}

VOID MY_OutputDebugStringA ( const char* szFormat,...)
{
#ifdef LOG_OUT
	va_list argList ;
	va_start ( argList, szFormat ) ;
	vsprintf ( szLogBuf, szFormat, argList ) ;
	va_end ( argList ) ;

	OutputDebugStringA ( szLogBuf ) ;
#endif
}

VOID MY_OutputDebugStringW ( const wchar_t* szFormat,...)
{
#ifdef LOG_OUT

	va_list argList ;
	va_start ( argList, szFormat ) ;
	vswprintf ( szLogBufW, sizeof(szLogBufW), szFormat, argList ) ;
	va_end ( argList ) ;

	OutputDebugStringW ( szLogBufW ) ;
#endif
}

SOCKET WSPAPI WSPSocket( int af, int type,int protocol,\
	LPWSAPROTOCOL_INFO lpProtocolInfo,GROUP g,DWORD dwFlags,LPINT lpErrno )
{
	SOCKET s = ProcTable.lpWSPSocket ( af, type, protocol, lpProtocolInfo, g, dwFlags, lpErrno ) ;

	MY_OutputDebugStringA ( szLogBuf, "[WSPSocket]socket=0x%08X", s ) ;

	SOCKET_INFO SockInfo = {0} ;
	SockInfo.s = s ;
	SockInfo.protocol = protocol ;
	SockInfo.dwProcessId = GetCurrentProcessId() ;
	GetLocalTime ( &SockInfo.CreateTime ) ; 
	data.AddSocketInfo ( &SockInfo ) ;
	return s ;
}


int WSPAPI WSPSend(
				   SOCKET s,
				   LPWSABUF lpBuffers,
				   DWORD dwBufferCount,
				   LPDWORD lpNumberOfBytesSent,
				   DWORD dwFlags,
				   LPWSAOVERLAPPED lpOverlapped,
				   LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine,
				   LPWSATHREADID lpThreadId,
				   LPINT lpErrno
				   )
{
	MY_OutputDebugStringA ( szLogBuf, "[WSPSend]socket=0x%08X", s ) ;

	int ret= ProcTable.lpWSPSend ( s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, \
		dwFlags, lpOverlapped, lpCompletionRoutine, lpThreadId, lpErrno ) ;


	OPERATE_INFO operate = {0} ;
	operate.s = s ;
	operate.dwProcessId = GetCurrentProcessId () ;
	operate.dwThreadId = GetCurrentThreadId() ;
	operate.nErrorCode = *lpErrno ;
	operate.dwDataSize = *lpNumberOfBytesSent ;
	operate.lpData	   = NULL ;	
	wcscpy ( operate.szOperate, L"WSPSend" ) ;

	int namelen = sizeof(sockaddr) ;
	getsockname ( s, (sockaddr*)&operate.LocalAddr, &namelen ) ;
	getpeername ( s, (sockaddr*)&operate.RemoteAddr, &namelen ) ;
	data.AddOperateInfo ( &operate ) ;

	return ret ;
}

int WSPAPI WSPSendTo(
					 SOCKET s,
					 LPWSABUF lpBuffers,
					 DWORD dwBufferCount,
					 LPDWORD lpNumberOfBytesSent,
					 DWORD dwFlags,
					 const struct sockaddr* lpTo,
					 int iTolen,
					 LPWSAOVERLAPPED lpOverlapped,
					 LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine,
					 LPWSATHREADID lpThreadId,
					 LPINT lpErrno
					 )
{
	MY_OutputDebugStringA ( szLogBuf, "[WSPSendTo]socket=0x%08X", s ) ;

	int ret = ProcTable.lpWSPSendTo ( s, lpBuffers, dwBufferCount, lpNumberOfBytesSent, \
		dwFlags, lpTo, iTolen, lpOverlapped, lpCompletionRoutine, lpThreadId, lpErrno ) ;

	OPERATE_INFO operate = {0} ;
	operate.s = s ;
	operate.dwDataSize = *lpNumberOfBytesSent ;
	operate.dwProcessId = GetCurrentProcessId () ;
	operate.dwThreadId = GetCurrentThreadId() ;
	operate.nErrorCode = *lpErrno ;
	operate.lpData	   = NULL ;	
	wcscpy ( operate.szOperate, L"WSPSendTo" ) ;

	int namelen = sizeof(sockaddr) ;
	getsockname ( s, (sockaddr*)&operate.LocalAddr, &namelen ) ;
	getpeername ( s, (sockaddr*)&operate.RemoteAddr, &namelen ) ;
	data.AddOperateInfo ( &operate ) ;

	return ret ;
}

int WSPAPI WSPRecv(
				   SOCKET s,
				   LPWSABUF lpBuffers,
				   DWORD dwBufferCount,
				   LPDWORD lpNumberOfBytesRecvd,
				   LPDWORD lpFlags,
				   LPWSAOVERLAPPED lpOverlapped,
				   LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine,
				   LPWSATHREADID lpThreadId,
				   LPINT lpErrno )
{
	MY_OutputDebugStringA ( szLogBuf, "[WSPRecv]socket=0x%08X", s ) ;

	int ret = ProcTable.lpWSPRecv ( s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, \
		lpFlags, lpOverlapped, lpCompletionRoutine, lpThreadId, lpErrno ) ;

	OPERATE_INFO operate = {0} ;
	operate.s = s ;
	operate.dwDataSize = *lpNumberOfBytesRecvd ;
	operate.dwProcessId = GetCurrentProcessId () ;
	operate.dwThreadId = GetCurrentThreadId() ;
	operate.nErrorCode = *lpErrno ;
	operate.lpData	   = NULL ;	
	wcscpy ( operate.szOperate, L"WSPRecv" ) ;

	int namelen = sizeof(sockaddr) ;
	getsockname ( s, (sockaddr*)&operate.LocalAddr, &namelen ) ;
	getpeername ( s, (sockaddr*)&operate.RemoteAddr, &namelen ) ;
	data.AddOperateInfo ( &operate ) ;

	return ret ;
}

int WSPAPI WSPRecvFrom(
					   SOCKET s,
					   LPWSABUF lpBuffers,
					   DWORD dwBufferCount,
					   LPDWORD lpNumberOfBytesRecvd,
					   LPDWORD lpFlags,
struct sockaddr* lpFrom,
	LPINT lpFromlen,
	LPWSAOVERLAPPED lpOverlapped,
	LPWSAOVERLAPPED_COMPLETION_ROUTINE lpCompletionRoutine,
	LPWSATHREADID lpThreadId,
	LPINT lpErrno
	)
{
	MY_OutputDebugStringA ( szLogBuf, "[WSPRecvFrom]socket=0x%08X", s ) ;

	int ret = ProcTable.lpWSPRecvFrom ( s, lpBuffers, dwBufferCount, lpNumberOfBytesRecvd, \
		lpFlags, lpFrom, lpFromlen, lpOverlapped, lpCompletionRoutine, lpThreadId, lpErrno ) ;

	OPERATE_INFO operate = {0} ;
	operate.s = s ;
	operate.dwDataSize = *lpNumberOfBytesRecvd ;
	operate.dwProcessId = GetCurrentProcessId () ;
	operate.dwThreadId = GetCurrentThreadId() ;
	operate.nErrorCode = *lpErrno ;
	operate.lpData	   = NULL ;	
	wcscpy ( operate.szOperate, L"WSPRecvFrom" ) ;

	int namelen = sizeof(sockaddr) ;
	getsockname ( s, (sockaddr*)&operate.LocalAddr, &namelen ) ;
	getpeername ( s, (sockaddr*)&operate.RemoteAddr, &namelen ) ;
	data.AddOperateInfo ( &operate ) ;

	return ret ;
}


int WSPAPI WSPStartup( WORD wVersionRequested, LPWSPDATA lpWSPData, \
					  LPWSAPROTOCOL_INFO lpProtocolInfo, WSPUPCALLTABLE UpcallTable, LPWSPPROC_TABLE lpProcTable )
{
	if ( lpProtocolInfo->ProtocolChain.ChainLen <= 1 )
	{
		return WSAEPROVIDERFAILEDINIT ;
	}

	// ö�ٷ����ṩ��
	Provider.GetProvider() ;

	// ȡ���²�Э���ṩ�������Ϣ
	DWORD dwNextEntryId = lpProtocolInfo->ProtocolChain.ChainEntries[1] ;
	LPWSAPROTOCOL_INFOW lpNextProviderInfo = Provider.SearchProviderByEntryId ( dwNextEntryId ) ;
	if ( lpNextProviderInfo == NULL )
	{
		return WSAEPROVIDERFAILEDINIT ;
	}

	int ret = 0 ;
	int nErrorCode = 0 ;
	int nLen = MAX_BUF_SIZE ;
	WCHAR szNextProviderPath[MAX_BUF_SIZE] = {0} ;

	// ȡ���²��ṩ�ߵ�DLL·��
	ret = ::WSCGetProviderPath(&lpNextProviderInfo->ProviderId, szNextProviderPath, &nLen, &nErrorCode ) ;
	if ( ret != SOCKET_ERROR )
	{
		DWORD dwRet = ::ExpandEnvironmentStrings ( szNextProviderPath, szNextProviderPath, MAX_BUF_SIZE ) ;
		if ( dwRet == 0 )
		{
			return WSAEPROVIDERFAILEDINIT ;
		}
	}
	else
	{
		return WSAEPROVIDERFAILEDINIT ;
	}

	// �����²��ṩ��
	HMODULE hModule = ::LoadLibrary ( szNextProviderPath ) ;
	if ( hModule == NULL )
	{
		return WSAEPROVIDERFAILEDINIT ;
	}

	// �����²��ṩ�ߵ�WSPStartup����
	LPWSPSTARTUP pfnWSPStartup = (LPWSPSTARTUP)::GetProcAddress ( hModule, "WSPStartup" ) ;
	if ( pfnWSPStartup == NULL )
	{
		return WSAEPROVIDERFAILEDINIT ;
	}

	// �����²��ṩ��WSPStartup����
	// ����²�Ϊ���������ṩ�ߣ��򴫵��䱾���Э����Ϣ
	// ����²�Ϊ�ֲ�����ṩ�ߣ�LSP�����򴫵ݵ�ǰЭ����Ϣ
	LPWSAPROTOCOL_INFOW lpInfo = NULL ;
	if ( lpNextProviderInfo->ProtocolChain.ChainLen == BASE_PROTOCOL )
		lpInfo = lpNextProviderInfo ;
	else
		lpInfo = lpProtocolInfo ;
	ret = pfnWSPStartup ( wVersionRequested, lpWSPData, lpInfo, UpcallTable, lpProcTable ) ;
	if ( ret == ERROR_SUCCESS )
	{
		// �����²㺯���б�
		ProcTable = *lpProcTable ;

		// �޸Ĵ��ݸ��ϲ�ĺ����б�HOOK����Ȥ�ĺ���
		lpProcTable->lpWSPSend = WSPSend ;
		lpProcTable->lpWSPSendTo = WSPSendTo ;
		lpProcTable->lpWSPRecvFrom = WSPRecvFrom ;
		lpProcTable->lpWSPRecv = WSPRecv ;
		lpProcTable->lpWSPSocket = WSPSocket ;

		// ��¼������Ϣ
		PROCESS_INFO	pi ;
		pi.dwProcessId = GetCurrentProcessId () ;
		GetModuleFileName ( GetModuleHandle(NULL), pi.lpPath, MAX_BUF_SIZE ) ;
		data.AddProcessInfo ( &pi ) ;
	}

	return ret ;
}