// FEInfoDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "SockMon.h"
#include "FEInfoDlg.h"


// CFEInfoDlg �Ի���

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

	//���οؼ���ʼ������ȫ���
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


// CFEInfoDlg ��Ϣ�������

void CFEInfoDlg::UpdateTreeCtrl(void)
{
	CString TempStr;
	list<RES_ITEM>::iterator p;
	HTREEITEM pRoot,pSub;
	DWORD i,j,k,dwTypeNum = this->PEInfo.dwNamedEntry + this->PEInfo.dwIdEntry;
	for (i = 0 ; i < dwTypeNum; i++)
	{
		//�ж���Դ����ΪID/�ַ���
		if (this->PEInfo.ResDirectory[i].dwNameOrId & 0x80000000)
		{
			LPTSTR lpStr = (LPTSTR)((this->PEInfo.ResDirectory[i].dwNameOrId & 0x7fffffff)+(DWORD)this->PEInfo.lpResBase);
            WORD nNum = *((short int*)lpStr); //��ָ�����ַ���
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

// ȡ�õ�ǰѡ���������ֵܽڵ������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO:  �ڴ˸��� DC ���κ�����
	pDC->SetBkMode(TRANSPARENT);
	pDC->SetBkColor(RGB(41,36,33));
	pDC->SetTextColor(RGB(0,255,0));
	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return CreateSolidBrush(RGB(41,36,33));
}

void CFEInfoDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    OnCancel();
	CDialog::OnLButtonDblClk(nFlags, point);
}

VOID CFEInfoDlg::GetInfo(void)
{
	if(this->PEInfo.isReady == TRUE)
		this->PEInfo.DeleteMap();

	this->m_ResDirectory.DeleteAllItems();
	//����ļ�·��
	if(!this->GetResourceInfo(this->m_FilePath.GetBuffer(this->m_FilePath.GetLength())))
		return;
	this->UpdateTreeCtrl();

	//���ø�����Ϣ
	DWORD dwVirtualAddr   = this->PEInfo.pOptionalHeader->DataDirectory[2].VirtualAddress;
    DWORD dwSize          = this->PEInfo.pOptionalHeader->DataDirectory[2].Size;
    this->m_ResInfo.Format(TEXT("��Դ��Ϣ��FileOffset=0X%08X,DataSize=0X08X"),\
		(DWORD)this->PEInfo.lpResBase - (DWORD)this->PEInfo.MapItem.lpMapAddr,dwVirtualAddr,dwSize);
}

// ȡ��Ŀ���ļ��������Ϣ
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
