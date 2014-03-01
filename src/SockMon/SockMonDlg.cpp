// SockMonDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SockMon.h"
#include "SockMonDlg.h"
#include "MyProcessDlg.h"
#include "windows.h"
#include "psapi.h"
#include "MyNtdll.h"
#include <tlhelp32.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <winsock2.h>
#pragma comment ( lib, "ws2_32.lib" )

#define   SIO_RCVALL   _WSAIOW(IOC_VENDOR,1) 

#define DEF_BUF_SIZE2 2048
// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���



class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CSockMonDlg �Ի���




CSockMonDlg::CSockMonDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSockMonDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CSockMonDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SOCK_TREE, SockTree);
	DDX_Control(pDX,IDC_SNILIST,m_SniferList);
	DDX_Control(pDX, IDC_IPADDRESS1, m_IPaddress);
	DDX_Control(pDX, IDC_BUTTON3, m_SendBtn);
	DDX_Control(pDX, IDC_BUTTON4, m_RecvBtn);
	DDX_Control(pDX, IDC_BUTTON2, m_TotalBtn);
	//DDX_Control(pDX, IDC_EMPTY, m_ClrBtn);
}

BEGIN_MESSAGE_MAP(CSockMonDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_INSTALL, &CSockMonDlg::OnBnClickedInstall)
	ON_BN_CLICKED(IDC_UNINSTALL, &CSockMonDlg::OnBnClickedUninstall)
	ON_WM_DESTROY()
	ON_MESSAGE(NOTIFY_MESSAGE,OnNotifyMessage)
	ON_BN_CLICKED(IDC_EMPTY, &CSockMonDlg::OnBnClickedEmpty)
	ON_BN_CLICKED(IDC_FLUSH, &CSockMonDlg::OnBnClickedFlush)
//	ON_WM_CTLCOLOR()
	ON_WM_MOUSEMOVE()
	ON_NOTIFY(NM_DBLCLK, IDC_SOCK_TREE, &CSockMonDlg::OnNMDblclkSockTree)
//	ON_NOTIFY(NM_DBLCLK, IDC_SNILIST, &CSockMonDlg::OnNMDblclkSnilist)
ON_NOTIFY(NM_CLICK, IDC_SNILIST, &CSockMonDlg::OnNMClickSnilist)
END_MESSAGE_MAP()


// CSockMonDlg ��Ϣ�������

BOOL CSockMonDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	//AfxGetApp()->LoadIcon(IDI_ICON2);
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	this->bStatus = FALSE ;

	this->SockData.SetTreeCtrl ( &this->SockTree ) ;

	this->OnBnClickedUninstall () ;
	this->OnBnClickedInstall () ;

	//��ʼ��SniferList�ؼ�
	this->m_SniferList.InsertColumn(0,L"ID",LVCFMT_CENTER,40);
	this->m_SniferList.InsertColumn(1,L"����",LVCFMT_CENTER,60);
	this->m_SniferList.InsertColumn(2,L"����",LVCFMT_CENTER,50);
	this->m_SniferList.InsertColumn(3,L"ԴIP",LVCFMT_CENTER,110);
	this->m_SniferList.InsertColumn(4,L"Դ�˿�",LVCFMT_CENTER,60);
	this->m_SniferList.InsertColumn(5,L"Ŀ��IP",LVCFMT_CENTER,110);
	this->m_SniferList.InsertColumn(6,L"Ŀ�Ķ˿�",LVCFMT_CENTER,80);
	this->m_SniferList.SetExtendedStyle(this->m_SniferList.GetExtendedStyle()|LVS_EX_FULLROWSELECT);
	this->m_SniferList.SetTextBkColor(RGB(41,36,33));
	this->m_SniferList.SetTextColor(RGB(0,255,0));
	this->m_SniferList.SetBkColor(RGB(41,36,33));


	//��ʼ��TreeView�ؼ�
	this->SockTree.SetBkColor(RGB(41,36,33));
	this->SockTree.SetTextColor(RGB(0,250,0));

	//��ʼ�����շ�������
	m_RecvCount = 0;
	m_SendCount = 0;

	//��ʾ����IP
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2),&wsaData);

	in_addr addr;
	char szHostName[1024] = {0};
	gethostname(szHostName,1024);
	hostent* pHost = gethostbyname(szHostName);
	if (pHost)
	{
		memcpy(&addr,pHost->h_addr_list[0],pHost->h_length);
		char* IP = inet_ntoa(addr);
		CString ip(IP);
		SetDlgItemText(IDC_IPADDRESS1,ip);
		m_IP = ip;
	}
	m_HighLock = FALSE;
	m_clickSniferList = FALSE;
	//������ͼ�߳�
	HANDLE hTread0 = CreateThread(NULL,0,DrawSockLine,this,0,NULL);
	if (hTread0 == NULL)
	{
		MessageBox(L"�������߷�����ʧ�ܣ�");
	}
	//���������߳�
	HANDLE hThread = CreateThread(NULL,0,MonitorThread,this,0,NULL);
	if (hThread == NULL)
	{
		MessageBox(L"����������ʧ�ܣ�");
	}
	else
		this->isMonitor = TRUE;

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CSockMonDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
	
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CSockMonDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
	//�ӳ��Ա��ܹ���ȷ����GDI+
	Sleep(100);
	//��ʼ������ͼ
	InitialSockLine();
	//IP���ݰ����
	InitialPictureBox();
	//�ػ�SockLine
	//ReDrawSockLine();
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
//
HCURSOR CSockMonDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CSockMonDlg::OnBnClickedInstall()
{
	if ( this->bStatus )
	{
		this->MessageBox ( L"�Ѿ���װ��" ) ;
		return ;
	}
	else
	{
		this->bStatus = TRUE ;
		
		WCHAR szPath [ MAX_BUF_SIZE ] = {0} ;
		GetCurrentDirectory ( MAX_BUF_SIZE, szPath ) ;
		wcscat ( szPath, L"\\LSP.dll" ) ;
		if ( this->LSP.InstallProvider ( szPath ) != 0 )
			this->MessageBox ( L"��װʧ�ܣ�" ) ;
	}
}

void CSockMonDlg::OnBnClickedUninstall()
{
	isMonitor = FALSE;
	this->LSP.RemoveProvider (); 
	this->bStatus = FALSE ;
}

void CSockMonDlg::OnDestroy()
{
	this->OnBnClickedUninstall () ;
	CDialog::OnDestroy();
}

LRESULT CSockMonDlg::OnNotifyMessage ( WPARAM wParam, LPARAM lParam )
{
	this->SockData.UpdateInfo () ;
	if ( this->SockData.IsFull() )
	{
		if ( this->MessageBox ( L"�洢�ռ��������Ƿ�������ݣ�" ) == IDOK )
			this->SockData.ResetSockData () ;
	}
	return 0 ;
}

void CSockMonDlg::OnBnClickedEmpty()
{
	//this->SockData.ResetSockData () ;
	//this->StartElevatedProcess();
}

void CSockMonDlg::OnBnClickedFlush()
{
	this->SockData.FlushSockData () ;
}
//������ݰ���Ϣ
VOID CSockMonDlg::AddPackInfo(LPPACK_INFO lpPackInfo)
{
	UINT nCount = this->m_SniferList.GetItemCount();

	CString TempStr;
	//����ID
	TempStr.Format(L"%d",nCount+1);
	this->m_SniferList.InsertItem(nCount,TempStr);

	//���ذ���С
	TempStr.Format(L"%d",lpPackInfo->nLength);
	this->m_SniferList.SetItemText(nCount,1,TempStr);

	//����Э������
	CString Protocol;
	switch(lpPackInfo->nProtocol)
	{
	case IPPROTO_IP:		Protocol = L"IP";	       break ;//0
	case IPPROTO_ICMP:		Protocol = L"ICMP";	       break ;//1
	case IPPROTO_IGMP:		Protocol = L"IGMP";	       break ;//2
	case IPPROTO_GGP:		Protocol = L"GGP";	       break ;//3
	case IPPROTO_IPV4:		Protocol = L"IPV4";	       break ;//4
	case IPPROTO_TCP:		Protocol = L"TCP";	       break ;//6
	case IPPROTO_PUP:		Protocol = L"PUP";	       break ;//12
	case IPPROTO_UDP:		Protocol = L"UDP";	       break ;//17
	case IPPROTO_IDP:		Protocol = L"IDP";	       break ;//22
	case IPPROTO_IPV6:		Protocol = L"IPV6";	       break ;//41
	case IPPROTO_ROUTING:	Protocol = L"ROUTING";     break ;//43
	case IPPROTO_FRAGMENT:	Protocol = L"FRAGMENT";    break ;//44
	case IPPROTO_ESP:		Protocol = L"ESP";	       break ;//50
	case IPPROTO_AH:		Protocol = L"AH";	       break ;//51
	case IPPROTO_ICMPV6:	Protocol = L"ICMPV6";  	   break ;//58
	case IPPROTO_NONE:		Protocol = L"NONE";	       break ;//59
	case IPPROTO_DSTOPTS:	Protocol = L"DSTOPTS";     break ;//60
	case IPPROTO_ND:		Protocol = L"ND"; 	       break ;//77
	case IPPROTO_ICLFXBM:	Protocol = L"ICLFXBM";     break ;//78
	case IPPROTO_RAW:		Protocol = L"RAW";	       break ;//255
	}
	this->m_SniferList.SetItemText(nCount,2,Protocol);
	
	in_addr addr;
	WCHAR szTempBufW[DEF_BUF_SIZE2] = {0};
   
	//����ԴIP
	addr.S_un.S_addr = lpPackInfo->nSourIp;
    MultiByteToWideChar(CP_ACP,0,inet_ntoa(addr),-1,szTempBufW,sizeof(szTempBufW));
    this->m_SniferList.SetItemText(nCount,3,szTempBufW);

	//����Դ�˿�
	if (lpPackInfo->nSourPort != 0)
	{
		TempStr.Format(L"%d",htons(lpPackInfo->nSourPort));
		this->m_SniferList.SetItemText(nCount,4,TempStr);
	}
	else
	{
		this->m_SniferList.SetItemText(nCount,4,L"δ֪");
	}

	//����Ŀ��IP
	addr.S_un.S_addr = lpPackInfo->nDestIp;
	MultiByteToWideChar(CP_ACP,0,inet_ntoa(addr),-1,szTempBufW,sizeof(szTempBufW));
	this->m_SniferList.SetItemText(nCount,5,szTempBufW);

	//����Ŀ�Ķ˿�
	if (lpPackInfo->nDestPort != 0)
	{
		TempStr.Format(L"%d",htons(lpPackInfo->nDestPort));
		this->m_SniferList.SetItemText(nCount,6,TempStr);
	}
	else
	{
		this->m_SniferList.SetItemText(nCount,6,L"δ֪");
	}

	this->m_SniferList.EnsureVisible(nCount,FALSE);
	this->m_SniferList.SetItemState(nCount,LVIS_SELECTED,LVIS_SELECTED);
	if(nCount > 0)
		this->m_SniferList.SetItemState(nCount - 1,0,LVIS_SELECTED);
	this->m_SniferList.SetFocus();
}
//������ͼ�߳�
DWORD WINAPI DrawSockLine(LPVOID lParam)
{
	//ȡ�ô���ָ��
	CSockMonDlg* pDlg = (CSockMonDlg*)lParam;
	CRect rect;
	CWnd* pWnd = pDlg->GetDlgItem(IDC_PICBOX);

	pWnd->GetClientRect(&rect);
	//ָ��
	CDC* pDC = pWnd->GetDC();
	pDC->Rectangle(&rect);


	CPen *pGreenLine = new CPen(0,1,RGB(0,255,0));//������
	CPen *pGreenLine2 = new CPen(2,1,RGB(0,255,0));//��������
    CPen *pBlueLine = new CPen(0,1,RGB(123,104,238));//Recv��
	int index = 0;
	int TrueIndex = 0;
	int OrgHigh = rect.bottom-20;
	if(!pDlg->m_HighLock)
	   pDlg->Highs.push_back(OrgHigh);
	pWnd->Invalidate();
	pWnd->UpdateWindow();
	while(TRUE)
	{
		pDlg->InitialSockLine();
		//������������
		if (TrueIndex+1 < pDlg->m_IpInfo.size())
		{
			pDC->SelectObject(pGreenLine);
			pDC->MoveTo(rect.left+4+index,OrgHigh);
            OrgHigh = rect.bottom-(int)(pDlg->m_IpInfo[TrueIndex]->nTotalLength)/500;
			if(OrgHigh < rect.top)
				OrgHigh = rect.top;
			pDC->LineTo(rect.left+4+index+1,OrgHigh);
		    //�����Recv��
			/*if (pDlg->m_IpInfo[TrueIndex]->nSourIp != )
			{
			}*/
			/*pDC->SelectObject(pBlueLine);
			pDC->MoveTo(rect.left+4+index,OrgHigh - 40);
			pDC->LineTo(rect.left+4+index+1,OrgHigh - 40);*/

			
			if(!pDlg->m_HighLock)
			{
			   pDlg->Highs.push_back(OrgHigh);
			}
			TrueIndex++;
		}
		else
		{
			pDC->SelectObject(pGreenLine);
			pDC->MoveTo(rect.left+4+index,OrgHigh);
			pDC->LineTo(rect.left+4+index+1,rect.bottom-20);


			OrgHigh = rect.bottom-20;
			if(!pDlg->m_HighLock)
			{
			   pDlg->Highs.push_back(OrgHigh);
			}
			
		}
        
		//����������
		pDC->SelectObject(pGreenLine2);
        pDC->MoveTo(rect.left+4+index,rect.top);
		pDC->LineTo(rect.left+4+index,rect.bottom);
		index++;
		if(index > rect.right - 2)//�����������
		{
			pDlg->Highs.clear();
			TrueIndex = 0;
			index = 0;
			OrgHigh = rect.bottom-5;
			pDlg->InitialSockLine();//ˢ��SockLine����
		}
		
		Sleep(200);
	}
	return 0;
}
//�����߳�
DWORD WINAPI MonitorThread(LPVOID lParam)
{

//	Sleep(100);//�����ӳ٣��ǵĶԻ�����ȫ��ʼ����Ϻ��ڽ���GDI+
	PACK_INFO	PackInfo = {0} ;
	int nRecvSize = 0 ;
	char szPackBuf[DEF_BUF_SIZE2] = {0} ;

	// ȡ�ô���ָ��
	CSockMonDlg* pDlg = (CSockMonDlg*)lParam ;
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != NO_ERROR)
		return 0 ;

	// ��ȡ���ص�ַ��Ϣ
	sockaddr_in LocalAddr ;
	char szLocalName[DEF_BUF_SIZE2] = {0} ;
	gethostname ( szLocalName, DEF_BUF_SIZE2 ) ;
	hostent* pHost = gethostbyname ( szLocalName ) ;
	if ( pHost != NULL )
	{
		LocalAddr.sin_family = AF_INET ;
		LocalAddr.sin_port = htons(0) ;
		memcpy ( &(LocalAddr.sin_addr.s_addr), pHost->h_addr_list[0], pHost->h_length ) ;
	}
	else
		return 0 ;

	// ���������׽���
	SOCKET MonSock = socket ( AF_INET, SOCK_RAW, IPPROTO_IP ) ;
	if ( MonSock == INVALID_SOCKET )
		return 0 ;

	// �󶨵�ַ��Ϣ���׽���
	if ( bind ( MonSock, (sockaddr*)&LocalAddr, sizeof(sockaddr) ) == SOCKET_ERROR )
		return 0 ;

	// ����Ϊ����ģʽ��������IP��
	DWORD dwValue = 1 ;
	if ( ioctlsocket ( MonSock, SIO_RCVALL, &dwValue ) != 0 )
		return 0 ;

	// �����Ʊ�־���Ƿ��������
	while ( pDlg->isMonitor )
	{
		// ȡ�����ݰ�
		nRecvSize = recv ( MonSock, szPackBuf, DEF_BUF_SIZE2, 0 ) ;
		if ( nRecvSize > 0 )
		{
			// ����IP��ͷ
			PIP_HEADER pIpHeader = (PIP_HEADER)szPackBuf ;
			PackInfo.nLength	= nRecvSize ;
			PackInfo.nProtocol	= (USHORT)pIpHeader->bProtocol ;
			PackInfo.nSourIp	= pIpHeader->nSourIp ;
			PackInfo.nDestIp	= pIpHeader->nDestIp ;
			UINT nIpHeadLength = ( pIpHeader->bVerAndHLen & 0x0F ) * sizeof(UINT) ;			// IP���ݰ�ͷ����

			CString tempStr;
			tempStr.Format(L"%i",PackInfo.nSourIp);
			//�ж��ǽ��ջ��Ƿ��Ͱ�
			if (tempStr == pDlg->m_IP)
			{
				pDlg->m_RecvCount++;
				tempStr.Format(L"�հ�������%i��",pDlg->m_RecvCount);
				pDlg->SetDlgItemText(IDC_RECVCNT,tempStr);
			}
			else
			{
				pDlg->m_SendCount++;
				tempStr.Format(L"����������%i��",pDlg->m_SendCount++);
				pDlg->SetDlgItemText(IDC_SENDCNT,tempStr);
			}
			pDlg->m_IpInfo.push_back(pIpHeader);
			// ֻ���TCP��UDP��
			switch ( pIpHeader->bProtocol )
			{
			case IPPROTO_TCP:
				{
					// ȡ��TCP���ݰ��˿ں�
					PTCP_HEADER pTcpHeader = (PTCP_HEADER)&szPackBuf[nIpHeadLength] ;
					PackInfo.nSourPort = pTcpHeader->nSourPort ;
					PackInfo.nDestPort = pTcpHeader->nDesPort ;
					pDlg->AddPackInfo ( &PackInfo ) ;	// ���������
				}
				break ;
			case IPPROTO_UDP:
				{
					// ȡ��UDP���ݰ��˿ں�
					PUDP_HEADER pUdpHeader = (PUDP_HEADER)&szPackBuf[nIpHeadLength] ;
					PackInfo.nSourPort = pUdpHeader->nSourPort ;
					PackInfo.nDestPort = pUdpHeader->nDestPort ;
					pDlg->AddPackInfo ( &PackInfo ) ;	// ���������
				}
				break ;
			default:
				{
					//ȡ������Э�����ݰ�
					PackInfo.nSourPort = 0;
					PackInfo.nDestPort = 0;
					pDlg->AddPackInfo(&PackInfo);
				}
				break;
			}
			//pDlg->m_IpInfo.push_back(PackInfo);
		}
		Sleep ( 100 ) ;
	}

}

VOID CSockMonDlg::InitialPictureBox(void)
{
	CRect rect;
	//��ȡ���������ͼƬ��
	CWnd* pWnd = GetDlgItem(IDC_IPREPORT);

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


	CPen *pGreenPen=new CPen(0,1,RGB(0,100,0));
	pDC->SelectObject(pGreenPen);
	//���߿�
    pDC->MoveTo(rect.left + 4,rect.top + 4);
	pDC->LineTo(rect.right - 4,rect.top + 4);

	pDC->MoveTo(rect.left + 4, rect.bottom - 4);
	pDC->LineTo(rect.right - 4,rect.bottom - 4);

	pDC->MoveTo(rect.left + 4,rect.top + 4);
	pDC->LineTo(rect.left + 4, rect.bottom - 4);

	pDC->MoveTo(rect.right - 4,rect.top + 4);
	pDC->LineTo(rect.right - 4,rect.bottom - 4);

	for (int i = 1 ; i <= 4 ; i++)
	{
		pDC->MoveTo(rect.left + 4,rect.top + 4 + i*30);
		pDC->LineTo(rect.right - 4,rect.top + 4 + i*30);
	}

	pDC->MoveTo((rect.right+rect.left)/2,rect.top + 4);
	pDC->LineTo((rect.right+rect.left)/2,rect.bottom - 65);

	pDC->MoveTo((rect.left+rect.right)/4,rect.top + 4);
	pDC->LineTo((rect.left+rect.right)/4,rect.top + 35);

	pDC->MoveTo((rect.left+rect.right)/4,rect.top + 65);
	pDC->LineTo((rect.left+rect.right)/4,rect.top + 95);

	for (int i = 1 ; i <= 3 ; i++)
	{
		pDC->MoveTo((rect.right+rect.left)/2 + i*20,rect.top + 35);
		pDC->LineTo((rect.right+rect.left)/2 + i*20,rect.top + 65);
	}
	for (int i = 1 ; i <= 4 ; i++)
	{
		pDC->MoveTo((rect.right+rect.left)/2 + i*5,rect.top + 35);
		pDC->LineTo((rect.right+rect.left)/2 + i*5,rect.top + 65);
	}
	for (int i= 1 ; i <= 6 ; i++)
	{
		pDC->MoveTo((rect.right+rect.left)/2,rect.top + 34 + i*5);
		pDC->LineTo((rect.right+rect.left)/2 +20,rect.top + 34 + i*5);
	}

	pDC->MoveTo((rect.right+rect.left)/2-20,rect.top+4);
	pDC->LineTo((rect.right+rect.left)/2-20,rect.top + 35);
	for (int i = 1 ; i <= 4 ; i++)
	{
		pDC->MoveTo((rect.right+rect.left)/2 - i*5,rect.top + 4);
		pDC->LineTo((rect.right+rect.left)/2 - i*5,rect.top + 35);
	}

	for (int i = 1 ; i <= 6 ;i++)
	{
		pDC->MoveTo((rect.right+rect.left)/2-20,rect.top+4+i*5);
		pDC->LineTo((rect.right+rect.left)/2,rect.top+4+i*5);
	}
	//��дTextOut
  /*  if (m_clickSniferList)
    {
		pDC->SetTextColor(RGB(0,250,0));
		pDC->SetBkMode(TRANSPARENT);
		CString tempStr;
		//��ð汾��Ϣ��ǰ4λ����ͷ���ȣ���4λ��
		tempStr.Format(L"V&L: %0x",m_IpInfo[m_ItemIndex]->bVerAndHLen);
		pDC->TextOut(rect.left+15,rect.top+15,tempStr);
		//��÷�������
		tempStr.Format(L"TOS: %0x",m_IpInfo[m_ItemIndex]->bTypeOfService);
		pDC->TextOut((rect.right+rect.left)/4+10,rect.top+15,tempStr);
		//������ݰ�����
		tempStr.Format(L"TL: %0x",m_IpInfo[m_ItemIndex]->nTotalLength);
		pDC->TextOut((rect.right+rect.left)/2+15,rect.top+15,tempStr);
		//������ݰ���ʶ
		tempStr.Format(L"IDF: %0x",m_IpInfo[m_ItemIndex]->nID);
		pDC->TextOut(rect.left+15,rect.top+35+5,tempStr);
		//��ñ����ֶ�
		tempStr.Format(L"FO: %0x",m_IpInfo[m_ItemIndex]->nReserved);
		pDC->TextOut((rect.right+rect.left)/2+75,rect.top+35+5,tempStr);
		//�������ʱ��
		tempStr.Format(L"TTL: %0x",m_IpInfo[m_ItemIndex]->nTTL);
		pDC->TextOut(rect.left+15,rect.top+70,tempStr);
		//���Э������
		tempStr.Format(L"PTC: %0x",m_IpInfo[m_ItemIndex]->bProtocol);
		pDC->TextOut((rect.right+rect.left)/4+15,rect.top+65+5,tempStr);
		//���У���
		tempStr.Format(L"HCS: %0x",m_IpInfo[m_ItemIndex]->nCheckSum);
		pDC->TextOut((rect.right+rect.left)/2+30,rect.top+65+5,tempStr);
		//���ԴIP
		tempStr.Format(L"SIP: %0x",m_IpInfo[m_ItemIndex]->nSourIp);
		pDC->TextOut(rect.left + 15,rect.top+105,tempStr);
		//���Ŀ��IP
		tempStr.Format(L"DIP: %0x",m_IpInfo[m_ItemIndex]->nDestIp);
		pDC->TextOut(rect.left + 15,rect.top+135,tempStr);
    }*/
	


}
//SniferList��Click�¼�
void CSockMonDlg::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
//	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<NMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_clickSniferList = TRUE;
	CRect rect;
	//��ȡ���������ͼƬ��
	CWnd* pWnd = GetDlgItem(IDC_IPREPORT);

	pWnd->GetClientRect(&rect);
	//ָ��
	CDC* pDC = pWnd->GetDC();
	pWnd->Invalidate();
	pWnd->UpdateWindow();

	pDC->Rectangle(&rect);
    
	pDC->SetTextColor(RGB(0,250,0));
	pDC->SetBkMode(TRANSPARENT);
	InitialPictureBox();
    int nItemNum;
	POSITION pos;
	pos = this->m_SniferList.GetFirstSelectedItemPosition();
	if (pos)//���ѡ��List���
	{
		nItemNum = this->m_SniferList.GetNextSelectedItem(pos);
		m_ItemIndex = nItemNum;
	}
	CString tempStr;
	//��ð汾��Ϣ��ǰ4λ����ͷ���ȣ���4λ��
	tempStr.Format(L"V&L: %0x",m_IpInfo[nItemNum]->bVerAndHLen);
	pDC->TextOut(rect.left+15,rect.top+15,tempStr);
	//��÷�������
	tempStr.Format(L"TOS: %0x",m_IpInfo[nItemNum]->bTypeOfService);
	pDC->TextOut((rect.right+rect.left)/4+10,rect.top+15,tempStr);
	//������ݰ�����
	tempStr.Format(L"TL: %0x",m_IpInfo[nItemNum]->nTotalLength);
	pDC->TextOut((rect.right+rect.left)/2+15,rect.top+15,tempStr);
	//������ݰ���ʶ
	tempStr.Format(L"IDF: %0x",m_IpInfo[nItemNum]->nID);
	pDC->TextOut(rect.left+15,rect.top+35+5,tempStr);
	//��ñ����ֶ�
	tempStr.Format(L"FO: %0x",m_IpInfo[nItemNum]->nReserved);
	pDC->TextOut((rect.right+rect.left)/2+75,rect.top+35+5,tempStr);
	//�������ʱ��
	tempStr.Format(L"TTL: %0x",m_IpInfo[nItemNum]->nTTL);
	pDC->TextOut(rect.left+15,rect.top+70,tempStr);
	//���Э������
	tempStr.Format(L"PTC: %0x",m_IpInfo[nItemNum]->bProtocol);
	pDC->TextOut((rect.right+rect.left)/4+15,rect.top+65+5,tempStr);
	//���У���
	tempStr.Format(L"HCS: %0x",m_IpInfo[nItemNum]->nCheckSum);
	pDC->TextOut((rect.right+rect.left)/2+30,rect.top+65+5,tempStr);
	//���ԴIP
	tempStr.Format(L"SIP: %0x",m_IpInfo[nItemNum]->nSourIp);
	pDC->TextOut(rect.left + 15,rect.top+105,tempStr);
	//���Ŀ��IP
	tempStr.Format(L"DIP: %0x",m_IpInfo[nItemNum]->nDestIp);
	pDC->TextOut(rect.left + 15,rect.top+135,tempStr);

	
	*pResult = 0;
}

VOID CSockMonDlg::InitialSockLine(void)
{
	CRect rect;
	//��ȡ���������ͼƬ��
	CWnd* pWnd = GetDlgItem(IDC_PICBOX);

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


	CPen *pBkLine=new CPen(0,1,RGB(0,100,0));
	pDC->SelectObject(pBkLine);
	//����������
	for (int i = 1 ; i <= 15 ; i++)//����
	{
		pDC->MoveTo(rect.left+1,(rect.top+rect.bottom)/1.005 - i*10);
		pDC->LineTo(rect.right-2,(rect.top+rect.bottom)/1.005 - i*10);
	}
	for (int i = 0 ; i <= 83 ; i++)//����
	{
		pDC->MoveTo(i*10,rect.top + 1);
		pDC->LineTo(i*10,rect.bottom - 2);
	}
	CPen *pGreenPen=new CPen(0,1,RGB(0,255,0));
	CPen *pBluePen=new CPen(0,1,RGB(123,104,238));
	pDC->SelectObject(pGreenPen);
	//�Ա�����������,����ͬ������
	m_HighLock = TRUE;
	for (int i = 0 ; i+1 < Highs.size(); i++)
	{
		pDC->MoveTo(rect.left+4+i,Highs[i]);
		pDC->LineTo(rect.left+5+i,Highs[i+1]);

		/*pDC->SelectObject(pBluePen);
		pDC->MoveTo(rect.left+4+i,Highs[i]);
		pDC->LineTo(rect.left+5+i,Highs[i+1]);*/
	}
	m_HighLock = FALSE;
}
//������WM_PAINT��Ϣʱ�ػ�SockLine
VOID CSockMonDlg::ReDrawSockLine(void)
{

	CRect rect;
	//��ȡ���������ͼƬ��
	CWnd* pWnd = GetDlgItem(IDC_PICBOX);

	pWnd->GetClientRect(&rect);
	//ָ��
	CDC* pDC = pWnd->GetDC();
	pWnd->Invalidate();
	pWnd->UpdateWindow();

	pDC->Rectangle(&rect);
	CPen *pGreenPen=new CPen(0,1,RGB(0,255,0));
	pDC->SelectObject(pGreenPen);
    //�Ա�����������,����ͬ������
	m_HighLock = TRUE;
	for (int i = 0 ; i+1 < Highs.size(); i++)
	{
		pDC->MoveTo(rect.left+4+i,Highs[i]);
		pDC->LineTo(rect.left+5+i,Highs[i+1]);
	}
	m_HighLock = FALSE;
}

HBRUSH CSockMonDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
    
	// TODO:  �ڴ˸��� DC ���κ�����
    //pDC->SetBkMode(TRANSPARENT);
	if (nCtlColor == IDC_LIST1)//�ػ�����ListBox�ؼ�
	{
		pDC->SetBkColor(RGB(41,36,33));
		pDC->SetTextColor(RGB(0,255,0));

	}
	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}
//����������SockLine�������¼�
void CSockMonDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    CRect rectPic;
	GetDlgItem(IDC_PICBOX)->GetWindowRect(&rectPic);
	if (PtInRect(&rectPic,point))//���������SockLine����
	{
        HCURSOR myCursor = LoadCursor(NULL,IDC_CROSS);
		SetCursor(myCursor);
	}
	else
	{
		HCURSOR myCursor = LoadCursor(NULL,IDC_ARROW);
		SetCursor(myCursor);
	}
	CDialog::OnMouseMove(nFlags, point);
}
//˫��TreeView��ʾ������ϸ��Ϣ
void CSockMonDlg::OnNMDblclkSockTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//�����ѡItem
	HTREEITEM hti = SockTree.GetSelectedItem();
	CString myText;
	if (hti != NULL)
	{
		while (SockTree.GetParentItem(hti) != NULL)
		{
			hti = SockTree.GetParentItem(hti);
		}
		//��ø��ڵ�
	     myText = SockTree.GetItemText(hti);
		//MessageBox(myText);
	}
	//��Text���д���ɸѡ����Ҫ�Ľ���ID
	int Index = 0;
	CString strID;
    for (int i = 0 ; i < myText.GetLength() ; i++)
	{
		if(myText[i] == '[' || myText[i] == ']')
			Index++;
		if (Index == 3)
		{
			i++;
			while(myText[i] != ']')
			{
				strID += myText[i];//��ý���ID
				i++;
			}
			break;;
		}
    }
	//StartElevatedProcess(SE_DEBUG_NAME);//����Ȩ��
	CMyProcessDlg dlg;
	//�жϴ���ID�ĵ�һ���ַ��Ƿ�Ϊ0
	if (strID[0] == '0')
	{
		strID.Delete(0);//ɾ����һ���ַ�
	}
	dlg.m_ProcessId = strID;
	dlg.DoModal();
	*pResult = 0;
}

//����Snifer��Click�¼�
void CSockMonDlg::OnNMClickSnilist(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<NMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_clickSniferList = TRUE;
	CRect rect;
	//��ȡ���������ͼƬ��
	CWnd* pWnd = GetDlgItem(IDC_IPREPORT);

	pWnd->GetClientRect(&rect);
	//ָ��
	CDC* pDC = pWnd->GetDC();
	pWnd->Invalidate();
	pWnd->UpdateWindow();

	pDC->Rectangle(&rect);

	pDC->SetTextColor(RGB(0,250,0));
	pDC->SetBkMode(TRANSPARENT);
	InitialPictureBox();
	int nItemNum;
	POSITION pos;
	pos = this->m_SniferList.GetFirstSelectedItemPosition();
	if (pos)//���ѡ��List���
	{
		nItemNum = this->m_SniferList.GetNextSelectedItem(pos);
		m_ItemIndex = nItemNum;
	}
	CString tempStr;
	//��ð汾��Ϣ��ǰ4λ����ͷ���ȣ���4λ��
	tempStr.Format(L"V&L: %0x",m_IpInfo[nItemNum]->bVerAndHLen);
	pDC->TextOut(rect.left+15,rect.top+15,tempStr);
	//��÷�������
	tempStr.Format(L"TOS: %0x",m_IpInfo[nItemNum]->bTypeOfService);
	pDC->TextOut((rect.right+rect.left)/4+10,rect.top+15,tempStr);
	//������ݰ�����
	tempStr.Format(L"TL: %0x",m_IpInfo[nItemNum]->nTotalLength);
	pDC->TextOut((rect.right+rect.left)/2+15,rect.top+15,tempStr);
	//������ݰ���ʶ
	tempStr.Format(L"IDF: %0x",m_IpInfo[nItemNum]->nID);
	pDC->TextOut(rect.left+15,rect.top+35+5,tempStr);
	//��ñ����ֶ�
	tempStr.Format(L"FO: %0x",m_IpInfo[nItemNum]->nReserved);
	pDC->TextOut((rect.right+rect.left)/2+75,rect.top+35+5,tempStr);
	//�������ʱ��
	tempStr.Format(L"TTL: %0x",m_IpInfo[nItemNum]->nTTL);
	pDC->TextOut(rect.left+15,rect.top+70,tempStr);
	//���Э������
	tempStr.Format(L"PTC: %0x",m_IpInfo[nItemNum]->bProtocol);
	pDC->TextOut((rect.right+rect.left)/4+15,rect.top+65+5,tempStr);
	//���У���
	tempStr.Format(L"HCS: %0x",m_IpInfo[nItemNum]->nCheckSum);
	pDC->TextOut((rect.right+rect.left)/2+30,rect.top+65+5,tempStr);
	//���ԴIP
	tempStr.Format(L"SIP: %0x",m_IpInfo[nItemNum]->nSourIp);
	pDC->TextOut(rect.left + 15,rect.top+105,tempStr);
	//���Ŀ��IP
	tempStr.Format(L"DIP: %0x",m_IpInfo[nItemNum]->nDestIp);
	pDC->TextOut(rect.left + 15,rect.top+135,tempStr);
	*pResult = 0;
}

// ��������Ȩ��
BOOL CSockMonDlg::StartElevatedProcess(LPCTSTR szPrivName)
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tp;
	LUID luid;
	//�򿪽������ƻ�
	if (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,
		&hToken))
	{
		printf("OpenProcessToken error.\n");
		return true;
	}
	//��ý��̱���ΨһID
	if (!LookupPrivilegeValue(NULL,szPrivName,&luid))
	{
		printf("LookupPrivilege error!\n");
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	tp.Privileges[0].Luid = luid;
	//����Ȩ��
	if (!AdjustTokenPrivileges(hToken,0,&tp,sizeof(TOKEN_PRIVILEGES),NULL,NULL))
	{
		printf("AdjustTokenPrivileges error!\n");
		return true;
	}
	return false;
}

// �����û�Ȩ��
DWORD CSockMonDlg::ElevatedProcess(LPCTSTR szExecutable, LPCTSTR szCmdLine)
{
	SHELLEXECUTEINFO sei = {sizeof(SHELLEXECUTEINFO)};

	sei.lpVerb = TEXT("runas");
	sei.lpFile = szExecutable;
	sei.lpParameters = szCmdLine;
	sei.nShow = SW_SHOWNORMAL;
	ShellExecuteEx(&sei);
	return(GetLastError());
}
