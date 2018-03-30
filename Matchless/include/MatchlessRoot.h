
#ifndef		__MATCHLESSROOT_H_vhs83_fj923_892hv__
#define		__MATCHLESSROOT_H_vhs83_fj923_892hv__



#include	<math.h>



#define		RANGE_UNIT		256.0f



class cIPacket;
class cOPacket;



namespace	Matchless
{
	enum	EMainStepState
	{
		EMSS_Idle,

		EMSS_Start,
		EMSS_Wait,
		EMSS_Play,
		EMSS_Adjust,
		EMSS_End,

		EMSS_TotalCount
	};

	enum	ECharacterClass
	{
		ECC_Idle,

		ECC_Breaker,
		ECC_Defender,
		ECC_Mage,
		ECC_Healer,

		ECC_TotalCount
	};

	enum	ECharacterSkill
	{
		ECS_Idle,

		ECS_Breaker_NormalAttack,
		ECS_Breaker_Raid,
		ECS_Breaker_Stun,
		ECS_Breaker_ArmorBreak,
		ECS_Breaker_PowerBreak,
		ECS_Breaker_Berserk,
		ECS_Breaker_Charge,
		ECS_Breaker_Silent,
		ECS_Breaker_LastHit,

		ECS_Defender_NormalAttack,
		ECS_Defender_StoneSkin,
		ECS_Defender_Natural,
		ECS_Defender_Sacrifice,
		ECS_Defender_AttackBreak,
		ECS_Defender_HeavyBlow,
		ECS_Defender_SpiritLink,
		ECS_Defender_Earthquake,
		ECS_Defender_FullThrow,

		ECS_Mage_NormalAttack,
		ECS_Mage_IceShield,
		ECS_Mage_MagicalArmor,
		ECS_Mage_IceBolt,
		ECS_Mage_FireBall,
		ECS_Mage_Blizzard,
		ECS_Mage_FairyRescue,
		ECS_Mage_ChainLightning,
		ECS_Mage_LightningEmission,

		ECS_Healer_NormalAttack,
		ECS_Healer_Perfect,
		ECS_Healer_QuickHeal,
		ECS_Healer_NormalHeal,
		ECS_Healer_LandBlessing,
		ECS_Healer_Dispel,
		ECS_Healer_FreePromise,
		ECS_Healer_SoulCry,
		ECS_Healer_Discharge,

		ECS_TotalCount
	};


	struct SMatrix4
	{
		void Reset( void )
		{
			for( int i = 0 ; i < 4 ; ++i )
			{
				for( int j = 0 ; j < 4 ; ++j )
				{
					m[ i ][ j ] = (i == j) ? 1.0f : 0.0f;
				}
			}
		}

		union {
			struct {
				float        _11, _12, _13, _14;
				float        _21, _22, _23, _24;
				float        _31, _32, _33, _34;
				float        _41, _42, _43, _44;

			};
			float m[4][4];
		};
	};

	void Encode( cOPacket& oPacket, const SMatrix4& m );
	void Decode( cIPacket& iPacket, SMatrix4& m );
}



float GetDistanceIn3D(  const float aStartPosX,  const float aStartPosY,  const float aStartPosZ,
						const float aTargetPosX,  const float aTargetPosY,  const float aTargetPosZ );

bool IsFrontOfThisIn2D(  const float aThisPosX,  const float aThisPosZ,  const float aThisLookAtX,  const float aThisLookAtZ,
						const float aTargetPosX,  const float aTargetPosZ  );
bool IsFrontOfThisIn2D(  const Matchless::SMatrix4 & aThis,  const Matchless::SMatrix4 & aTarget  );



#endif		// __MATCHLESSROOT_H_vhs83_fj923_892hv__
