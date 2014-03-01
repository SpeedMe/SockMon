#pragma once

typedef struct tagThreadParam 
{
	DWORD dwFreeLibrary;                    // FreeLibrary ��ַ
	DWORD dwGetModuleHandle;                // GetModuleHandle ��ַ
	TCHAR szModuleName[16];// ��Ҫж�ص�ģ�������
}ThreadParam,*PThreadParam;

class CMySystem
{
public:
	CMySystem(void);
	~CMySystem(void);
	static void KillProcessFun(PKillThreadParam PParam);
	static BOOL FreeModuleByPid(TCHAR* szModuleName, DWORD dwPid);
	static void RemoteThreadFun(PThreadParam PParam);
};
