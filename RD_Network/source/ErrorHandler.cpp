
#include	"ErrorHandler.h"


void err_quit( LPCTSTR msg )
{
	int tempErrorCode;
	LPVOID	lpMsgBuf;

	FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL,
					tempErrorCode = WSAGetLastError(), MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
					(LPTSTR)&lpMsgBuf, 0, NULL );

	MessageBox( NULL, (LPCTSTR)lpMsgBuf, (LPCTSTR)msg, MB_ICONERROR );
	LocalFree( lpMsgBuf );
	exit( -1 );
}


void err_display( LPCTSTR msg )
{
	int tempErrorCode;
	LPVOID	lpMsgBuf;
	char	tempMsg[ 128 ];
	char	tempLPMsgBuf[ 256 ];

	FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL,
					tempErrorCode = WSAGetLastError(), MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
					(LPTSTR)&lpMsgBuf, 0, NULL );

#if defined(UNICODE) | defined(_UNICODE)
	WideCharToMultiByte( CP_ACP, 0, msg, -1, tempMsg, 128, NULL, FALSE );
	WideCharToMultiByte( CP_ACP, 0, (LPCTSTR)lpMsgBuf, -1, tempLPMsgBuf, 256, NULL, FALSE );
#else
	_tcscpy( tempMsg, msg );
	_tcscpy( tempLPMsgBuf, lpMsgBuf );
#endif

	printf( "[ %s ] %s", tempMsg, tempLPMsgBuf );
	LocalFree( lpMsgBuf );
}
