#pragma once


namespace	Matchless
{
	class cDBServer
	{

	public:
		const SOCKET& GetSocket() const			{ return m_socket; }

		void SetSocket( const SOCKET& socket )	{ m_socket = socket; }

		int Connect( const int family, const unsigned short port, const tstring& sIP );

	private:
		SOCKET			m_socket;
		SOCKADDR_IN		m_sockaddr;

	};
}
