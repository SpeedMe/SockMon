#pragma once
#include "afxcmn.h"
#include "PEInfo.h"

#define MAX_DEF_RES_TYPE 24
// CFEInfoDlg �Ի���

class CFEInfoDlg : public CDialog
{
	DECLARE_DYNAMIC(CFEInfoDlg)

public:
	CFEInfoDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CFEInfoDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CTreeCtrl m_ResDirectory;   // ���οؼ�
	CPEInfo PEInfo;
	BOOL isDumpValid;           // ��ʶ���οؼ��ĵ�ǰ��ѡ���Ƿ����DUMP�����Ƿ�ΪҶ�ӽڵ㣩
    LPVOID lpDumpResStart;      // ��ǰѡ������Դ��VA
    DWORD dwResSize;            // ��ǰѡ������Դ��SIZE
    CString szResourceType[MAX_DEF_RES_TYPE];     // ��Դ���͵�ID���ַ�����Ӧ��

	CString m_FilePath;
	CString m_RootNameEntryNum;
	CString m_RootIdEntryNum;
	CString m_SubNameEntryNum;
	CString m_SubIdEntryNum;
	CString m_DataOffset;
	CString m_DataRVA;
	CString m_DataSize;
	CString m_ResInfo;


	void UpdateTreeCtrl(void);
	// ȡ�õ�ǰѡ���������ֵܽڵ������
	int GetIndexOfTreeCtrl(HTREEITEM hTreeItem);
	afx_msg void OnTvnSelchangedTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	VOID GetInfo(void);
	// ȡ��Ŀ���ļ��������Ϣ
	BOOL GetResourceInfo(LPTSTR lpFilePath);
};
