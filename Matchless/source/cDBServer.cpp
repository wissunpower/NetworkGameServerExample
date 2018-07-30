
#include	"stdafx.h"
#include	"cDBServer.h"


int Matchless::cDBServer::Connect( const int family, const unsigned short port, const tstring& sIP )
{
	m_sockaddr = SOCKADDR_IN {};

	m_sockaddr.sin_family = family;
	m_sockaddr.sin_port = htons( port );
	InetPton( m_sockaddr.sin_family, sIP.c_str(), &m_sockaddr.sin_addr.s_addr );
	return connect( GetSocket(), (SOCKADDR*)&m_sockaddr, sizeof( m_sockaddr ) );
}
