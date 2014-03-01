#pragma once


// CMyButton2

class CMyButton2 : public CButton
{
	DECLARE_DYNAMIC(CMyButton2)

public:
	CMyButton2();
	virtual ~CMyButton2();
	//ÖØÐ´DrawItemº¯Êý
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

protected:
	DECLARE_MESSAGE_MAP()
};


