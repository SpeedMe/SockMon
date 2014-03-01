#pragma once
#include "afxcmn.h"
#include "PEInfo.h"

#define MAX_DEF_RES_TYPE 24
// CFEInfoDlg 对话框

class CFEInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CFEInfoDlg)

public:
	CFEInfoDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFEInfoDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_ResDirectory;   // 树形控件
	CPEInfo PEInfo;
	BOOL isDumpValid;           // 标识树形控件的当前所选项是否可以DUMP（即是否为叶子节点）
    LPVOID lpDumpResStart;      // 当前选定的资源的VA
    DWORD dwResSize;            // 当前选定的资源的SIZE
    CString szResourceType[MAX_DEF_RES_TYPE];     // 资源类型的ID与字符串对应表

	CString m_FilePath;
	CString m_RootNameEntryNum;
	CString m_RootIdEntryNum;
	CString m_SubNameEntryNum;
	CString m_SubIdEntryNum;
	CString m_DataOffset;
	CString m_DataRVA;
	CString m_DataSize;
	CString m_ResInfo;


	void UpdateTreeCtrl(void);
	// 取得当前选定项在其兄弟节点的索引
	int GetIndexOfTreeCtrl(HTREEITEM hTreeItem);
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	VOID GetInfo(void);
	// 取得目标文件的相关信息
	BOOL GetResourceInfo(LPTSTR lpFilePath);
};
