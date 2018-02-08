#include "Settings.h"
#include <stdio.h>
#include <io.h>

namespace PongSettings
{
	BYTE BarYLengh = 5;
	BYTE BarSpeed = 1;

	BOOL IsBotEnabled = FALSE;

	float BallSpeed = 0.6;
};

#pragma region IniAccessFunctionsWrappers

#define WcharToChar( wcBuffer, cBuffer ) for (int i = 0; i < wcslen(wcBuffer)+1; i++) cBuffer[ i ] = wcBuffer[ i ];
#define CharToWchar( cBuffer, wcBuffer ) for (int i = 0; i < strlen(cBuffer)+1; i++) wcBuffer[ i ] = cBuffer[ i ];

float IniReadFloat( LPCWSTR lpAppName, LPCWSTR lpKeyName, LPCWSTR lpDefault, LPCWSTR lpFileName )
{
	wchar_t wcBuffer[10] = {0};
	char cBuffer[10] = {0};
	GetPrivateProfileStringW( lpAppName, lpKeyName, lpDefault, wcBuffer, 10, L".\\PongSettings.ini" );

	WcharToChar( wcBuffer, cBuffer );

	return atof( cBuffer );
}

void IniWriteFloat( LPCWSTR lpAppName, LPCWSTR lpKeyName, float flValue, LPCWSTR lpFileName )
{
	wchar_t wcBuffer[10] = {0};
	char cBuffer[10] = {0};
	sprintf_s( cBuffer, "%f", flValue );

	CharToWchar( cBuffer, wcBuffer );

	WritePrivateProfileStringW( lpAppName, lpKeyName, wcBuffer, lpFileName );
}

void IniWriteInt( LPCWSTR lpAppName, LPCWSTR lpKeyName, int iValue, LPCWSTR lpFileName )
{
	wchar_t wcBuffer[10] = {0};
	char cBuffer[10] = {0};
	sprintf_s( cBuffer, "%i", iValue );

	CharToWchar( cBuffer, wcBuffer );

	WritePrivateProfileStringW( lpAppName, lpKeyName, wcBuffer, lpFileName );
}
#pragma endregion IniAccessFunctionsWrappers

bool PongSettings::DoesSettingsFileExist()
{
	return _access_s(".\\PongSettings.ini",0) == 0;
}

void PongSettings::LoadSettings()
{
	PongSettings::BarYLengh = (BYTE)GetPrivateProfileIntW(L"Integers", L"BarYLengh", (INT)PongSettings::BarYLengh, L".\\PongSettings.ini");
	PongSettings::BarSpeed = (BYTE)GetPrivateProfileIntW(L"Integers", L"BarSpeed", (INT)PongSettings::BarSpeed, L".\\PongSettings.ini");

	PongSettings::IsBotEnabled = (BOOL)GetPrivateProfileIntW(L"Integers", L"IsBotEnabled", (INT)PongSettings::IsBotEnabled, L".\\PongSettings.ini");

	PongSettings::BallSpeed = IniReadFloat( L"Floats", L"BallSpeed", L"0.5", L".\\PongSettings.ini" );
}

void PongSettings::SaveSettings()
{
	IniWriteInt( L"Integers", L"BarYLengh", (INT)PongSettings::BarYLengh, L".\\PongSettings.ini" );
	IniWriteInt( L"Integers", L"BarSpeed", (INT)PongSettings::BarSpeed, L".\\PongSettings.ini" );

	IniWriteInt( L"Integers", L"IsBotEnabled", (INT)PongSettings::IsBotEnabled, L".\\PongSettings.ini" );

	IniWriteFloat( L"Floats", L"BallSpeed", PongSettings::BallSpeed, L".\\PongSettings.ini" );
}