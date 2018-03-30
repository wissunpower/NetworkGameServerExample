
#include	"stdafx.h"
#include	"MatchlessCSkill.h"



Matchless::CSkill		Matchless::BreakerNormalAttack;
Matchless::CSkill		Matchless::BreakerRaid;
Matchless::CSkill		Matchless::BreakerStun;
Matchless::CSkill		Matchless::BreakerArmorBreak;
Matchless::CSkill		Matchless::BreakerPowerBreak;
Matchless::CSkill		Matchless::BreakerBerserk;
Matchless::CSkill		Matchless::BreakerCharge;
Matchless::CSkill		Matchless::BreakerSilent;
Matchless::CSkill		Matchless::BreakerLastHit;
	
Matchless::CSkill		Matchless::DefenderNormalAttack;
Matchless::CSkill		Matchless::DefenderStoneSkin;
Matchless::CSkill		Matchless::DefenderNatural;
Matchless::CSkill		Matchless::DefenderSacrifice;
Matchless::CSkill		Matchless::DefenderAttackBreak;
Matchless::CSkill		Matchless::DefenderHeavyBlow;
Matchless::CSkill		Matchless::DefenderSpiritLink;
Matchless::CSkill		Matchless::DefenderEarthquake;
Matchless::CSkill		Matchless::DefenderFullThrow;

Matchless::CSkill		Matchless::MageNormalAttack;
Matchless::CSkill		Matchless::MageIceShield;
Matchless::CSkill		Matchless::MageMagicalArmor;
Matchless::CSkill		Matchless::MageIceBolt;
Matchless::CSkill		Matchless::MageFireBall;
Matchless::CSkill		Matchless::MageBlizzard;
Matchless::CSkill		Matchless::MageFairyRescue;
Matchless::CSkill		Matchless::MageChainLightning;
Matchless::CSkill		Matchless::MageLightningEmission;
	
Matchless::CSkill		Matchless::HealerNormalAttack;
Matchless::CSkill		Matchless::HealerPerfect;
Matchless::CSkill		Matchless::HealerQuickHeal;
Matchless::CSkill		Matchless::HealerNormalHeal;
Matchless::CSkill		Matchless::HealerLandBlessing;
Matchless::CSkill		Matchless::HealerDispel;
Matchless::CSkill		Matchless::HealerFreePromise;
Matchless::CSkill		Matchless::HealerSoulCry;
Matchless::CSkill		Matchless::HealerDischarge;



bool Matchless::SetSampleSkillInfo( void )
{
	BreakerNormalAttack.SetKind( ECS_Breaker_NormalAttack );
	BreakerNormalAttack.SetEnergyCost( 0 );
	BreakerNormalAttack.SetToggle( false );
	BreakerNormalAttack.SetAreaTarget( false );
	BreakerNormalAttack.SetTargetType( CSkill::ETT_Enemy );
	BreakerNormalAttack.SetRange( 1 );
	BreakerNormalAttack.SetCastingTime( 0 );
	BreakerNormalAttack.SetPhysicalDamageScale( 1.0f );
	BreakerNormalAttack.SetMagicalDamageScale( 0.0f );


	BreakerRaid.SetKind( ECS_Breaker_Raid );
	BreakerRaid.SetEnergyCost( 5 );
	BreakerRaid.SetToggle( false );
	BreakerRaid.SetAreaTarget( false );
	BreakerRaid.SetTargetType( CSkill::ETT_Enemy );
	BreakerRaid.SetRange( 1 );
	BreakerRaid.SetCastingTime( 0 );
	BreakerRaid.SetPhysicalDamageScale( 2.0f );
	BreakerRaid.SetMagicalDamageScale( 0.0f );


	BreakerStun.SetKind( ECS_Breaker_Stun );
	BreakerStun.SetEnergyCost( 10 );
	BreakerStun.SetToggle( false );
	BreakerStun.SetAreaTarget( false );
	BreakerStun.SetTargetType( CSkill::ETT_Enemy );
	BreakerStun.SetRange( 1 );
	BreakerStun.SetCastingTime( 0 );
	BreakerStun.SetPhysicalDamageScale( 0.75f );
	BreakerStun.SetMagicalDamageScale( 0.0f );


	BreakerArmorBreak.SetKind( ECS_Breaker_ArmorBreak );
	BreakerArmorBreak.SetEnergyCost( 10 );
	BreakerArmorBreak.SetToggle( false );
	BreakerArmorBreak.SetAreaTarget( false );
	BreakerArmorBreak.SetTargetType( CSkill::ETT_Enemy );
	BreakerArmorBreak.SetRange( 1 );
	BreakerArmorBreak.SetCastingTime( 0 );
	BreakerArmorBreak.SetPhysicalDamageScale( 0.0f );
	BreakerArmorBreak.SetMagicalDamageScale( 0.0f );


	BreakerPowerBreak.SetKind( ECS_Breaker_PowerBreak );
	BreakerPowerBreak.SetEnergyCost( 10 );
	BreakerPowerBreak.SetToggle( false );
	BreakerPowerBreak.SetAreaTarget( false );
	BreakerPowerBreak.SetTargetType( CSkill::ETT_Enemy );
	BreakerPowerBreak.SetRange( 1 );
	BreakerPowerBreak.SetCastingTime( 0 );
	BreakerPowerBreak.SetPhysicalDamageScale( 0.0f );
	BreakerPowerBreak.SetMagicalDamageScale( 0.0f );


	BreakerBerserk.SetKind( ECS_Breaker_Berserk );
	BreakerBerserk.SetEnergyCost( 0 );
	BreakerBerserk.SetToggle( true );
	BreakerBerserk.SetAreaTarget( false );
	BreakerBerserk.SetTargetType( CSkill::ETT_Friend );
	BreakerBerserk.SetRange( 1 );
	BreakerBerserk.SetCastingTime( 0 );
	BreakerBerserk.SetPhysicalDamageScale( 1.5f );
	//BreakerBerserk.SetPhysicalArmorScale( 0.75f );
	BreakerBerserk.SetMagicalDamageScale( 0.0f );


	BreakerCharge.SetKind( ECS_Breaker_Charge );
	BreakerCharge.SetEnergyCost( 0 );
	BreakerCharge.SetToggle( false );
	BreakerCharge.SetAreaTarget( false );
	BreakerCharge.SetTargetType( CSkill::ETT_Enemy );
	BreakerCharge.SetRange( 1 );
	BreakerCharge.SetCastingTime( 0 );
	BreakerCharge.SetPhysicalDamageScale( 0.5f );
	BreakerCharge.SetMagicalDamageScale( 0.0f );


	BreakerSilent.SetKind( ECS_Breaker_Silent );
	BreakerSilent.SetEnergyCost( 20 );
	BreakerSilent.SetToggle( false );
	BreakerSilent.SetAreaTarget( false );
	BreakerSilent.SetTargetType( CSkill::ETT_Enemy );
	BreakerSilent.SetRange( 1 );
	BreakerSilent.SetCastingTime( 0 );
	BreakerSilent.SetPhysicalDamageScale( 0.0f );
	BreakerSilent.SetMagicalDamageScale( 0.0f );


	BreakerLastHit.SetKind( ECS_Breaker_LastHit );
	BreakerLastHit.SetEnergyCost( 0 );
	BreakerLastHit.SetToggle( false );
	BreakerLastHit.SetAreaTarget( false );
	BreakerLastHit.SetTargetType( CSkill::ETT_Enemy );
	BreakerLastHit.SetRange( 1 );
	BreakerLastHit.SetCastingTime( 0 );
	BreakerLastHit.SetPhysicalDamageScale( 1.0f );
	BreakerLastHit.SetMagicalDamageScale( 0.0f );


	DefenderNormalAttack.SetKind( ECS_Defender_NormalAttack );
	DefenderNormalAttack.SetEnergyCost( 0 );
	DefenderNormalAttack.SetToggle( false );
	DefenderNormalAttack.SetAreaTarget( false );
	DefenderNormalAttack.SetTargetType( CSkill::ETT_Enemy );
	DefenderNormalAttack.SetRange( 1 );
	DefenderNormalAttack.SetCastingTime( 0 );
	DefenderNormalAttack.SetPhysicalDamageScale( 1.0f );
	DefenderNormalAttack.SetMagicalDamageScale( 0.0f );


	DefenderStoneSkin.SetKind( ECS_Defender_StoneSkin );
	DefenderStoneSkin.SetEnergyCost( 10 );
	DefenderStoneSkin.SetToggle( false );
	DefenderStoneSkin.SetAreaTarget( false );
	DefenderStoneSkin.SetTargetType( CSkill::ETT_Friend );
	DefenderStoneSkin.SetRange( 3 );
	DefenderStoneSkin.SetCastingTime( 0 );
	DefenderStoneSkin.SetPhysicalDamageScale( 0.0f );
	DefenderStoneSkin.SetMagicalDamageScale( 0.0f );


	DefenderNatural.SetKind( ECS_Defender_Natural );
	DefenderNatural.SetEnergyCost( 10 );
	DefenderNatural.SetToggle( false );
	DefenderNatural.SetAreaTarget( false );
	DefenderNatural.SetTargetType( CSkill::ETT_Friend );
	DefenderNatural.SetRange( 3 );
	DefenderNatural.SetCastingTime( 0 );
	DefenderNatural.SetPhysicalDamageScale( 0.0f );
	DefenderNatural.SetMagicalDamageScale( 0.0f );


	DefenderSacrifice.SetKind( ECS_Defender_Sacrifice );
	DefenderSacrifice.SetEnergyCost( 0 );
	DefenderSacrifice.SetToggle( false );
	DefenderSacrifice.SetAreaTarget( false );
	DefenderSacrifice.SetTargetType( CSkill::ETT_Friend );
	DefenderSacrifice.SetRange( 3 );
	DefenderSacrifice.SetCastingTime( 0 );
	DefenderSacrifice.SetPhysicalDamageScale( 0.0f );
	DefenderSacrifice.SetMagicalDamageScale( 0.0f );


	DefenderAttackBreak.SetKind( ECS_Defender_AttackBreak );
	DefenderAttackBreak.SetEnergyCost( 25 );
	DefenderAttackBreak.SetToggle( false );
	DefenderAttackBreak.SetAreaTarget( false );
	DefenderAttackBreak.SetTargetType( CSkill::ETT_Enemy );
	DefenderAttackBreak.SetRange( 1 );
	DefenderAttackBreak.SetCastingTime( 0 );
	DefenderAttackBreak.SetPhysicalDamageScale( 0.0f );
	DefenderAttackBreak.SetMagicalDamageScale( 0.0f );


	DefenderHeavyBlow.SetKind( ECS_Defender_HeavyBlow );
	DefenderHeavyBlow.SetEnergyCost( 5 );
	DefenderHeavyBlow.SetToggle( false );
	DefenderHeavyBlow.SetAreaTarget( false );
	DefenderHeavyBlow.SetTargetType( CSkill::ETT_Enemy );
	DefenderHeavyBlow.SetRange( 1 );
	DefenderHeavyBlow.SetCastingTime( 0 );
	DefenderHeavyBlow.SetPhysicalDamageScale( 3.0f );
	DefenderHeavyBlow.SetMagicalDamageScale( 0.0f );


	DefenderSpiritLink.SetKind( ECS_Defender_SpiritLink );
	DefenderSpiritLink.SetEnergyCost( 0 );
	DefenderSpiritLink.SetToggle( false );
	DefenderSpiritLink.SetAreaTarget( false );
	DefenderSpiritLink.SetTargetType( CSkill::ETT_Friend );
	DefenderSpiritLink.SetRange( 5 );
	DefenderSpiritLink.SetCastingTime( 0 );
	DefenderSpiritLink.SetPhysicalDamageScale( 0.0f );
	DefenderSpiritLink.SetMagicalDamageScale( 0.0f );


	DefenderEarthquake.SetKind( ECS_Defender_Earthquake );
	DefenderEarthquake.SetEnergyCost( 0 );
	DefenderEarthquake.SetToggle( false );
	DefenderEarthquake.SetAreaTarget( true );
	DefenderEarthquake.SetTargetType( CSkill::ETT_Enemy );
	DefenderEarthquake.SetRange( 5 );
	DefenderEarthquake.SetCastingTime( 0 );
	DefenderEarthquake.SetPhysicalDamageScale( 0.0f );
	DefenderEarthquake.SetMagicalDamageScale( 0.0f );


	DefenderFullThrow.SetKind( ECS_Defender_FullThrow );
	DefenderFullThrow.SetEnergyCost( 0 );
	DefenderFullThrow.SetToggle( false );
	DefenderFullThrow.SetAreaTarget( false );
	DefenderFullThrow.SetTargetType( CSkill::ETT_Enemy );
	DefenderFullThrow.SetRange( 5 );
	DefenderFullThrow.SetCastingTime( 0 );
	DefenderFullThrow.SetPhysicalDamageScale( 1.5f );
	DefenderFullThrow.SetMagicalDamageScale( 0.0f );


	MageNormalAttack.SetKind( ECS_Mage_NormalAttack );
	MageNormalAttack.SetEnergyCost( 0 );
	MageNormalAttack.SetToggle( false );
	MageNormalAttack.SetAreaTarget( false );
	MageNormalAttack.SetTargetType( CSkill::ETT_Enemy );
	MageNormalAttack.SetRange( 5 );
	MageNormalAttack.SetCastingTime( 0 );
	MageNormalAttack.SetPhysicalDamageScale( 0.0f );
	MageNormalAttack.SetMagicalDamageScale( 1.0f );


	MageIceShield.SetKind( ECS_Mage_IceShield );
	MageIceShield.SetEnergyCost( 0 );
	MageIceShield.SetToggle( false );
	MageIceShield.SetAreaTarget( false );
	MageIceShield.SetTargetType( CSkill::ETT_Friend );
	MageIceShield.SetRange( 1 );
	MageIceShield.SetCastingTime( 0 );
	MageIceShield.SetPhysicalDamageScale( 0.0f );
	//MageIceShield.SetPhysicalArmorScale( 0.0f );
	MageIceShield.SetMagicalDamageScale( 0.0f );


	MageMagicalArmor.SetKind( ECS_Mage_MagicalArmor );
	MageMagicalArmor.SetEnergyCost( 0 );
	MageMagicalArmor.SetToggle( false );
	MageMagicalArmor.SetAreaTarget( false );
	MageMagicalArmor.SetTargetType( CSkill::ETT_Friend );
	MageMagicalArmor.SetRange( 1 );
	MageMagicalArmor.SetCastingTime( 0 );
	MageMagicalArmor.SetPhysicalDamageScale( 0.0f );
	//MageMagicalArmor.SetPhysicalArmorScale( 0.0f );
	MageMagicalArmor.SetMagicalDamageScale( 0.0f );


	MageIceBolt.SetKind( ECS_Mage_IceBolt );
	MageIceBolt.SetEnergyCost( 5 );
	MageIceBolt.SetToggle( false );
	MageIceBolt.SetAreaTarget( false );
	MageIceBolt.SetTargetType( CSkill::ETT_Enemy );
	MageIceBolt.SetRange( 5 );
	MageIceBolt.SetCastingTime( 1 );
	MageIceBolt.SetPhysicalDamageScale( 0.0f );
	MageIceBolt.SetMagicalDamageScale( 1.0f );


	MageFireBall.SetKind( ECS_Mage_FireBall );
	MageFireBall.SetEnergyCost( 7 );
	MageFireBall.SetToggle( false );
	MageFireBall.SetAreaTarget( false );
	MageFireBall.SetTargetType( CSkill::ETT_Enemy );
	MageFireBall.SetRange( 5 );
	MageFireBall.SetCastingTime( 2 );
	MageFireBall.SetPhysicalDamageScale( 0.0f );
	MageFireBall.SetMagicalDamageScale( 2.0f );


	MageBlizzard.SetKind( ECS_Mage_Blizzard );
	MageBlizzard.SetEnergyCost( 0 );
	MageBlizzard.SetToggle( false );
	MageBlizzard.SetAreaTarget( true );
	MageBlizzard.SetTargetType( CSkill::ETT_Enemy );
	MageBlizzard.SetRange( 5 );
	MageBlizzard.SetCastingTime( 0 );
	MageBlizzard.SetPhysicalDamageScale( 0.0f );
	MageBlizzard.SetMagicalDamageScale( 1.0f );


	MageFairyRescue.SetKind( ECS_Mage_FairyRescue );
	MageFairyRescue.SetEnergyCost( 0 );
	MageFairyRescue.SetToggle( false );
	MageFairyRescue.SetAreaTarget( false );
	MageFairyRescue.SetTargetType( CSkill::ETT_Friend );
	MageFairyRescue.SetRange( 1 );
	MageFairyRescue.SetCastingTime( 0 );
	MageFairyRescue.SetPhysicalDamageScale( 0.0f );
	MageFairyRescue.SetMagicalDamageScale( 0.0f );


	MageChainLightning.SetKind( ECS_Mage_ChainLightning );
	MageChainLightning.SetEnergyCost( 0 );
	MageChainLightning.SetToggle( false );
	MageChainLightning.SetAreaTarget( false );
	MageChainLightning.SetTargetType( CSkill::ETT_Enemy );
	MageChainLightning.SetRange( 3 );
	MageChainLightning.SetCastingTime( 0 );
	MageChainLightning.SetPhysicalDamageScale( 0.0f );
	MageChainLightning.SetMagicalDamageScale( 1.0f );


	MageLightningEmission.SetKind( ECS_Mage_LightningEmission );
	MageLightningEmission.SetEnergyCost( 0 );
	MageLightningEmission.SetToggle( false );
	MageLightningEmission.SetAreaTarget( false );
	MageLightningEmission.SetTargetType( CSkill::ETT_Enemy );
	MageLightningEmission.SetRange( 5 );
	MageLightningEmission.SetCastingTime( 0 );
	MageLightningEmission.SetPhysicalDamageScale( 0.0f );
	MageLightningEmission.SetMagicalDamageScale( 1.0f );


	HealerNormalAttack.SetKind( ECS_Healer_NormalAttack );
	HealerNormalAttack.SetEnergyCost( 10 );
	HealerNormalAttack.SetToggle( false );
	HealerNormalAttack.SetAreaTarget( false );
	HealerNormalAttack.SetTargetType( CSkill::ETT_Friend );
	HealerNormalAttack.SetRange( 5 );
	HealerNormalAttack.SetCastingTime( 0 );
	HealerNormalAttack.SetPhysicalDamageScale( 0.0f );
	HealerNormalAttack.SetMagicalDamageScale( 1.0f );


	HealerPerfect.SetKind( ECS_Healer_Perfect );
	HealerPerfect.SetEnergyCost( 10 );
	HealerPerfect.SetToggle( false );
	HealerPerfect.SetAreaTarget( false );
	HealerPerfect.SetTargetType( CSkill::ETT_Friend );
	HealerPerfect.SetRange( 1 );
	HealerPerfect.SetCastingTime( 0 );
	HealerPerfect.SetPhysicalDamageScale( 0.0f );
	HealerPerfect.SetMagicalDamageScale( 0.0f );


	HealerQuickHeal.SetKind( ECS_Healer_QuickHeal );
	HealerQuickHeal.SetEnergyCost( 10 );
	HealerQuickHeal.SetToggle( false );
	HealerQuickHeal.SetAreaTarget( false );
	HealerQuickHeal.SetTargetType( CSkill::ETT_Friend );
	HealerQuickHeal.SetRange( 5 );
	HealerQuickHeal.SetCastingTime( 1 );
	HealerQuickHeal.SetPhysicalDamageScale( 0.0f );
	HealerQuickHeal.SetMagicalDamageScale( 1.0f );


	HealerNormalHeal.SetKind( ECS_Healer_NormalHeal );
	HealerNormalHeal.SetEnergyCost( 7 );
	HealerNormalHeal.SetToggle( false );
	HealerNormalHeal.SetAreaTarget( false );
	HealerNormalHeal.SetTargetType( CSkill::ETT_Friend );
	HealerNormalHeal.SetRange( 5 );
	HealerNormalHeal.SetCastingTime( 2 );
	HealerNormalHeal.SetPhysicalDamageScale( 0.0f );
	HealerNormalHeal.SetMagicalDamageScale( 3.0f );


	HealerLandBlessing.SetKind( ECS_Healer_LandBlessing );
	HealerLandBlessing.SetEnergyCost( 10 );
	HealerLandBlessing.SetToggle( false );
	HealerLandBlessing.SetAreaTarget( true );
	HealerLandBlessing.SetTargetType( CSkill::ETT_Friend );
	HealerLandBlessing.SetRange( 5 );
	HealerLandBlessing.SetCastingTime( 0 );
	HealerLandBlessing.SetPhysicalDamageScale( 0.0f );
	HealerLandBlessing.SetMagicalDamageScale( 1.0f );


	HealerDispel.SetKind( ECS_Healer_Dispel );
	HealerDispel.SetEnergyCost( 10 );
	HealerDispel.SetToggle( false );
	HealerDispel.SetAreaTarget( false );
	HealerDispel.SetTargetType( CSkill::ETT_Friend );
	HealerDispel.SetRange( 5 );
	HealerDispel.SetCastingTime( 0 );
	HealerDispel.SetPhysicalDamageScale( 0.0f );
	HealerDispel.SetMagicalDamageScale( 0.0f );


	HealerFreePromise.SetKind( ECS_Healer_FreePromise );
	HealerFreePromise.SetEnergyCost( 10 );
	HealerFreePromise.SetToggle( false );
	HealerFreePromise.SetAreaTarget( false );
	HealerFreePromise.SetTargetType( CSkill::ETT_AnyOne );
	HealerFreePromise.SetRange( 5 );
	HealerFreePromise.SetCastingTime( 0 );
	HealerFreePromise.SetPhysicalDamageScale( 0.0f );
	HealerFreePromise.SetMagicalDamageScale( 0.0f );


	HealerSoulCry.SetKind( ECS_Healer_SoulCry );
	HealerSoulCry.SetEnergyCost( 10 );
	HealerSoulCry.SetToggle( false );
	HealerSoulCry.SetAreaTarget( true );
	HealerSoulCry.SetTargetType( CSkill::ETT_Enemy );
	HealerSoulCry.SetRange( 5 );
	HealerSoulCry.SetCastingTime( 0 );
	HealerSoulCry.SetPhysicalDamageScale( 0.0f );
	HealerSoulCry.SetMagicalDamageScale( 0.0f );


	HealerDischarge.SetKind( ECS_Healer_Discharge );
	HealerDischarge.SetEnergyCost( 10 );
	HealerDischarge.SetToggle( false );
	HealerDischarge.SetAreaTarget( false );
	HealerDischarge.SetTargetType( CSkill::ETT_Enemy );
	HealerDischarge.SetRange( 5 );
	HealerDischarge.SetCastingTime( 0 );
	HealerDischarge.SetPhysicalDamageScale( 0.0f );
	HealerDischarge.SetMagicalDamageScale( 0.2f );


	return	true;
}


bool Matchless::IsSkillPossible( const CSkill & aSkill, CPlayerInfo & aCaster, CPlayerInfo & aTarget )
{
	bool	isApplyTeam = false;


	switch( aSkill.GetTargetType() )
	{

	case CSkill::ETT_Friend:
		if( aCaster.GetTeamNum() == aTarget.GetTeamNum() )
		{
			isApplyTeam = true;
		}
		break;

	case CSkill::ETT_Enemy:
		if( aCaster.GetTeamNum() != aTarget.GetTeamNum()  &&  !aTarget.GetCharacterInfo().FindState( Matchless::EST_Perfect ) )
		{
			isApplyTeam = true;
		}
		break;

	case CSkill::ETT_AnyOne:
		isApplyTeam = true;
		break;

	}

	if( !aCaster.GetCharacterInfo().FindState( Matchless::EST_Silence )  &&
		aCaster.GetCharacterInfo().GetCurrentHealth() > 0  &&  aTarget.GetCharacterInfo().GetCurrentHealth() > 0  &&
		isApplyTeam  &&  aSkill.GetEnergyCost() <= aCaster.GetCharacterInfo().GetCurrentEnergy() )
	{
		if(  GetDistanceIn3D(  aCaster.GetTransform()._41,  aCaster.GetTransform()._42,  aCaster.GetTransform()._43,
								aTarget.GetTransform()._41,  aTarget.GetTransform()._42,  aTarget.GetTransform()._43  ) <=
								RANGE_UNIT * aSkill.GetRange()  )
		{
			if(  IsFrontOfThisIn2D(  aCaster.GetTransform(),  aTarget.GetTransform()  ) )
			{
				return	true;
			}
		}
	}

	return	false;
}


bool Matchless::IsNormalAttackPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget )
{
	if( aCaster.GetCharacterInfo().GetCurrentHealth() > 0  &&  aTarget.GetCharacterInfo().GetCurrentHealth() > 0  &&
		aCaster.GetTeamNum() != aTarget.GetTeamNum() )
	{
		if(  GetDistanceIn3D(  aCaster.GetTransform()._41,  aCaster.GetTransform()._42,  aCaster.GetTransform()._43,
								aTarget.GetTransform()._41,  aTarget.GetTransform()._42,  aTarget.GetTransform()._43  ) <=
								RANGE_UNIT * BreakerNormalAttack.GetRange()  )
		{
			if(  IsFrontOfThisIn2D(  aCaster.GetTransform(),  aTarget.GetTransform()  ) )
			{
				return	true;
			}
		}
	}

	return	false;
}


bool Matchless::IsBreakerRaidPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget )
{
	if( aCaster.GetCharacterInfo().GetCurrentHealth() > 0  &&  aTarget.GetCharacterInfo().GetCurrentHealth() > 0  &&
		aCaster.GetTeamNum() != aTarget.GetTeamNum() )
	{
		if(  GetDistanceIn3D(  aCaster.GetTransform()._41,  aCaster.GetTransform()._42,  aCaster.GetTransform()._43,
								aTarget.GetTransform()._41,  aTarget.GetTransform()._42,  aTarget.GetTransform()._43  ) <=
								RANGE_UNIT * BreakerNormalAttack.GetRange()  )
		{
			if(  IsFrontOfThisIn2D(  aCaster.GetTransform(),  aTarget.GetTransform()  ) )
			{
				return	true;
			}
		}
	}

	return	false;
}


bool Matchless::IsBreakerStunPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget )
{
	if( aCaster.GetCharacterInfo().GetCurrentHealth() > 0  &&  aTarget.GetCharacterInfo().GetCurrentHealth() > 0  &&
		aCaster.GetTeamNum() != aTarget.GetTeamNum() )
	{
		if(  GetDistanceIn3D(  aCaster.GetTransform()._41,  aCaster.GetTransform()._42,  aCaster.GetTransform()._43,
								aTarget.GetTransform()._41,  aTarget.GetTransform()._42,  aTarget.GetTransform()._43  ) <=
								RANGE_UNIT * BreakerNormalAttack.GetRange()  )
		{
			if(  IsFrontOfThisIn2D(  aCaster.GetTransform(),  aTarget.GetTransform()  ) )
			{
				return	true;
			}
		}
	}

	return	false;
}


bool Matchless::IsBreakerArmorBreakPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget )
{
	if( aCaster.GetCharacterInfo().GetCurrentHealth() > 0  &&  aTarget.GetCharacterInfo().GetCurrentHealth() > 0  &&
		aCaster.GetTeamNum() != aTarget.GetTeamNum() )
	{
		if(  GetDistanceIn3D(  aCaster.GetTransform()._41,  aCaster.GetTransform()._42,  aCaster.GetTransform()._43,
								aTarget.GetTransform()._41,  aTarget.GetTransform()._42,  aTarget.GetTransform()._43  ) <=
								RANGE_UNIT * BreakerNormalAttack.GetRange()  )
		{
			if(  IsFrontOfThisIn2D(  aCaster.GetTransform(),  aTarget.GetTransform()  ) )
			{
				return	true;
			}
		}
	}

	return	false;
}


bool Matchless::IsBreakerPowerBreakPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget )
{
	if( aCaster.GetCharacterInfo().GetCurrentHealth() > 0  &&  aTarget.GetCharacterInfo().GetCurrentHealth() > 0  &&
		aCaster.GetTeamNum() != aTarget.GetTeamNum() )
	{
		if(  GetDistanceIn3D(  aCaster.GetTransform()._41,  aCaster.GetTransform()._42,  aCaster.GetTransform()._43,
								aTarget.GetTransform()._41,  aTarget.GetTransform()._42,  aTarget.GetTransform()._43  ) <=
								RANGE_UNIT * BreakerNormalAttack.GetRange()  )
		{
			if(  IsFrontOfThisIn2D(  aCaster.GetTransform(),  aTarget.GetTransform()  ) )
			{
				return	true;
			}
		}
	}

	return	false;
}


bool Matchless::IsBreakerBerserkPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget )
{
	if( aCaster.GetCharacterInfo().GetCurrentHealth() > 0  &&  aTarget.GetCharacterInfo().GetCurrentHealth() > 0  &&
		aCaster.GetTeamNum() != aTarget.GetTeamNum() )
	{
		if(  GetDistanceIn3D(  aCaster.GetTransform()._41,  aCaster.GetTransform()._42,  aCaster.GetTransform()._43,
								aTarget.GetTransform()._41,  aTarget.GetTransform()._42,  aTarget.GetTransform()._43  ) <=
								RANGE_UNIT * BreakerNormalAttack.GetRange()  )
		{
			if(  IsFrontOfThisIn2D(  aCaster.GetTransform(),  aTarget.GetTransform()  ) )
			{
				return	true;
			}
		}
	}

	return	false;
}


bool Matchless::IsBreakerChargePossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget )
{
	if( aCaster.GetCharacterInfo().GetCurrentHealth() > 0  &&  aTarget.GetCharacterInfo().GetCurrentHealth() > 0  &&
		aCaster.GetTeamNum() != aTarget.GetTeamNum() )
	{
		if(  GetDistanceIn3D(  aCaster.GetTransform()._41,  aCaster.GetTransform()._42,  aCaster.GetTransform()._43,
								aTarget.GetTransform()._41,  aTarget.GetTransform()._42,  aTarget.GetTransform()._43  ) <=
								RANGE_UNIT * BreakerNormalAttack.GetRange()  )
		{
			if(  IsFrontOfThisIn2D(  aCaster.GetTransform(),  aTarget.GetTransform()  ) )
			{
				return	true;
			}
		}
	}

	return	false;
}


bool Matchless::IsBreakerSilentPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget )
{
	if( aCaster.GetCharacterInfo().GetCurrentHealth() > 0  &&  aTarget.GetCharacterInfo().GetCurrentHealth() > 0  &&
		aCaster.GetTeamNum() != aTarget.GetTeamNum() )
	{
		if(  GetDistanceIn3D(  aCaster.GetTransform()._41,  aCaster.GetTransform()._42,  aCaster.GetTransform()._43,
								aTarget.GetTransform()._41,  aTarget.GetTransform()._42,  aTarget.GetTransform()._43  ) <=
								RANGE_UNIT * BreakerNormalAttack.GetRange()  )
		{
			if(  IsFrontOfThisIn2D(  aCaster.GetTransform(),  aTarget.GetTransform()  ) )
			{
				return	true;
			}
		}
	}

	return	false;
}


bool Matchless::IsBreakerLastHitPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget )
{
	if( aCaster.GetCharacterInfo().GetCurrentHealth() > 0  &&  aTarget.GetCharacterInfo().GetCurrentHealth() > 0  &&
		aCaster.GetTeamNum() != aTarget.GetTeamNum() )
	{
		if(  GetDistanceIn3D(  aCaster.GetTransform()._41,  aCaster.GetTransform()._42,  aCaster.GetTransform()._43,
								aTarget.GetTransform()._41,  aTarget.GetTransform()._42,  aTarget.GetTransform()._43  ) <=
								RANGE_UNIT * BreakerNormalAttack.GetRange()  )
		{
			if(  IsFrontOfThisIn2D(  aCaster.GetTransform(),  aTarget.GetTransform()  ) )
			{
				return	true;
			}
		}
	}

	return	false;
}


bool Matchless::IsDefenderStoneSkinPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget )
{
	if( aCaster.GetCharacterInfo().GetCurrentHealth() > 0  &&  aTarget.GetCharacterInfo().GetCurrentHealth() > 0  &&
		aCaster.GetTeamNum() != aTarget.GetTeamNum() )
	{
		if(  GetDistanceIn3D(  aCaster.GetTransform()._41,  aCaster.GetTransform()._42,  aCaster.GetTransform()._43,
								aTarget.GetTransform()._41,  aTarget.GetTransform()._42,  aTarget.GetTransform()._43  ) <=
								RANGE_UNIT * BreakerNormalAttack.GetRange()  )
		{
			if(  IsFrontOfThisIn2D(  aCaster.GetTransform(),  aTarget.GetTransform()  ) )
			{
				return	true;
			}
		}
	}

	return	false;
}


bool Matchless::IsDefenderNaturalPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget )
{
	if( aCaster.GetCharacterInfo().GetCurrentHealth() > 0  &&  aTarget.GetCharacterInfo().GetCurrentHealth() > 0  &&
		aCaster.GetTeamNum() != aTarget.GetTeamNum() )
	{
		if(  GetDistanceIn3D(  aCaster.GetTransform()._41,  aCaster.GetTransform()._42,  aCaster.GetTransform()._43,
								aTarget.GetTransform()._41,  aTarget.GetTransform()._42,  aTarget.GetTransform()._43  ) <=
								RANGE_UNIT * BreakerNormalAttack.GetRange()  )
		{
			if(  IsFrontOfThisIn2D(  aCaster.GetTransform(),  aTarget.GetTransform()  ) )
			{
				return	true;
			}
		}
	}

	return	false;
}


bool Matchless::IsDefenderSacrificePossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget )
{
	if( aCaster.GetCharacterInfo().GetCurrentHealth() > 0  &&  aTarget.GetCharacterInfo().GetCurrentHealth() > 0  &&
		aCaster.GetTeamNum() != aTarget.GetTeamNum() )
	{
		if(  GetDistanceIn3D(  aCaster.GetTransform()._41,  aCaster.GetTransform()._42,  aCaster.GetTransform()._43,
								aTarget.GetTransform()._41,  aTarget.GetTransform()._42,  aTarget.GetTransform()._43  ) <=
								RANGE_UNIT * BreakerNormalAttack.GetRange()  )
		{
			if(  IsFrontOfThisIn2D(  aCaster.GetTransform(),  aTarget.GetTransform()  ) )
			{
				return	true;
			}
		}
	}

	return	false;
}


bool Matchless::IsDefenderAttackBreakPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget )
{
	if( aCaster.GetCharacterInfo().GetCurrentHealth() > 0  &&  aTarget.GetCharacterInfo().GetCurrentHealth() > 0  &&
		aCaster.GetTeamNum() != aTarget.GetTeamNum() )
	{
		if(  GetDistanceIn3D(  aCaster.GetTransform()._41,  aCaster.GetTransform()._42,  aCaster.GetTransform()._43,
								aTarget.GetTransform()._41,  aTarget.GetTransform()._42,  aTarget.GetTransform()._43  ) <=
								RANGE_UNIT * BreakerNormalAttack.GetRange()  )
		{
			if(  IsFrontOfThisIn2D(  aCaster.GetTransform(),  aTarget.GetTransform()  ) )
			{
				return	true;
			}
		}
	}

	return	false;
}


bool Matchless::IsDefenderHeavyBlowPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget )
{
	if( aCaster.GetCharacterInfo().GetCurrentHealth() > 0  &&  aTarget.GetCharacterInfo().GetCurrentHealth() > 0  &&
		aCaster.GetTeamNum() != aTarget.GetTeamNum() )
	{
		if(  GetDistanceIn3D(  aCaster.GetTransform()._41,  aCaster.GetTransform()._42,  aCaster.GetTransform()._43,
								aTarget.GetTransform()._41,  aTarget.GetTransform()._42,  aTarget.GetTransform()._43  ) <=
								RANGE_UNIT * BreakerNormalAttack.GetRange()  )
		{
			if(  IsFrontOfThisIn2D(  aCaster.GetTransform(),  aTarget.GetTransform()  ) )
			{
				return	true;
			}
		}
	}

	return	false;
}


bool Matchless::IsDefenderSpiritLinkPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget )
{
	if( aCaster.GetCharacterInfo().GetCurrentHealth() > 0  &&  aTarget.GetCharacterInfo().GetCurrentHealth() > 0  &&
		aCaster.GetTeamNum() != aTarget.GetTeamNum() )
	{
		if(  GetDistanceIn3D(  aCaster.GetTransform()._41,  aCaster.GetTransform()._42,  aCaster.GetTransform()._43,
								aTarget.GetTransform()._41,  aTarget.GetTransform()._42,  aTarget.GetTransform()._43  ) <=
								RANGE_UNIT * BreakerNormalAttack.GetRange()  )
		{
			if(  IsFrontOfThisIn2D(  aCaster.GetTransform(),  aTarget.GetTransform()  ) )
			{
				return	true;
			}
		}
	}

	return	false;
}


bool Matchless::IsDefenderEarthquakePossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget )
{
	if( aCaster.GetCharacterInfo().GetCurrentHealth() > 0  &&  aTarget.GetCharacterInfo().GetCurrentHealth() > 0  &&
		aCaster.GetTeamNum() != aTarget.GetTeamNum() )
	{
		if(  GetDistanceIn3D(  aCaster.GetTransform()._41,  aCaster.GetTransform()._42,  aCaster.GetTransform()._43,
								aTarget.GetTransform()._41,  aTarget.GetTransform()._42,  aTarget.GetTransform()._43  ) <=
								RANGE_UNIT * BreakerNormalAttack.GetRange()  )
		{
			if(  IsFrontOfThisIn2D(  aCaster.GetTransform(),  aTarget.GetTransform()  ) )
			{
				return	true;
			}
		}
	}

	return	false;
}


bool Matchless::IsDefenderFullThrowPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget )
{
	if( aCaster.GetCharacterInfo().GetCurrentHealth() > 0  &&  aTarget.GetCharacterInfo().GetCurrentHealth() > 0  &&
		aCaster.GetTeamNum() != aTarget.GetTeamNum() )
	{
		if(  GetDistanceIn3D(  aCaster.GetTransform()._41,  aCaster.GetTransform()._42,  aCaster.GetTransform()._43,
								aTarget.GetTransform()._41,  aTarget.GetTransform()._42,  aTarget.GetTransform()._43  ) <=
								RANGE_UNIT * BreakerNormalAttack.GetRange()  )
		{
			if(  IsFrontOfThisIn2D(  aCaster.GetTransform(),  aTarget.GetTransform()  ) )
			{
				return	true;
			}
		}
	}

	return	false;
}


bool Matchless::IsMageIceShieldPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget )
{
	if( aCaster.GetCharacterInfo().GetCurrentHealth() > 0  &&  aTarget.GetCharacterInfo().GetCurrentHealth() > 0  &&
		aCaster.GetTeamNum() != aTarget.GetTeamNum() )
	{
		if(  GetDistanceIn3D(  aCaster.GetTransform()._41,  aCaster.GetTransform()._42,  aCaster.GetTransform()._43,
								aTarget.GetTransform()._41,  aTarget.GetTransform()._42,  aTarget.GetTransform()._43  ) <=
								RANGE_UNIT * BreakerNormalAttack.GetRange()  )
		{
			if(  IsFrontOfThisIn2D(  aCaster.GetTransform(),  aTarget.GetTransform()  ) )
			{
				return	true;
			}
		}
	}

	return	false;
}


bool Matchless::IsMageMagicalArmorPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget )
{
	if( aCaster.GetCharacterInfo().GetCurrentHealth() > 0  &&  aTarget.GetCharacterInfo().GetCurrentHealth() > 0  &&
		aCaster.GetTeamNum() != aTarget.GetTeamNum() )
	{
		if(  GetDistanceIn3D(  aCaster.GetTransform()._41,  aCaster.GetTransform()._42,  aCaster.GetTransform()._43,
								aTarget.GetTransform()._41,  aTarget.GetTransform()._42,  aTarget.GetTransform()._43  ) <=
								RANGE_UNIT * BreakerNormalAttack.GetRange()  )
		{
			if(  IsFrontOfThisIn2D(  aCaster.GetTransform(),  aTarget.GetTransform()  ) )
			{
				return	true;
			}
		}
	}

	return	false;
}


bool Matchless::IsMageIceBoltPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget )
{
	if( aCaster.GetCharacterInfo().GetCurrentHealth() > 0  &&  aTarget.GetCharacterInfo().GetCurrentHealth() > 0  &&
		aCaster.GetTeamNum() != aTarget.GetTeamNum() )
	{
		if(  GetDistanceIn3D(  aCaster.GetTransform()._41,  aCaster.GetTransform()._42,  aCaster.GetTransform()._43,
								aTarget.GetTransform()._41,  aTarget.GetTransform()._42,  aTarget.GetTransform()._43  ) <=
								RANGE_UNIT * BreakerNormalAttack.GetRange()  )
		{
			if(  IsFrontOfThisIn2D(  aCaster.GetTransform(),  aTarget.GetTransform()  ) )
			{
				return	true;
			}
		}
	}

	return	false;
}


bool Matchless::IsMageFireBallPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget )
{
	if( aCaster.GetCharacterInfo().GetCurrentHealth() > 0  &&  aTarget.GetCharacterInfo().GetCurrentHealth() > 0  &&
		aCaster.GetTeamNum() != aTarget.GetTeamNum() )
	{
		if(  GetDistanceIn3D(  aCaster.GetTransform()._41,  aCaster.GetTransform()._42,  aCaster.GetTransform()._43,
								aTarget.GetTransform()._41,  aTarget.GetTransform()._42,  aTarget.GetTransform()._43  ) <=
								RANGE_UNIT * BreakerNormalAttack.GetRange()  )
		{
			if(  IsFrontOfThisIn2D(  aCaster.GetTransform(),  aTarget.GetTransform()  ) )
			{
				return	true;
			}
		}
	}

	return	false;
}


bool Matchless::IsMageBlizzardPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget )
{
	if( aCaster.GetCharacterInfo().GetCurrentHealth() > 0  &&  aTarget.GetCharacterInfo().GetCurrentHealth() > 0  &&
		aCaster.GetTeamNum() != aTarget.GetTeamNum() )
	{
		if(  GetDistanceIn3D(  aCaster.GetTransform()._41,  aCaster.GetTransform()._42,  aCaster.GetTransform()._43,
								aTarget.GetTransform()._41,  aTarget.GetTransform()._42,  aTarget.GetTransform()._43  ) <=
								RANGE_UNIT * BreakerNormalAttack.GetRange()  )
		{
			if(  IsFrontOfThisIn2D(  aCaster.GetTransform(),  aTarget.GetTransform()  ) )
			{
				return	true;
			}
		}
	}

	return	false;
}


bool Matchless::IsMageFairyRescuePossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget )
{
	if( aCaster.GetCharacterInfo().GetCurrentHealth() > 0  &&  aTarget.GetCharacterInfo().GetCurrentHealth() > 0  &&
		aCaster.GetTeamNum() != aTarget.GetTeamNum() )
	{
		if(  GetDistanceIn3D(  aCaster.GetTransform()._41,  aCaster.GetTransform()._42,  aCaster.GetTransform()._43,
								aTarget.GetTransform()._41,  aTarget.GetTransform()._42,  aTarget.GetTransform()._43  ) <=
								RANGE_UNIT * BreakerNormalAttack.GetRange()  )
		{
			if(  IsFrontOfThisIn2D(  aCaster.GetTransform(),  aTarget.GetTransform()  ) )
			{
				return	true;
			}
		}
	}

	return	false;
}


bool Matchless::IsMageChainLightningPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget )
{
	if( aCaster.GetCharacterInfo().GetCurrentHealth() > 0  &&  aTarget.GetCharacterInfo().GetCurrentHealth() > 0  &&
		aCaster.GetTeamNum() != aTarget.GetTeamNum() )
	{
		if(  GetDistanceIn3D(  aCaster.GetTransform()._41,  aCaster.GetTransform()._42,  aCaster.GetTransform()._43,
								aTarget.GetTransform()._41,  aTarget.GetTransform()._42,  aTarget.GetTransform()._43  ) <=
								RANGE_UNIT * BreakerNormalAttack.GetRange()  )
		{
			if(  IsFrontOfThisIn2D(  aCaster.GetTransform(),  aTarget.GetTransform()  ) )
			{
				return	true;
			}
		}
	}

	return	false;
}


bool Matchless::IsMageLightningEmissionPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget )
{
	if( aCaster.GetCharacterInfo().GetCurrentHealth() > 0  &&  aTarget.GetCharacterInfo().GetCurrentHealth() > 0  &&
		aCaster.GetTeamNum() != aTarget.GetTeamNum() )
	{
		if(  GetDistanceIn3D(  aCaster.GetTransform()._41,  aCaster.GetTransform()._42,  aCaster.GetTransform()._43,
								aTarget.GetTransform()._41,  aTarget.GetTransform()._42,  aTarget.GetTransform()._43  ) <=
								RANGE_UNIT * BreakerNormalAttack.GetRange()  )
		{
			if(  IsFrontOfThisIn2D(  aCaster.GetTransform(),  aTarget.GetTransform()  ) )
			{
				return	true;
			}
		}
	}

	return	false;
}


bool Matchless::IsHealerPerfectPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget )
{
	if( aCaster.GetCharacterInfo().GetCurrentHealth() > 0  &&  aTarget.GetCharacterInfo().GetCurrentHealth() > 0  &&
		aCaster.GetTeamNum() != aTarget.GetTeamNum() )
	{
		if(  GetDistanceIn3D(  aCaster.GetTransform()._41,  aCaster.GetTransform()._42,  aCaster.GetTransform()._43,
								aTarget.GetTransform()._41,  aTarget.GetTransform()._42,  aTarget.GetTransform()._43  ) <=
								RANGE_UNIT * BreakerNormalAttack.GetRange()  )
		{
			if(  IsFrontOfThisIn2D(  aCaster.GetTransform(),  aTarget.GetTransform()  ) )
			{
				return	true;
			}
		}
	}

	return	false;
}


bool Matchless::IsHealerQuickHealPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget )
{
	if( aCaster.GetCharacterInfo().GetCurrentHealth() > 0  &&  aTarget.GetCharacterInfo().GetCurrentHealth() > 0  &&
		aCaster.GetTeamNum() == aTarget.GetTeamNum()  &&  HealerQuickHeal.GetEnergyCost() <= aCaster.GetCharacterInfo().GetCurrentEnergy() )
	{
		if(  GetDistanceIn3D(  aCaster.GetTransform()._41,  aCaster.GetTransform()._42,  aCaster.GetTransform()._43,
								aTarget.GetTransform()._41,  aTarget.GetTransform()._42,  aTarget.GetTransform()._43  ) <=
								RANGE_UNIT * BreakerNormalAttack.GetRange()  )
		{
			if(  IsFrontOfThisIn2D(  aCaster.GetTransform(),  aTarget.GetTransform()  ) )
			{
				return	true;
			}
		}
	}

	return	false;
}


bool Matchless::IsHealerNormalHealPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget )
{
	if( aCaster.GetCharacterInfo().GetCurrentHealth() > 0  &&  aTarget.GetCharacterInfo().GetCurrentHealth() > 0  &&
		aCaster.GetTeamNum() != aTarget.GetTeamNum() )
	{
		if(  GetDistanceIn3D(  aCaster.GetTransform()._41,  aCaster.GetTransform()._42,  aCaster.GetTransform()._43,
								aTarget.GetTransform()._41,  aTarget.GetTransform()._42,  aTarget.GetTransform()._43  ) <=
								RANGE_UNIT * BreakerNormalAttack.GetRange()  )
		{
			if(  IsFrontOfThisIn2D(  aCaster.GetTransform(),  aTarget.GetTransform()  ) )
			{
				return	true;
			}
		}
	}

	return	false;
}


bool Matchless::IsHealerLandBlessingPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget )
{
	if( aCaster.GetCharacterInfo().GetCurrentHealth() > 0  &&  aTarget.GetCharacterInfo().GetCurrentHealth() > 0  &&
		aCaster.GetTeamNum() != aTarget.GetTeamNum() )
	{
		if(  GetDistanceIn3D(  aCaster.GetTransform()._41,  aCaster.GetTransform()._42,  aCaster.GetTransform()._43,
								aTarget.GetTransform()._41,  aTarget.GetTransform()._42,  aTarget.GetTransform()._43  ) <=
								RANGE_UNIT * BreakerNormalAttack.GetRange()  )
		{
			if(  IsFrontOfThisIn2D(  aCaster.GetTransform(),  aTarget.GetTransform()  ) )
			{
				return	true;
			}
		}
	}

	return	false;
}


bool Matchless::IsHealerDispelPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget )
{
	if( aCaster.GetCharacterInfo().GetCurrentHealth() > 0  &&  aTarget.GetCharacterInfo().GetCurrentHealth() > 0  &&
		aCaster.GetTeamNum() != aTarget.GetTeamNum() )
	{
		if(  GetDistanceIn3D(  aCaster.GetTransform()._41,  aCaster.GetTransform()._42,  aCaster.GetTransform()._43,
								aTarget.GetTransform()._41,  aTarget.GetTransform()._42,  aTarget.GetTransform()._43  ) <=
								RANGE_UNIT * BreakerNormalAttack.GetRange()  )
		{
			if(  IsFrontOfThisIn2D(  aCaster.GetTransform(),  aTarget.GetTransform()  ) )
			{
				return	true;
			}
		}
	}

	return	false;
}


bool Matchless::IsHealerFreePromisePossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget )
{
	if( aCaster.GetCharacterInfo().GetCurrentHealth() > 0  &&  aTarget.GetCharacterInfo().GetCurrentHealth() > 0  &&
		aCaster.GetTeamNum() != aTarget.GetTeamNum() )
	{
		if(  GetDistanceIn3D(  aCaster.GetTransform()._41,  aCaster.GetTransform()._42,  aCaster.GetTransform()._43,
								aTarget.GetTransform()._41,  aTarget.GetTransform()._42,  aTarget.GetTransform()._43  ) <=
								RANGE_UNIT * BreakerNormalAttack.GetRange()  )
		{
			if(  IsFrontOfThisIn2D(  aCaster.GetTransform(),  aTarget.GetTransform()  ) )
			{
				return	true;
			}
		}
	}

	return	false;
}


bool Matchless::IsHealerSoulCryPossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget )
{
	if( aCaster.GetCharacterInfo().GetCurrentHealth() > 0  &&  aTarget.GetCharacterInfo().GetCurrentHealth() > 0  &&
		aCaster.GetTeamNum() != aTarget.GetTeamNum() )
	{
		if(  GetDistanceIn3D(  aCaster.GetTransform()._41,  aCaster.GetTransform()._42,  aCaster.GetTransform()._43,
								aTarget.GetTransform()._41,  aTarget.GetTransform()._42,  aTarget.GetTransform()._43  ) <=
								RANGE_UNIT * BreakerNormalAttack.GetRange()  )
		{
			if(  IsFrontOfThisIn2D(  aCaster.GetTransform(),  aTarget.GetTransform()  ) )
			{
				return	true;
			}
		}
	}

	return	false;
}


bool Matchless::IsHealerDischargePossible( CPlayerInfo & aCaster, CPlayerInfo & aTarget )
{
	if( aCaster.GetCharacterInfo().GetCurrentHealth() > 0  &&  aTarget.GetCharacterInfo().GetCurrentHealth() > 0  &&
		aCaster.GetTeamNum() != aTarget.GetTeamNum() )
	{
		if(  GetDistanceIn3D(  aCaster.GetTransform()._41,  aCaster.GetTransform()._42,  aCaster.GetTransform()._43,
								aTarget.GetTransform()._41,  aTarget.GetTransform()._42,  aTarget.GetTransform()._43  ) <=
								RANGE_UNIT * BreakerNormalAttack.GetRange()  )
		{
			if(  IsFrontOfThisIn2D(  aCaster.GetTransform(),  aTarget.GetTransform()  ) )
			{
				return	true;
			}
		}
	}

	return	false;
}
