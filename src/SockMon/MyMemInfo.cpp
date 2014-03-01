// MyMemInfo.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SockMon.h"
#include "MyMemInfo.h"


// CMyMemInfo �Ի���

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
    

	//��ʼ��MemList��Ϣ
	m_MemList.InsertColumn(0,L"ID",LVCFMT_LEFT,30);
	m_MemList.InsertColumn(1,L"��ַ",LVCFMT_LEFT,70);
	m_MemList.InsertColumn(2,L"��С",LVCFMT_CENTER,70);
	m_MemList.InsertColumn(3,L"״̬",LVCFMT_CENTER,65);
	m_MemList.InsertColumn(4,L"����",LVCFMT_CENTER,45);
	m_MemList.InsertColumn(5,L"��ʼ����",LVCFMT_CENTER,70);
	m_MemList.InsertColumn(6,L"���ʱ���",LVCFMT_CENTER,70);
	m_MemList.SetExtendedStyle(this->m_MemList.GetExtendedStyle()|LVS_EX_FULLROWSELECT);
    m_MemList.SetTextBkColor(RGB(41,36,33));
	m_MemList.SetTextColor(RGB(0,255,0));
	m_MemList.SetBkColor(RGB(41,36,33));

	this->m_TJcheck.SetCheck(BST_CHECKED);
	this->m_BLcheck.SetCheck(BST_CHECKED);
	this->m_KXcheck.SetCheck(BST_CHECKED);

	this->SetTimer(1,1000,NULL);


	//��ʾ�����ڴ���Ϣ
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


// CMyMemInfo ��Ϣ�������

HBRUSH CMyMemInfo::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	/*// TODO:  �ڴ˸��� DC ���κ�����
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetBkColor(RGB(41,36,33));
	pDC->SetTextColor(RGB(0,255,0));
	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return CreateSolidBrush(RGB(41,36,33));*/
	return hbr;
}

//˫�����رա�
void CMyMemInfo::OnStnDblclickPicboxmem()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnCancel();
}

void CMyMemInfo::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    if (nIDCtl == IDC_CHECK1 || nIDCtl == IDC_CHECK2 || nIDCtl == IDC_CHECK3)//�ı�CheckBox��������ɫ
    {
		CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
		pDC->SetTextColor(RGB(0,255,0));
    }
	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}

void CMyMemInfo::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    OnCancel();
	CDialog::OnLButtonDblClk(nFlags, point);
}

void CMyMemInfo::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	CRect rect;
	//��ȡ���������ͼƬ��
	CWnd* pWnd = GetDlgItem(IDC_PICBOXMEM);

	pWnd->GetClientRect(&rect);
	//ָ��
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
	//ҳ��С
	TempStr.Format(L"%08X",this->VMInfo.SysInfo.dwPageSize);
	pDC->TextOut(rect.left+70,rect.top+25,TempStr);
	//��С��ַ
	TempStr.Format(L"%08X",this->VMInfo.SysInfo.lpMinimumApplicationAddress);
	pDC->TextOut(rect.left+90,rect.top+65,TempStr);
	//����ַ
	TempStr.Format(L"%08X",this->VMInfo.SysInfo.lpMaximumApplicationAddress);
	pDC->TextOut(rect.left+90,rect.top+105,TempStr);
	//�������ڴ�
	TempStr.Format(L"%08X",this->VMInfo.MemStatus.dwTotalPhys);
	pDC->TextOut(rect.left+110,rect.top+145,TempStr);
	//��ҳ���ļ�
	TempStr.Format(L"%08X",this->VMInfo.MemStatus.dwTotalPageFile);
	pDC->TextOut(rect.left+110,rect.top+185,TempStr);
	//�������ڴ�
	TempStr.Format(L"%08X",this->VMInfo.MemStatus.dwTotalVirtual);
	pDC->TextOut(rect.left+110,rect.top+225,TempStr);
	//���������ڴ�
	TempStr.Format(L"%08X",this->VMInfo.MemStatus.dwAvailPhys);
	pDC->TextOut(rect.left+120,rect.top+265,TempStr);
	//����ҳ�ļ�
	TempStr.Format(L"%08X",this->VMInfo.MemStatus.dwAvailPageFile);
	pDC->TextOut(rect.left+110,rect.top+305,TempStr);
	//���������ڴ�
	TempStr.Format(L"%08X",this->VMInfo.MemStatus.dwAvailVirtual);
	pDC->TextOut(rect.left+120,rect.top+345,TempStr);
    
	CDialog::OnTimer(nIDEvent);
}

//��ʾ���������ڴ���Ϣ
VOID CMyMemInfo::DisplayProcMemInfo(void)
{
	MessageBox(L"Enter");
	//ת��CString��DWORD
	WCHAR* wTem = {'\0'};
	DWORD dwProcessId = 0;
	wTem = m_ProcId.GetBuffer(m_ProcId.GetLength());
	dwProcessId = _wtoi(wTem);
	if (this->VMInfo.VM_GetMemInfoList(dwProcessId) == FALSE)
	{
		MessageBox(L"��ȡ��Ϣʧ�ܣ�");
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

		//[1]��ʼ��ַ
		TempStr.Format(L"%08X",p->BaseAddress);
		this->m_MemList.SetItemText(nIndex,1,TempStr);

		//[2]��С
        TempStr.Format(L"%08X",p->RegionSize);
		this->m_MemList.SetItemText(nIndex,2,TempStr);

		//[3]״̬
		switch (p->State)
		{
		case MEM_COMMIT:     TempStr = L"�ύ"; break;
		case MEM_RESERVE:    TempStr = L"����"; break;
		case MEM_FREE:       TempStr = L"����"; break;
		}
		this->m_MemList.SetItemText(nIndex,3,TempStr);
		//[4]����
		switch(p->Type)
		{
		case MEM_IMAGE:      TempStr = L"ӳ��"; break;
		case MEM_PRIVATE:    TempStr = L"˽��"; break;
		case MEM_MAPPED:     TempStr = L"ӳ��"; break;
		}
		this->m_MemList.SetItemText(nIndex,4,TempStr);

		//[5]��ʼ����
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

		//[6]���ʱ���
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

// ��PicBoxMem���л���
VOID CMyMemInfo::DrawPicBoxMem(void)
{
	CRect rect;
	//��ȡ���������ͼƬ��
	CWnd* pWnd = GetDlgItem(IDC_PICBOXMEM);

	pWnd->GetClientRect(&rect);
	//ָ��
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

	//���߿�
	pDC->MoveTo(rect.left+5,rect.top+10);
	pDC->LineTo(rect.right-5,rect.top+10);

    pDC->MoveTo(rect.left+5,rect.bottom-10);
	pDC->LineTo(rect.right-5,rect.bottom-10);

	pDC->MoveTo(rect.left+5,rect.top+10);
	pDC->LineTo(rect.left+5,rect.bottom-10);

	pDC->MoveTo(rect.right-5,rect.top+10);
	pDC->LineTo(rect.right-5,rect.bottom-10);


    //ҳ��С
	pDC->MoveTo(rect.left+5,rect.top+50);
	pDC->LineTo(rect.right-5,rect.top+50);

	//��С��ַ
	pDC->MoveTo(rect.left+5,rect.top+90);
	pDC->LineTo(rect.right-5,rect.top+90);

	//����ַ
	pDC->MoveTo(rect.left+5,rect.top+130);
	pDC->LineTo(rect.right-5,rect.top+130);

	//�������ڴ�
	pDC->MoveTo(rect.left+5,rect.top+170);
	pDC->LineTo(rect.right-5,rect.top+170);

	//��ҳ���ļ�
	pDC->MoveTo(rect.left+5,rect.top+210);
	pDC->LineTo(rect.right-5,rect.top+210);

	//�������ڴ�
	pDC->MoveTo(rect.left+5,rect.top+250);
	pDC->LineTo(rect.right-5,rect.top+250);

	//���������ڴ�
	pDC->MoveTo(rect.left+5,rect.top+290);
	pDC->LineTo(rect.right-5,rect.top+290);

	//����ҳ�ļ�
	pDC->MoveTo(rect.left+5,rect.top+330);
	pDC->LineTo(rect.right-5,rect.top+330);

	//���������ڴ�
	//pDC->MoveTo(rect.left+10,rect.top-10);
	//pDC->LineTo(rect.right-10,rect.bottom-10);

	//д��
	pDC->SetTextColor(RGB(0,255,0));
	pDC->SetBkMode(TRANSPARENT);

	pDC->TextOut(rect.left+10,rect.top+25,L"ҳ��С:");
	pDC->TextOut(rect.left+10,rect.top+65,L"��С��ַ:");
	pDC->TextOut(rect.left+10,rect.top+105,L"����ַ:");
	pDC->TextOut(rect.left+10,rect.top+145,L"�������ڴ�:");
	pDC->TextOut(rect.left+10,rect.top+185,L"��ҳ���ļ�:");
	pDC->TextOut(rect.left+10,rect.top+225,L"�������ڴ�:");
	pDC->TextOut(rect.left+10,rect.top+265,L"���������ڴ�:");
	pDC->TextOut(rect.left+10,rect.top+305,L"����ҳ�ļ�:");
	pDC->TextOut(rect.left+10,rect.top+345,L"���������ڴ�:");
}

void CMyMemInfo::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: �ڴ˴������Ϣ����������
	DrawPicBoxMem();
	// ��Ϊ��ͼ��Ϣ���� CDialog::OnPaint()
}
