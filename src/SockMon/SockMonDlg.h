// SockMonDlg.h : 头文件
//

#pragma once

#include "LSPInstall.h"
#include "afxcmn.h"
#include "Data.h"
#include "IpPack.h"
#include "MyButton.h"
#include "mybutton2.h"
#include "mybutton3.h"



// CSockMonDlg 对话框
class CSockMonDlg : public CDialog
{
// 构造
public:
	CSockMonDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_SOCKMON_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedInstall();
	afx_msg void OnBnClickedUninstall();
	afx_msg void OnDestroy();
	afx_msg LRESULT OnNotifyMessage ( WPARAM wParam, LPARAM lParam ) ;

private:
	BOOL			bStatus ;
	CLSPInstall		LSP ;
	CTreeCtrl		SockTree;
	CData			SockData ;
	afx_msg void OnBnClickedEmpty();
	afx_msg void OnBnClickedFlush();
public:
	int m_RecvCount;
	int m_SendCount;
	int m_ItemIndex;
	bool m_clickSniferList;
	bool m_HighLock;
	vector<int> Highs;
	vector<PIP_HEADER> m_IpInfo;
	BOOL isMonitor;
	CIPAddressCtrl m_IPaddress;
	CString m_IP;
	CListCtrl m_SniferList;
	VOID AddPackInfo(LPPACK_INFO lpPackInfo);
	friend DWORD WINAPI MonitorThread(LPVOID lParam);//监听线程
	friend DWORD WINAPI DrawSockLine(LPVOID lParam);//画曲线图线程
	VOID InitialPictureBox(void);
	CMyButton m_SendBtn;
	CMyButton2 m_RecvBtn;
	CMyButton3 m_TotalBtn;
	CMyButton3 m_ClrBtn;
	afx_msg void OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	VOID InitialSockLine(void);
	VOID ReDrawSockLine(void);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnNMDblclkSockTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickSnilist(NMHDR *pNMHDR, LRESULT *pResult);
	// 提升进程权限
	BOOL StartElevatedProcess(LPCTSTR szPrivName);
	// 提升用户权限
	DWORD ElevatedProcess(LPCTSTR szExecutable, LPCTSTR szCmdLine);
};
