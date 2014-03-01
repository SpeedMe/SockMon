#pragma once


// CMyButton

class CMyButton : public CButton
{
	DECLARE_DYNAMIC(CMyButton)

public:
	CMyButton();
	virtual ~CMyButton();
	//ÖØÐ´DrawItemº¯Êý
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

protected:
	DECLARE_MESSAGE_MAP()
};


