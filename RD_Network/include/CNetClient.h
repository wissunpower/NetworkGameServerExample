
#ifndef		__CNETCLIENT_H_29ht3_sh28d_8d28f__
#define		__CNETCLIENT_H_29ht3_sh28d_8d28f__



#include		"RD_NetworkRoot.h"


class cOPacket;
class cIPacket;


class CNetClient
{

public:

	// Get member function
	const unsigned int GetID( void ) const			{ return m_ID; }
	const SOCKET & GetSocket( void ) const			{ return m_Socket; }

	// Set member function
	void SetID( const unsigned int aID )			{ m_ID = aID; }
	void SetSocket( const SOCKET & aSocket )		{ m_Socket = aSocket; }


private:

	unsigned int		m_ID;
	SOCKET				m_Socket;

};

void Encode( cOPacket& oPacket, const CNetClient& client );
void Decode( cIPacket& iPacket, CNetClient& client );



#endif		// __CNETCLIENT_H_29ht3_sh28d_8d28f__
