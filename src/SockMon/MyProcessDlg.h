#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "MyNtdll.h"
#include "MyListBoxCtl.h"

// CMyProcessDlg 对话框

class CMyProcessDlg : public CDialog
{
	DECLARE_DYNAMIC(CMyProcessDlg)

public:
	CMyProcessDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMyProcessDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString m_ProcessId;
	afx_msg void OnBnClickedCancel();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	CMyListBoxCtl m_ProcessModulesInfo;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CEdit m_ProcessBaseInfo;
	BOOL ListProcessBaseInfo(void);
	// 初始化枚举进程基本信息
	BOOL InitialProcessInfo(void);
	VOID QuerySystemInformation(SYSTEM_INFORMATION_CLASS SystemInformationClass);
	void DisplayProcessInformation(LPBYTE lpBuf);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	BOOL EnumProcessMemInfo(void);
	BOOL EnumThreadInfo(void);
	// 枚举模块信息
	BOOL EnumModulesInfo(void);
	//普通杀进程
	afx_msg void OnKillProcNormal();
	afx_msg void OnLbnDblclkList1();
	// 提升用户权限
	BOOL EnablePrivilege(LPCTSTR szPrivName);
	afx_msg void OnKillProcessZero();
	// 清零杀进程
	int KillProcess(ULONG dwProcessId);
	DWORD GetPidByName(LPCWSTR szName);
	// 打开进程
	HANDLE My_OpenProcess(DWORD dwDesiredAccess, bool bInhert, DWORD ProcessId, bool bOpenIt, LPDWORD aryPids);
	BOOL AdjustPurview(void);
	afx_msg void OnKillThread();
	// 普通杀线程
	void KillThreadByTerminate(DWORD dwProcessId, DWORD dwCid);
	VOID KillMyThreadFun(PKillThreadParam PParam);
	afx_msg void OnKillModule();
	afx_msg void OnLookMenInfo();
	afx_msg void OnLookPEInfo();
};
