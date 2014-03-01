#pragma once

#include <ws2spi.h>
#include <Sporder.h>
#pragma comment ( lib, "Ws2_32.lib" )

#include <rpc.h>
#pragma comment ( lib, "Rpcrt4.lib" )		// ʵ��UuidCreate����

class CLSPInstall
{
public:
	CLSPInstall(void);
public:
	~CLSPInstall(void);

private:	
	GUID				ProviderGuid ;			// LSP��ȫ��Ψһ��ʶ��GUID��
	UINT				nProtocolNum ;			// Э������
	LPWSAPROTOCOL_INFOW lpProtocolInfo ;		// ָ��Э����Ϣ�ṹ����

public:
	// ��װLSP
	int		InstallProvider ( LPWSTR lpProviderPath ) ;
	// ж��LSP
	VOID	RemoveProvider () ;

protected:
	// ö��Э��Ŀ¼��������Ϣ��lpProtocolInfo
	BOOL	GetProvider () ;
	// ����Ŀ¼���ID����Э����Ϣ�ṹ
	BOOL	SearchProviderByEntryId ( DWORD dwEntryId, LPWSAPROTOCOL_INFOW lpInfo ) ;
	// ����Э�����Ͳ���Э����Ϣ�ṹ
	BOOL	SearchProviderByProtocol ( int iProtocol, LPWSAPROTOCOL_INFOW lpInfo ) ;
};
