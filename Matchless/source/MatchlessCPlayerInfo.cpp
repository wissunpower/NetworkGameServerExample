
#include	"stdafx.h"
#include	"MatchlessCPlayerInfo.h"
#include	"cPacket.h"


void Matchless::Encode( cOPacket& oPacket, const CPlayerInfo& info )
{
	oPacket.Encode4u( info.GetMainStepState() );
	oPacket.EncodeBool( info.IsRoomMaster() );
	oPacket.Encode4u( info.GetTeamNum() );
	Encode( oPacket, info.GetTransform() );
	Encode( oPacket, info.GetCharacterInfo() );
	oPacket.Encode4u( info.GetCurrentCastStartTick() );
}

void Matchless::Decode( cIPacket& iPacket, CPlayerInfo& info )
{
	info.SetMainStepState( static_cast<EMainStepState>( iPacket.Decode4u() ) );
	info.SetbRoomMaster( iPacket.DecodeBool() );
	info.SetTeamNum( static_cast<unsigned short>( iPacket.Decode4u() ) );
	Decode( iPacket, info.GetTransform() );
	Decode( iPacket, info.GetCharacterInfo() );
	info.SetCurrentCastStartTick( iPacket.Decode4u() );
}

void Encode( cOPacket& oPacket, const cAniTrackInfo& info )
{
	oPacket.Encode4u( info.Priority );
	oPacket.Encode4f( info.Weight );
	oPacket.Encode4f( info.Speed );
	oPacket.Encode8d( info.Position );
	oPacket.Encode4( info.Enable );
	oPacket.Encode4u( info.AniIndex );
	oPacket.EncodeStr( info.AniName );
}

void Decode( cIPacket& iPacket, cAniTrackInfo& info )
{
	info.Priority = iPacket.Decode4u();
	info.Weight = iPacket.Decode4f();
	info.Speed = iPacket.Decode4f();
	info.Position = iPacket.Decode8d();
	info.Enable = iPacket.Decode4();
	info.AniIndex = iPacket.Decode4u();
	info.AniName = iPacket.DecodeStr( std::string{} );
}
