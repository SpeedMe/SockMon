// MyListBoxCtl.cpp : 实现文件
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



// CMyListBoxCtl 消息处理程序



void CMyListBoxCtl::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
    this->CloseWindow();
	CListBox::OnRButtonDblClk(nFlags, point);
}
