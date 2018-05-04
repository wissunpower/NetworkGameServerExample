
#include	"stdafx.h"
#include	"ClientAppRoot.h"
#include	"cPacket.h"



//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
#ifdef	CS_TESTCODE_ON
DebugConsole				g_Console;
#endif

std::list< int >			g_CSHistory;

ID3DXSprite *				g_pd3dSprite = NULL;
ID3DXFont *					g_pFont = NULL;
ID3DXSprite *				g_pTextSprite = NULL;
CD3DArcBall					g_ArcBall;
CDXUTDialogResourceManager	g_DialogResourceManager;
CD3DSettingsDlg				g_SettingsDlg;
CDXUTDialog					g_SystemUI;
CDXUTDialog					g_GlobalUI;

CDXUTDialog					g_MainStepStartUI;
CDXUTDialog					g_MainStepWaitUI;
CDXUTDialog					g_MainStepPlayUI;
CDXUTDialog					g_MainStepAdjustUI;
CDXUTDialog					g_MainStepEndUI;

Matchless::Timer			g_Timer( 60 );

CCamera						g_Camera;

tstring						g_Notice;

std::wstring				g_ConnectIP;

cMonitor										g_Monitor;

Matchless::CClient								g_ThisClient;
CAnimateMesh									g_ThisCharacter;
std::map< unsigned int, Matchless::CClient >	g_AnotherClientList;
std::map< unsigned int, CAnimateMesh >			g_AnotherCharacterList;

bool											g_IsGameStartable = false;
bool											g_IsGraphicResourceLoaded = false;


bool						g_IsFrameUpdated = false;
bool						g_IsUpdateMovePosition = false;
bool						g_IsUpdateMoveAnimation = false;
unsigned int				g_TargetID = 0;

bool						g_IsNowCasting = false;
unsigned int				g_CastStartTick = 0;
unsigned int				g_CastEndTick = 0;


CAnimateMesh	g_BreakerMesh;
CAnimateMesh	g_DefenderMesh;
CAnimateMesh	g_MageMesh;
CAnimateMesh	g_HealerMesh;

CAnimateMesh *	g_pSelectCharacter = NULL;


LPD3DXMESH					g_pTerrain;
D3DXMATRIX					g_TerrainTransform;
DWORD						g_NumTerrainMaterial;
D3DMATERIAL9 *				g_pTerrainMaterial = NULL;
LPDIRECT3DTEXTURE9 *		g_ppTerrainTexture = NULL;

CSkyBox						g_SkyBox;
CBackground					g_StartBackground;
CBackground					g_WaitBackground;
CBackground					g_AdjustBackground;

LPD3DXMESH					g_pTargetCharacterMark;

LPD3DXMESH					g_pHomeTeamMark;
DWORD						g_NumHomeTeamMarkMaterial;
D3DMATERIAL9 *				g_pHomeTeamMarkMaterial;
LPDIRECT3DTEXTURE9 *		g_ppHomeTeamMarkTexture;

LPD3DXMESH					g_pAwayTeamMark;
DWORD						g_NumAwayTeamMarkMaterial;
D3DMATERIAL9 *				g_pAwayTeamMarkMaterial;
LPDIRECT3DTEXTURE9 *		g_ppAwayTeamMarkTexture;


std::list< Matchless::SEffect >		g_EffectList;

CEffectBillboard					g_BreakerNormalAttackEffect;
CEffectBillboard					g_BreakerRaidEffect;
CEffectBillboard					g_BreakerStunEffect;
CEffectBillboard					g_BreakerArmorBreakEffect;
CEffectBillboard					g_BreakerPowerBreakEffect;
CEffectBillboard					g_BreakerBerserkEffect;
CEffectBillboard					g_BreakerChargeEffect;
CEffectBillboard					g_BreakerSilentEffect;
CEffectBillboard					g_BreakerLastHitEffect;

CEffectBillboard					g_DefenderNormalAttackEffect;
CEffectBillboard					g_DefenderStoneSkinEffect;
CEffectBillboard					g_DefenderNaturalEffect;
CEffectBillboard					g_DefenderSacrificeEffect;
CEffectBillboard					g_DefenderAttackBreakEffect;
CEffectBillboard					g_DefenderHeavyBlowEffect;
CEffectBillboard					g_DefenderSpiritLinkEffect;
CEffectBillboard					g_DefenderEarthquakeEffect;
CEffectBillboard					g_DefenderFullThrowEffect;

CEffectBillboard					g_MageNormalAttackEffect;
CEffectBillboard					g_MageIceShieldEffect;
CEffectBillboard					g_MageMagicalArmorEffect;
CEffectBillboard					g_MageIceBoltEffect;
CEffectBillboard					g_MageFireBallEffect;
CEffectBillboard					g_MageBlizzardEffect;
CEffectBillboard					g_MageFairyRescueEffect;
CEffectBillboard					g_MageChainLightningEffect;
CEffectBillboard					g_MageLightningEmissionEffect;

CEffectBillboard					g_HealerNormalAttackEffect;
CEffectBillboard					g_HealerPerfectEffect;
CEffectBillboard					g_HealerQuickHealEffect;
CEffectBillboard					g_HealerNormalHealEffect;
CEffectBillboard					g_HealerLandBlessingEffect;
CEffectBillboard					g_HealerDispelEffect;
CEffectBillboard					g_HealerFreePromiseEffect;
CEffectBillboard					g_HealerSoulCryEffect;
CEffectBillboard					g_HealerDischargeEffect;


LPDIRECT3DTEXTURE9	g_pTexBreakerEmblem = NULL;
LPDIRECT3DTEXTURE9	g_pTexDefenderEmblem = NULL;
LPDIRECT3DTEXTURE9	g_pTexMageEmblem = NULL;
LPDIRECT3DTEXTURE9	g_pTexHealerEmblem = NULL;

LPDIRECT3DTEXTURE9	g_pTexTerrain1Emblem = NULL;
LPDIRECT3DTEXTURE9	g_pTexTerrain2Emblem = NULL;

LPDIRECT3DTEXTURE9	g_pTexGaugePanel = NULL;
LPDIRECT3DTEXTURE9	g_pTexSkillPanel = NULL;
LPDIRECT3DTEXTURE9	g_pTexHealthGauge = NULL;
LPDIRECT3DTEXTURE9	g_pTexEnergyGauge = NULL;

LPDIRECT3DTEXTURE9	g_pTexGameLeaveButton = NULL;
LPDIRECT3DTEXTURE9	g_pTexGameStartButton = NULL;
LPDIRECT3DTEXTURE9	g_pTexAwayPartyWord = NULL;
LPDIRECT3DTEXTURE9	g_pTexHomePartyWord = NULL;
LPDIRECT3DTEXTURE9	g_pTexPartyInfoFrame = NULL;
LPDIRECT3DTEXTURE9	g_pTexAnCharInfoFrame = NULL;
LPDIRECT3DTEXTURE9	g_pTexAnEnergyBar = NULL;
LPDIRECT3DTEXTURE9	g_pTexAnEnergyGauge = NULL;
LPDIRECT3DTEXTURE9	g_pTexAnHealthBar = NULL;
LPDIRECT3DTEXTURE9	g_pTexAnHealthGauge = NULL;

LPDIRECT3DTEXTURE9	g_pTexCastBarGauge = NULL;
LPDIRECT3DTEXTURE9	g_pTexCastBarGrowth = NULL;

LPDIRECT3DTEXTURE9		g_pTexBreakerNormalAttackIcon = NULL;
LPDIRECT3DTEXTURE9		g_pTexBreakerRaidIcon = NULL;
LPDIRECT3DTEXTURE9		g_pTexBreakerStunIcon = NULL;
LPDIRECT3DTEXTURE9		g_pTexBreakerArmorBreakIcon = NULL;
LPDIRECT3DTEXTURE9		g_pTexBreakerPowerBreakIcon = NULL;
LPDIRECT3DTEXTURE9		g_pTexBreakerBerserkIcon = NULL;
LPDIRECT3DTEXTURE9		g_pTexBreakerChargeIcon = NULL;
LPDIRECT3DTEXTURE9		g_pTexBreakerSilentIcon = NULL;
LPDIRECT3DTEXTURE9		g_pTexBreakerLastHitIcon = NULL;

LPDIRECT3DTEXTURE9		g_pTexDefenderNormalAttackIcon = NULL;
LPDIRECT3DTEXTURE9		g_pTexDefenderStoneSkinIcon = NULL;
LPDIRECT3DTEXTURE9		g_pTexDefenderNaturalIcon = NULL;
LPDIRECT3DTEXTURE9		g_pTexDefenderSacrificeIcon = NULL;
LPDIRECT3DTEXTURE9		g_pTexDefenderAttackBreakIcon = NULL;
LPDIRECT3DTEXTURE9		g_pTexDefenderHeavyBlowIcon = NULL;
LPDIRECT3DTEXTURE9		g_pTexDefenderSpiritLinkIcon = NULL;
LPDIRECT3DTEXTURE9		g_pTexDefenderEarthquakeIcon = NULL;
LPDIRECT3DTEXTURE9		g_pTexDefenderFullThrowIcon = NULL;

LPDIRECT3DTEXTURE9		g_pTexMageNormalAttackIcon = NULL;
LPDIRECT3DTEXTURE9		g_pTexMageIceShieldIcon = NULL;
LPDIRECT3DTEXTURE9		g_pTexMageMagicalArmorIcon = NULL;
LPDIRECT3DTEXTURE9		g_pTexMageIceBoltIcon = NULL;
LPDIRECT3DTEXTURE9		g_pTexMageFireBallIcon = NULL;
LPDIRECT3DTEXTURE9		g_pTexMageBlizzardIcon = NULL;
LPDIRECT3DTEXTURE9		g_pTexMageFairyRescueIcon = NULL;
LPDIRECT3DTEXTURE9		g_pTexMageChainLightningIcon = NULL;
LPDIRECT3DTEXTURE9		g_pTexMageLightningEmissionIcon = NULL;

LPDIRECT3DTEXTURE9		g_pTexHealerNormalAttackIcon = NULL;
LPDIRECT3DTEXTURE9		g_pTexHealerPerfectIcon = NULL;
LPDIRECT3DTEXTURE9		g_pTexHealerQuickHealIcon = NULL;
LPDIRECT3DTEXTURE9		g_pTexHealerNormalHealIcon = NULL;
LPDIRECT3DTEXTURE9		g_pTexHealerLandBlessingIcon = NULL;
LPDIRECT3DTEXTURE9		g_pTexHealerDispelIcon = NULL;
LPDIRECT3DTEXTURE9		g_pTexHealerFreePromiseIcon = NULL;
LPDIRECT3DTEXTURE9		g_pTexHealerSoulCryIcon = NULL;
LPDIRECT3DTEXTURE9		g_pTexHealerDischargeIcon = NULL;

D3DXIMAGE_INFO		g_ImgInfoHealthGauge;
D3DXIMAGE_INFO		g_ImgInfoEnergyGauge;

D3DXIMAGE_INFO		g_ImgInfoAnEnergyBar;
D3DXIMAGE_INFO		g_ImgInfoAnHealthBar;

D3DXIMAGE_INFO		g_ImgCastBarGauge;
D3DXIMAGE_INFO		g_ImgCastBarGrowth;

unsigned int		g_CurrentMapKind = 0;


CSoundManager		g_SoundManager;

CSound *			g_pBackgroundSound;

CSound *			g_pBreakerNormalAttackSound;

CSound *			g_pDefenderNormalAttackSound;

CSound *			g_pMageNormalAttackSound;

CSound *			g_pHealerNormalAttackSound;
CSound *			g_pHealerQuickHealSound;



bool UpdateRoomMasterUI( const Matchless::EMainStepState aState, const bool aOnOff )
{
	switch( aState )
	{

	case Matchless::EMSS_Wait:
		g_MainStepWaitUI.GetControl( IDC_WAIT_PLAYSTART )->SetEnabled( aOnOff );
		g_MainStepWaitUI.GetControl( IDC_WAIT_MAPSELECT_PREV )->SetEnabled( aOnOff );
		g_MainStepWaitUI.GetControl( IDC_WAIT_MAPSELECT_NEXT )->SetEnabled( aOnOff );
		break;

	case Matchless::EMSS_Play:
		g_MainStepPlayUI.GetControl( IDC_PLAY_PLAYQUIT )->SetEnabled( aOnOff );
		break;

	}

	return	true;
}


bool HandleSkillCommand( const Matchless::ECharacterClass aClass, const unsigned int aSkillKind, const unsigned int aCurrentTarget )
{
	unsigned int				bufIndex = 0;
	CNetMessage					tempMessage;
	Matchless::ECharacterSkill	tempECS = Matchless::ECS_Idle;
	Matchless::CClient *		pTarget = NULL;


	{
		cMonitor::Owner lock{ g_Monitor };
		if ( g_AnotherClientList.end() != g_AnotherClientList.find( aCurrentTarget ) )
		{
			pTarget = ( &g_AnotherClientList[ aCurrentTarget ] );
		}
		else if ( g_ThisClient.m_NetSystem.GetID() == aCurrentTarget )
		{
			pTarget = ( &g_ThisClient );
		}
		else
		{
			return	false;
		}
	}

	switch( aClass )
	{


	case Matchless::ECC_Breaker:
		switch( aSkillKind )
		{
		case 1:
			if( Matchless::IsSkillPossible( Matchless::BreakerNormalAttack, g_ThisClient.m_PlayerInfo, pTarget->m_PlayerInfo ) )
			{
				tempECS = Matchless::ECS_Breaker_NormalAttack;

				cOPacket oPacket;
				oPacket.Encode4u( Matchless::FCTS_GAME_SKILL_REQUEST );
				oPacket.Encode4u( tempECS );
				oPacket.Encode4u( aCurrentTarget );
				oPacket.Send( g_ThisClient.m_NetSystem.GetSocket() );
			}
			break;
		case 2:
			if( Matchless::IsSkillPossible( Matchless::BreakerRaid, g_ThisClient.m_PlayerInfo, pTarget->m_PlayerInfo ) )
			{
				tempECS = Matchless::ECS_Breaker_Raid;

				cOPacket oPacket;
				oPacket.Encode4u( Matchless::FCTS_GAME_SKILL_REQUEST );
				oPacket.Encode4u( tempECS );
				oPacket.Encode4u( aCurrentTarget );
				oPacket.Send( g_ThisClient.m_NetSystem.GetSocket() );
			}
			break;
		case 3:
			if( Matchless::IsSkillPossible( Matchless::BreakerStun, g_ThisClient.m_PlayerInfo, pTarget->m_PlayerInfo ) )
			{
				tempECS = Matchless::ECS_Breaker_Stun;

				cOPacket oPacket;
				oPacket.Encode4u( Matchless::FCTS_GAME_SKILL_REQUEST );
				oPacket.Encode4u( tempECS );
				oPacket.Encode4u( aCurrentTarget );
				oPacket.Send( g_ThisClient.m_NetSystem.GetSocket() );
			}
			break;
		case 4:
			if( Matchless::IsSkillPossible( Matchless::BreakerArmorBreak, g_ThisClient.m_PlayerInfo, pTarget->m_PlayerInfo ) )
			{
				tempECS = Matchless::ECS_Breaker_ArmorBreak;

				cOPacket oPacket;
				oPacket.Encode4u( Matchless::FCTS_GAME_SKILL_REQUEST );
				oPacket.Encode4u( tempECS );
				oPacket.Encode4u( aCurrentTarget );
				oPacket.Send( g_ThisClient.m_NetSystem.GetSocket() );
			}
			break;
		case 5:
			if( Matchless::IsSkillPossible( Matchless::BreakerPowerBreak, g_ThisClient.m_PlayerInfo, pTarget->m_PlayerInfo ) )
			{
				tempECS = Matchless::ECS_Breaker_PowerBreak;

				cOPacket oPacket;
				oPacket.Encode4u( Matchless::FCTS_GAME_SKILL_REQUEST );
				oPacket.Encode4u( tempECS );
				oPacket.Encode4u( aCurrentTarget );
				oPacket.Send( g_ThisClient.m_NetSystem.GetSocket() );
			}
			break;
		case 6:
			break;
		case 7:
			break;
		case 8:
			if( Matchless::IsSkillPossible( Matchless::BreakerSilent, g_ThisClient.m_PlayerInfo, pTarget->m_PlayerInfo ) )
			{
				tempECS = Matchless::ECS_Breaker_Silent;

				cOPacket oPacket;
				oPacket.Encode4u( Matchless::FCTS_GAME_SKILL_REQUEST );
				oPacket.Encode4u( tempECS );
				oPacket.Encode4u( aCurrentTarget );
				oPacket.Send( g_ThisClient.m_NetSystem.GetSocket() );
			}
			break;
		case 9:
			break;
		case 0:
			break;
		}
		break;


	case Matchless::ECC_Defender:
		switch( aSkillKind )
		{
		case 1:
			if( Matchless::IsSkillPossible( Matchless::BreakerNormalAttack, g_ThisClient.m_PlayerInfo, pTarget->m_PlayerInfo ) )
			{
				tempECS = Matchless::ECS_Defender_NormalAttack;

				cOPacket oPacket;
				oPacket.Encode4u( Matchless::FCTS_GAME_SKILL_REQUEST );
				oPacket.Encode4u( tempECS );
				oPacket.Encode4u( aCurrentTarget );
				oPacket.Send( g_ThisClient.m_NetSystem.GetSocket() );
			}
			break;
		case 2:
			if( Matchless::IsSkillPossible( Matchless::DefenderStoneSkin, g_ThisClient.m_PlayerInfo, pTarget->m_PlayerInfo ) )
			{
				tempECS = Matchless::ECS_Defender_StoneSkin;

				cOPacket oPacket;
				oPacket.Encode4u( Matchless::FCTS_GAME_SKILL_REQUEST );
				oPacket.Encode4u( tempECS );
				oPacket.Encode4u( aCurrentTarget );
				oPacket.Send( g_ThisClient.m_NetSystem.GetSocket() );
			}
			break;
		case 3:
			if( Matchless::IsSkillPossible( Matchless::DefenderNatural, g_ThisClient.m_PlayerInfo, pTarget->m_PlayerInfo ) )
			{
				tempECS = Matchless::ECS_Defender_Natural;

				cOPacket oPacket;
				oPacket.Encode4u( Matchless::FCTS_GAME_SKILL_REQUEST );
				oPacket.Encode4u( tempECS );
				oPacket.Encode4u( aCurrentTarget );
				oPacket.Send( g_ThisClient.m_NetSystem.GetSocket() );
			}
			break;
		case 4:
			break;
		case 5:
			if( Matchless::IsSkillPossible( Matchless::DefenderAttackBreak, g_ThisClient.m_PlayerInfo, pTarget->m_PlayerInfo ) )
			{
				tempECS = Matchless::ECS_Defender_AttackBreak;

				cOPacket oPacket;
				oPacket.Encode4u( Matchless::FCTS_GAME_SKILL_REQUEST );
				oPacket.Encode4u( tempECS );
				oPacket.Encode4u( aCurrentTarget );
				oPacket.Send( g_ThisClient.m_NetSystem.GetSocket() );
			}
			break;
		case 6:
			if( Matchless::IsSkillPossible( Matchless::DefenderHeavyBlow, g_ThisClient.m_PlayerInfo, pTarget->m_PlayerInfo ) )
			{
				tempECS = Matchless::ECS_Defender_HeavyBlow;

				cOPacket oPacket;
				oPacket.Encode4u( Matchless::FCTS_GAME_SKILL_REQUEST );
				oPacket.Encode4u( tempECS );
				oPacket.Encode4u( aCurrentTarget );
				oPacket.Send( g_ThisClient.m_NetSystem.GetSocket() );
			}
			break;
		case 7:
			break;
		case 8:
			break;
		case 9:
			break;
		case 0:
			break;
		}
		break;


	case Matchless::ECC_Mage:
		switch( aSkillKind )
		{
		case 1:
			if( Matchless::IsSkillPossible( Matchless::BreakerNormalAttack, g_ThisClient.m_PlayerInfo, pTarget->m_PlayerInfo ) )
			{
				tempECS = Matchless::ECS_Mage_NormalAttack;

				cOPacket oPacket;
				oPacket.Encode4u( Matchless::FCTS_GAME_SKILL_REQUEST );
				oPacket.Encode4u( tempECS );
				oPacket.Encode4u( aCurrentTarget );
				oPacket.Send( g_ThisClient.m_NetSystem.GetSocket() );
			}
			break;
		case 2:
			break;
		case 3:
			break;
		case 4:
			if( Matchless::IsSkillPossible( Matchless::MageIceBolt, g_ThisClient.m_PlayerInfo, pTarget->m_PlayerInfo ) )
			{
				tempECS = Matchless::ECS_Mage_IceBolt;

				cOPacket oPacket;
				oPacket.Encode4u( Matchless::FCTS_GAME_SKILL_REQUEST );
				oPacket.Encode4u( tempECS );
				oPacket.Encode4u( aCurrentTarget );
				oPacket.Send( g_ThisClient.m_NetSystem.GetSocket() );
			}
			break;
		case 5:
			if( Matchless::IsSkillPossible( Matchless::MageFireBall, g_ThisClient.m_PlayerInfo, pTarget->m_PlayerInfo ) )
			{
				tempECS = Matchless::ECS_Mage_FireBall;

				cOPacket oPacket;
				oPacket.Encode4u( Matchless::FCTS_GAME_SKILL_REQUEST );
				oPacket.Encode4u( tempECS );
				oPacket.Encode4u( aCurrentTarget );
				oPacket.Send( g_ThisClient.m_NetSystem.GetSocket() );
			}
			break;
		case 6:
			break;
		case 7:
			break;
		case 8:
			break;
		case 9:
			break;
		case 0:
			break;
		}
		break;


	case Matchless::ECC_Healer:
		switch( aSkillKind )
		{
		case 1:
			if( Matchless::IsSkillPossible( Matchless::BreakerNormalAttack, g_ThisClient.m_PlayerInfo, pTarget->m_PlayerInfo ) )
			{
				tempECS = Matchless::ECS_Healer_NormalAttack;

				cOPacket oPacket;
				oPacket.Encode4u( Matchless::FCTS_GAME_SKILL_REQUEST );
				oPacket.Encode4u( tempECS );
				oPacket.Encode4u( aCurrentTarget );
				oPacket.Send( g_ThisClient.m_NetSystem.GetSocket() );
			}
			break;
		case 2:
			if( Matchless::IsSkillPossible( Matchless::HealerPerfect, g_ThisClient.m_PlayerInfo, pTarget->m_PlayerInfo ) )
			{
				tempECS = Matchless::ECS_Healer_Perfect;

				cOPacket oPacket;
				oPacket.Encode4u( Matchless::FCTS_GAME_SKILL_REQUEST );
				oPacket.Encode4u( tempECS );
				oPacket.Encode4u( aCurrentTarget );
				oPacket.Send( g_ThisClient.m_NetSystem.GetSocket() );
			}
			break;
		case 3:
			if( Matchless::IsSkillPossible( Matchless::HealerQuickHeal, g_ThisClient.m_PlayerInfo, pTarget->m_PlayerInfo ) )
			{
				tempECS = Matchless::ECS_Healer_QuickHeal;

				cOPacket oPacket;
				oPacket.Encode4u( Matchless::FCTS_GAME_SKILL_REQUEST );
				oPacket.Encode4u( tempECS );
				oPacket.Encode4u( aCurrentTarget );
				oPacket.Send( g_ThisClient.m_NetSystem.GetSocket() );
			}
			break;
		case 4:
			if( Matchless::IsSkillPossible( Matchless::HealerNormalHeal, g_ThisClient.m_PlayerInfo, pTarget->m_PlayerInfo ) )
			{
				tempECS = Matchless::ECS_Healer_NormalHeal;

				cOPacket oPacket;
				oPacket.Encode4u( Matchless::FCTS_GAME_SKILL_REQUEST );
				oPacket.Encode4u( tempECS );
				oPacket.Encode4u( aCurrentTarget );
				oPacket.Send( g_ThisClient.m_NetSystem.GetSocket() );
			}
			break;
		case 5:
			break;
		case 6:
			break;
		case 7:
			break;
		case 8:
			break;
		case 9:
			break;
		case 0:
			break;
		}
		break;


	}


	return	true;
}


bool LoadSound( void )
{
	TCHAR	tempFilePath[ MAX_PATH ];


	g_SoundManager.Initialize( DXUTGetHWND(), DSSCL_PRIORITY );

	DXUTFindDXSDKMediaFileCch( tempFilePath, MAX_PATH, TEXT( "Sound/Background.wav" ) );
	g_SoundManager.Create( &g_pBackgroundSound, tempFilePath, DSBCAPS_CTRLVOLUME );

	DXUTFindDXSDKMediaFileCch( tempFilePath, MAX_PATH, TEXT( "Sound/BreakerNormalAttack.wav" ) );
	g_SoundManager.Create( &g_pBreakerNormalAttackSound, tempFilePath, DSBCAPS_CTRLVOLUME );

	DXUTFindDXSDKMediaFileCch( tempFilePath, MAX_PATH, TEXT( "Sound/DefenderNormalAttack.wav" ) );
	g_SoundManager.Create( &g_pDefenderNormalAttackSound, tempFilePath, DSBCAPS_CTRLVOLUME );

	DXUTFindDXSDKMediaFileCch( tempFilePath, MAX_PATH, TEXT( "Sound/MageNormalAttack.wav" ) );
	g_SoundManager.Create( &g_pMageNormalAttackSound, tempFilePath, DSBCAPS_CTRLVOLUME );

	DXUTFindDXSDKMediaFileCch( tempFilePath, MAX_PATH, TEXT( "Sound/HealerNormalAttack.wav" ) );
	g_SoundManager.Create( &g_pHealerNormalAttackSound, tempFilePath, DSBCAPS_CTRLVOLUME );

	DXUTFindDXSDKMediaFileCch( tempFilePath, MAX_PATH, TEXT( "Sound/HealerQuickHeal.wav" ) );
	g_SoundManager.Create( &g_pHealerQuickHealSound, tempFilePath, DSBCAPS_CTRLVOLUME );

	return	true;
}


bool ReleaseSound( void )
{
	delete	g_pBackgroundSound;

	delete	g_pBreakerNormalAttackSound;

	delete	g_pDefenderNormalAttackSound;

	delete	g_pMageNormalAttackSound;

	delete	g_pHealerNormalAttackSound;
	delete	g_pHealerQuickHealSound;

	return	true;
}

Matchless::ECharacterClass VoidPtrToCharacterClass( const void* src )
{
	using Type = std::conditional< sizeof( const void * ) == sizeof( unsigned int ), unsigned int, unsigned long long >::type;
	return static_cast<Matchless::ECharacterClass>( reinterpret_cast<Type>( src ) );
}

unsigned short VoidPtrToShort( const void* src )
{
	using Type = std::conditional< sizeof( const void * ) == sizeof( unsigned int ), unsigned int, unsigned long long >::type;
	return static_cast<unsigned short>( reinterpret_cast<Type>( src ) );
}
