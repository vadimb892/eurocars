#include "general.h"

void checkPathLength(TCHAR* path) {
	size_t path_length;
	StringCchLength(path, MAX_PATH, &path_length);
	assert(path_length < MAX_PATH);
}
/*
bool GetFileOwner(const std::wstring& filePath)
{
	SECURITY_INFORMATION secInfo = DACL_SECURITY_INFORMATION;
	PSECURITY_DESCRIPTOR pFileSD = NULL;  
	DWORD          cbFileSD = 0;   
	TCHAR* lpszFilePath = new TCHAR[MAX_PATH];
	BOOL fAPISuccess = 0;
	PSID pOwner;
	BOOL OwnerDefaulted;
	_tcscpy_s(lpszFilePath, MAX_PATH, filePath.c_str());
	fAPISuccess = GetFileSecurity(lpszFilePath, secInfo, pFileSD, 0, &cbFileSD);
	GetSecurityDescriptorOwner(&secInfo, &pOwner, &OwnerDefaulted);
		//return(FALSE);
	wchar_t AccountName[200], DomainName[200];
	DWORD AccountLength = sizeof(AccountName), DomainLength = sizeof(DomainName);
	SID_NAME_USE snu;
	LookupAccountSid(NULL, pOwner, AccountName, &AccountLength, DomainName, &DomainLength, &snu);
		//return(FALSE);

	return 0;
}*/

void _dir(std::vector<TCHAR*> pathVector) {
	WIN32_FIND_DATA ffd;
	LARGE_INTEGER filesize;
	TCHAR szDir[MAX_PATH];
	HANDLE hFind = INVALID_HANDLE_VALUE;
	unsigned int vectorSize = pathVector.size();

	for (int i = 0; i < vectorSize; i++) {
		checkPathLength(pathVector[i]);
		StringCchCopy(szDir, MAX_PATH, pathVector[i]);
		StringCchCat(szDir, MAX_PATH, TEXT("\\*"));
		hFind = FindFirstFile(szDir, &ffd);
		assert(INVALID_HANDLE_VALUE != hFind);
		std::wcout << "+--DIR[" << pathVector[i] << "]:\n";
		std::wcout << "| Name, Size, LastAccessTime\n";
		do
		{
			if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)  
			{
				_tprintf(TEXT("| %s   <DIR>\n"), ffd.cFileName);
			}
			else
			{
				filesize.LowPart = ffd.nFileSizeLow;
				filesize.HighPart = ffd.nFileSizeHigh;
				_tprintf(TEXT("| %s   %ld bytes   %ld s\n"), ffd.cFileName, filesize.QuadPart, ffd.ftLastAccessTime);
			}
			//GetFileOwner(pathVector[i]);
		} while (FindNextFile(hFind, &ffd) != 0);
		_tprintf(TEXT("+---------------------------------------\n"));
	}
	FindClose(hFind);
}

void task3(){
	TCHAR* tcharPath = new TCHAR[MAX_PATH];
	std::wstring strPath;
	std::vector<TCHAR*> pathVector;
	std::wcout << "(\n    ";//"+--\n| ";
	while (true) {
		std::wcin >> strPath;
		_tcscpy_s(tcharPath, MAX_PATH, strPath.c_str());
		pathVector.push_back(tcharPath);
		if (strPath[strPath.size()-1] != ',') {
			break;
		}
		std::wcout << "    ";//"| ";
	}
	std::wcout << ")\n";//"+--\n";
	_dir(pathVector);
}