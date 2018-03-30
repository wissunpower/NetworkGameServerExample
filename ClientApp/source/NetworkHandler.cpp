
#include	"stdafx.h"
#include	"NetworkHandler.h"
#include	"cException.h"
#include	"cPacket.h"


void SetAniTrackInfo( SAniTrackInfo& dst, const cAniTrackInfo& src )
{
	dst.m_TrackDesc.Priority = static_cast<D3DXPRIORITY_TYPE>( src.Priority );
	dst.m_TrackDesc.Weight = src.Weight;
	dst.m_TrackDesc.Speed = src.Speed;
	dst.m_TrackDesc.Position = src.Position;
	dst.m_TrackDesc.Enable = src.Enable;
	dst.m_AniIndex = src.AniIndex;
	//dst.m_AniName = src.AniName.c_str();
}

void SetAniTrackInfo( cAniTrackInfo& dst, const SAniTrackInfo& src )
{
	dst.Priority = src.m_TrackDesc.Priority;
	dst.Weight = src.m_TrackDesc.Weight;
	dst.Speed = src.m_TrackDesc.Speed;
	dst.Position = src.m_TrackDesc.Position;
	dst.Enable = src.m_TrackDesc.Enable;
	dst.AniIndex = src.m_AniIndex;
	//dst.AniName = src.m_AniName;
}


DWORD WINAPI NetReceiveProcess( LPVOID arg )
try {
	unsigned int				bufIndex;
	const SOCKET				thisSocket = (SOCKET)arg;
	cIPacket					iPacket;
	Matchless::CClient			tempClient;
	unsigned int				tempID;
	unsigned int				tempCasterID;
	unsigned int				tempTargetID;
	unsigned int				tempTeamNo;
	bool						tempbRM;
	Matchless::SMatrix4			tempMtx;
	D3DXMATRIX					tempD3DXMtx;
	UINT						tempAniTrackIndex;
	SAniTrackInfo				tempAniTrackInfo;
	Matchless::ECharDataType	tempCDT;
	Matchless::ECharacterSkill	tempCharacterSkill = Matchless::ECS_Idle;
	unsigned int				tempNumeric;
	Matchless::ECharacterClass	tempCC;
	unsigned int				tempCastTime;


	while( true )
	{
		if( SOCKET_ERROR == iPacket.Recv( thisSocket ) )
		{
			continue;
		}

		const Matchless::ENetMessageType nMsgType = static_cast<Matchless::ENetMessageType>( iPacket.Decode4u() );

		switch( nMsgType )
		{

		case Matchless::FSTC_INFORM_ANOTHERCLIENT_ENTER:
			tempID = iPacket.Decode4u();
			tempTeamNo = iPacket.Decode4u();
			tempbRM = iPacket.DecodeBool();
			tempCC = static_cast<Matchless::ECharacterClass>( iPacket.Decode4u() );
			tempClient = Matchless::CClient();
			tempClient.m_NetSystem.SetID( tempID );
			tempClient.m_PlayerInfo.SetTeamNum( tempTeamNo );
			tempClient.m_PlayerInfo.SetbRoomMaster( tempbRM );
			tempClient.m_PlayerInfo.GetCharacterInfo().SetClass( tempCC );

			{
				cMonitor::Owner lock{ g_Monitor };
				if ( 0 != tempID && g_ThisClient.m_NetSystem.GetID() != tempID )
				{
					g_AnotherClientList[ tempID ] = tempClient;
					//g_AnotherClientList.insert( std::map< unsigned int, Matchless::CClient >::value_type( tempID, tempClient ) );
				}
			}
			break;

		case Matchless::FSTC_INFORM_ANOTHERCLIENT_LEAVE:
			tempID = iPacket.Decode4u();
			{
				cMonitor::Owner lock{ g_Monitor };
				if ( 0 != tempID && g_ThisClient.m_NetSystem.GetID() != tempID )
				{
					g_AnotherClientList.erase( tempID );
					g_AnotherCharacterList.erase( tempID );
				}
			}
			break;

		case Matchless::FSTC_CHAT:
			break;

		case Matchless::FSTC_CHARCLASS_UPDATE:
			tempID = iPacket.Decode4u();
			tempCC = static_cast<Matchless::ECharacterClass>( iPacket.Decode4u() );
			{
				cMonitor::Owner lock{ g_Monitor };
				if ( g_ThisClient.m_NetSystem.GetID() == tempID )
				{
					g_ThisClient.m_PlayerInfo.GetCharacterInfo().SetClass( tempCC );
				}
				else if ( g_AnotherClientList.end() != g_AnotherClientList.find( tempID ) )
				{
					g_AnotherClientList[ tempID ].m_PlayerInfo.GetCharacterInfo().SetClass( tempCC );
				}
			}
			break;

		case Matchless::FSTC_TEAM_UPDATE:
			tempID = iPacket.Decode4u();
			tempTeamNo = iPacket.Decode4u();
			{
				cMonitor::Owner lock{ g_Monitor };
				if ( g_ThisClient.m_NetSystem.GetID() == tempID )
				{
					g_ThisClient.m_PlayerInfo.SetTeamNum( tempTeamNo );
				}
				else if ( g_AnotherClientList.end() != g_AnotherClientList.find( tempID ) )
				{
					g_AnotherClientList[ tempID ].m_PlayerInfo.SetTeamNum( tempTeamNo );
				}
			}
			break;

		case Matchless::FSTC_MAP_UPDATE:
			{
				cMonitor::Owner lock{ g_Monitor };
				g_CurrentMapKind = iPacket.Decode4u();
			}
			break;

		case Matchless::FSTC_GAMEOUT:
			{
				cMonitor::Owner lock{ g_Monitor };
				ChangeAndInformMainStepState( g_ThisClient, Matchless::EMSS_Adjust );
				//if( g_pBackgroundSound )
				//{
				//	g_pBackgroundSound->Stop();
				//}
			}
			break;

		case Matchless::FSTC_GAME_MOVE_POSITION:
			if( !g_IsGraphicResourceLoaded )
			{
				break;
			}
			tempID = iPacket.Decode4u();
			Matchless::Decode( iPacket, tempMtx );
			for( int i = 0 ; i < 4 ; ++i )
			{
				for( int j = 0 ; j < 4 ; ++j )
				{
					tempD3DXMtx.m[ i ][ j ] = tempMtx.m[ i ][ j ];
				}
			}
			{
				cMonitor::Owner lock{ g_Monitor };
				g_AnotherClientList[ tempID ].m_PlayerInfo.SetTransform( tempMtx );
				g_AnotherCharacterList[ tempID ].SetPosition( D3DXVECTOR3( tempD3DXMtx._41, tempD3DXMtx._42, tempD3DXMtx._43 ) );
				g_AnotherCharacterList[ tempID ].SetTransforMatrix( tempD3DXMtx );
			}
			break;

		case Matchless::FSTC_GAME_MOVE_ANIMATION:
			if( !g_IsGraphicResourceLoaded )
			{
				break;
			}
			tempID = iPacket.Decode4u();
			{
				cAniTrackInfo aniInfo;
				auto nAniCount = iPacket.Decode4u();

				cMonitor::Owner lock{ g_Monitor };
				for ( auto i = 0 ; i < nAniCount ; ++i )
				{
					tempAniTrackIndex = iPacket.Decode4u();
					Decode( iPacket, aniInfo );
					SetAniTrackInfo( tempAniTrackInfo, aniInfo );
					g_AnotherCharacterList[ tempID ].SetCurrentAnimationSet( tempAniTrackIndex, tempAniTrackInfo.m_AniIndex );
					g_AnotherCharacterList[ tempID ].SetAniTrackInfoDesc( tempAniTrackIndex, tempAniTrackInfo.m_TrackDesc );
				}
			}
			break;

		case Matchless::FSTC_GAME_SKILL_CASTSTART:
			tempCastTime = iPacket.Decode4u();
			{
				cMonitor::Owner lock{ g_Monitor };
				g_IsNowCasting = true;
				g_CastStartTick = g_Timer.GetTick();
				g_CastEndTick = g_CastStartTick + tempCastTime;
			}
			break;

		case Matchless::FSTC_GAME_SKILL_APPLY:
			tempCasterID = iPacket.Decode4u();
			tempTargetID = iPacket.Decode4u();
			tempCharacterSkill = static_cast<Matchless::ECharacterSkill>( iPacket.Decode4u() );
			HandleSkillWork( tempCharacterSkill, tempCasterID, tempTargetID );
			break;

		case Matchless::FSTC_GAME_SKILL_CANCEL:
			{
				cMonitor::Owner lock{ g_Monitor };
				g_IsNowCasting = false;
				g_CastStartTick = 0;
				g_CastEndTick = 0;
			}
			break;

		case Matchless::FSTC_GAME_SKILL_FAILED:
			break;

		case Matchless::FSTC_GAME_CHAR_UPDATE:
			tempID = iPacket.Decode4u();
			tempCDT = static_cast<Matchless::ECharDataType>( iPacket.Decode4u() );
			tempNumeric = iPacket.Decode4u();
			{
				cMonitor::Owner lock{ g_Monitor };
				if ( g_ThisClient.m_NetSystem.GetID() == tempID )
				{
					g_ThisClient.m_PlayerInfo.GetCharacterInfo().SetData( tempCDT, tempNumeric, 0, 0 );
				}
				else if ( g_AnotherClientList.end() != g_AnotherClientList.find( tempID ) )
				{
					g_AnotherClientList[ tempID ].m_PlayerInfo.GetCharacterInfo().SetData( tempCDT, tempNumeric, 0, 0 );
				}
			}
			break;

		case Matchless::FSTC_GAMESTART_FAILED:
			{
				cMonitor::Owner lock{ g_Monitor };
				UpdateRoomMasterUI( Matchless::EMSS_Wait, (g_IsGameStartable = false) && g_ThisClient.m_PlayerInfo.IsRoomMaster() );
			}
			break;

		case Matchless::FSTC_GAMESTART_SUCCEED:
			{
				cMonitor::Owner lock{ g_Monitor };
				ChangeAndInformMainStepState( g_ThisClient, Matchless::EMSS_Play );
				UpdateRoomMasterUI( Matchless::EMSS_Play, g_ThisClient.m_PlayerInfo.IsRoomMaster() );
				//if( g_pBackgroundSound )
				//{
				//	g_pBackgroundSound->Reset();
				//	g_pBackgroundSound->Play( 0, DSBPLAY_LOOPING, 1 );
				//}
			}
			break;

		case Matchless::FSTC_INFORM_CLIENTINFO:
			Matchless::Decode( iPacket, tempClient );
			{
				cMonitor::Owner lock{ g_Monitor };
				if ( g_ThisClient.m_NetSystem.GetID() != tempClient.m_NetSystem.GetID() )
				{
					g_AnotherClientList[ tempClient.m_NetSystem.GetID() ] = tempClient;
				}
				else
				{
					g_ThisClient.m_PlayerInfo.SetCharacterInfo( tempClient.m_PlayerInfo.GetCharacterInfo() );
					g_ThisClient.m_PlayerInfo.SetTransform( tempClient.m_PlayerInfo.GetTransform() );
				}
			}
			break;

		case Matchless::FSTC_STARTABLE:
			{
				cMonitor::Owner lock{ g_Monitor };
				g_IsGameStartable = iPacket.DecodeBool();
				UpdateRoomMasterUI( Matchless::EMSS_Play, g_IsGameStartable && g_ThisClient.m_PlayerInfo.IsRoomMaster() );
			}
			break;

		}
	}


	return 0;
}
catch ( const cException& e ) {
#if defined( UNICODE ) | defined( _UNICODE )
	MultiByteToWideChar( CP_ACP, 0, e.what(), -1, g_Notice, 512 );
#else
	_tcsncpy( g_Notice, e.what(), 512 );
#endif

	return 0xa0000003;
}
catch ( const std::runtime_error& e ) {
#if defined( UNICODE ) | defined( _UNICODE )
	MultiByteToWideChar( CP_ACP, 0, e.what(), -1, g_Notice, 512 );
#else
	_tcsncpy( g_Notice, e.what(), 512 );
#endif

	return 0xa0000002;
}
catch ( const std::exception& e ) {
#if defined( UNICODE ) | defined( _UNICODE )
	MultiByteToWideChar( CP_ACP, 0, e.what(), -1, g_Notice, 512 );
#else
	_tcsncpy( g_Notice, e.what(), 512 );
#endif

	return 0xa0000001;
}
catch ( ... ) {
	_tcsncpy( g_Notice, _T( "Unknown Exception" ), 512 );

	return 0xa0000000;
}


int ChangeAndInformMainStepState( Matchless::CClient & aDestModule, const Matchless::EMainStepState & aSrc )
{
	aDestModule.m_PlayerInfo.SetMainStepState( aSrc );

	cOPacket oPacket;
	oPacket.Encode4u( Matchless::FCTS_MSS_UPDATE );
	oPacket.Encode4u( aDestModule.m_PlayerInfo.GetMainStepState() );
	oPacket.Send( aDestModule.m_NetSystem.GetSocket() );

	return 0;
}


bool HandleSkillWork( const Matchless::ECharacterSkill aSkillKind, const unsigned int aCasterID, const unsigned int aTargetID )
{
	Matchless::SEffect		tempEffectInfo;
	CSound *				pPlaySound = NULL;
	UINT					tempAniKind;
	Matchless::CClient *	pCaster = NULL;
	unsigned int			tempPlayTime = 60;


	{
		cMonitor::Owner lock{ g_Monitor };
		if ( aCasterID == g_ThisClient.m_NetSystem.GetID() )
		{
			pCaster = &g_ThisClient;
		}
		else if ( g_AnotherClientList.end() != g_AnotherClientList.find( aCasterID ) )
		{
			pCaster = &( g_AnotherClientList[ aCasterID ] );
		}
		else
		{
			pCaster = NULL;
			return	false;
		}
	}

	switch( aSkillKind )
	{

	case Matchless::ECS_Breaker_NormalAttack:
		tempEffectInfo.m_PosType = Matchless::SEffect::EPT_Character;
		pPlaySound = g_pBreakerNormalAttackSound;
		tempAniKind = CHARACTER_ANIINDEX_ATTACK;
		break;

	case Matchless::ECS_Breaker_Raid:
		tempEffectInfo.m_PosType = Matchless::SEffect::EPT_Character;
		pPlaySound = g_pBreakerNormalAttackSound;
		tempAniKind = CHARACTER_ANIINDEX_ATTACK;
		break;

	case Matchless::ECS_Breaker_Stun:
		tempEffectInfo.m_PosType = Matchless::SEffect::EPT_Character;
		pPlaySound = g_pBreakerNormalAttackSound;
		tempAniKind = CHARACTER_ANIINDEX_ATTACK;
		break;

	case Matchless::ECS_Breaker_ArmorBreak:
		tempEffectInfo.m_PosType = Matchless::SEffect::EPT_Character;
		pPlaySound = g_pBreakerNormalAttackSound;
		tempAniKind = CHARACTER_ANIINDEX_ATTACK;
		break;

	case Matchless::ECS_Breaker_PowerBreak:
		tempEffectInfo.m_PosType = Matchless::SEffect::EPT_Character;
		pPlaySound = g_pBreakerNormalAttackSound;
		tempAniKind = CHARACTER_ANIINDEX_ATTACK;
		break;

	case Matchless::ECS_Breaker_Silent:
		tempEffectInfo.m_PosType = Matchless::SEffect::EPT_Character;
		//pPlaySound = g_pBreakerNormalAttackSound;
		tempAniKind = CHARACTER_ANIINDEX_ATTACK;
		break;

	case Matchless::ECS_Defender_NormalAttack:
		tempEffectInfo.m_PosType = Matchless::SEffect::EPT_Character;
		pPlaySound = g_pDefenderNormalAttackSound;
		tempAniKind = CHARACTER_ANIINDEX_ATTACK;
		break;

	case Matchless::ECS_Defender_StoneSkin:
		tempEffectInfo.m_PosType = Matchless::SEffect::EPT_Character;
		pPlaySound = g_pDefenderNormalAttackSound;
		tempAniKind = CHARACTER_ANIINDEX_ATTACK;
		break;

	case Matchless::ECS_Defender_Natural:
		tempEffectInfo.m_PosType = Matchless::SEffect::EPT_Character;
		pPlaySound = g_pDefenderNormalAttackSound;
		tempAniKind = CHARACTER_ANIINDEX_ATTACK;
		break;

	case Matchless::ECS_Defender_AttackBreak:
		tempEffectInfo.m_PosType = Matchless::SEffect::EPT_Character;
		pPlaySound = g_pDefenderNormalAttackSound;
		tempAniKind = CHARACTER_ANIINDEX_ATTACK;
		break;

	case Matchless::ECS_Defender_HeavyBlow:
		tempEffectInfo.m_PosType = Matchless::SEffect::EPT_Character;
		pPlaySound = g_pDefenderNormalAttackSound;
		tempAniKind = CHARACTER_ANIINDEX_ATTACK;
		break;

	case Matchless::ECS_Mage_NormalAttack:
		tempEffectInfo.m_PosType = Matchless::SEffect::EPT_Character;
		pPlaySound = g_pMageNormalAttackSound;
		tempAniKind = CHARACTER_ANIINDEX_ATTACK;
		break;

	case Matchless::ECS_Mage_IceBolt:
		tempEffectInfo.m_PosType = Matchless::SEffect::EPT_Active;
		tempEffectInfo.m_CharacterID = aTargetID;
		{
			cMonitor::Owner lock{ g_Monitor };
			tempEffectInfo.m_StartPosX = pCaster->m_PlayerInfo.GetTransform()._41;
			tempEffectInfo.m_StartPosY = pCaster->m_PlayerInfo.GetTransform()._42;
			tempEffectInfo.m_StartPosZ = pCaster->m_PlayerInfo.GetTransform()._43;
		}
		tempEffectInfo.m_StartPosY += (CHARACTER_COMMON_HEIGHT / 2);
		//pPlaySound = g_pMageNormalAttackSound;
		tempAniKind = CHARACTER_ANIINDEX_ATTACK;
		break;

	case Matchless::ECS_Mage_FireBall:
		tempEffectInfo.m_PosType = Matchless::SEffect::EPT_Active;
		tempEffectInfo.m_CharacterID = aTargetID;
		{
			cMonitor::Owner lock{ g_Monitor };
			tempEffectInfo.m_StartPosX = pCaster->m_PlayerInfo.GetTransform()._41;
			tempEffectInfo.m_StartPosY = pCaster->m_PlayerInfo.GetTransform()._42;
			tempEffectInfo.m_StartPosZ = pCaster->m_PlayerInfo.GetTransform()._43;
		}
		tempEffectInfo.m_StartPosY += (CHARACTER_COMMON_HEIGHT / 2);
		//pPlaySound = g_pMageNormalAttackSound;
		tempAniKind = CHARACTER_ANIINDEX_ATTACK;
		break;

	case Matchless::ECS_Healer_NormalAttack:
		tempEffectInfo.m_PosType = Matchless::SEffect::EPT_Character;
		pPlaySound = g_pHealerNormalAttackSound;
		tempAniKind = CHARACTER_ANIINDEX_ATTACK;
		break;

	case Matchless::ECS_Healer_Perfect:
		tempEffectInfo.m_PosType = Matchless::SEffect::EPT_Character;
		//pPlaySound = g_pHealerQuickHealSound;
		tempAniKind = CHARACTER_ANIINDEX_ATTACK;
		break;

	case Matchless::ECS_Healer_QuickHeal:
		tempEffectInfo.m_PosType = Matchless::SEffect::EPT_Character;
		pPlaySound = g_pHealerQuickHealSound;
		tempAniKind = CHARACTER_ANIINDEX_ATTACK;
		break;

	case Matchless::ECS_Healer_NormalHeal:
		tempEffectInfo.m_PosType = Matchless::SEffect::EPT_Character;
		//pPlaySound = g_pHealerQuickHealSound;
		tempAniKind = CHARACTER_ANIINDEX_ATTACK;
		tempPlayTime = 120;
		break;

	default:
		return	false;
		break;

	}


	if( g_ThisClient.m_NetSystem.GetID() == aCasterID )
	{
		g_ThisCharacter.SetCurrentAnimationSet( 1, tempAniKind );
		g_IsUpdateMoveAnimation = true;
	}

	tempEffectInfo.m_Type1 = EFFECTTYPE_SKILL;
	tempEffectInfo.m_Type2 = (unsigned int)aSkillKind;
	tempEffectInfo.m_CharacterID = aTargetID;
	{
		cMonitor::Owner lock{ g_Monitor };
		tempEffectInfo.m_StartTick = g_Timer.GetTick();
		tempEffectInfo.m_EndTick = g_Timer.GetTick() + tempPlayTime;
		tempEffectInfo.m_CurrentTick = tempEffectInfo.m_StartTick;
		g_EffectList.push_back( tempEffectInfo );

		if ( pPlaySound )
		{
			pPlaySound->Reset();
			pPlaySound->Play( 0, 0, 10 );
		}
	}


	return	true;
}
