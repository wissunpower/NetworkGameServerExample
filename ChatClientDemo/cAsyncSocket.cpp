
#include	"stdafx.h"
#include	"cAsyncSocket.h"


cAsyncSocket::cAsyncSocket()
	: m_socket( INVALID_SOCKET )
	, m_hWnd( nullptr )
	, m_pMainDlg( nullptr )
	, m_vSocketBuf( 1024, 0 )
{
}

cAsyncSocket::~cAsyncSocket()
{
	WSACleanup();
}

bool cAsyncSocket::InitSocket( HWND hWnd )
{
	if ( INVALID_SOCKET != m_socket )
	{
		CloseSocket( m_socket );
	}

	WSADATA wsaData;
	int nRet = WSAStartup( MAKEWORD( 2, 2 ), &wsaData );
	if ( 0 != nRet )
	{
		m_pMainDlg->OutputMsg( "[에러] WSAStartup() 함수 실패 : %d", WSAGetLastError() );
		return false;
	}

	m_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if ( INVALID_SOCKET == m_socket )
	{
		m_pMainDlg->OutputMsg( "[에러] socket() 함수 실패 : %d", WSAGetLastError() );
		return false;
	}

	m_hWnd = hWnd;
	m_pMainDlg->OutputMsg( "소켓 초기화 성공" );

	return true;
}

void cAsyncSocket::CloseSocket( SOCKET socketClose, bool bIsForce )
{
	struct linger stLinger = { 0, 0 };

	if ( true == bIsForce )
	{
		stLinger.l_onoff = 1;
	}

	shutdown( socketClose, SD_BOTH );
	setsockopt( socketClose, SOL_SOCKET, SO_LINGER, reinterpret_cast<char*>( &stLinger ), sizeof( stLinger ) );
	closesocket( socketClose );

	socketClose = INVALID_SOCKET;
}

bool cAsyncSocket::ConnectTo( std::wstring strIP, int nPort )
{
	SOCKADDR_IN		stServerAddr;

	stServerAddr.sin_family = AF_INET;
	stServerAddr.sin_port = htons( nPort );
	InetPton( stServerAddr.sin_family, strIP.c_str(), &stServerAddr.sin_addr.s_addr );

	int nRet = connect( m_socket, reinterpret_cast<sockaddr*>( &stServerAddr ), sizeof( sockaddr ) );
	if ( SOCKET_ERROR == nRet )
	{
		m_pMainDlg->OutputMsg( "[ERROR] 접속 실패 : connect() failed" );
		return false;
	}

	WSAEVENT	NewEvent;
	NewEvent = WSACreateEvent();
	WSAAsyncSelect( m_socket, m_hWnd, WM_SOCKETMSG, FD_READ | FD_CLOSE );
	//WSAEventSelect( m_socket, NewEvent, FD_READ | FD_CLOSE );
	m_pMainDlg->OutputMsg( "[SUCCESS] 접속 성공" );

	return true;
}

int cAsyncSocket::SendMsg( char*/*std::vector<char>*/ msg, int nLen )
{
	return send( m_socket, msg/*.data()*/, nLen, 0 );
}
