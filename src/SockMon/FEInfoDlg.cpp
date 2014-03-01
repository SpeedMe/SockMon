// FEInfoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "SockMon.h"
#include "FEInfoDlg.h"


// CFEInfoDlg 对话框

IMPLEMENT_DYNAMIC(CFEInfoDlg, CDialog)

CFEInfoDlg::CFEInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFEInfoDlg::IDD, pParent)
{

}

CFEInfoDlg::~CFEInfoDlg()
{
}

void CFEInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE1, m_ResDirectory);

	this->m_ResDirectory.SetTextColor(RGB(0,255,0));
	this->m_ResDirectory.SetBkColor(RGB(41,36,33));

	//树形控件初始化，完全清空
	this->m_ResDirectory.DeleteAllItems();

	this->isDumpValid = FALSE;
	this->lpDumpResStart = NULL;
	this->dwResSize = 0;

	GetInfo();
//	return TRUE;


}


BEGIN_MESSAGE_MAP(CFEInfoDlg, CDialog)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE1, &CFEInfoDlg::OnTvnSelchangedTree1)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDBLCLK()
END_MESSAGE_MAP()


// CFEInfoDlg 消息处理程序

void CFEInfoDlg::UpdateTreeCtrl(void)
{
	CString TempStr;
	list<RES_ITEM>::iterator p;
	HTREEITEM pRoot,pSub;
	DWORD i,j,k,dwTypeNum = this->PEInfo.dwNamedEntry + this->PEInfo.dwIdEntry;
	for (i = 0 ; i < dwTypeNum; i++)
	{
		//判断资源类型为ID/字符串
		if (this->PEInfo.ResDirectory[i].dwNameOrId & 0x80000000)
		{
			LPTSTR lpStr = (LPTSTR)((this->PEInfo.ResDirectory[i].dwNameOrId & 0x7fffffff)+(DWORD)this->PEInfo.lpResBase);
            WORD nNum = *((short int*)lpStr); //所指的首字符串
			TempStr.Format(TEXT("%s"),(LPTSTR)((DWORD)lpStr+2));
			if(TempStr.GetLength() > nNum)
				TempStr.SetAt(nNum,'\0');
		}
		else
		{
			DWORD dwResType = this->PEInfo.ResDirectory[i].dwNameOrId;
			if(dwResType < MAX_RES_TYPES && this->szResourceType[dwResType])
				TempStr.Format(TEXT("%s"),this->szResourceType[dwResType]);
			else
				TempStr.Format(TEXT("%s"),this->PEInfo.ResDirectory[i].dwNameOrId);
		}

		pRoot = this->m_ResDirectory.InsertItem(TempStr.GetBuffer(TempStr.GetLength()));

		for (p = this->PEInfo.ResList[i].begin(); p != this->PEInfo.ResList[i].end(); p++)
		{
			if (p->dwNameOrId & 0x80000000)
			{
				LPTSTR lpStr = (LPTSTR)((p->dwNameOrId & 0x7777777)+(DWORD)this->PEInfo.lpResBase);
				WORD nNum = *((short int*)lpStr);
				TempStr.Format(TEXT("%s"),(LPTSTR)((DWORD)lpStr+2));
				if(TempStr.GetLength() > nNum)
					TempStr.SetAt(nNum,'\0');
			}
			else
				TempStr.Format(TEXT("%d"),p->dwNameOrId);

			pSub = this->m_ResDirectory.InsertItem(TempStr.GetBuffer(TempStr.GetLength()),pRoot);
		}
	}

	this->m_RootNameEntryNum.Format(TEXT("%04X"),this->PEInfo.dwNamedEntry);
	this->m_RootIdEntryNum.Format(TEXT("%04X"),this->PEInfo.dwIdEntry);
}

// 取得当前选定项在其兄弟节点的索引
int CFEInfoDlg::GetIndexOfTreeCtrl(HTREEITEM hTreeItem)
{
	int nCount = 0;
	HTREEITEM hItem = hTreeItem;
	while ((hItem = this->m_ResDirectory.GetNextItem(hItem,TVGN_PREVIOUS)) != NULL)
	{
		nCount++;
	}
	return nCount + 1;
}

void CFEInfoDlg::OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
    HTREEITEM hParent = NULL, hChild = NULL, hCurSel = NULL, hTemp = NULL;
	hCurSel = this->m_ResDirectory.GetNextItem(TVI_ROOT,TVGN_CARET);

	hParent = this->m_ResDirectory.GetParentItem(hCurSel);
	hChild = this->m_ResDirectory.GetChildItem(hCurSel);

	RES_ITEM CurResItem;
	int nRootIndex,nSubIndex;
	if (hParent == NULL)
	{
		nRootIndex = this->GetIndexOfTreeCtrl(hCurSel);
		this->m_DataOffset = this->m_DataRVA = this->m_DataSize = "";
	}
	else
	{
		nRootIndex = this->GetIndexOfTreeCtrl(hParent);
		nSubIndex = this->GetIndexOfTreeCtrl(hCurSel);

		this->PEInfo.GetResItemByIndex(nRootIndex,nSubIndex,&CurResItem);
		this->m_DataRVA.Format(TEXT("%08X"),CurResItem.dwDataOffset);
		this->m_DataSize.Format(TEXT("%08X"),CurResItem.dwDataSize);

		DWORD dwFileOffset = (DWORD)ImageRvaToVa(this->PEInfo.pNtHeader,NULL,CurResItem.dwDataOffset,NULL);
		this->m_DataOffset.Format(TEXT("%08X"),dwFileOffset);

		this->isDumpValid = TRUE;
		this->lpDumpResStart = (LPVOID)((DWORD)this->PEInfo.MapItem.lpMapAddr+dwFileOffset);
		this->dwResSize = CurResItem.dwDataSize;
	}
	this->m_SubNameEntryNum.Format(TEXT("%04X"),this->PEInfo.ResDirectory[nRootIndex-1].wNumOfNamedEntry);
	this->m_SubIdEntryNum.Format(TEXT("%04X"),this->PEInfo.ResDirectory[nRootIndex-1].wNumOfIdEntry);
	*pResult = 0;
}

HBRUSH CFEInfoDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	// TODO:  在此更改 DC 的任何属性
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetBkColor(RGB(41,36,33));
	pDC->SetTextColor(RGB(0,255,0));
	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return CreateSolidBrush(RGB(41,36,33));
}

void CFEInfoDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
    OnCancel();
	CDialog::OnLButtonDblClk(nFlags, point);
}

VOID CFEInfoDlg::GetInfo(void)
{
	if(this->PEInfo.isReady == TRUE)
		this->PEInfo.DeleteMap();

	this->m_ResDirectory.DeleteAllItems();
	//获得文件路径
	if(!this->GetResourceInfo(this->m_FilePath.GetBuffer(this->m_FilePath.GetLength())))
		return;
	this->UpdateTreeCtrl();

	//设置附加信息
	DWORD dwVirtualAddr   = this->PEInfo.pOptionalHeader->DataDirectory[2].VirtualAddress;
    DWORD dwSize          = this->PEInfo.pOptionalHeader->DataDirectory[2].Size;
    this->m_ResInfo.Format(TEXT("资源信息：FileOffset=0X%08X,DataSize=0X08X"),\
		(DWORD)this->PEInfo.lpResBase - (DWORD)this->PEInfo.MapItem.lpMapAddr,dwVirtualAddr,dwSize);
}

// 取得目标文件的相关信息
BOOL CFEInfoDlg::GetResourceInfo(LPTSTR lpFilePath)
{
	if (this->PEInfo.CreateMap(lpFilePath) == FALSE)
	{
		this->MessageBox(TEXT("Please Ensure This File is Not been using!"));
		return FALSE;
	}
	this->PEInfo.GetResInfo();
	return TRUE;
}
