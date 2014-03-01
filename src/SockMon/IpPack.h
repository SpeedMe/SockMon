#include <windows.h>

typedef struct _PACK_INFO{
	USHORT nLength;            // ���ݰ�����
	USHORT nProtocol;          // Э������
	UINT   nSourIp;            // ԴIP
	UINT   nDestIp;            // Ŀ��IP
	USHORT nSourPort;          // Դ�˿ں�
	USHORT nDestPort;          // Ŀ�Ķ˿ں�
}PACK_INFO,*LPPACK_INFO;

typedef struct _IP_HEADER{
	BYTE     bVerAndHLen;      // �汾��Ϣ��ǰ4λ����ͷ���ȣ���4λ��
	BYTE     bTypeOfService;   // ��������
	USHORT   nTotalLength;     // ���ݰ�����
	USHORT   nID;              // ���ݰ���ʶ
	USHORT   nReserved;        // �����ֶ�
	BYTE     nTTL;             // ����ʱ��
	BYTE     bProtocol;        // Э������
	USHORT   nCheckSum;        // У���
	UINT     nSourIp;          // ԴIP
	UINT     nDestIp;          // Ŀ��IP
}IP_HEADER,*PIP_HEADER;

typedef struct _TCP_HEADER{
	USHORT   nSourPort;        // Դ�˿ں�
	USHORT   nDesPort;         // Ŀ�Ķ˿ں�
	UINT     nSequNum;         // ���к�
	UINT     nAckknowledgeNum; // ȷ�Ϻ�
	USHORT   nHLenAndFlag;     // ǰ��λ��TCPͷ���ȣ���6λ����������6λ����־λ
	USHORT   nWindowSize;      // ���ڴ�С
	USHORT   nCheckSum;        // j�����
	USHORT   nrgentPointer;    // У���
}TCP_HEADER,*PTCP_HEADER;

typedef struct _UDP_HEADER{
	USHORT   nSourPort;        // Դ�˿ں�
	USHORT   nDestPort;        // Ŀ�Ķ˿ں�
	USHORT   nLength;          // ���ݰ�����
	USHORT   nCheckSum;        // У���
}UDP_HEADER,*PUDP_HEADER;

