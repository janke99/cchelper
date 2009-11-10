#include <direct.h>
#include <stdlib.h>
#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include "log.h"
#include "appenv.h"

using namespace base;

TCHAR AppEnv::szAppPath[_MAX_PATH];
TCHAR AppEnv::szMediaPath[_MAX_PATH];
TCHAR AppEnv::szEngine[_MAX_PATH]; 
TCHAR AppEnv::szBackgoundBmp[_MAX_PATH];

BOOL AppEnv::bAutoPlay = FALSE;
int	AppEnv::nThinkTime ;

bool AppEnv::LoadEnv(TCHAR * szSettingFile)
{
	_tgetcwd(AppEnv::szAppPath, sizeof(AppEnv::szAppPath));
	GetPrivateProfileString(
		_T("AppEnv"), 
		_T("MediaPath"), 
		_T("../../media/"), 
		AppEnv::szMediaPath, 
		sizeof(AppEnv::szMediaPath), 
		szSettingFile
		);
	GetPrivateProfileString(
		_T("AppEnv"), 
		_T("ChessEngine"), 
		_T("eleeye.exe"), 
		AppEnv::szEngine, 
		sizeof(AppEnv::szEngine), 
		szSettingFile
		);

	GetPrivateProfileString(
		_T("AppEnv"), 
		_T("Background"), 
		_T("background.bmp"), 
		AppEnv::szBackgoundBmp, 
		sizeof(AppEnv::szBackgoundBmp), 
		szSettingFile
		);

	AppEnv::nThinkTime = GetPrivateProfileInt(_T("AppEnv"), _T("ThinkTime"),10,szSettingFile);

	return true;
}

TCHAR * AppEnv::GetMediaPath(TCHAR * szMediaFile)
{
	static TCHAR szBuf[_MAX_PATH];

	_sntprintf(szBuf, sizeof(szBuf), _T("%s%s"), AppEnv::szMediaPath, szMediaFile);

	return szBuf;
}

