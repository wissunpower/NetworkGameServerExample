
#ifndef		__DEBUGCONSOLE_H__
#define		__DEBUGCONSOLE_H__



#include	<winsock2.h>
#include	<tchar.h>
#include	<stdio.h>



class DebugConsole
{
public:
	void					Output( char * fmt, ... );

	DebugConsole();
	~DebugConsole();

private:
	HANDLE					m_hConsoleOutput;
	BOOL					m_isAllocated;
};




#endif		// __DEBUGCONSOLE_H__
