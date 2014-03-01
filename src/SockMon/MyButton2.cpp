// MyButton2.cpp : 实现文件
//

#include "stdafx.h"
#include "SockMon.h"
#include "MyButton2.h"


// CMyButton2

IMPLEMENT_DYNAMIC(CMyButton2, CButton)

CMyButton2::CMyButton2()
{

}

CMyButton2::~CMyButton2()
{
}

void CMyButton2::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{	
	// TODO: Add your code to draw the specified item
	UINT uStyle = DFCS_BUTTONPUSH;

	// This code only works with buttons.
	ASSERT(lpDrawItemStruct->CtlType == ODT_BUTTON);

	// If drawing selected, add the pushed style to DrawFrameControl.
	if (lpDrawItemStruct->itemState & ODS_SELECTED)
		uStyle |= DFCS_PUSHED;

	// Draw the button frame.
	::DrawFrameControl(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, 
		DFC_BUTTON, uStyle);

	// Get the button's text.
	CString strText;
	GetWindowText(strText);

	// Draw the button text using the text color red.
	COLORREF crOldColor = ::SetTextColor(lpDrawItemStruct->hDC, RGB(46,36,250));
	::DrawText(lpDrawItemStruct->hDC, strText, strText.GetLength(), 
		&lpDrawItemStruct->rcItem, DT_SINGLELINE|DT_VCENTER|DT_CENTER);
	::SetTextColor(lpDrawItemStruct->hDC, crOldColor);	
}

BEGIN_MESSAGE_MAP(CMyButton2, CButton)
END_MESSAGE_MAP()



// CMyButton2 消息处理程序


