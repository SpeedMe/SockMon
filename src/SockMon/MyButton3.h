#pragma once


// CMyButton3

class CMyButton3 : public CButton
{
	DECLARE_DYNAMIC(CMyButton3)

public:
	CMyButton3();
	virtual ~CMyButton3();
	//��дDrawItem����
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

protected:
	DECLARE_MESSAGE_MAP()
};


