
// MyFileDlg.h : 헤더 파일
//

#pragma once

#include "SimpleFile.h"


// CMyFileDlg 대화 상자
class CMyFileDlg : public CDialogEx
{
	CSimpleFile *m_pFile;

	CString m_sPw;

// 생성입니다.
public:
	CMyFileDlg(CWnd* pParent = NULL);	// 표준 생성자입니다.
	~CMyFileDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_MYFILE_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
