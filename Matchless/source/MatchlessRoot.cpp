
#include	"MatchlessRoot.h"



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
