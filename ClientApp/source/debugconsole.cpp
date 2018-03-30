
#include	"stdafx.h"
#include	"debugconsole.h"



DebugConsole::DebugConsole()
{
	//AllocConsole함수는 자신의 윈도우 외에 콘솔 프로세스를 하나 더 작동시킴
	if( !AllocConsole() )
	{
		MessageBox( NULL, TEXT( "AllocConsole Error!!!" ), TEXT( "Error" ), MB_OK );
		m_isAllocated = FALSE;
	}
	else
	{
		//m_hConsoleOutput에 받아온 콘솔 아웃풋 핸들을 줌
		m_hConsoleOutput = GetStdHandle( STD_OUTPUT_HANDLE );
		m_isAllocated = TRUE;
	}
}

DebugConsole::~DebugConsole()
{
	//제거자가 호출되었을때 콘솔 프로세스를 제거해주어야 됨
	if( !FreeConsole() )
	{
		MessageBox( NULL, TEXT( "FreeConsole Error!!!" ), TEXT( "Error" ), MB_OK );
	}
}

void DebugConsole::Output( char * fmt, ... )
{
	va_list					argptr;
	char					cBuf[512];
	int						iCnt;
	DWORD					dwWritten;


	if( !m_isAllocated ) return;

	va_start( argptr, fmt );
	iCnt = vsprintf( cBuf, fmt, argptr );
	va_end( argptr );

	//콘솔에다가 출력 해주는 함수를 써줌
	WriteConsoleA( m_hConsoleOutput, cBuf, iCnt, &dwWritten, NULL );	
}
