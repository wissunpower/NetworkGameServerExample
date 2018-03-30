
#include	"stdafx.h"
#include	"MatchlessCClient.h"


void Matchless::Encode( cOPacket& oPacket, const CClient& client )
{
	Encode( oPacket, client.m_NetSystem );
	Encode( oPacket, client.m_PlayerInfo );
}

void Matchless::Decode( cIPacket& iPacket, CClient& client )
{
	Decode( iPacket, client.m_NetSystem );
	Decode( iPacket, client.m_PlayerInfo );
}
