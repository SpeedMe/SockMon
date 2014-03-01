#pragma once


// CMyListBoxCtl

class CMyListBoxCtl : public CListBox
{
	DECLARE_DYNAMIC(CMyListBoxCtl)

public:
	CMyListBoxCtl();
	virtual ~CMyListBoxCtl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
};


