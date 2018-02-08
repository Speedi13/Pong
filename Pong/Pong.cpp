#include <Windows.h>
#include <stdio.h>
#include "Settings.h"

HANDLE hStdOutHandle, hStdInHandle;

#define GameFieldSize_X 80
#define GameFieldSize_Y 25

#define BallSize 1

CHAR_INFO consoleBuffer[GameFieldSize_X * GameFieldSize_Y];

//so the directx sdk is not needed
typedef struct D3DXVECTOR2
{
	float x;
	float y;
};


COORD Bar1 = {0,0};
COORD Bar2 = {GameFieldSize_X-1,0};

COORD BallPos = {0,0};
D3DXVECTOR2 floatBallPos = {GameFieldSize_X/2,GameFieldSize_Y/2};
D3DXVECTOR2 BallVelocity = {0,0};


void UpdateBar( CHAR_INFO* consoleBuffer, COORD NewPos , COORD* Bar, SHORT XOffset = 0)
{
	if ( (NewPos.Y+PongSettings::BarYLengh) >= GameFieldSize_Y || NewPos.Y < 0 )
		return;

	for (int i = 0; i < PongSettings::BarYLengh; i++)
	{
		DWORD Y_Pos = (Bar->Y + i);
		DWORD dwBufferOffset = (XOffset) + Y_Pos * GameFieldSize_X;
		consoleBuffer[ dwBufferOffset ].Char.AsciiChar = ' ';
	}

	for (int i2 = 0; i2 < PongSettings::BarYLengh; i2++)
	{
		DWORD Y_Pos2 = (NewPos.Y + i2);
		DWORD dwBufferOffset = (XOffset) + Y_Pos2 * GameFieldSize_X;
		consoleBuffer[ dwBufferOffset ].Char.AsciiChar = '|';
	}
	Bar->Y = NewPos.Y;
}

bool IsColludedWithPaddel( COORD* Bar, COORD* NextBallPos )
{
	if (	( Bar->X == NextBallPos->X ) 
		||	( Bar->X != 0 && NextBallPos->X >= GameFieldSize_X )
		||	( Bar->X == 0 && NextBallPos->X < 0 ) )
	{
		if ( Bar->Y <= NextBallPos->Y && ((Bar->Y + PongSettings::BarYLengh) > NextBallPos->Y) )
			return true;
	}
	return false;
}

#include <math.h>
#define MATH_PI 3.14159265
void UpdateBall()
{
	//so if the speed is below 1
	floatBallPos.x += BallVelocity.x;
	floatBallPos.y += BallVelocity.y;

	COORD OldBallPos = BallPos;

	BallPos.X = floatBallPos.x;
	BallPos.Y = floatBallPos.y;

	if ( OldBallPos.X == BallPos.X && OldBallPos.Y == BallPos.Y ) //no update needed
		return;

	bool IsColluded = false;
	bool IsBar1 = IsColludedWithPaddel( &Bar1, &BallPos ) == true;
	if ( IsBar1 == true || IsColludedWithPaddel( &Bar2, &BallPos ) == true ) IsColluded = true;

	if (IsColluded == true)
	{
		COORD Paddle = {0,0};
		if (IsBar1)
			Paddle = Bar1;
		else
			Paddle = Bar2;

		//credits to https://gamedev.stackexchange.com/a/125281

		float offset = (float)((float)BallPos.Y+BallSize - (float)Paddle.Y) / (float)(PongSettings::BarYLengh + BallSize);
		float phi = 0.25 * MATH_PI * (2 * offset - 1);
		BallVelocity.y = PongSettings::BallSpeed * sin(phi);
		BallVelocity.x *= -1;
		floatBallPos.x = BallPos.X = (OldBallPos.X + BallVelocity.x);
	}
	else
	if ( BallPos.X >= (GameFieldSize_X) || BallPos.X < 0 )
	{
		SHORT XSpawnOffset = GameFieldSize_X/3;
		SHORT YSpawnOffset = GameFieldSize_Y/2;
		float SpawnSpeed = PongSettings::BallSpeed;

		if ( BallPos.X < 0 )
		{
			XSpawnOffset = GameFieldSize_X - XSpawnOffset;
			SpawnSpeed = SpawnSpeed * -1;

			YSpawnOffset = Bar1.Y + PongSettings::BarYLengh/2;
		}
		else
			YSpawnOffset = Bar2.Y + PongSettings::BarYLengh/2;

		BallVelocity.x = SpawnSpeed;
		BallVelocity.y = 0;
		floatBallPos.x = BallPos.X = XSpawnOffset;
		floatBallPos.y = BallPos.Y = YSpawnOffset;
	}

	if ( BallPos.Y >= GameFieldSize_Y || BallPos.Y < 0 )
	{
		BallVelocity.y *= -1;
		floatBallPos.y = BallPos.Y = OldBallPos.Y + BallVelocity.y;
	}

	DWORD dwBufferOffset = (OldBallPos.X) + OldBallPos.Y * GameFieldSize_X;
	consoleBuffer[ dwBufferOffset ].Char.AsciiChar = ' ';


	dwBufferOffset = (BallPos.X) + BallPos.Y * GameFieldSize_X;
	consoleBuffer[ dwBufferOffset ].Char.AsciiChar = '*';
}

void UpdateBot()
{
	float NewYPos = (float)BallPos.Y - ( ( PongSettings::BarYLengh - BallSize )/2 );
	if ( NewYPos < 0 )
		 NewYPos = 0;
	if ( NewYPos >= ( GameFieldSize_X-1 - PongSettings::BarYLengh ) )
		 NewYPos = ( GameFieldSize_X-1 - PongSettings::BarYLengh );

	float flCurrent = BallPos.Y;

	float flDelta = NewYPos - flCurrent;

	NewYPos = flCurrent + flDelta;

	COORD NewPos;
	NewPos.Y = NewYPos;
	UpdateBar( consoleBuffer, NewPos, &Bar2, GameFieldSize_X-1 );
}

COORD GameFieldSize = {GameFieldSize_X,GameFieldSize_Y};
COORD characterPos = {0,0};
SMALL_RECT writeArea = {0,0,GameFieldSize.X-1,GameFieldSize.Y-1};

DWORD __stdcall GameThread( void* a1 )
{
	UNREFERENCED_PARAMETER( a1 );

	BallVelocity.x = PongSettings::BallSpeed;

	floatBallPos.x = GameFieldSize_X/2;
	floatBallPos.y = GameFieldSize_Y/2;

	while (true)
	{
		Sleep( 10 );

		UpdateBall();

		if ( PongSettings::IsBotEnabled == TRUE )
			UpdateBot();

		//Frame update
		WriteConsoleOutputA(hStdOutHandle, consoleBuffer, GameFieldSize, characterPos, &writeArea);
	}
}

int main()
{
	SetConsoleTitleA("Basic-Pong game from Speedi13");

	if ( PongSettings::DoesSettingsFileExist() != true )
		PongSettings::SaveSettings();
	else
		PongSettings::LoadSettings();

	hStdOutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	hStdInHandle = GetStdHandle(STD_INPUT_HANDLE);

	for (int i = 0; i < GameFieldSize.X * GameFieldSize.Y; i++) 
	{
		consoleBuffer[i].Char.AsciiChar = ' ';
		consoleBuffer[i].Attributes =  FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
	}

	UpdateBar( consoleBuffer, characterPos, &Bar1 );
	UpdateBar( consoleBuffer, characterPos, &Bar2, GameFieldSize_X-1 );

	CreateThread( NULL, NULL, (LPTHREAD_START_ROUTINE)GameThread, NULL, NULL, NULL );

	while (true)
	{
		DWORD dwNumberOfEvents = 0;
		GetNumberOfConsoleInputEvents(hStdInHandle, &dwNumberOfEvents);
		if ( dwNumberOfEvents == NULL )
			continue;

		INPUT_RECORD* InputEventBuffer = (INPUT_RECORD*)malloc( dwNumberOfEvents * sizeof(INPUT_RECORD) );
		DWORD dwEventsRead = 0;
		ReadConsoleInputA(hStdInHandle, InputEventBuffer, dwNumberOfEvents, &dwEventsRead);

		for ( DWORD i = 0; i < dwEventsRead; i++ ) 
		{
			if ( InputEventBuffer[i].EventType == MOUSE_EVENT && 
				(InputEventBuffer[i].Event.MouseEvent.dwEventFlags & MOUSE_MOVED) )
			{
				UpdateBar( consoleBuffer, InputEventBuffer[i].Event.MouseEvent.dwMousePosition, &Bar1 );
			}
			else
				if ( InputEventBuffer[i].EventType == KEY_EVENT )
				{
					WORD wVirtualKeyCode = InputEventBuffer[i].Event.KeyEvent.wVirtualKeyCode;

					if (wVirtualKeyCode == VK_UP || wVirtualKeyCode == VK_DOWN )
					{
						COORD NewPos = Bar2;
						if ( wVirtualKeyCode == VK_UP )
							NewPos.Y -= PongSettings::BarSpeed;

						if ( wVirtualKeyCode == VK_DOWN )
							NewPos.Y += PongSettings::BarSpeed;

						UpdateBar( consoleBuffer, NewPos, &Bar2, GameFieldSize_X-1 );
					}
					else
					//You can find the key-codes @http://msdn.microsoft.com/en-us/library/windows/desktop/dd375731(v=vs.85).aspx
					if (wVirtualKeyCode == 0x57 /*W key*/ || wVirtualKeyCode == 0x53 /*S key*/ )
					{
						COORD NewPos = Bar1;
						if ( wVirtualKeyCode == 0x57 /*W key*/ )
							NewPos.Y -= PongSettings::BarSpeed;

						if ( wVirtualKeyCode == 0x53 /*S key*/ )
							NewPos.Y += PongSettings::BarSpeed;

						UpdateBar( consoleBuffer, NewPos, &Bar1 );
					}
				}

		}
		free( InputEventBuffer );
	}
	return 0;
}

