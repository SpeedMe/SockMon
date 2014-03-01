// MyMemInfo.cpp : 实现文件
//

#include "stdafx.h"
#include "SockMon.h"
#include "MyMemInfo.h"


// CMyMemInfo 对话框

IMPLEMENT_DYNAMIC(CMyMemInfo, CDialog)

CMyMemInfo::CMyMemInfo(CWnd* pParent /*=NULL*/)
	: CDialog(CMyMemInfo::IDD, pParent)
{

}

CMyMemInfo::~CMyMemInfo()
{
}

void CMyMemInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_MemList);
	DDX_Control(pDX, IDC_CHECK1, m_TJcheck);
	DDX_Control(pDX, IDC_CHECK2, m_BLcheck);
	DDX_Control(pDX, IDC_CHECK3, m_KXcheck);

	this->VMInfo.VM_GetSysInfo();
	this->VMInfo.VM_GetMemStatus();
    

	//初始化MemList信息
	m_MemList.InsertColumn(0,L"ID",LVCFMT_LEFT,30);
	m_MemList.InsertColumn(1,L"地址",LVCFMT_LEFT,70);
	m_MemList.InsertColumn(2,L"大小",LVCFMT_CENTER,70);
	m_MemList.InsertColumn(3,L"状态",LVCFMT_CENTER,65);
	m_MemList.InsertColumn(4,L"类型",LVCFMT_CENTER,45);
	m_MemList.InsertColumn(5,L"初始保护",LVCFMT_CENTER,70);
	m_MemList.InsertColumn(6,L"访问保护",LVCFMT_CENTER,70);
	m_MemList.SetExtendedStyle(this->m_MemList.GetExtendedStyle()|LVS_EX_FULLROWSELECT);
    m_MemList.SetTextBkColor(RGB(41,36,33));
	m_MemList.SetTextColor(RGB(0,255,0));
	m_MemList.SetBkColor(RGB(41,36,33));

	this->m_TJcheck.SetCheck(BST_CHECKED);
	this->m_BLcheck.SetCheck(BST_CHECKED);
	this->m_KXcheck.SetCheck(BST_CHECKED);

	this->SetTimer(1,1000,NULL);


	//显示进程内存信息
	DisplayProcMemInfo();



}


BEGIN_MESSAGE_MAP(CMyMemInfo, CDialog)
	ON_WM_CTLCOLOR()
	ON_STN_DBLCLK(IDC_PICBOXMEM, &CMyMemInfo::OnStnDblclickPicboxmem)
	ON_WM_DRAWITEM()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_TIMER()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CMyMemInfo 消息处理程序

HBRUSH CMyMemInfo::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	/*// TODO:  在此更改 DC 的任何属性
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetBkColor(RGB(41,36,33));
	pDC->SetTextColor(RGB(0,255,0));
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return CreateSolidBrush(RGB(41,36,33));*/
	return hbr;
}

//双击“关闭”
void CMyMemInfo::OnStnDblclickPicboxmem()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}

void CMyMemInfo::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
    if (nIDCtl == IDC_CHECK1 || nIDCtl == IDC_CHECK2 || nIDCtl == IDC_CHECK3)//改变CheckBox的字体颜色
    {
		CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
		pDC->SetTextColor(RGB(0,255,0));
    }
	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CMyMemInfo::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
    OnCancel();
	CDialog::OnLButtonDblClk(nFlags, point);
}

void CMyMemInfo::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CRect rect;
	//获取绘制坐标的图片框
	CWnd* pWnd = GetDlgItem(IDC_PICBOXMEM);

	pWnd->GetClientRect(&rect);
	//指针
	CDC* pDC = pWnd->GetDC();
	pWnd->Invalidate();
	pWnd->UpdateWindow();

	pDC->Rectangle(&rect);
	pDC->SetTextColor(RGB(0,255,0));
	pDC->SetBkMode(TRANSPARENT);

	DrawPicBoxMem();

    this->VMInfo.VM_GetSysInfo();
	this->VMInfo.VM_GetMemStatus();

	CString TempStr;
	//页大小
	TempStr.Format(L"%08X",this->VMInfo.SysInfo.dwPageSize);
	pDC->TextOut(rect.left+70,rect.top+25,TempStr);
	//最小地址
	TempStr.Format(L"%08X",this->VMInfo.SysInfo.lpMinimumApplicationAddress);
	pDC->TextOut(rect.left+90,rect.top+65,TempStr);
	//最大地址
	TempStr.Format(L"%08X",this->VMInfo.SysInfo.lpMaximumApplicationAddress);
	pDC->TextOut(rect.left+90,rect.top+105,TempStr);
	//总物理内存
	TempStr.Format(L"%08X",this->VMInfo.MemStatus.dwTotalPhys);
	pDC->TextOut(rect.left+110,rect.top+145,TempStr);
	//总页面文件
	TempStr.Format(L"%08X",this->VMInfo.MemStatus.dwTotalPageFile);
	pDC->TextOut(rect.left+110,rect.top+185,TempStr);
	//总虚拟内存
	TempStr.Format(L"%08X",this->VMInfo.MemStatus.dwTotalVirtual);
	pDC->TextOut(rect.left+110,rect.top+225,TempStr);
	//可用物理内存
	TempStr.Format(L"%08X",this->VMInfo.MemStatus.dwAvailPhys);
	pDC->TextOut(rect.left+120,rect.top+265,TempStr);
	//可用页文件
	TempStr.Format(L"%08X",this->VMInfo.MemStatus.dwAvailPageFile);
	pDC->TextOut(rect.left+110,rect.top+305,TempStr);
	//可用虚拟内存
	TempStr.Format(L"%08X",this->VMInfo.MemStatus.dwAvailVirtual);
	pDC->TextOut(rect.left+120,rect.top+345,TempStr);
    
	CDialog::OnTimer(nIDEvent);
}

//显示进程虚拟内存信息
VOID CMyMemInfo::DisplayProcMemInfo(void)
{
	MessageBox(L"Enter");
	//转化CString到DWORD
	WCHAR* wTem = {'\0'};
	DWORD dwProcessId = 0;
	wTem = m_ProcId.GetBuffer(m_ProcId.GetLength());
	dwProcessId = _wtoi(wTem);
	if (this->VMInfo.VM_GetMemInfoList(dwProcessId) == FALSE)
	{
		MessageBox(L"获取信息失败！");
		return;
	}
	CString TempStr;
	UINT nIndex = 0;
	list<MEMORY_BASIC_INFORMATION>::iterator p;
    for (p = this->VMInfo.MemList.begin(); p != this->VMInfo.MemList.end(); p++)
    {
		if ((p->State == MEM_COMMIT && this->m_TJcheck.GetCheck() == BST_UNCHECKED)||\
			(p->State == MEM_RESERVE && this->m_BLcheck.GetCheck() == BST_UNCHECKED)||\
			(p->State == MEM_FREE) && this->m_KXcheck.GetCheck() == BST_UNCHECKED)
		{
			continue;
		}
		//MessageBox(L"FOR");
		TempStr.Format(L"%d",nIndex+1);
		this->m_MemList.InsertItem(nIndex,TempStr);

		//[1]起始地址
		TempStr.Format(L"%08X",p->BaseAddress);
		this->m_MemList.SetItemText(nIndex,1,TempStr);

		//[2]大小
        TempStr.Format(L"%08X",p->RegionSize);
		this->m_MemList.SetItemText(nIndex,2,TempStr);

		//[3]状态
		switch (p->State)
		{
		case MEM_COMMIT:     TempStr = L"提交"; break;
		case MEM_RESERVE:    TempStr = L"保留"; break;
		case MEM_FREE:       TempStr = L"空闲"; break;
		}
		this->m_MemList.SetItemText(nIndex,3,TempStr);
		//[4]类型
		switch(p->Type)
		{
		case MEM_IMAGE:      TempStr = L"映像"; break;
		case MEM_PRIVATE:    TempStr = L"私有"; break;
		case MEM_MAPPED:     TempStr = L"映射"; break;
		}
		this->m_MemList.SetItemText(nIndex,4,TempStr);

		//[5]初始保护
        switch(p->AllocationProtect)
		{
		case PAGE_READONLY:             TempStr = L"-R--";  break;
		case PAGE_READWRITE:            TempStr = L"-RW-";  break;
		case PAGE_WRITECOPY:            TempStr = L"-RWC";  break;
		case PAGE_EXECUTE:              TempStr = L"E---";  break;
		case PAGE_EXECUTE_READ:         TempStr = L"ER--";  break;
		case PAGE_EXECUTE_READWRITE:    TempStr = L"ERW-";  break;
		case PAGE_EXECUTE_WRITECOPY:    TempStr = L"ERWC";  break;
		case PAGE_NOACCESS:             TempStr = L"----";  break;
		default:                        TempStr = L"----";  break;
		}
		this->m_MemList.SetItemText(nIndex,5,TempStr);

		//[6]访问保护
		switch(p->Protect)
		{
		case PAGE_READONLY:             TempStr = L"-R--"; break;
		case PAGE_READWRITE:            TempStr = L"-RW-"; break;
		case PAGE_WRITECOPY:            TempStr = L"-RWC"; break;
		case PAGE_EXECUTE:              TempStr = L"E---"; break;
		case PAGE_EXECUTE_READ:         TempStr = L"ER--"; break;
		case PAGE_EXECUTE_READWRITE:    TempStr = L"ERW-"; break;
		case PAGE_EXECUTE_WRITECOPY:    TempStr = L"ERWC"; break;
		case PAGE_NOACCESS:             TempStr = L"----"; break;
		default:                        TempStr = L"----"; break;
		}
		this->m_MemList.SetItemText(nIndex,6,TempStr);

		nIndex++;
    }
}

// 对PicBoxMem进行绘制
VOID CMyMemInfo::DrawPicBoxMem(void)
{
	CRect rect;
	//获取绘制坐标的图片框
	CWnd* pWnd = GetDlgItem(IDC_PICBOXMEM);

	pWnd->GetClientRect(&rect);
	//指针
	CDC* pDC = pWnd->GetDC();
	pWnd->Invalidate();
	pWnd->UpdateWindow();

	pDC->Rectangle(&rect);

	CBrush brush;
	COLORREF bk = RGB(41,36,33);
	brush.CreateSolidBrush(bk);
	pDC->FillRect(rect,&brush);

	CPen* pGeernLine = new CPen(0,1,RGB(0,255,0));
	pDC->SelectObject(pGeernLine);

	//画边框
	pDC->MoveTo(rect.left+5,rect.top+10);
	pDC->LineTo(rect.right-5,rect.top+10);

    pDC->MoveTo(rect.left+5,rect.bottom-10);
	pDC->LineTo(rect.right-5,rect.bottom-10);

	pDC->MoveTo(rect.left+5,rect.top+10);
	pDC->LineTo(rect.left+5,rect.bottom-10);

	pDC->MoveTo(rect.right-5,rect.top+10);
	pDC->LineTo(rect.right-5,rect.bottom-10);


    //页大小
	pDC->MoveTo(rect.left+5,rect.top+50);
	pDC->LineTo(rect.right-5,rect.top+50);

	//最小地址
	pDC->MoveTo(rect.left+5,rect.top+90);
	pDC->LineTo(rect.right-5,rect.top+90);

	//最大地址
	pDC->MoveTo(rect.left+5,rect.top+130);
	pDC->LineTo(rect.right-5,rect.top+130);

	//总物理内存
	pDC->MoveTo(rect.left+5,rect.top+170);
	pDC->LineTo(rect.right-5,rect.top+170);

	//总页面文件
	pDC->MoveTo(rect.left+5,rect.top+210);
	pDC->LineTo(rect.right-5,rect.top+210);

	//总虚拟内存
	pDC->MoveTo(rect.left+5,rect.top+250);
	pDC->LineTo(rect.right-5,rect.top+250);

	//可用物理内存
	pDC->MoveTo(rect.left+5,rect.top+290);
	pDC->LineTo(rect.right-5,rect.top+290);

	//可用页文件
	pDC->MoveTo(rect.left+5,rect.top+330);
	pDC->LineTo(rect.right-5,rect.top+330);

	//可用虚拟内存
	//pDC->MoveTo(rect.left+10,rect.top-10);
	//pDC->LineTo(rect.right-10,rect.bottom-10);

	//写字
	pDC->SetTextColor(RGB(0,255,0));
	pDC->SetBkMode(TRANSPARENT);

	pDC->TextOut(rect.left+10,rect.top+25,L"页大小:");
	pDC->TextOut(rect.left+10,rect.top+65,L"最小地址:");
	pDC->TextOut(rect.left+10,rect.top+105,L"最大地址:");
	pDC->TextOut(rect.left+10,rect.top+145,L"总物理内存:");
	pDC->TextOut(rect.left+10,rect.top+185,L"总页面文件:");
	pDC->TextOut(rect.left+10,rect.top+225,L"总虚拟内存:");
	pDC->TextOut(rect.left+10,rect.top+265,L"可用物理内存:");
	pDC->TextOut(rect.left+10,rect.top+305,L"可用页文件:");
	pDC->TextOut(rect.left+10,rect.top+345,L"可用虚拟内存:");
}

void CMyMemInfo::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此处添加消息处理程序代码
	DrawPicBoxMem();
	// 不为绘图消息调用 CDialog::OnPaint()
}
