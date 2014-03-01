// MyProcessDlg.cpp : ʵ���ļ�
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

// CMyProcessDlg �Ի���


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

	//�оٽ��̻�����Ϣ
	ListProcessBaseInfo();
	//�оٽ����ڴ���Ϣ
	EnumProcessMemInfo();
	//�оٽ��̵��߳���Ϣ
	EnumThreadInfo();
	//�о�ģ����Ϣ
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


// CMyProcessDlg ��Ϣ�������

void CMyProcessDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnCancel();
}

void CMyProcessDlg::OnLButtonDblClk(UINT nFlags,CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    OnCancel();
	CDialog::OnLButtonDblClk(nFlags, point);
}

HBRUSH CMyProcessDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����
    pDC->SetBkMode(TRANSPARENT);
	pDC->SetBkColor(RGB(41,36,33));
	pDC->SetTextColor(RGB(0,255,0));
	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return CreateSolidBrush(RGB(41,36,33));
	
	//return hbr;
}

//�оٽ��̻�����Ϣ
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

// ��ʼ��ö�ٽ��̻�����Ϣ
BOOL CMyProcessDlg::InitialProcessInfo(void)
{
	//MessageBox(L"InitialProcessInfo()");
	//��⵱ǰ�����Ƿ���ntdll.dll
	HMODULE hMod = GetModuleHandle(L"ntdll.dll");
	if (hMod == NULL)
	{
		//��������ڣ�������LoadLibrary������
		hMod = LoadLibrary(L"ntdll.dll");
		if (hMod == NULL)
		{
			MessageBox(L"�޷�����ntdll.dll");
			return FALSE;
		}
	}
	//ȡ�ú�����ַ
	NtQuerySystemInformation = (PFNNtQuerySystemInformation)GetProcAddress ( hMod, "NtQuerySystemInformation" ) ;
	return TRUE ;
}

VOID CMyProcessDlg::QuerySystemInformation(SYSTEM_INFORMATION_CLASS SystemInformationClass)
{
	//MessageBox(L"QuertSystemInformation()");
	NTSTATUS status;
	UINT nSize = DEF_BUF_SIZE;
	LPBYTE lpBuf = NULL;

	//�������Ȳ���֪����Ҫ���ٿռ����洢������Ϣ
	//�������ѭ�����Է�
	while (TRUE)
	{
		//��̬����ռ䣬�����洢������Ϣ
		if ((lpBuf = new BYTE[nSize]) == NULL)
		{
			MessageBox(L"Allocate memory failed!");
			return;
		}
		//ö�ٽ�����Ϣ
		status = NtQuerySystemInformation(SystemInformationClass,lpBuf,nSize,0);
		if (!NT_SUCCESS(status))
		{
			//����Ƿ񷵻ػ�����������
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
	//���������Ϣ
	DisplayProcessInformation(lpBuf);
	
	
	
}
//�����̻�����Ϣ���
void CMyProcessDlg::DisplayProcessInformation(LPBYTE lpBuf)
{
	//MessageBox(L"Display()");
	PSYSTEM_PROCESSES pSysProcess = (PSYSTEM_PROCESSES)lpBuf;
	while(TRUE)
	{
		if (pSysProcess->ProcessName.Buffer != NULL)
		{
			//��ý���ID
			CString processID;
			processID.Format(_T("%d"),pSysProcess->ProcessId);
			if (processID == m_ProcessId)//�ҵ�ID
			{
				//��ý�����
				CString processName = (CString)(pSysProcess->ProcessName.Buffer);
				//��ø�����ID
				CString InheritedID;
				InheritedID.Format(_T("%i"),pSysProcess->InheritedFromProcessId);
				//��þ������
				CString HandlleCount;
				HandlleCount.Format(_T("%i"),pSysProcess->HandleCount);
				//����߳�����
				CString ThreadCount;
				ThreadCount.Format(_T("%i"),pSysProcess->ThreadCount);
				//����Ϣ����
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
//˫���Ҽ��رնԻ���
void CMyProcessDlg::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    OnCancel();
	CDialog::OnRButtonDblClk(nFlags, point);
}
//�оٽ����ڴ���Ϣ
BOOL CMyProcessDlg::EnumProcessMemInfo(void)
{
	/*DWORD dwProcessId;
	//��CStringת��ΪDWORD
	char *buf = (LPSTR)(LPCTSTR)m_ProcessId;
	MessageBox((LPCTSTR)m_ProcessId);
	int iProcessId = atoi(buf);
	MessageBox((LPCTSTR)iProcessId);
	dwProcessId = (DWORD)iProcessId;
	//��ý����ڴ���Ϣ
	MessageBox((LPCTSTR)dwProcessId);*/

	DWORD dwProcessId[1024],cbNeededProcess;
	//ö�ٽ���
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

			//ö�ٽ�����Ϣ
			EnumProcessModules(hProcess,hMods,sizeof(hMods),&cbNeededModules);
			//ȡ����ģ��ȫ����ÿ�����̵ĵ�һ��ĳ��Ϊ���̵���ģ�飬����ʹ��ANSI��
			::GetModuleFileNameExA(hProcess,hMods[0],szModuleName,sizeof(szModuleName));
			//ȡ�ý��̵��ڴ�ʹ��
			GetProcessMemoryInfo(hProcess,&ProcessMemCounters,sizeof(ProcessMemCounters));
			//ת����Ϣ��ʽ
			CString ProcessBoot,PageFalutCount,PeakWorkingSetSize,WorkingSetSize;
			CString QuotaPeakPagedPoolUsage,QuotaPagedPoolUsage,QuotaPeakNonPagedPoolUsage;
			CString QuotaNonPagedPoolUsage,PagefileUsage,PeakPagefileUsage;

			ProcessBoot = (CString)szModuleName;
			PageFalutCount.Format(_T("PageFaultCount�� %i"),ProcessMemCounters.PageFaultCount);
			PeakWorkingSetSize.Format(_T("PeakWorkingSetSize�� %x"),ProcessMemCounters.PeakWorkingSetSize);
			WorkingSetSize.Format(_T("WorkingSetSize�� %x"),ProcessMemCounters.WorkingSetSize);
			QuotaPeakPagedPoolUsage.Format(_T("QuotaPeakPagedPoolUsage�� %x"),ProcessMemCounters.QuotaPeakPagedPoolUsage);
			QuotaPagedPoolUsage.Format(_T("QuotaPagedPoolUsage�� %x"),ProcessMemCounters.QuotaPagedPoolUsage);
			QuotaPeakNonPagedPoolUsage.Format(_T("QuotaPeakNonPagedPoolUsage�� %x"),ProcessMemCounters.QuotaPeakNonPagedPoolUsage);
			QuotaNonPagedPoolUsage.Format(_T("QuotaNonPagedPoolUsage�� %x"),ProcessMemCounters.QuotaNonPagedPoolUsage);
			PeakPagefileUsage.Format(_T("PeakPagefileUsage�� %x"),ProcessMemCounters.PeakPagefileUsage);
			PagefileUsage.Format(_T("PagefileUage�� %x"),ProcessMemCounters.PagefileUsage);

			//������Ϣ
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
			//�����Ϣ
			SetDlgItemText(IDC_EDIT3,totalInfo);
			m_ProcessModulesInfo.AddString(ProcessBoot);
	      }
  
    }
	return TRUE;
}
//�оٽ��̵��߲���Ϣ
BOOL CMyProcessDlg::EnumThreadInfo(void)
{
	//�����߳̽ṹ
	THREADENTRY32 te32 = {sizeof(THREADENTRY32)};
	//����ϵͳ�߳̿���
	HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD,0);
	if (hThreadSnap == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	m_ProcessModulesInfo.AddString(L"Thread Infomation:");
	//ö������߳���Ϣ
	if (Thread32First(hThreadSnap,&te32))
	{
		do 
		{
			//�����������ID
			CString OwnerID;
			OwnerID.Format(_T("%d"),te32.th32OwnerProcessID);
			if (OwnerID == m_ProcessId)
			{
				//���ThreadID
				CString ThreadID;
				ThreadID.Format(_T("ThreadId: %d "),te32.th32ThreadID);
				//���cntUsage
				CString cntUsage;
				cntUsage.Format(_T("Usage: %d "),te32.cntUsage);
				//���Flag
				CString flags;
				flags.Format(_T("Flags: %d "),te32.dwFlags);
				//���Size
				CString Size;
				Size.Format(_T("Size: %d "),te32.dwSize);
				//���BasePri
				CString BasePri;
				BasePri.Format(_T("BasePri: %d "),te32.tpBasePri);
				//���DeltaPri
				CString DeltaPri;
				DeltaPri.Format(_T("DeltaPri: %d "),te32.tpDeltaPri);
				//�����Ϣ
				m_ProcessModulesInfo.AddString(ThreadID+L" "+cntUsage+L" "+flags+L" "+Size+L" "+BasePri+L" "+DeltaPri);
			}
		} while (Thread32Next(hThreadSnap,&te32));
	}
	return TRUE;
}



// ö��ģ����Ϣ
BOOL CMyProcessDlg::EnumModulesInfo(void)
{
	//����ģ��ṹ
	MODULEENTRY32 mod = {sizeof(MODULEENTRY32)};
	//����ϵͳ����
	HANDLE hModSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE,0);
	if (hModSnap == NULL)
	{
		return FALSE;
	}
	m_ProcessModulesInfo.AddString(L"Modules Information:");
	//ö�����ĳ����Ϣ
	if (Module32First(hModSnap,&mod))
	{
		do 
		{
			//��ý���ID
			CString ProcID;
			ProcID.Format(_T("%d"),mod.th32ProcessID);
			if (ProcID == m_ProcessId)
			{
				//��ȡModID
				CString ModID;
				ModID.Format(_T("ID: %d"),mod.th32ModuleID);
				//���ModPatn
				CString ModPath;
				ModPath.Format(_T("ExePath: %s"),mod.szExePath);
				//���Size
				CString Size;
				Size.Format(_T("Size: %d"),mod.dwSize);
				//���GlblcntUsage
				CString glbUsage;
				glbUsage.Format(_T("GlblcnUsage: %d"),mod.GlblcntUsage);
				//���ModBaseAddress
				CString BaseAddress;
				int tempBase = (int)mod.modBaseAddr;
				BaseAddress.Format(_T("BaseAddress: %x"),tempBase);
				//���BaseSize
				CString BaseSize;
				BaseSize.Format(_T("BaseSzie: %d"),mod.modBaseSize);
				//���ProccntUsage
				CString ProccntUsage;
				ProccntUsage.Format(_T("ProccntUsage: %d"),mod.ProccntUsage);
				//���Module
				CString Module;
				Module.Format(_T("Module: %s"),mod.szModule);
				
				//�����Ϣ
				m_ProcessModulesInfo.AddString(ModID+L" "+Module+L" "+ModPath);
				m_ProcessModulesInfo.AddString(Size+L" "+glbUsage+L" "+BaseAddress);
				m_ProcessModulesInfo.AddString(BaseSize+L" "+ProccntUsage);
				
				
			}
		} while (Module32Next(hModSnap,&mod));
	}
	return TRUE;
}

//��ͨɱ���̷�
void CMyProcessDlg::OnKillProcNormal()
{
	// TODO: �ڴ���������������
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
	MessageBox(L"���̽�����ϣ��������оٽ��̣�");
    TerminateProcess(hProcess,0);
}
//˫�������˵�
void CMyProcessDlg::OnLbnDblclkList1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CPoint pt;
	GetCursorPos(&pt);
	CMenu menu;
	menu.LoadMenu(IDR_MENU1);
	CMenu* pMenu = menu.GetSubMenu(0);
	pMenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_LEFTBUTTON,pt.x,pt.y,this);
}

// �����û�Ȩ��
 BOOL CMyProcessDlg::EnablePrivilege(LPCTSTR szPrivName)
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
//���㷨ɱ����
void CMyProcessDlg::OnKillProcessZero()
{
	// TODO: �ڴ���������������
	CString str = m_ProcessId;//��õ�ǰ����
	WCHAR* wTem =str.GetBuffer(str.GetLength());
	int dwProcessId = _wtoi(wTem);
	KillProcess(dwProcessId);
	MessageBox(L"����������ϣ������оٽ��̣�");
}

// ����ɱ����
int CMyProcessDlg::KillProcess(ULONG dwProcessId)
{
	HMODULE hNTDLL = GetModuleHandle(L"ntdll.dll");
	HANDLE ph,h_dup;

	PSYSTEM_HANDLE_INFORMATION     h_info={0}; 
	PROCESS_BASIC_INFORMATION      pbi={0};
	//�õ�csrss.exe���̵�PID
	HANDLE csrss_id = (HANDLE)GetPidByName(L"csrss.exe");
	CLIENT_ID client_id;
	client_id.UniqueProcess = csrss_id;
	client_id.UniqueThread = 0;

	//��ʼ������ṹ��
	OBJECT_ATTRIBUTES   attr;
	attr.Length        =sizeof(OBJECT_ATTRIBUTES);
	attr.RootDirectory = 0;
	attr.ObjectName = 0;
	attr.Attributes = 0;
	attr.SecurityDescriptor = 0;
	attr.SecurityQualityOfService = 0;

	/////////////////////////�����Щ������ʵ�ʵ�ַ
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

	EnablePrivilege(SE_DEBUG_NAME);//����Ȩ��
	//��CSRSS.EXE���������
	ZwOpenProcess(&ph,PROCESS_ALL_ACCESS,&attr,&client_id);
	ULONG bytesIO = 0x400000;
	PVOID buf = 0;

	ZwAllocateVirtualMemory(GetCurrentProcess(),&buf,0,&bytesIO,MEM_COMMIT,PAGE_READWRITE);
    //ΪZwQuerySystemInformation��������16�Ų��������ϵͳ�����Ϣ������buff��
	//buff�Ŀ�ʼ���������ϵͳ�����������ƫ��4���Ǿ����Ϣ
	ZwQuerySystemInformation(SystemHandleInformation,buf,0x400000,&bytesIO);
    ULONG NumOfHandle = (ULONG)buf;
	h_info = (PSYSTEM_HANDLE_INFORMATION)((ULONG)buf+4);

	for (ULONG i = 0 ; i < NumOfHandle ; i++,h_info++)
	{
		if ((h_info->ProcessId == (ULONG)csrss_id)&&(h_info->ObjectTypeNumber == 5))
		{
			//���ƾ��
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
				MessageBox(L"Ŀ����ȷ����",L"��",MB_OK);

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
				MessageBox(L"��������ɣ�",L"!",0);
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

// �򿪽���
HANDLE CMyProcessDlg::My_OpenProcess(
									 DWORD dwDesiredAccess,
									 bool bInhert,
									 DWORD ProcessId,
									 bool bOpenIt, 
									 LPDWORD aryPids
									 )
									  /*++

				 ���� :
				 dwDesiredAccess - ϣ���������ķ�ʽ�򿪽���           
				 bInhert - �Ƿ��м̳�Ȩ��           
				 ProcessId - PID
				 bOpenIt - ��Ҫ�򿪽��̣�����Ҫ�������н���ID
				 paryPids - ����PID������

				 ���� :
				 �ɹ� - ָ���Ľ��̾��

				 ʧ�� - 0

				 ���� : 

				 1. ����������ȥ,��֮��Ҫ���ָ�����̵ľ��
				 2. ����ͨ�����õ����н���ID
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
		//�����ܷ�ֱ�ӵõ����
		cid.UniqueProcess = (HANDLE)(ProcessId);
		cid.UniqueThread = (HANDLE)0;
		st = NtOpenProcess(&hProcessToRet,dwDesiredAccess,&oa,&cid);

		if (NT_SUCCESS(st))
		{
			return hProcessToRet;
		}
	}
	//����16�Ż�����о�������ܱ�hook���������ˣ��߹���
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
			if (5 == h_info[i].ObjectTypeIndex)//�ǽ��̾��������
			{
				cid.UniqueProcess = (HANDLE)h_info[i].UniqueProcessId;
				st = NtOpenProcess(&hProcessToDup,PROCESS_DUP_HANDLE,&oa,&cid);

				if (NT_SUCCESS(st))//���Ƹþ������ȫ��Ȩ�ޣ��Ա����ZwQuery*ʱ˳��
				{
					st = ZwDuplicateObject(hProcessToDup,(PHANDLE)h_info[i].HandleValue,(HANDLE)-1,
						&hProcessCur,PROCESS_ALL_ACCESS,0,DUPLICATE_SAME_ACCESS);
					if (NT_SUCCESS(st))//�鿴���Ƶľ���Ƿ�Ϊ������Ҫ��
					{
						st = ZwQueryInformationProcess(hProcessCur,0,\
							&pbi,sizeof(pbi),0);

						if (NT_SUCCESS(st))
						{
							if (ProcessId == pbi.UniqueProcessId)//������Ҫ��PID���ͰѸþ������ϣ���ķ���Ȩ�޸��ƣ�Ȼ�󷵻�
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

//����Ȩ��
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
//ɱ�߳�
void CMyProcessDlg::OnKillThread()
{
	// TODO: �ڴ���������������
	int i = m_ProcessModulesInfo.GetCurSel();
	CString str;
	m_ProcessModulesInfo.GetText(i,str);
	if (str.Left(8) != "ThreadId")
	{
		MessageBox(L"��ѡ��Ҫ�رյ��̣߳�");
		return;
	}
    
	WCHAR* wTem = {'\0'};
	DWORD dwProcessId = 0;
	DWORD dwCid = 0;
    //����߳�ID
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

	//��ý���ID
	str = m_ProcessId;
	wTem = str.GetBuffer(str.GetLength());
	dwProcessId = _wtoi(wTem);
	KillThreadByTerminate(dwProcessId,dwCid);
	MessageBox(L"�����������");

    


}

// ��ͨɱ�߳�
void CMyProcessDlg::KillThreadByTerminate(DWORD dwProcessId, DWORD dwCid)
{
	//��������
	//�ֲ�����
	HINSTANCE Kernel32lib = LoadLibrary(L"kernel32.dll");
	typedef HANDLE (* PNOpenThread)(DWORD,BOOL,DWORD);//OpenThread
	typedef BOOL   (* PNTerminateThread)(
		HANDLE hThread,
		DWORD dwExitCode
		);
	PNTerminateThread MyTerminateThread;
	PNOpenThread MyOpenThread;

	//��������
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
    AdjustPurview();//����Ȩ��
	//��ָ���Ľ���
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS,FALSE,dwProcessId);
	if(hProcess == NULL)
	{
		OutputDebugString(_T("OpenProcess failed!"));
		return;
	}
	//����д��
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
	
	//����д��
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
	//�����߳�
    HANDLE hThread = NULL;
	hThread = CreateRemoteThread(hProcess,0,0,(LPTHREAD_START_ROUTINE)lpThread,lpParam,0,NULL);
    if (hThread == NULL)
    {
		return;
    }
	//�ȴ��߳̽���
	//������
	VirtualFreeEx(hProcess,lpThread,0,MEM_RELEASE);
	VirtualFreeEx(hProcess,lpParam,0,MEM_RELEASE);
	CloseHandle(hProcess);
}

VOID CMyProcessDlg::KillMyThreadFun(PKillThreadParam PParam)
{
	DWORD dwOpenThread;                 //OpenThread��ַ
	DWORD dwTerminateThread;            //TerminateThread��ַ
	DWORD dwCid;                        //��Ҫж�ص��߳�Cid
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

//ж��ģ��
void CMyProcessDlg::OnKillModule()
{
	// TODO: �ڴ���������������
	CString csPid,csName;
	csPid = m_ProcessId;
	//���ID
	WCHAR* wTem = csPid.GetBuffer(csPid.GetLength());
	DWORD dwPid = _wtoi(wTem);

	//���ģ����
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
    //ж��ģ��
	CMySystem::FreeModuleByPid(szModuleName,dwPid);

    
}

//�鿴�����ڴ���Ϣ
void CMyProcessDlg::OnLookMenInfo()
{
	// TODO: �ڴ���������������
	CMyMemInfo dlg;
	dlg.m_ProcId = m_ProcessId;
	dlg.DoModal();
}

//�鿴PE��Ϣ
void CMyProcessDlg::OnLookPEInfo()
{
	// TODO: �ڴ���������������
	CString Path;
	m_ProcessModulesInfo.GetText(0,Path);
	CFEInfoDlg dlg;
	dlg.m_FilePath = Path;
	dlg.DoModal();
}
