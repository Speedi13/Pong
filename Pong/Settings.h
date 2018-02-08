#pragma once
#include <Windows.h>
#ifndef lol
#define lol


namespace PongSettings
{
	extern BYTE BarYLengh ;//= 5;
	extern BYTE BarSpeed ;//= 1;

	extern BOOL IsBotEnabled ;//= FALSE;

	extern float BallSpeed ;//= 0.6;
	//static float StartBallVelocity = BallSpeed;

	bool DoesSettingsFileExist();

	void LoadSettings();
	void SaveSettings();
};
#endif