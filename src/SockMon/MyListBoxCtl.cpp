// MyListBoxCtl.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SockMon.h"
#include "MyListBoxCtl.h"


// CMyListBoxCtl

IMPLEMENT_DYNAMIC(CMyListBoxCtl, CListBox)

CMyListBoxCtl::CMyListBoxCtl()
{

}

CMyListBoxCtl::~CMyListBoxCtl()
{
}


BEGIN_MESSAGE_MAP(CMyListBoxCtl, CListBox)
	ON_WM_RBUTTONDBLCLK()
END_MESSAGE_MAP()



// CMyListBoxCtl ��Ϣ�������



void CMyListBoxCtl::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    this->CloseWindow();
	CListBox::OnRButtonDblClk(nFlags, point);
}
