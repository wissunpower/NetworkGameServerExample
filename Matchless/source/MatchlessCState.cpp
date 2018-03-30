
#include	"stdafx.h"
#include	"MatchlessCState.h"
#include	"cPacket.h"


void Matchless::Encode( cOPacket& oPacket, const CState& info )
{
	oPacket.Encode4u( info.GetType() );
	oPacket.Encode4u( info.GetRemoveTime() );
	oPacket.Encode4u( info.GetExtraValue() );
}

void Matchless::Decode( cIPacket& iPacket, CState& info )
{
	info.SetType( static_cast<EStateType>( iPacket.Decode4u() ) );
	info.SetRemoveTime( iPacket.Decode4u() );
	info.SetExtraValue( iPacket.Decode4u() );
}
