#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "VMInfo.h"

// CMyMemInfo 对话框

class CMyMemInfo : public CDialog
{
	DECLARE_DYNAMIC(CMyMemInfo)

public:
	CMyMemInfo(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CMyMemInfo();

// 对话框数据
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CVMInfo VMInfo;
	CString m_ProcId;
	CListCtrl m_MemList;
	CButton m_TJcheck;
	CButton m_BLcheck;
	CButton m_KXcheck;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnStnDblclickPicboxmem();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	VOID DisplayProcMemInfo(void);
	// 对PicBoxMem进行绘制
	VOID DrawPicBoxMem(void);
	afx_msg void OnPaint();
};
