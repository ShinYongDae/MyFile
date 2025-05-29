#pragma once
#include <thread>

#define WM_FILE_MSG		(WM_USER + 0x73FE)
#define WM_THREAD_END	(WM_USER + 0x73FF)	// 0x7FFF (Range of WM_USER is 0x400 ~ 0x7FFF)

#define PATH_WORKING_INFO		_T("C:\\R2RSet\\WorkingInfo-test.ini")

namespace ProfileString
{
	typedef enum Type { None = 0, Get = 1, Write = 2 };
}

namespace Path
{
	typedef enum Type { Error = -1, NotFound = 0, IsFile = 1, IsFolder = 2 };
}

typedef struct stProfileString
{
	int nType;						// 0 : None , 1 : Get , 2 : Write
	CString sMenu, sItem, sData;	// [sMenu] , sItem = sData
	CString *pData;

	stProfileString()
	{
		Init();
	}
	stProfileString(int _nType, CString _sMenu, CString _sItem, CString _sData)
	{
		nType = _nType;
		sMenu = _sMenu;
		sItem = _sItem;
		sData = _sData;
	}
	stProfileString(int _nType, CString _sMenu, CString _sItem, CString *_pData)
	{
		nType = _nType;
		sMenu = _sMenu;
		sItem = _sItem;
		pData = _pData;
	}

	void Init()
	{
		nType = 0;
		sMenu = _T("");
		sItem = _T("");
		sData = _T("");
		pData = NULL;
	}
};
typedef CArray<stProfileString, stProfileString> CArProfileString;

// CSimpleFile

class CSimpleFile : public CWnd
{
	DECLARE_DYNAMIC(CSimpleFile)

	CWnd* m_pParent;
	HWND m_hParent;
	BOOL m_bThreadAlive, m_bThreadStateEnd;
	std::thread t1;

	CString m_sPath;
	CFileStatus m_stStatus;
	BOOL m_bPrivateProfileStringUseBuffer;
	BOOL m_bExist, m_bOnReading, m_bOnWriting;
	BOOL m_bNormal, m_bReadOnly, m_bHidden, m_bSystem, m_bVolume, m_bDirectory, m_bArchive;
	CArProfileString  m_arProfileString;
	BOOL m_bFlush;

	BOOL CreateWndForm(DWORD dwStyle);

	void ThreadStart();
	void ThreadStop();

	void StringToChar(CString str, char* szStr);
	void StringToTChar(CString str, TCHAR* tszStr);
	CString CharToString(char *szStr);
	CString TCharToString(TCHAR *szStr);

	BOOL GetStatus();						// 리턴값 : 파일 유무
	BOOL PrivateProfileStringFlush();
	int CheckPath(CString sPath);

public:
	CSimpleFile(CString sPath, CWnd* pParent = NULL);
	virtual ~CSimpleFile();


	static void thrdFile(const LPVOID lpContext);
	BOOL Update();							// 리턴값 : 파일 유무
	BOOL Read(CString &sData);
	BOOL Write(CString sData);
	BOOL IsOnReading();
	BOOL IsOnWriting();
	BOOL IsFlushed();
	BOOL PrivateProfileStringOpen();		// Thread처리를 위한 버퍼를 준비 ( 버퍼 Reset() )	// 리턴값 : 파일 유무
	DWORD PrivateProfileStringGet(LPCTSTR lpAppName, LPCTSTR lpKeyName, CString &lpReturnedString);
	BOOL PrivateProfileStringSet(LPCTSTR lpAppName, LPCTSTR lpKeyName, CString lpString);
	BOOL PrivateProfileStringClose();		// Thread에서 버퍼를 가지고 파일작업 ( Opne() -> ~Get()/~Set() -> Close() )
	BOOL DirectoryIsExist(LPCTSTR szPath);
	BOOL DirectoryCreate(LPCTSTR szPath);
	void DirectoryDeleteAllFiles(CString sPath);

protected:
	//afx_msg LRESULT wmThreadEnd(WPARAM wParam, LPARAM lParam);
	void ThreadEnd();
	BOOL FileProc();
	BOOL ThreadIsAlive();

protected:
	DECLARE_MESSAGE_MAP()
};


