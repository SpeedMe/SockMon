// LSP.h : LSP DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������





// CLSPApp
// �йش���ʵ�ֵ���Ϣ������� LSP.cpp
//

class CLSPApp : public CWinApp
{
public:
	CLSPApp();

// ��д
public:
	virtual BOOL InitInstance ();
	virtual BOOL ExitInstance () ;

	DECLARE_MESSAGE_MAP()
};
