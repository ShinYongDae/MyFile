// SimpleFile.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "MyFile.h"
#include "SimpleFile.h"


// CSimpleFile

IMPLEMENT_DYNAMIC(CSimpleFile, CWnd)

CSimpleFile::CSimpleFile(CString sPath, CWnd* pParent/*=NULL*/)
{
	m_pParent = pParent;
	if (pParent)
		m_hParent = pParent->GetSafeHwnd();

	m_sPath = sPath;
	m_bPrivateProfileStringUseBuffer = FALSE;
	m_bOnReading = FALSE;
	m_bOnWriting = FALSE;
	m_bFlush = FALSE;

	GetStatus();
	CreateWndForm(WS_CHILD | WS_OVERLAPPED);
	ThreadStart();
}

CSimpleFile::~CSimpleFile()
{
	ThreadStop();
	Sleep(30);
	t1.join();
}


BEGIN_MESSAGE_MAP(CSimpleFile, CWnd)
	//ON_MESSAGE(WM_THREAD_END, wmThreadEnd)
END_MESSAGE_MAP()



// CSimpleFile 메시지 처리기입니다.
BOOL CSimpleFile::CreateWndForm(DWORD dwStyle)
{
	if (!Create(NULL, _T("SimpleFile"), dwStyle, CRect(0, 0, 0, 0), m_pParent, (UINT)this))
	{
		AfxMessageBox(_T("CSimpleServer::Create() Failed!!!"));
		return FALSE;
	}

	return TRUE;
}

void CSimpleFile::thrdFile(const LPVOID lpContext)
{
	CSimpleFile* pSimpleFile = reinterpret_cast<CSimpleFile*>(lpContext);

	while (pSimpleFile->ThreadIsAlive())
	{
		if (!pSimpleFile->FileProc())
			break;
	}

	pSimpleFile->ThreadEnd();
	//HWND hWnd = pSimpleFile->GetSafeHwnd();
	//::SendMessage(hWnd, WM_THREAD_END, (WPARAM)0, (LPARAM)0);
}

BOOL CSimpleFile::FileProc()
{
	if (m_bFlush)
	{
		PrivateProfileStringFlush();
		m_bFlush = FALSE;
	}
	Sleep(100);
	return TRUE;
}

void CSimpleFile::ThreadStart()
{
	m_bThreadStateEnd = FALSE;
	m_bThreadAlive = TRUE;
	t1 = std::thread(thrdFile, this);
}

void CSimpleFile::ThreadStop()
{
	m_bThreadAlive = FALSE;
	MSG message;
	const DWORD dwTimeOut = 1000 * 60 * 3; // 3 Minute
	DWORD dwStartTick = GetTickCount();
	Sleep(30);
	while (!m_bThreadStateEnd)
	{
		if (GetTickCount() >= (dwStartTick + dwTimeOut))
		{
			AfxMessageBox(_T("WaitUntilThreadEnd() Time Out!!!", NULL, MB_OK | MB_ICONSTOP));
			return;
		}
		if (::PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&message);
			::DispatchMessage(&message);
		}
		Sleep(30);
	}
}

//LRESULT CSimpleFile::wmThreadEnd(WPARAM wParam, LPARAM lParam)
//{
//	m_bThreadEndState = TRUE;
//	return (LRESULT)1;
//}

void CSimpleFile::ThreadEnd()
{
	//HWND hWnd = this->GetSafeHwnd();
	//if(!hWnd)
		m_bThreadStateEnd = TRUE;
}

BOOL CSimpleFile::ThreadIsAlive()
{
	return m_bThreadAlive;
}

BOOL CSimpleFile::Read(CString &sData)
{
	CFile file;
	BOOL bOk = file.Open(m_sPath, CFile::modeRead, NULL);
	if (!bOk)
	{
		AfxMessageBox(_T("Error-Read()"));
		return FALSE;
	}
	m_bOnReading = TRUE;
	int nLen = file.GetLength();
	char* pData = new char[nLen + 1]; // for '\0'
	file.Read((void*)pData, nLen * sizeof(char));
	file.Close();
	m_bOnReading = FALSE;
	pData[nLen] = _T('\0');
	sData = CharToString(pData);
	delete pData;
	return TRUE;
}

BOOL CSimpleFile::Write(CString sData)
{
	CFile file;
	BOOL bOk = FALSE;

	if(m_bExist)
		bOk = file.Open(m_sPath, CFile::modeWrite, NULL);
	else
		bOk = file.Open(m_sPath, CFile::modeCreate | CFile::modeWrite, NULL);

	if (!bOk)
	{
		AfxMessageBox(_T("Error-Write()"));
		return FALSE;
	}
	m_bOnWriting = TRUE;
	int nLen = sData.GetLength();
	char* pData = new char[nLen + 1]; // for '\0'
	StringToChar(sData, pData);
	file.Write((void*)pData, nLen * sizeof(char));
	file.Close();
	m_bOnWriting = FALSE;

	if (!m_bExist) GetStatus();
	m_bExist = TRUE;
	delete pData;
	return TRUE;
}

BOOL CSimpleFile::GetStatus()					// 리턴값 : 파일 유무
{
	CFileStatus status;
	m_bExist = CFile::GetStatus(m_sPath, status);
	if (!m_bExist)
		return FALSE;
	m_stStatus = status;

	int nLen = status.m_size;													// 파일의 크기 (Byte수)
	CString sMake, sModify, sAccess;
	sMake.Format(_T("%s"), status.m_ctime.Format("%H:%M:%S, %B, %d, %Y"));		// 파일이 만들어진 날짜 정보
	sModify.Format(_T("%s"), status.m_mtime.Format("%H:%M:%S, %B, %d, %Y"));	// 파일이 수정된 날짜 정보
	sAccess.Format(_T("%s"), status.m_atime.Format("%H:%M:%S, %B, %d, %Y"));	// 파일이 액세스된 날짜 정보

	m_bNormal = status.m_attribute & 0x00;
	m_bReadOnly = status.m_attribute & 0x01;
	m_bHidden = status.m_attribute & 0x02;
	m_bSystem = status.m_attribute & 0x04;
	m_bVolume = status.m_attribute & 0x08;
	m_bDirectory = status.m_attribute & 0x10;
	m_bArchive = status.m_attribute & 0x20;

	return TRUE;
}

BOOL CSimpleFile::Update()						// 리턴값 : 파일 유무
{
	GetStatus();
	return m_bExist;
}

BOOL CSimpleFile::PrivateProfileStringOpen()	// 리턴값 : TRUE (Be Ready), FALSE (Not Ready) 
{
	CFileFind findfile;
	m_bExist = findfile.FindFile(m_sPath);
	int nTotal = m_arProfileString.GetCount();
	if (!m_bPrivateProfileStringUseBuffer && m_bExist && nTotal < 1)
	{
		m_bPrivateProfileStringUseBuffer = TRUE;
		return TRUE;
	}

	return FALSE;
}

DWORD CSimpleFile::PrivateProfileStringGet(LPCTSTR lpAppName, LPCTSTR lpKeyName, CString &lpReturnedString)
{
	DWORD dwSize = 0; // null 문자를 포함하지 않은 문자 수.

	if (!m_bExist)
	{
		AfxMessageBox(_T("CSimpleFile::PrivateProfileStringGet() Failed!!!"));
		return dwSize;
	}

	if (m_bPrivateProfileStringUseBuffer)
	{
		stProfileString stData(ProfileString::Get, lpAppName, lpKeyName, &lpReturnedString); // lpReturnedString : Must be member variable.
		m_arProfileString.Add(stData);
	}
	else
	{
		if (!m_bFlush)
		{
			TCHAR szData[MAX_PATH];
			dwSize = ::GetPrivateProfileString(lpAppName, lpKeyName, NULL, szData, sizeof(szData), m_sPath);
			if (dwSize > 0)
				lpReturnedString = CString(szData);
			else
				lpReturnedString = _T("");
		}
	}

	return dwSize;
}

BOOL CSimpleFile::PrivateProfileStringSet(LPCTSTR lpAppName, LPCTSTR lpKeyName, CString lpString)
{
	BOOL bOk = FALSE;

	if (m_bPrivateProfileStringUseBuffer)
	{
		stProfileString stData(ProfileString::Write, lpAppName, lpKeyName, lpString);
		m_arProfileString.Add(stData);
	}
	else
	{
		if(!m_bFlush)
			bOk = ::WritePrivateProfileString(lpAppName, lpKeyName, lpString, m_sPath);
	}

	if (bOk)
		m_bExist = TRUE;

	return bOk;
}

BOOL CSimpleFile::PrivateProfileStringClose()
{
	m_bPrivateProfileStringUseBuffer = FALSE;
	m_bFlush = TRUE;
	return TRUE;
}

BOOL CSimpleFile::PrivateProfileStringFlush()
{
	CString sData;
	stProfileString stProfile;
	int i = 0;
	int nTotal = m_arProfileString.GetCount();
	if (nTotal < 1)
		return FALSE;

	for (i = 0; i < nTotal; i++)
	{
		stProfile = m_arProfileString.GetAt(i);

		if (stProfile.nType == ProfileString::Get)
		{
			CString *pData = stProfile.pData;
			TCHAR szData[MAX_PATH];
			DWORD dwSize = ::GetPrivateProfileString(stProfile.sMenu, stProfile.sItem, NULL, szData, sizeof(szData), m_sPath);
			if (dwSize > 0)
				(*pData).Format(_T("%s"), CString(szData));
			else
				(*pData) = _T("");
		}
		else if (stProfile.nType == ProfileString::Write)
		{
			::WritePrivateProfileString(stProfile.sMenu, stProfile.sItem, stProfile.sData, m_sPath);
		}
	}

	m_arProfileString.RemoveAll();
	return TRUE;
}

BOOL CSimpleFile::IsOnReading()
{
	return m_bOnReading;
}

BOOL CSimpleFile::IsOnWriting()
{
	return m_bOnWriting;
}

BOOL CSimpleFile::IsFlushed()
{
	return (!m_bFlush);
}

int CSimpleFile::CheckPath(CString sPath)
{
	DWORD dwAttr = GetFileAttributes(sPath);
	if (dwAttr == 0xffffffff)
	{
		if (GetLastError() == ERROR_FILE_NOT_FOUND || GetLastError() == ERROR_PATH_NOT_FOUND)
			return Path::NotFound;
		return Path::Error;
	}
	if (dwAttr & FILE_ATTRIBUTE_DIRECTORY) return Path::IsFolder;
	return Path::IsFile;
}

BOOL CSimpleFile::DirectoryIsExist(LPCTSTR szPath)
{
	DWORD dwAttrib = GetFileAttributes(szPath);
	return (dwAttrib != INVALID_FILE_ATTRIBUTES && (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

BOOL CSimpleFile::DirectoryCreate(LPCTSTR szPath)
{
	CString sMsg;
	if (!CreateDirectory(szPath, NULL))
	{
		sMsg.Format(_T("Can not Create Directory.\r\n%s"), CString(szPath));
		AfxMessageBox(sMsg);
		return FALSE;
	}
	return TRUE;
}

void CSimpleFile::DirectoryDeleteAllFiles(CString sPath)
{

	BOOL bExist;
	CFileFind findfile;

	if (CheckPath(sPath) == Path::IsFile)
	{
		DeleteFile(sPath);
		return;
	}

	if (sPath.Right(1) != "\\")
		sPath += "\\";

	sPath += "*.*";

	bExist = findfile.FindFile(sPath);

	while (bExist)
	{
		bExist = findfile.FindNextFile();
		if (findfile.IsDots())
			continue;
		if (findfile.IsDirectory())
		{
			sPath = findfile.GetFilePath();
			DirectoryDeleteAllFiles(sPath);
		}
		else
			DirectoryDeleteAllFiles(findfile.GetFilePath());
	}

	findfile.Close();
}

void CSimpleFile::StringToChar(CString str, char* szStr)  // char* returned must be deleted... 
{
	int nLen = str.GetLength();
	strcpy(szStr, CT2A(str));
	szStr[nLen] = _T('\0');
}

void CSimpleFile::StringToTChar(CString str, TCHAR* tszStr) // TCHAR* returned must be deleted... 
{
	int nLen = str.GetLength() + 1;
	memset(tszStr, 0x00, nLen * sizeof(TCHAR));
	_tcscpy(tszStr, str);
}

CString CSimpleFile::CharToString(char *szStr)
{
	CString strRet;

	int nLen = strlen(szStr) + sizeof(char);
	wchar_t *tszTemp = NULL;
	tszTemp = new WCHAR[nLen];

	MultiByteToWideChar(CP_ACP, 0, szStr, -1, tszTemp, nLen * sizeof(WCHAR));

	strRet.Format(_T("%s"), (CString)tszTemp);
	if (tszTemp)
	{
		delete[] tszTemp;
		tszTemp = NULL;
	}
	return strRet;
}

CString CSimpleFile::TCharToString(TCHAR *szStr)
{
	CString cStr;
	cStr.Format(_T("%s"), szStr);
	return cStr;
}

/*
CFile::Seek(LONGLONG lOff, UINT nFrom);		nFrom 에서 loff 로 포인터 이동
CFile::begin								Seek from the start of the file.
CFile::current								Seek from the current location of the file pointer.
CFile::end									Seek from the end of the file.
*/