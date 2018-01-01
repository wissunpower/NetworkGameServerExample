
#ifndef		__MATCHLESSCSKILL_H_fj932_892ji_fj932__
#define		__MATCHLESSCSKILL_H_fj932_892ji_fj932__



#include	"MatchlessRoot.h"
#include	"MatchlessCPlayerInfo.h"



namespace	Matchless
{
	class CSkill
	{

	public:

		enum ETargetType
		{
			ETT_Idle,

			ETT_Friend,
			ETT_Enemy,

			ETT_AnyOne,

			ETT_TotalCount
		};


	public:

		CSkill( void ) : m_Kind( ECS_Idle ), m_EnergyCost( 0 ), m_IsToggle( false ), m_IsAreaTarget( false ), m_TargetType( ETT_Idle ),
						m_Range( 0 ), m_CastingTime( 0 ), m_PhysicalDamageScale( 0.0f ), m_MagicalDamageScale( 0.0f )
		{}

		// Get Member Function
		const ECharacterSkill GetKind( void ) const				{ return m_Kind; }
		const unsigned int GetEnergyCost( void ) const			{ return m_EnergyCost; }
		const bool IsToggle( void ) const						{ return m_IsToggle; }
		const bool IsAreaTarget( void ) const					{ return m_IsAreaTarget; }
		const ETargetType GetTargetType( void ) const			{ return m_TargetType; }
		const unsigned int GetRange( void ) const				{ return m_Range; }
		const unsigned int GetCastingTime( void ) const			{ return m_CastingTime; }
		const float GetPhysicalDamageScale( void ) const		{ return m_PhysicalDamageScale; }
		const float GetMagicalDamageScale( void ) const			{ return m_MagicalDamageScale; }

		// Set Member Function
		void SetKind( const ECharacterSkill aKind )				{ m_Kind = aKind; }
		void SetEnergyCost( const unsigned int aValue )			{ m_EnergyCost = aValue; }
		void SetToggle( const bool aBool )						{ m_IsToggle = aBool; }
		void SetAreaTarget( const bool aBool )					{ m_IsAreaTarget = aBool; }
		void SetTargetType( const ETargetType aType )			{ m_TargetType = aType; }
		void SetRange( const unsigned int aValue )				{ m_Range = aValue; }
		void SetCastingTime( const unsigned int aValue )		{ m_CastingTime = aValue; }
		void SetPhysicalDamageScale( const float aValue )		{ m_PhysicalDamageScale = aValue; }
		void SetMagicalDamageScale( const float aValue )		{ m_MagicalDamageScale = aValue; }


	private:

		ECharacterSkill		m_Kind;
		unsigned int		m_EnergyCost;
		bool				m_IsToggle;
		bool				m_IsAreaTarget;
		ETargetType			m_TargetType;
		unsigned int		m_Range;
		unsigned int		m_CastingTime;
		float				m_PhysicalDamageScale;
		float				m_MagicalDamageScale;
		//unsigned int		m_AddEffectStatus;		// 추가 부여 상태 ( 긍정적, 부정적 효과 )
		//unsigned int		m_AddMotion;			// 추가 동작 ( 예 : 순간 이동 )

	};


	extern CSkill		BreakerNormalAttack;
	extern CSkill		BreakerRaid;
	extern CSkill		BreakerStun;
	extern CSkill		BreakerArmorBreak;
	extern CSkill		BreakerPowerBreak;
	extern CSkill		BreakerBerserk;
	extern CSkill		BreakerCharge;
	extern CSkill		BreakerSilent;
	extern CSkill		BreakerLastHit;
	
	extern CSkill		DefenderNormalAttack;
	extern CSkill		DefenderStoneSkin;
	extern CSkill		DefenderNatural;
	extern CSkill		DefenderSacrifice;
	extern CSkill		DefenderAttackBreak;
	extern CSkill		DefenderHeavyBlow;
	extern CSkill		DefenderSpiritLink;
	extern CSkill		DefenderEarthquake;
	extern CSkill		DefenderFullThrow;

	extern CSkill		MageNormalAttack;
	extern CSkill		MageIceShield;
	extern CSkill		MageMagicalArmor;
	extern CSkill		MageIceBolt;
	extern CSkill		MageFireBall;
	extern CSkill		MageBlizzard;
	extern CSkill		MageFairyRescue;
	extern CSkill		MageChainLightning;
	extern CSkill		MageLightningEmission;
	
	extern CSkill		HealerNormalAttack;
	extern CSkill		HealerPerfect;
	extern CSkill		HealerQuickHeal;
	extern CSkill		HealerNormalHeal;
	extern CSkill		HealerLandBlessing;
	extern CSkill		HealerDispel;
	extern CSkill		HealerFreePromise;
	extern CSkill		HealerSoulCry;
	extern CSkill		HealerDischarge;


	bool SetSampleSkillInfo( void );

	bool IsSkillPossible( const CSkill & aSkill, CPlayerInfo & aCaster, CPlayerInfo & aTarget );

	bool IsNormalAttackPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget );

	bool IsBreakerRaidPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget );
	bool IsBreakerStunPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget );
	bool IsBreakerArmorBreakPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget );
	bool IsBreakerPowerBreakPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget );
	bool IsBreakerBerserkPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget );
	bool IsBreakerChargePossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget );
	bool IsBreakerSilentPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget );
	bool IsBreakerLastHitPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget );

	bool IsDefenderStoneSkinPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget );
	bool IsDefenderNaturalPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget );
	bool IsDefenderSacrificePossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget );
	bool IsDefenderAttackBreakPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget );
	bool IsDefenderHeavyBlowPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget );
	bool IsDefenderSpiritLinkPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget );
	bool IsDefenderEarthquakePossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget );
	bool IsDefenderFullThrowPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget );

	bool IsMageIceShieldPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget );
	bool IsMageMagicalArmorPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget );
	bool IsMageIceBoltPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget );
	bool IsMageFireBallPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget );
	bool IsMageBlizzardPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget );
	bool IsMageFairyRescuePossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget );
	bool IsMageChainLightningPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget );
	bool IsMageLightningEmissionPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget );

	bool IsHealerPerfectPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget );
	bool IsHealerQuickHealPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget );
	bool IsHealerNormalHealPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget );
	bool IsHealerLandBlessingPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget );
	bool IsHealerDispelPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget );
	bool IsHealerFreePromisePossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget );
	bool IsHealerSoulCryPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget );
	bool IsHealerDischargePossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget );
}





#endif		// __MATCHLESSCSKILL_H_fj932_892ji_fj932__
