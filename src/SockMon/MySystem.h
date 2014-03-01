#pragma once

typedef struct tagThreadParam 
{
	DWORD dwFreeLibrary;                    // FreeLibrary 地址
	DWORD dwGetModuleHandle;                // GetModuleHandle 地址
	TCHAR szModuleName[16];// 需要卸载的模块的名称
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
