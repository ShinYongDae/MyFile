
// MyFile.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CMyFileApp:
// �� Ŭ������ ������ ���ؼ��� MyFile.cpp�� �����Ͻʽÿ�.
//

class CMyFileApp : public CWinApp
{
public:
	CMyFileApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CMyFileApp theApp;