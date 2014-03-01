#pragma once


// CMyButton3

class CMyButton3 : public CButton
{
	DECLARE_DYNAMIC(CMyButton3)

public:
	CMyButton3();
	virtual ~CMyButton3();
	//ÖØÐ´DrawItemº¯Êý
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

protected:
	DECLARE_MESSAGE_MAP()
};


