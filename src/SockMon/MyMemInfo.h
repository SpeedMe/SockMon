#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include "VMInfo.h"

// CMyMemInfo �Ի���

class CMyMemInfo : public CDialog
{
	DECLARE_DYNAMIC(CMyMemInfo)

public:
	CMyMemInfo(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CMyMemInfo();

// �Ի�������
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
	// ��PicBoxMem���л���
	VOID DrawPicBoxMem(void);
	afx_msg void OnPaint();
};
