#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "MyNtdll.h"
#include "MyListBoxCtl.h"

// CMyProcessDlg �Ի���

class CMyProcessDlg : public CDialog
{
	DECLARE_DYNAMIC(CMyProcessDlg)

public:
	CMyProcessDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMyProcessDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CString m_ProcessId;
	afx_msg void OnBnClickedCancel();
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	CMyListBoxCtl m_ProcessModulesInfo;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	CEdit m_ProcessBaseInfo;
	BOOL ListProcessBaseInfo(void);
	// ��ʼ��ö�ٽ��̻�����Ϣ
	BOOL InitialProcessInfo(void);
	VOID QuerySystemInformation(SYSTEM_INFORMATION_CLASS SystemInformationClass);
	void DisplayProcessInformation(LPBYTE lpBuf);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	BOOL EnumProcessMemInfo(void);
	BOOL EnumThreadInfo(void);
	// ö��ģ����Ϣ
	BOOL EnumModulesInfo(void);
	//��ͨɱ����
	afx_msg void OnKillProcNormal();
	afx_msg void OnLbnDblclkList1();
	// �����û�Ȩ��
	BOOL EnablePrivilege(LPCTSTR szPrivName);
	afx_msg void OnKillProcessZero();
	// ����ɱ����
	int KillProcess(ULONG dwProcessId);
	DWORD GetPidByName(LPCWSTR szName);
	// �򿪽���
	HANDLE My_OpenProcess(DWORD dwDesiredAccess, bool bInhert, DWORD ProcessId, bool bOpenIt, LPDWORD aryPids);
	BOOL AdjustPurview(void);
	afx_msg void OnKillThread();
	// ��ͨɱ�߳�
	void KillThreadByTerminate(DWORD dwProcessId, DWORD dwCid);
	VOID KillMyThreadFun(PKillThreadParam PParam);
	afx_msg void OnKillModule();
	afx_msg void OnLookMenInfo();
	afx_msg void OnLookPEInfo();
};
