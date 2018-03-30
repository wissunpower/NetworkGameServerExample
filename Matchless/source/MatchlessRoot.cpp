
#include	"stdafx.h"
#include	"MatchlessRoot.h"
#include	"cPacket.h"



void Matchless::Encode( cOPacket& oPacket, const SMatrix4& m )
{
	oPacket.Encode4f( m._11 );
	oPacket.Encode4f( m._12 );
	oPacket.Encode4f( m._13 );
	oPacket.Encode4f( m._14 );
	oPacket.Encode4f( m._21 );
	oPacket.Encode4f( m._22 );
	oPacket.Encode4f( m._23 );
	oPacket.Encode4f( m._24 );
	oPacket.Encode4f( m._31 );
	oPacket.Encode4f( m._32 );
	oPacket.Encode4f( m._33 );
	oPacket.Encode4f( m._34 );
	oPacket.Encode4f( m._41 );
	oPacket.Encode4f( m._42 );
	oPacket.Encode4f( m._43 );
	oPacket.Encode4f( m._44 );
}

void Matchless::Decode( cIPacket& iPacket, SMatrix4& m )
{
	m._11 = iPacket.Decode4f();
	m._12 = iPacket.Decode4f();
	m._13 = iPacket.Decode4f();
	m._14 = iPacket.Decode4f();
	m._21 = iPacket.Decode4f();
	m._22 = iPacket.Decode4f();
	m._23 = iPacket.Decode4f();
	m._24 = iPacket.Decode4f();
	m._31 = iPacket.Decode4f();
	m._32 = iPacket.Decode4f();
	m._33 = iPacket.Decode4f();
	m._34 = iPacket.Decode4f();
	m._41 = iPacket.Decode4f();
	m._42 = iPacket.Decode4f();
	m._43 = iPacket.Decode4f();
	m._44 = iPacket.Decode4f();
}


float GetDistanceIn3D(  const float aStartPosX,  const float aStartPosY,  const float aStartPosZ,
						const float aTargetPosX,  const float aTargetPosY,  const float aTargetPosZ  )
{
	float	tempXDiff = aTargetPosX - aStartPosX;
	float	tempYDiff = aTargetPosY - aStartPosY;
	float	tempZDiff = aTargetPosZ - aStartPosZ;

	return	( sqrt( tempXDiff * tempXDiff + tempYDiff * tempYDiff + tempZDiff * tempZDiff ) );
}


bool IsFrontOfThisIn2D(  const float aThisPosX,  const float aThisPosZ,  const float aThisLookAtX,  const float aThisLookAtZ,
						const float aTargetPosX,  const float aTargetPosZ  )
{
	return	( 0  <=  aThisLookAtX * (aTargetPosX - aThisPosX) + aThisLookAtZ * (aTargetPosZ - aThisPosZ) );
}


bool IsFrontOfThisIn2D(  const Matchless::SMatrix4 & aThis,  const Matchless::SMatrix4 & aTarget  )
{
	return	IsFrontOfThisIn2D( aThis._41, aThis._43, -(aThis._31), -(aThis._33), aTarget._41, aTarget._43 );
}
