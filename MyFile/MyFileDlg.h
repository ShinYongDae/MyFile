
// MyFileDlg.h : ��� ����
//

#pragma once

#include "SimpleFile.h"


// CMyFileDlg ��ȭ ����
class CMyFileDlg : public CDialogEx
{
	CSimpleFile *m_pFile;

	CString m_sPw;

// �����Դϴ�.
public:
	CMyFileDlg(CWnd* pParent = NULL);	// ǥ�� �������Դϴ�.
	~CMyFileDlg();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MYFILE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �����Դϴ�.


// �����Դϴ�.
protected:
	HICON m_hIcon;

	// ������ �޽��� �� �Լ�
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
