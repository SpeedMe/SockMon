// MyProcessDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SockMon.h"
#include "MyProcessDlg.h"
#include "windows.h"
#include "psapi.h"
#include "MyNtdll.h"
#include <tlhelp32.h>
#include "MyNativeHead.h"
#include "MySystem.h"
#include "MyMemInfo.h"
#include "FEInfoDlg.h"

#pragma comment ( lib, "psapi.lib" )

#define MAX_BUF_SIZE    1024
#define DEF_BUF_SIZE	0x1000
#define MAX_BUF_SIZE2   2048

// CMyProcessDlg 对话框


PFNNtQuerySystemInformation NtQuerySystemInformation ;

IMPLEMENT_DYNAMIC(CMyProcessDlg, CDialog)

CMyProcessDlg::CMyProcessDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMyProcessDlg::IDD, pParent)
{

}

CMyProcessDlg::~CMyProcessDlg()
{
}

void CMyProcessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_ProcessModulesInfo);
    DDX_Control(pDX, IDC_EDIT3, m_ProcessBaseInfo);
  
    //MessageBox(m_ProcessId);

	//列举进程基本信息
	ListProcessBaseInfo();
	//列举进程内存信息
	EnumProcessMemInfo();
	//列举进程的线程信息
	EnumThreadInfo();
	//列举模块信息
	EnumModulesInfo();
}


BEGIN_MESSAGE_MAP(CMyProcessDlg, CDialog)
	ON_BN_CLICKED(IDCANCEL, &CMyProcessDlg::OnBnClickedCancel)
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CTLCOLOR()
	ON_WM_RBUTTONDBLCLK()
	ON_COMMAND(ID_1_1, &CMyProcessDlg::OnKillProcNormal)
	ON_LBN_DBLCLK(IDC_LIST1, &CMyProcessDlg::OnLbnDblclkList1)
	ON_COMMAND(ID_1_2, &CMyProcessDlg::OnKillProcessZero)
	ON_COMMAND(ID_1_3, &CMyProcessDlg::OnKillThread)
	ON_COMMAND(ID_Menu, &CMyProcessDlg::OnKillModule)
	ON_COMMAND(ID_1_32778, &CMyProcessDlg::OnLookMenInfo)
	ON_COMMAND(ID_1_32779, &CMyProcessDlg::OnLookPEInfo)
END_MESSAGE_MAP()


// CMyProcessDlg 消息处理程序

void CMyProcessDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	OnCancel();
}

void CMyProcessDlg::OnLButtonDblClk(UINT nFlags,CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
    OnCancel();
	CDialog::OnLButtonDblClk(nFlags, point);
}

HBRUSH CMyProcessDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何属性
    pDC->SetBkMode(TRANSPARENT);
	pDC->SetBkColor(RGB(41,36,33));
	pDC->SetTextColor(RGB(0,255,0));
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return CreateSolidBrush(RGB(41,36,33));
	
	//return hbr;
}

//列举进程基本信息
BOOL CMyProcessDlg::ListProcessBaseInfo(void)
{
	//MessageBox(L"ListProcessBaseInfo()");
	if (InitialProcessInfo() == FALSE)
	{
		return FALSE;
	}
	QuerySystemInformation(SystemProcessesAndThreadsInformation);
	return TRUE;
}

// 初始化枚举进程基本信息
BOOL CMyProcessDlg::InitialProcessInfo(void)
{
	//MessageBox(L"InitialProcessInfo()");
	//检测当前进程是否在ntdll.dll
	HMODULE hMod = GetModuleHandle(L"ntdll.dll");
	if (hMod == NULL)
	{
		//如果不存在，就是用LoadLibrary来加载
		hMod = LoadLibrary(L"ntdll.dll");
		if (hMod == NULL)
		{
			MessageBox(L"无法加载ntdll.dll");
			return FALSE;
		}
	}
	//取得函数地址
	NtQuerySystemInformation = (PFNNtQuerySystemInformation)GetProcAddress ( hMod, "NtQuerySystemInformation" ) ;
	return TRUE ;
}

VOID CMyProcessDlg::QuerySystemInformation(SYSTEM_INFORMATION_CLASS SystemInformationClass)
{
	//MessageBox(L"QuertSystemInformation()");
	NTSTATUS status;
	UINT nSize = DEF_BUF_SIZE;
	LPBYTE lpBuf = NULL;

	//由于事先并不知道需要多少空间来存储进程信息
	//因而采用循环测试法
	while (TRUE)
	{
		//动态分配空间，用来存储进程信息
		if ((lpBuf = new BYTE[nSize]) == NULL)
		{
			MessageBox(L"Allocate memory failed!");
			return;
		}
		//枚举进程信息
		status = NtQuerySystemInformation(SystemInformationClass,lpBuf,nSize,0);
		if (!NT_SUCCESS(status))
		{
			//检测是否返回缓冲区不够大
			if (status == STATUS_INFO_LENGTH_MISMATCH)
			{
				nSize += DEF_BUF_SIZE;
				delete lpBuf;
				continue;
			}
			else
			{
				MessageBox(L"NtQuerySystemInformation Error");
				return;
			}
		}
		else
			break;
	}
	//输出进程信息
	DisplayProcessInformation(lpBuf);
	
	
	
}
//将进程基本信息输出
void CMyProcessDlg::DisplayProcessInformation(LPBYTE lpBuf)
{
	//MessageBox(L"Display()");
	PSYSTEM_PROCESSES pSysProcess = (PSYSTEM_PROCESSES)lpBuf;
	while(TRUE)
	{
		if (pSysProcess->ProcessName.Buffer != NULL)
		{
			//获得进程ID
			CString processID;
			processID.Format(_T("%d"),pSysProcess->ProcessId);
			if (processID == m_ProcessId)//找到ID
			{
				//获得进程名
				CString processName = (CString)(pSysProcess->ProcessName.Buffer);
				//获得父进程ID
				CString InheritedID;
				InheritedID.Format(_T("%i"),pSysProcess->InheritedFromProcessId);
				//获得句柄数量
				CString HandlleCount;
				HandlleCount.Format(_T("%i"),pSysProcess->HandleCount);
				//获得线程数量
				CString ThreadCount;
				ThreadCount.Format(_T("%i"),pSysProcess->ThreadCount);
				//将信息整合
				CString totalStr;
				totalStr.Format(_T("ProcessId: %s ProcessName: %s InheritedId: %s HandleCount: %s ThreadCount: %s"),processID,processName,InheritedID,HandlleCount,ThreadCount);
				SetDlgItemText(IDC_EDIT3,totalStr);
				break;
			}
			
		}
	
		if ( pSysProcess->NextEntryDelta == 0 )
			break ;
		pSysProcess = (PSYSTEM_PROCESSES)( (DWORD)pSysProcess + pSysProcess->NextEntryDelta ) ;
		
	}
}
//双击右键关闭对话框
void CMyProcessDlg::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
    OnCancel();
	CDialog::OnRButtonDblClk(nFlags, point);
}
//列举进程内存信息
BOOL CMyProcessDlg::EnumProcessMemInfo(void)
{
	/*DWORD dwProcessId;
	//将CString转化为DWORD
	char *buf = (LPSTR)(LPCTSTR)m_ProcessId;
	MessageBox((LPCTSTR)m_ProcessId);
	int iProcessId = atoi(buf);
	MessageBox((LPCTSTR)iProcessId);
	dwProcessId = (DWORD)iProcessId;
	//获得进程内存信息
	MessageBox((LPCTSTR)dwProcessId);*/

	DWORD dwProcessId[1024],cbNeededProcess;
	//枚举进程
	if (!EnumProcesses(dwProcessId,sizeof(dwProcessId),&cbNeededProcess))
	{
		return FALSE;
	}
	for (unsigned int i = 0 ; i < (cbNeededProcess/sizeof(DWORD)); i++)
	{
		char szProcessName[MAX_PATH] = "unKnow";
		CString strID;
		strID.Format(_T("%i"),dwProcessId[i]);
		if(strID != m_ProcessId)
			continue;
		//MessageBox(strID);
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,FALSE,dwProcessId[i]);
		if (hProcess != NULL)
		{
			//MessageBox(L"OK");
			HMODULE hMods[MAX_BUF_SIZE] = {0};
			DWORD cbNeededModules = 0;
			char szModuleName[MAX_BUF_SIZE] = {0};
			PROCESS_MEMORY_COUNTERS ProcessMemCounters;

			//枚举进程信息
			EnumProcessModules(hProcess,hMods,sizeof(hMods),&cbNeededModules);
			//取得主模块全名，每个进程的第一个某块为进程的主模块，这里使用ANSI版
			::GetModuleFileNameExA(hProcess,hMods[0],szModuleName,sizeof(szModuleName));
			//取得进程的内存使用
			GetProcessMemoryInfo(hProcess,&ProcessMemCounters,sizeof(ProcessMemCounters));
			//转化信息格式
			CString ProcessBoot,PageFalutCount,PeakWorkingSetSize,WorkingSetSize;
			CString QuotaPeakPagedPoolUsage,QuotaPagedPoolUsage,QuotaPeakNonPagedPoolUsage;
			CString QuotaNonPagedPoolUsage,PagefileUsage,PeakPagefileUsage;

			ProcessBoot = (CString)szModuleName;
			PageFalutCount.Format(_T("PageFaultCount： %i"),ProcessMemCounters.PageFaultCount);
			PeakWorkingSetSize.Format(_T("PeakWorkingSetSize： %x"),ProcessMemCounters.PeakWorkingSetSize);
			WorkingSetSize.Format(_T("WorkingSetSize： %x"),ProcessMemCounters.WorkingSetSize);
			QuotaPeakPagedPoolUsage.Format(_T("QuotaPeakPagedPoolUsage： %x"),ProcessMemCounters.QuotaPeakPagedPoolUsage);
			QuotaPagedPoolUsage.Format(_T("QuotaPagedPoolUsage： %x"),ProcessMemCounters.QuotaPagedPoolUsage);
			QuotaPeakNonPagedPoolUsage.Format(_T("QuotaPeakNonPagedPoolUsage： %x"),ProcessMemCounters.QuotaPeakNonPagedPoolUsage);
			QuotaNonPagedPoolUsage.Format(_T("QuotaNonPagedPoolUsage： %x"),ProcessMemCounters.QuotaNonPagedPoolUsage);
			PeakPagefileUsage.Format(_T("PeakPagefileUsage： %x"),ProcessMemCounters.PeakPagefileUsage);
			PagefileUsage.Format(_T("PagefileUage： %x"),ProcessMemCounters.PagefileUsage);

			//整合信息
			CString totalInfo;
			CString baseInfo;
			GetDlgItemText(IDC_EDIT3,baseInfo);
			totalInfo.Format(_T("%s\r\n%s %s\r\n%s %s\r\n%s %s\r\n%s %s\r\n%s %s\r\n"),\
				baseInfo,\
				ProcessBoot,PageFalutCount,\
				PeakWorkingSetSize,WorkingSetSize,\
				QuotaPeakPagedPoolUsage,QuotaPagedPoolUsage,\
				QuotaPeakNonPagedPoolUsage,QuotaNonPagedPoolUsage,\
				PeakPagefileUsage,PagefileUsage);
			//输出信息
			SetDlgItemText(IDC_EDIT3,totalInfo);
			m_ProcessModulesInfo.AddString(ProcessBoot);
	      }
  
    }
	return TRUE;
}
//列举进程的线层信息
BOOL CMyProcessDlg::EnumThreadInfo(void)
{
	//定义线程结构
	THREADENTRY32 te32 = {sizeof(THREADENTRY32)};
	//创建系统线程快照
	HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD,0);
	if (hThreadSnap == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	m_ProcessModulesInfo.AddString(L"Thread Infomation:");
	//枚举输出线程信息
	if (Thread32First(hThreadSnap,&te32))
	{
		do 
		{
			//获得所属进程ID
			CString OwnerID;
			OwnerID.Format(_T("%d"),te32.th32OwnerProcessID);
			if (OwnerID == m_ProcessId)
			{
				//获得ThreadID
				CString ThreadID;
				ThreadID.Format(_T("ThreadId: %d "),te32.th32ThreadID);
				//获得cntUsage
				CString cntUsage;
				cntUsage.Format(_T("Usage: %d "),te32.cntUsage);
				//获得Flag
				CString flags;
				flags.Format(_T("Flags: %d "),te32.dwFlags);
				//获得Size
				CString Size;
				Size.Format(_T("Size: %d "),te32.dwSize);
				//获得BasePri
				CString BasePri;
				BasePri.Format(_T("BasePri: %d "),te32.tpBasePri);
				//获得DeltaPri
				CString DeltaPri;
				DeltaPri.Format(_T("DeltaPri: %d "),te32.tpDeltaPri);
				//输出信息
				m_ProcessModulesInfo.AddString(ThreadID+L" "+cntUsage+L" "+flags+L" "+Size+L" "+BasePri+L" "+DeltaPri);
			}
		} while (Thread32Next(hThreadSnap,&te32));
	}
	return TRUE;
}



// 枚举模块信息
BOOL CMyProcessDlg::EnumModulesInfo(void)
{
	//定义模块结构
	MODULEENTRY32 mod = {sizeof(MODULEENTRY32)};
	//创建系统快照
	HANDLE hModSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,0);
	if (hModSnap == NULL)
	{
		return FALSE;
	}
	m_ProcessModulesInfo.AddString(L"Modules Information:");
	//枚举输出某块信息
	if (Module32First(hModSnap,&mod))
	{
		do 
		{
			//获得进程ID
			CString ProcID;
			ProcID.Format(_T("%d"),mod.th32ProcessID);
			if (ProcID == m_ProcessId)
			{
				//获取ModID
				CString ModID;
				ModID.Format(_T("ID: %d"),mod.th32ModuleID);
				//获得ModPatn
				CString ModPath;
				ModPath.Format(_T("ExePath: %s"),mod.szExePath);
				//获得Size
				CString Size;
				Size.Format(_T("Size: %d"),mod.dwSize);
				//获得GlblcntUsage
				CString glbUsage;
				glbUsage.Format(_T("GlblcnUsage: %d"),mod.GlblcntUsage);
				//获得ModBaseAddress
				CString BaseAddress;
				int tempBase = (int)mod.modBaseAddr;
				BaseAddress.Format(_T("BaseAddress: %x"),tempBase);
				//获得BaseSize
				CString BaseSize;
				BaseSize.Format(_T("BaseSzie: %d"),mod.modBaseSize);
				//获得ProccntUsage
				CString ProccntUsage;
				ProccntUsage.Format(_T("ProccntUsage: %d"),mod.ProccntUsage);
				//获得Module
				CString Module;
				Module.Format(_T("Module: %s"),mod.szModule);
				
				//输出信息
				m_ProcessModulesInfo.AddString(ModID+L" "+Module+L" "+ModPath);
				m_ProcessModulesInfo.AddString(Size+L" "+glbUsage+L" "+BaseAddress);
				m_ProcessModulesInfo.AddString(BaseSize+L" "+ProccntUsage);
				
				
			}
		} while (Module32Next(hModSnap,&mod));
	}
	return TRUE;
}

//普通杀进程法
void CMyProcessDlg::OnKillProcNormal()
{
	// TODO: 在此添加命令处理程序代码
    CString str = m_ProcessId;
	WCHAR* wTem = {'\0'};
	DWORD dwProcessId = 0;
	wTem = str.GetBuffer(str.GetLength());
	dwProcessId = _wtoi(wTem);
	HANDLE hProcess = My_OpenProcess(
		PROCESS_ALL_ACCESS,
		0,
		dwProcessId,
		true,
		(LPDWORD)0
		);
	MessageBox(L"进程结束完毕，请重新列举进程！");
    TerminateProcess(hProcess,0);
}
//双击弹出菜单
void CMyProcessDlg::OnLbnDblclkList1()
{
	// TODO: 在此添加控件通知处理程序代码
	CPoint pt;
	GetCursorPos(&pt);
	CMenu menu;
	menu.LoadMenu(IDR_MENU1);
	CMenu* pMenu = menu.GetSubMenu(0);
	pMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON,pt.x,pt.y,this);
}

// 提升用户权限
 BOOL CMyProcessDlg::EnablePrivilege(LPCTSTR szPrivName)
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
//清零法杀进程
void CMyProcessDlg::OnKillProcessZero()
{
	// TODO: 在此添加命令处理程序代码
	CString str = m_ProcessId;//获得当前进程
	WCHAR* wTem =str.GetBuffer(str.GetLength());
	int dwProcessId = _wtoi(wTem);
	KillProcess(dwProcessId);
	MessageBox(L"结束进程完毕，重新列举进程！");
}

// 清零杀进程
int CMyProcessDlg::KillProcess(ULONG dwProcessId)
{
	HMODULE hNTDLL = GetModuleHandle(L"ntdll.dll");
	HANDLE ph,h_dup;

	PSYSTEM_HANDLE_INFORMATION     h_info={0}; 
	PROCESS_BASIC_INFORMATION      pbi={0};
	//得到csrss.exe进程的PID
	HANDLE csrss_id = (HANDLE)GetPidByName(L"csrss.exe");
	CLIENT_ID client_id;
	client_id.UniqueProcess = csrss_id;
	client_id.UniqueThread = 0;

	//初始化对象结构体
	OBJECT_ATTRIBUTES   attr;
	attr.Length        =sizeof(OBJECT_ATTRIBUTES);
	attr.RootDirectory = 0;
	attr.ObjectName = 0;
	attr.Attributes = 0;
	attr.SecurityDescriptor = 0;
	attr.SecurityQualityOfService = 0;

	/////////////////////////获得这些函数的实际地址
	pFnZwQuerySystemInformation ZwQuerySystemInformation = 
		(pFnZwQuerySystemInformation) GetProcAddress (hNTDLL, "ZwQuerySystemInformation");

	pFnNtOpenProcess ZwOpenProcess = 
		(pFnNtOpenProcess) GetProcAddress (hNTDLL, "ZwOpenProcess");

	pFnZwDuplicateObject ZwDuplicateObject =
		(pFnZwDuplicateObject) GetProcAddress (hNTDLL, "ZwDuplicateObject");

	pFnZwQueryInformationProcess ZwQueryInformationProcess = 
		(pFnZwQueryInformationProcess) GetProcAddress (hNTDLL, "ZwQueryInformationProcess");

	pFnZwAllocateVirtualMemory   ZwAllocateVirtualMemory =
		(pFnZwAllocateVirtualMemory) GetProcAddress (hNTDLL, "ZwAllocateVirtualMemory");

	pFnZwProtectVirtualMemory ZwProtectVirtualMemory = 
		(pFnZwProtectVirtualMemory) GetProcAddress (hNTDLL, "ZwProtectVirtualMemory");

	pFnZwWriteVirtualMemory ZwWriteVirtualMemory = 
		(pFnZwWriteVirtualMemory) GetProcAddress (hNTDLL, "ZwWriteVirtualMemory");

	pFnZwFreeVirtualMemory   ZwFreeVirtualMemory = 
		(pFnZwFreeVirtualMemory) GetProcAddress (GetModuleHandle(L"ntdll.dll"),   "ZwFreeVirtualMemory");

	pFnZwClose   ZwClose = 
		(pFnZwClose) GetProcAddress (hNTDLL, "ZwClose");

	EnablePrivilege(SE_DEBUG_NAME);//提升权限
	//打开CSRSS.EXE，获得其句柄
	ZwOpenProcess(&ph,PROCESS_ALL_ACCESS,&attr,&client_id);
	ULONG bytesIO = 0x400000;
	PVOID buf = 0;

	ZwAllocateVirtualMemory(GetCurrentProcess(),&buf,0,&bytesIO,MEM_COMMIT,PAGE_READWRITE);
    //为ZwQuerySystemInformation函数传递16号参数，获得系统句柄信息保存在buff中
	//buff的开始出保存的是系统句柄的数量，偏移4才是句柄信息
	ZwQuerySystemInformation(SystemHandleInformation,buf,0x400000,&bytesIO);
    ULONG NumOfHandle = (ULONG)buf;
	h_info = (PSYSTEM_HANDLE_INFORMATION)((ULONG)buf+4);

	for (ULONG i = 0 ; i < NumOfHandle ; i++,h_info++)
	{
		if ((h_info->ProcessId == (ULONG)csrss_id)&&(h_info->ObjectTypeNumber == 5))
		{
			//复制句柄
			if (ZwDuplicateObject(
				ph,
				(PHANDLE)h_info->Handle,
				(HANDLE)-1,
				&h_dup,
				0,
				0,
				DUPLICATE_SAME_ACCESS) == STATUS_SUCCESS)
			{
				ZwQueryInformationProcess(h_dup,0,&pbi,sizeof(pbi),&bytesIO);
			}

			if (pbi.UniqueProcessId == dwProcessId)
			{
				MessageBox(L"目标已确定！",L"！",MB_OK);

				for (i = 0x1000 ; i < 0x8000000 ; i = i + 0x1000)
				{
					PVOID pAddress = (PVOID)i;
					ULONG sz = 0x1000;
					ULONG oldp;

					if (ZwProtectVirtualMemory(h_dup,&pAddress,&sz,PAGE_EXECUTE_READWRITE,&oldp)==STATUS_SUCCESS)
					{
						ZwWriteVirtualMemory(h_dup,pAddress,buf,0x1000,&oldp);
					}
				}
				MessageBox(L"任务已完成！",L"!",0);
				break;
			}
		}
	}

   bytesIO = 0;
   ZwFreeVirtualMemory(GetCurrentProcess(),&buf,&bytesIO,MEM_RELEASE);

	return 0;

	
}

DWORD CMyProcessDlg::GetPidByName(LPCWSTR szName)
{
	HANDLE hProcessSnap = INVALID_HANDLE_VALUE;
	PROCESSENTRY32 pe32 = {0};
	DWORD dwRet = 0;

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS,0);
	if(hProcessSnap == INVALID_HANDLE_VALUE)
		return 0;

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (Process32First(hProcessSnap,&pe32))
	{
		do 
		{
			if (lstrcmpiW(szName,pe32.szExeFile) == 0)
			{
				dwRet = pe32.th32ProcessID;
				break;
			}
		} while (Process32Next(hProcessSnap,&pe32));
	}
	else
		return 0;
   
	if(hProcessSnap != INVALID_HANDLE_VALUE)
		CloseHandle(hProcessSnap);

	return dwRet;

}

// 打开进程
HANDLE CMyProcessDlg::My_OpenProcess(
									 DWORD dwDesiredAccess,
									 bool bInhert,
									 DWORD ProcessId,
									 bool bOpenIt, 
									 LPDWORD aryPids
									 )
									  /*++

				 参数 :
				 dwDesiredAccess - 希望以怎样的方式打开进程           
				 bInhert - 是否有继承权限           
				 ProcessId - PID
				 bOpenIt - 是要打开进程，还是要保存所有进程ID
				 paryPids - 保存PID的数组

				 返回 :
				 成功 - 指定的进程句柄

				 失败 - 0

				 功能 : 

				 1. 复制来复制去,总之是要获得指定进程的句柄
				 2. 用普通方法得到所有进程ID
				 --*/ 
{
	ULONG             uReturn;
	ULONG             NumOfHandle = 0;
	PCHAR             pBuffer = NULL;
	PVOID             pOneEprocess = 0;
	CLIENT_ID         cid;
	NTSTATUS          st;
	OBJECT_ATTRIBUTES   oa = {sizeof(OBJECT_ATTRIBUTES),0,NULL,NULL};
	PROCESS_BASIC_INFORMATION  pbi;
	PSYSTEM_HANDLE_TABLE_ENTRY_INFO h_info;
    HANDLE hProcessToDup,hProcessCur,hProcessToRet;
	ULONG cbBuffer = 0x4000;
	HMODULE hModulentdll = LoadLibrary(L"ntdll.dll");
	AdjustPurview();
	pFnZwQueryInformationProcess ZwQueryInformationProcess = (pFnZwQueryInformationProcess)GetProcAddress ( hModulentdll, "ZwQueryInformationProcess" ) ;
	pFnNtOpenProcess NtOpenProcess=(pFnNtOpenProcess)GetProcAddress ( hModulentdll, "NtOpenProcess" ) ;
	pFnZwQuerySystemInformation ZwQuerySystemInformation =(pFnZwQuerySystemInformation)GetProcAddress ( hModulentdll, "ZwQuerySystemInformation" ) ;
	pFnZwDuplicateObject ZwDuplicateObject=(pFnZwDuplicateObject)GetProcAddress ( hModulentdll, "ZwDuplicateObject" ) ;
	pFnZwClose ZwClose=(pFnZwClose)GetProcAddress ( hModulentdll, "ZwClose" ) ;

	if (bInhert)
	{
		//看看能否直接得到句柄
		cid.UniqueProcess = (HANDLE)(ProcessId);
		cid.UniqueThread = (HANDLE)0;
		st = NtOpenProcess(&hProcessToRet,dwDesiredAccess,&oa,&cid);

		if (NT_SUCCESS(st))
		{
			return hProcessToRet;
		}
	}
	//传递16号获得所有句柄，可能被hook过，不管了，走过场
	do 
	{
		pBuffer = (PCHAR)malloc(cbBuffer);
		if(pBuffer == NULL)
			return 0;
		memset(pBuffer,0,cbBuffer);
		st = ZwQuerySystemInformation(SystemHandleInformation,pBuffer,cbBuffer,&uReturn);
		if (st == STATUS_INFO_LENGTH_MISMATCH)
		{
			free(pBuffer);
			cbBuffer = cbBuffer * 2;
		}
	} while (st == STATUS_INFO_LENGTH_MISMATCH);
	NumOfHandle = *(ULONG*)pBuffer;
	h_info = (PSYSTEM_HANDLE_TABLE_ENTRY_INFO)((ULONG)pBuffer + 4);

	for (ULONG i = 0 ; i < NumOfHandle ; i++)
	{
		if(!bOpenIt)
			aryPids[i] = h_info[i].UniqueProcessId;

		if (bOpenIt)
		{
			if (5 == h_info[i].ObjectTypeIndex)//是进程句柄，打开它
			{
				cid.UniqueProcess = (HANDLE)h_info[i].UniqueProcessId;
				st = NtOpenProcess(&hProcessToDup,PROCESS_DUP_HANDLE,&oa,&cid);

				if (NT_SUCCESS(st))//复制该句柄后赋予全部权限，以便调用ZwQuery*时顺利
				{
					st = ZwDuplicateObject(hProcessToDup,(PHANDLE)h_info[i].HandleValue,(HANDLE)-1,
						&hProcessCur,PROCESS_ALL_ACCESS,0,DUPLICATE_SAME_ACCESS);
					if (NT_SUCCESS(st))//查看复制的句柄是否为我们想要的
					{
						st = ZwQueryInformationProcess(hProcessCur,0,\
							&pbi,sizeof(pbi),0);

						if (NT_SUCCESS(st))
						{
							if (ProcessId == pbi.UniqueProcessId)//若是想要的PID，就把该句柄按照希望的访问权限复制，然后返回
							{
								st = ZwDuplicateObject(hProcessToDup,(PHANDLE)h_info[i].HandleValue,\
									(HANDLE)-1,&hProcessToRet,dwDesiredAccess,\
									OBJ_INHERIT,DUPLICATE_SAME_ATTRIBUTES);

								ZwClose(hProcessCur);
								ZwClose(hProcessToDup);
								return hProcessToRet;
							}
						}
					}
					ZwClose(hProcessCur);

				}
				ZwClose(hProcessToDup);
			}
		}
	}

    return 0;
 
}

//调整权限
BOOL CMyProcessDlg::AdjustPurview(void)
{
	TOKEN_PRIVILEGES TokenPrivileges;
	bool bRet;
	HANDLE hToken;

	LookupPrivilegeValue(NULL,SE_DEBUG_NAME,&TokenPrivileges.Privileges[0].Luid);
	OpenProcessToken(GetCurrentProcess(),TOKEN_ADJUST_PRIVILEGES,&hToken);

	TokenPrivileges.PrivilegeCount = 1;
	TokenPrivileges.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

	bRet = !!AdjustTokenPrivileges(hToken,FALSE,&TokenPrivileges,0,NULL,NULL);

	CloseHandle(hToken);
	return bRet;
}
//杀线程
void CMyProcessDlg::OnKillThread()
{
	// TODO: 在此添加命令处理程序代码
	int i = m_ProcessModulesInfo.GetCurSel();
	CString str;
	m_ProcessModulesInfo.GetText(i,str);
	if (str.Left(8) != "ThreadId")
	{
		MessageBox(L"请选择要关闭的线程！");
		return;
	}
    
	WCHAR* wTem = {'\0'};
	DWORD dwProcessId = 0;
	DWORD dwCid = 0;
    //获得线程ID
	CString strCid;
	CString temp = L" ";
	for (int i = 9 ; i < str.GetLength() ; i++)
	{
        if(str[i] == temp)
			break;
		strCid += str[i];
	}
	wTem = strCid.GetBuffer(strCid.GetLength());
	dwCid = _wtoi(wTem);

	//获得进程ID
	str = m_ProcessId;
	wTem = str.GetBuffer(str.GetLength());
	dwProcessId = _wtoi(wTem);
	KillThreadByTerminate(dwProcessId,dwCid);
	MessageBox(L"结束进程完毕");

    


}

// 普通杀线程
void CMyProcessDlg::KillThreadByTerminate(DWORD dwProcessId, DWORD dwCid)
{
	//参数构造
	//局部变量
	HINSTANCE Kernel32lib = LoadLibrary(L"kernel32.dll");
	typedef HANDLE (* PNOpenThread)(DWORD,BOOL,DWORD);//OpenThread
	typedef BOOL   (* PNTerminateThread)(
		HANDLE hThread,
		DWORD dwExitCode
		);
	PNTerminateThread MyTerminateThread;
	PNOpenThread MyOpenThread;

	//函数动作
	if (NULL == Kernel32lib)
	{
		return;
	}
	HANDLE my_thread = NULL;
	MyOpenThread = (PNOpenThread)GetProcAddress(Kernel32lib,"OpenThread");
	MyTerminateThread = (PNTerminateThread)GetProcAddress(Kernel32lib,"TerminateThread");
	KillThreadParam Param = {0};

	Param.dwOpenThread = (DWORD)MyOpenThread;
	Param.dwTerminateThread = (DWORD)MyTerminateThread;
	Param.dwCid = dwCid;
    AdjustPurview();//提升权限
	//打开指定的进程
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwProcessId);
	if(hProcess == NULL)
	{
		OutputDebugString(_T("OpenProcess failed!"));
		return;
	}
	//参数写入
	LPVOID lpParam = NULL;
	lpParam = VirtualAllocEx(hProcess,NULL,sizeof(Param),MEM_COMMIT,PAGE_READWRITE);
    if (lpParam == NULL)
    {
		return;
    }
	if (!WriteProcessMemory(hProcess,lpParam,&Param,sizeof(Param),0))
	{
		VirtualFreeEx(hProcess,lpParam,0,MEM_RELEASE);
		return;
	}
	
	//函数写入
	LPVOID lpThread = NULL;
	lpThread = VirtualAllocEx(hProcess,NULL,0x100,MEM_COMMIT,PAGE_EXECUTE_READWRITE);
	if (lpThread == NULL)
	{
		return;
	}
	if (!WriteProcessMemory(hProcess,lpThread,(LPCVOID)*CMySystem::KillProcessFun,0x100,0))
	{
		VirtualFreeEx(hProcess,lpThread,0,MEM_RELEASE);
		return ;
	}
	//创建线程
    HANDLE hThread = NULL;
	hThread = CreateRemoteThread(hProcess,0,0,(LPTHREAD_START_ROUTINE)lpThread,lpParam,0,NULL);
    if (hThread == NULL)
    {
		return;
    }
	//等待线程结束
	//清理工作
	VirtualFreeEx(hProcess,lpThread,0,MEM_RELEASE);
	VirtualFreeEx(hProcess,lpParam,0,MEM_RELEASE);
	CloseHandle(hProcess);
}

VOID CMyProcessDlg::KillMyThreadFun(PKillThreadParam PParam)
{
	DWORD dwOpenThread;                 //OpenThread地址
	DWORD dwTerminateThread;            //TerminateThread地址
	DWORD dwCid;                        //需要卸载的线程Cid
	HANDLE hThreadHandle = 0;
	dwOpenThread = PParam->dwOpenThread;
	dwTerminateThread = PParam->dwTerminateThread;
	dwCid = (DWORD)PParam->dwCid;
    __asm
	{
		push dwCid
		push 0
		push THREAD_ALL_ACCESS
		mov eax,dwOpenThread
		call eax
        mov hThreadHandle,eax
		push 0
		push hThreadHandle
		mov eax,dwTerminateThread
		call eax
	}
	return VOID();
}

//卸载模块
void CMyProcessDlg::OnKillModule()
{
	// TODO: 在此添加命令处理程序代码
	CString csPid,csName;
	csPid = m_ProcessId;
	//获得ID
	WCHAR* wTem = csPid.GetBuffer(csPid.GetLength());
	DWORD dwPid = _wtoi(wTem);

	//获得模块名
	int i = m_ProcessModulesInfo.GetCurSel();
	CString temp;
	m_ProcessModulesInfo.GetText(i,temp);
	CString csStr = L":";
	//MessageBox(L"Enter");
	for (int i = 4; i < temp.GetLength(); i++)
	{
		if (temp[i] == csStr)
		{
			i += 2;
			csStr = L" ";
			do 
			{
				csName += temp[i];
				i++;
			} while (temp[i] != csStr);
			break;
		}
	}
    TCHAR* szModuleName = csName.GetBuffer();
    //卸载模块
	CMySystem::FreeModuleByPid(szModuleName,dwPid);

    
}

//查看进程内存信息
void CMyProcessDlg::OnLookMenInfo()
{
	// TODO: 在此添加命令处理程序代码
	CMyMemInfo dlg;
	dlg.m_ProcId = m_ProcessId;
	dlg.DoModal();
}

//查看PE信息
void CMyProcessDlg::OnLookPEInfo()
{
	// TODO: 在此添加命令处理程序代码
	CString Path;
	m_ProcessModulesInfo.GetText(0,Path);
	CFEInfoDlg dlg;
	dlg.m_FilePath = Path;
	dlg.DoModal();
}
