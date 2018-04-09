
#include	"stdafx.h"
#include	"CNetClient.h"
#include	"cPacket.h"


void Encode( cOPacket& oPacket, const CNetClient& client )
{
	oPacket.Encode4u( client.GetID() );
}

void Decode( cIPacket& iPacket, CNetClient& client )
{
	client.SetID( iPacket.Decode4u() );
}
