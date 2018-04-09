
#include	"stdafx.h"
#include	<stdio.h>
#include	<winsock2.h>
#include	<tchar.h>

#include	<DbgHelp.h>

#include	"CMiniDump.h"



typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(  HANDLE hProcess,  DWORD dwPid,  HANDLE hFile,  MINIDUMP_TYPE DumpType,
											CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
											CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam,
											CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam );



LPTOP_LEVEL_EXCEPTION_FILTER	PreviousExceptionFilter = NULL;



LONG WINAPI UnHandledExceptionFilter( struct _EXCEPTION_POINTERS * exceptionInfo )
{
	HMODULE		DllHandle = NULL;

	DllHandle = LoadLibrary( TEXT( "DBGHELP.DLL" ) );


	if( DllHandle )
	{
		MINIDUMPWRITEDUMP	Dump = (MINIDUMPWRITEDUMP)GetProcAddress( DllHandle, "MiniDumpWriteDump" );

		if( Dump )
		{
			TCHAR		DumpPath[ MAX_PATH ] = { 0, };
			SYSTEMTIME	SystemTime;

			GetLocalTime( &SystemTime );

			_sntprintf_s(  DumpPath,  MAX_PATH,  TEXT( "%d-%d-%d %d_%d_%d.dmp" ),
							SystemTime.wYear,  SystemTime.wMonth,  SystemTime.wDay,
							SystemTime.wHour,  SystemTime.wMinute,  SystemTime.wSecond );

			HANDLE	FileHandle = CreateFile( DumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

			if( INVALID_HANDLE_VALUE != FileHandle )
			{
				_MINIDUMP_EXCEPTION_INFORMATION		MiniDumpExceptionInfo;

				MiniDumpExceptionInfo.ThreadId = GetCurrentThreadId();
				MiniDumpExceptionInfo.ExceptionPointers = exceptionInfo;
				MiniDumpExceptionInfo.ClientPointers = NULL;

				BOOL	Success = Dump( GetCurrentProcess(), GetCurrentProcessId(), FileHandle, MiniDumpNormal, &MiniDumpExceptionInfo, NULL, NULL );

				if( Success )
				{
					CloseHandle( FileHandle );

					return	EXCEPTION_EXECUTE_HANDLER;
				}
			}

			CloseHandle( FileHandle );
		}
	}


	return	EXCEPTION_CONTINUE_SEARCH;
}


bool CMiniDump::Begin( void )
{
	SetErrorMode( SEM_FAILCRITICALERRORS );
	PreviousExceptionFilter = SetUnhandledExceptionFilter( UnHandledExceptionFilter );
	return	true;
}


bool CMiniDump::End( void )
{
	SetUnhandledExceptionFilter( PreviousExceptionFilter );
	return	true;
}
