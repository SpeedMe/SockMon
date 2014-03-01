// SockMonDlg.cpp : 实现文件
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
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框



class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CSockMonDlg 对话框




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


// CSockMonDlg 消息处理程序

BOOL CSockMonDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	//AfxGetApp()->LoadIcon(IDI_ICON2);
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	this->bStatus = FALSE ;

	this->SockData.SetTreeCtrl ( &this->SockTree ) ;

	this->OnBnClickedUninstall () ;
	this->OnBnClickedInstall () ;

	//初始化SniferList控件
	this->m_SniferList.InsertColumn(0,L"ID",LVCFMT_CENTER,40);
	this->m_SniferList.InsertColumn(1,L"长度",LVCFMT_CENTER,60);
	this->m_SniferList.InsertColumn(2,L"类型",LVCFMT_CENTER,50);
	this->m_SniferList.InsertColumn(3,L"源IP",LVCFMT_CENTER,110);
	this->m_SniferList.InsertColumn(4,L"源端口",LVCFMT_CENTER,60);
	this->m_SniferList.InsertColumn(5,L"目的IP",LVCFMT_CENTER,110);
	this->m_SniferList.InsertColumn(6,L"目的端口",LVCFMT_CENTER,80);
	this->m_SniferList.SetExtendedStyle(this->m_SniferList.GetExtendedStyle()|LVS_EX_FULLROWSELECT);
	this->m_SniferList.SetTextBkColor(RGB(41,36,33));
	this->m_SniferList.SetTextColor(RGB(0,255,0));
	this->m_SniferList.SetBkColor(RGB(41,36,33));


	//初始化TreeView控件
	this->SockTree.SetBkColor(RGB(41,36,33));
	this->SockTree.SetTextColor(RGB(0,250,0));

	//初始化接收发送数量
	m_RecvCount = 0;
	m_SendCount = 0;

	//显示本机IP
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
	//创建画图线程
	HANDLE hTread0 = CreateThread(NULL,0,DrawSockLine,this,0,NULL);
	if (hTread0 == NULL)
	{
		MessageBox(L"创建曲线分析器失败！");
	}
	//创建监听线程
	HANDLE hThread = CreateThread(NULL,0,MonitorThread,this,0,NULL);
	if (hThread == NULL)
	{
		MessageBox(L"创建监视器失败！");
	}
	else
		this->isMonitor = TRUE;

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CSockMonDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
	//延迟以便能够正确进行GDI+
	Sleep(100);
	//初始化流量图
	InitialSockLine();
	//IP数据包表格
	InitialPictureBox();
	//重绘SockLine
	//ReDrawSockLine();
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR CSockMonDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CSockMonDlg::OnBnClickedInstall()
{
	if ( this->bStatus )
	{
		this->MessageBox ( L"已经安装！" ) ;
		return ;
	}
	else
	{
		this->bStatus = TRUE ;
		
		WCHAR szPath [ MAX_BUF_SIZE ] = {0} ;
		GetCurrentDirectory ( MAX_BUF_SIZE, szPath ) ;
		wcscat ( szPath, L"\\LSP.dll" ) ;
		if ( this->LSP.InstallProvider ( szPath ) != 0 )
			this->MessageBox ( L"安装失败！" ) ;
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
		if ( this->MessageBox ( L"存储空间已满，是否清空数据？" ) == IDOK )
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
//添加数据包信息
VOID CSockMonDlg::AddPackInfo(LPPACK_INFO lpPackInfo)
{
	UINT nCount = this->m_SniferList.GetItemCount();

	CString TempStr;
	//加载ID
	TempStr.Format(L"%d",nCount+1);
	this->m_SniferList.InsertItem(nCount,TempStr);

	//加载包大小
	TempStr.Format(L"%d",lpPackInfo->nLength);
	this->m_SniferList.SetItemText(nCount,1,TempStr);

	//加载协议类型
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
   
	//加载源IP
	addr.S_un.S_addr = lpPackInfo->nSourIp;
    MultiByteToWideChar(CP_ACP,0,inet_ntoa(addr),-1,szTempBufW,sizeof(szTempBufW));
    this->m_SniferList.SetItemText(nCount,3,szTempBufW);

	//加载源端口
	if (lpPackInfo->nSourPort != 0)
	{
		TempStr.Format(L"%d",htons(lpPackInfo->nSourPort));
		this->m_SniferList.SetItemText(nCount,4,TempStr);
	}
	else
	{
		this->m_SniferList.SetItemText(nCount,4,L"未知");
	}

	//加载目的IP
	addr.S_un.S_addr = lpPackInfo->nDestIp;
	MultiByteToWideChar(CP_ACP,0,inet_ntoa(addr),-1,szTempBufW,sizeof(szTempBufW));
	this->m_SniferList.SetItemText(nCount,5,szTempBufW);

	//加载目的端口
	if (lpPackInfo->nDestPort != 0)
	{
		TempStr.Format(L"%d",htons(lpPackInfo->nDestPort));
		this->m_SniferList.SetItemText(nCount,6,TempStr);
	}
	else
	{
		this->m_SniferList.SetItemText(nCount,6,L"未知");
	}

	this->m_SniferList.EnsureVisible(nCount,FALSE);
	this->m_SniferList.SetItemState(nCount,LVIS_SELECTED,LVIS_SELECTED);
	if(nCount > 0)
		this->m_SniferList.SetItemState(nCount - 1,0,LVIS_SELECTED);
	this->m_SniferList.SetFocus();
}
//画曲线图线程
DWORD WINAPI DrawSockLine(LPVOID lParam)
{
	//取得窗体指针
	CSockMonDlg* pDlg = (CSockMonDlg*)lParam;
	CRect rect;
	CWnd* pWnd = pDlg->GetDlgItem(IDC_PICBOX);

	pWnd->GetClientRect(&rect);
	//指针
	CDC* pDC = pWnd->GetDC();
	pDC->Rectangle(&rect);


	CPen *pGreenLine = new CPen(0,1,RGB(0,255,0));//主曲线
	CPen *pGreenLine2 = new CPen(2,1,RGB(0,255,0));//行走纵线
    CPen *pBlueLine = new CPen(0,1,RGB(123,104,238));//Recv线
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
		//画总流量曲线
		if (TrueIndex+1 < pDlg->m_IpInfo.size())
		{
			pDC->SelectObject(pGreenLine);
			pDC->MoveTo(rect.left+4+index,OrgHigh);
            OrgHigh = rect.bottom-(int)(pDlg->m_IpInfo[TrueIndex]->nTotalLength)/500;
			if(OrgHigh < rect.top)
				OrgHigh = rect.top;
			pDC->LineTo(rect.left+4+index+1,OrgHigh);
		    //如果是Recv线
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
        
		//画行走纵线
		pDC->SelectObject(pGreenLine2);
        pDC->MoveTo(rect.left+4+index,rect.top);
		pDC->LineTo(rect.left+4+index,rect.bottom);
		index++;
		if(index > rect.right - 2)//如果超出画面
		{
			pDlg->Highs.clear();
			TrueIndex = 0;
			index = 0;
			OrgHigh = rect.bottom-5;
			pDlg->InitialSockLine();//刷新SockLine界面
		}
		
		Sleep(200);
	}
	return 0;
}
//监视线程
DWORD WINAPI MonitorThread(LPVOID lParam)
{

//	Sleep(100);//创建延迟，是的对话框完全初始化完毕后在进行GDI+
	PACK_INFO	PackInfo = {0} ;
	int nRecvSize = 0 ;
	char szPackBuf[DEF_BUF_SIZE2] = {0} ;

	// 取得窗体指针
	CSockMonDlg* pDlg = (CSockMonDlg*)lParam ;
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != NO_ERROR)
		return 0 ;

	// 获取本地地址信息
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

	// 创建监听套接字
	SOCKET MonSock = socket ( AF_INET, SOCK_RAW, IPPROTO_IP ) ;
	if ( MonSock == INVALID_SOCKET )
		return 0 ;

	// 绑定地址信息到套接字
	if ( bind ( MonSock, (sockaddr*)&LocalAddr, sizeof(sockaddr) ) == SOCKET_ERROR )
		return 0 ;

	// 设置为混杂模式，收所有IP包
	DWORD dwValue = 1 ;
	if ( ioctlsocket ( MonSock, SIO_RCVALL, &dwValue ) != 0 )
		return 0 ;

	// 检测控制标志，是否继续监视
	while ( pDlg->isMonitor )
	{
		// 取得数据包
		nRecvSize = recv ( MonSock, szPackBuf, DEF_BUF_SIZE2, 0 ) ;
		if ( nRecvSize > 0 )
		{
			// 解析IP包头
			PIP_HEADER pIpHeader = (PIP_HEADER)szPackBuf ;
			PackInfo.nLength	= nRecvSize ;
			PackInfo.nProtocol	= (USHORT)pIpHeader->bProtocol ;
			PackInfo.nSourIp	= pIpHeader->nSourIp ;
			PackInfo.nDestIp	= pIpHeader->nDestIp ;
			UINT nIpHeadLength = ( pIpHeader->bVerAndHLen & 0x0F ) * sizeof(UINT) ;			// IP数据包头长度

			CString tempStr;
			tempStr.Format(L"%i",PackInfo.nSourIp);
			//判断是接收还是发送包
			if (tempStr == pDlg->m_IP)
			{
				pDlg->m_RecvCount++;
				tempStr.Format(L"收包个数：%i个",pDlg->m_RecvCount);
				pDlg->SetDlgItemText(IDC_RECVCNT,tempStr);
			}
			else
			{
				pDlg->m_SendCount++;
				tempStr.Format(L"发包个数：%i个",pDlg->m_SendCount++);
				pDlg->SetDlgItemText(IDC_SENDCNT,tempStr);
			}
			pDlg->m_IpInfo.push_back(pIpHeader);
			// 只检测TCP和UDP包
			switch ( pIpHeader->bProtocol )
			{
			case IPPROTO_TCP:
				{
					// 取得TCP数据包端口号
					PTCP_HEADER pTcpHeader = (PTCP_HEADER)&szPackBuf[nIpHeadLength] ;
					PackInfo.nSourPort = pTcpHeader->nSourPort ;
					PackInfo.nDestPort = pTcpHeader->nDesPort ;
					pDlg->AddPackInfo ( &PackInfo ) ;	// 输出到界面
				}
				break ;
			case IPPROTO_UDP:
				{
					// 取得UDP数据包端口号
					PUDP_HEADER pUdpHeader = (PUDP_HEADER)&szPackBuf[nIpHeadLength] ;
					PackInfo.nSourPort = pUdpHeader->nSourPort ;
					PackInfo.nDestPort = pUdpHeader->nDestPort ;
					pDlg->AddPackInfo ( &PackInfo ) ;	// 输出到界面
				}
				break ;
			default:
				{
					//取得其他协议数据包
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
	//获取绘制坐标的图片框
	CWnd* pWnd = GetDlgItem(IDC_IPREPORT);

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


	CPen *pGreenPen=new CPen(0,1,RGB(0,100,0));
	pDC->SelectObject(pGreenPen);
	//画边框
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
	//重写TextOut
  /*  if (m_clickSniferList)
    {
		pDC->SetTextColor(RGB(0,250,0));
		pDC->SetBkMode(TRANSPARENT);
		CString tempStr;
		//获得版本信息（前4位）和头长度（后4位）
		tempStr.Format(L"V&L: %0x",m_IpInfo[m_ItemIndex]->bVerAndHLen);
		pDC->TextOut(rect.left+15,rect.top+15,tempStr);
		//获得服务类型
		tempStr.Format(L"TOS: %0x",m_IpInfo[m_ItemIndex]->bTypeOfService);
		pDC->TextOut((rect.right+rect.left)/4+10,rect.top+15,tempStr);
		//获得数据包长度
		tempStr.Format(L"TL: %0x",m_IpInfo[m_ItemIndex]->nTotalLength);
		pDC->TextOut((rect.right+rect.left)/2+15,rect.top+15,tempStr);
		//获得数据包标识
		tempStr.Format(L"IDF: %0x",m_IpInfo[m_ItemIndex]->nID);
		pDC->TextOut(rect.left+15,rect.top+35+5,tempStr);
		//获得保留字段
		tempStr.Format(L"FO: %0x",m_IpInfo[m_ItemIndex]->nReserved);
		pDC->TextOut((rect.right+rect.left)/2+75,rect.top+35+5,tempStr);
		//获得生成时间
		tempStr.Format(L"TTL: %0x",m_IpInfo[m_ItemIndex]->nTTL);
		pDC->TextOut(rect.left+15,rect.top+70,tempStr);
		//获得协议类型
		tempStr.Format(L"PTC: %0x",m_IpInfo[m_ItemIndex]->bProtocol);
		pDC->TextOut((rect.right+rect.left)/4+15,rect.top+65+5,tempStr);
		//获得校验合
		tempStr.Format(L"HCS: %0x",m_IpInfo[m_ItemIndex]->nCheckSum);
		pDC->TextOut((rect.right+rect.left)/2+30,rect.top+65+5,tempStr);
		//获得源IP
		tempStr.Format(L"SIP: %0x",m_IpInfo[m_ItemIndex]->nSourIp);
		pDC->TextOut(rect.left + 15,rect.top+105,tempStr);
		//获得目的IP
		tempStr.Format(L"DIP: %0x",m_IpInfo[m_ItemIndex]->nDestIp);
		pDC->TextOut(rect.left + 15,rect.top+135,tempStr);
    }*/
	


}
//SniferList的Click事件
void CSockMonDlg::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
//	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<NMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	m_clickSniferList = TRUE;
	CRect rect;
	//获取绘制坐标的图片框
	CWnd* pWnd = GetDlgItem(IDC_IPREPORT);

	pWnd->GetClientRect(&rect);
	//指针
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
	if (pos)//获得选中List标号
	{
		nItemNum = this->m_SniferList.GetNextSelectedItem(pos);
		m_ItemIndex = nItemNum;
	}
	CString tempStr;
	//获得版本信息（前4位）和头长度（后4位）
	tempStr.Format(L"V&L: %0x",m_IpInfo[nItemNum]->bVerAndHLen);
	pDC->TextOut(rect.left+15,rect.top+15,tempStr);
	//获得服务类型
	tempStr.Format(L"TOS: %0x",m_IpInfo[nItemNum]->bTypeOfService);
	pDC->TextOut((rect.right+rect.left)/4+10,rect.top+15,tempStr);
	//获得数据包长度
	tempStr.Format(L"TL: %0x",m_IpInfo[nItemNum]->nTotalLength);
	pDC->TextOut((rect.right+rect.left)/2+15,rect.top+15,tempStr);
	//获得数据包标识
	tempStr.Format(L"IDF: %0x",m_IpInfo[nItemNum]->nID);
	pDC->TextOut(rect.left+15,rect.top+35+5,tempStr);
	//获得保留字段
	tempStr.Format(L"FO: %0x",m_IpInfo[nItemNum]->nReserved);
	pDC->TextOut((rect.right+rect.left)/2+75,rect.top+35+5,tempStr);
	//获得生成时间
	tempStr.Format(L"TTL: %0x",m_IpInfo[nItemNum]->nTTL);
	pDC->TextOut(rect.left+15,rect.top+70,tempStr);
	//获得协议类型
	tempStr.Format(L"PTC: %0x",m_IpInfo[nItemNum]->bProtocol);
	pDC->TextOut((rect.right+rect.left)/4+15,rect.top+65+5,tempStr);
	//获得校验合
	tempStr.Format(L"HCS: %0x",m_IpInfo[nItemNum]->nCheckSum);
	pDC->TextOut((rect.right+rect.left)/2+30,rect.top+65+5,tempStr);
	//获得源IP
	tempStr.Format(L"SIP: %0x",m_IpInfo[nItemNum]->nSourIp);
	pDC->TextOut(rect.left + 15,rect.top+105,tempStr);
	//获得目的IP
	tempStr.Format(L"DIP: %0x",m_IpInfo[nItemNum]->nDestIp);
	pDC->TextOut(rect.left + 15,rect.top+135,tempStr);

	
	*pResult = 0;
}

VOID CSockMonDlg::InitialSockLine(void)
{
	CRect rect;
	//获取绘制坐标的图片框
	CWnd* pWnd = GetDlgItem(IDC_PICBOX);

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


	CPen *pBkLine=new CPen(0,1,RGB(0,100,0));
	pDC->SelectObject(pBkLine);
	//画背景线条
	for (int i = 1 ; i <= 15 ; i++)//横线
	{
		pDC->MoveTo(rect.left+1,(rect.top+rect.bottom)/1.005 - i*10);
		pDC->LineTo(rect.right-2,(rect.top+rect.bottom)/1.005 - i*10);
	}
	for (int i = 0 ; i <= 83 ; i++)//纵线
	{
		pDC->MoveTo(i*10,rect.top + 1);
		pDC->LineTo(i*10,rect.bottom - 2);
	}
	CPen *pGreenPen=new CPen(0,1,RGB(0,255,0));
	CPen *pBluePen=new CPen(0,1,RGB(123,104,238));
	pDC->SelectObject(pGreenPen);
	//对变量进行上锁,进行同步处理
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
//当发生WM_PAINT消息时重绘SockLine
VOID CSockMonDlg::ReDrawSockLine(void)
{

	CRect rect;
	//获取绘制坐标的图片框
	CWnd* pWnd = GetDlgItem(IDC_PICBOX);

	pWnd->GetClientRect(&rect);
	//指针
	CDC* pDC = pWnd->GetDC();
	pWnd->Invalidate();
	pWnd->UpdateWindow();

	pDC->Rectangle(&rect);
	CPen *pGreenPen=new CPen(0,1,RGB(0,255,0));
	pDC->SelectObject(pGreenPen);
    //对变量进行上锁,进行同步处理
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
    
	// TODO:  在此更改 DC 的任何属性
    //pDC->SetBkMode(TRANSPARENT);
	if (nCtlColor == IDC_LIST1)//重绘两个ListBox控件
	{
		pDC->SetBkColor(RGB(41,36,33));
		pDC->SetTextColor(RGB(0,255,0));

	}
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}
//处理鼠标进入SockLine区域后的事件
void CSockMonDlg::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
    CRect rectPic;
	GetDlgItem(IDC_PICBOX)->GetWindowRect(&rectPic);
	if (PtInRect(&rectPic,point))//如果鼠标进入SockLine区域
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
//双击TreeView显示进程详细信息
void CSockMonDlg::OnNMDblclkSockTree(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	//获得所选Item
	HTREEITEM hti = SockTree.GetSelectedItem();
	CString myText;
	if (hti != NULL)
	{
		while (SockTree.GetParentItem(hti) != NULL)
		{
			hti = SockTree.GetParentItem(hti);
		}
		//获得根节点
	     myText = SockTree.GetItemText(hti);
		//MessageBox(myText);
	}
	//对Text进行处理，筛选出需要的进程ID
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
				strID += myText[i];//获得进程ID
				i++;
			}
			break;;
		}
    }
	//StartElevatedProcess(SE_DEBUG_NAME);//提升权限
	CMyProcessDlg dlg;
	//判断传入ID的第一个字符是否为0
	if (strID[0] == '0')
	{
		strID.Delete(0);//删除第一个字符
	}
	dlg.m_ProcessId = strID;
	dlg.DoModal();
	*pResult = 0;
}

//单击Snifer的Click事件
void CSockMonDlg::OnNMClickSnilist(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<NMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	m_clickSniferList = TRUE;
	CRect rect;
	//获取绘制坐标的图片框
	CWnd* pWnd = GetDlgItem(IDC_IPREPORT);

	pWnd->GetClientRect(&rect);
	//指针
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
	if (pos)//获得选中List标号
	{
		nItemNum = this->m_SniferList.GetNextSelectedItem(pos);
		m_ItemIndex = nItemNum;
	}
	CString tempStr;
	//获得版本信息（前4位）和头长度（后4位）
	tempStr.Format(L"V&L: %0x",m_IpInfo[nItemNum]->bVerAndHLen);
	pDC->TextOut(rect.left+15,rect.top+15,tempStr);
	//获得服务类型
	tempStr.Format(L"TOS: %0x",m_IpInfo[nItemNum]->bTypeOfService);
	pDC->TextOut((rect.right+rect.left)/4+10,rect.top+15,tempStr);
	//获得数据包长度
	tempStr.Format(L"TL: %0x",m_IpInfo[nItemNum]->nTotalLength);
	pDC->TextOut((rect.right+rect.left)/2+15,rect.top+15,tempStr);
	//获得数据包标识
	tempStr.Format(L"IDF: %0x",m_IpInfo[nItemNum]->nID);
	pDC->TextOut(rect.left+15,rect.top+35+5,tempStr);
	//获得保留字段
	tempStr.Format(L"FO: %0x",m_IpInfo[nItemNum]->nReserved);
	pDC->TextOut((rect.right+rect.left)/2+75,rect.top+35+5,tempStr);
	//获得生成时间
	tempStr.Format(L"TTL: %0x",m_IpInfo[nItemNum]->nTTL);
	pDC->TextOut(rect.left+15,rect.top+70,tempStr);
	//获得协议类型
	tempStr.Format(L"PTC: %0x",m_IpInfo[nItemNum]->bProtocol);
	pDC->TextOut((rect.right+rect.left)/4+15,rect.top+65+5,tempStr);
	//获得校验合
	tempStr.Format(L"HCS: %0x",m_IpInfo[nItemNum]->nCheckSum);
	pDC->TextOut((rect.right+rect.left)/2+30,rect.top+65+5,tempStr);
	//获得源IP
	tempStr.Format(L"SIP: %0x",m_IpInfo[nItemNum]->nSourIp);
	pDC->TextOut(rect.left + 15,rect.top+105,tempStr);
	//获得目的IP
	tempStr.Format(L"DIP: %0x",m_IpInfo[nItemNum]->nDestIp);
	pDC->TextOut(rect.left + 15,rect.top+135,tempStr);
	*pResult = 0;
}

// 提升进程权限
BOOL CSockMonDlg::StartElevatedProcess(LPCTSTR szPrivName)
{
	HANDLE hToken;
	TOKEN_PRIVILEGES tp;
	LUID luid;
	//打开进程令牌环
	if (!OpenProcessToken(GetCurrentProcess(),
		TOKEN_ADJUST_PRIVILEGES|TOKEN_QUERY,
		&hToken))
	{
		printf("OpenProcessToken error.\n");
		return true;
	}
	//获得进程本地唯一ID
	if (!LookupPrivilegeValue(NULL,szPrivName,&luid))
	{
		printf("LookupPrivilege error!\n");
	}

	tp.PrivilegeCount = 1;
	tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	tp.Privileges[0].Luid = luid;
	//调整权限
	if (!AdjustTokenPrivileges(hToken,0,&tp,sizeof(TOKEN_PRIVILEGES),NULL,NULL))
	{
		printf("AdjustTokenPrivileges error!\n");
		return true;
	}
	return false;
}

// 提升用户权限
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
