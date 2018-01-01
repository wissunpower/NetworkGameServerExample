
#include	"NetworkHandler.h"



DWORD WINAPI NetReceiveProcess( LPVOID arg )
{
	unsigned int				bufIndex;
	const SOCKET				thisSocket = (SOCKET)arg;
	CNetMessage					tempMessage;
	Matchless::CClient			tempClient;
	unsigned int				tempID;
	unsigned int				tempCasterID;
	unsigned int				tempTargetID;
	unsigned short int			tempTeamNo;
	bool						tempBool;
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
		if( SOCKET_ERROR == tempMessage.ReceiveData( thisSocket, 0 ) )
		{
			continue;
		}

		switch( (Matchless::ENetMessageType)tempMessage.GetType() )
		{

		case Matchless::FSTC_INFORM_ANOTHERCLIENT_ENTER:
			memcpy( &tempID, tempMessage.GetpAddData(), sizeof( tempID ) );							bufIndex = sizeof( tempID );
			memcpy( &tempTeamNo, tempMessage.GetpAddData() + bufIndex, sizeof( tempTeamNo ) );		bufIndex += sizeof( tempTeamNo );
			memcpy( &tempbRM, tempMessage.GetpAddData() + bufIndex, sizeof( tempbRM ) );			bufIndex += sizeof( tempbRM );
			memcpy( &tempCC, tempMessage.GetpAddData() + bufIndex, sizeof( tempCC ) );				bufIndex += sizeof( tempCC );
			tempClient = Matchless::CClient();
			tempClient.m_NetSystem.SetID( tempID );
			tempClient.m_PlayerInfo.SetTeamNum( tempTeamNo );
			tempClient.m_PlayerInfo.SetbRoomMaster( tempbRM );
			tempClient.m_PlayerInfo.GetCharacterInfo().SetClass( tempCC );

			WRAP_ENTER_CS( &g_CS, 17 );
			if( 0 != tempID  &&  g_ThisClient.m_NetSystem.GetID() != tempID )
			{
				g_AnotherClientList[ tempID ] = tempClient;
				//g_AnotherClientList.insert( std::map< unsigned int, Matchless::CClient >::value_type( tempID, tempClient ) );
			}
			WRAP_LEAVE_CS( &g_CS, 17 );
			break;

		case Matchless::FSTC_INFORM_ANOTHERCLIENT_LEAVE:
			memcpy( &tempID, tempMessage.GetpAddData(), sizeof( tempID ) );
			WRAP_ENTER_CS( &g_CS, 18 );
			if( 0 != tempID  &&  g_ThisClient.m_NetSystem.GetID() != tempID )
			{
				g_AnotherClientList.erase( tempID );
				g_AnotherCharacterList.erase( tempID );
			}
			WRAP_LEAVE_CS( &g_CS, 18 );
			break;

		case Matchless::FSTC_CHAT:
			break;

		case Matchless::FSTC_CHARCLASS_UPDATE:
			memcpy( &tempID, tempMessage.GetpAddData(), sizeof( tempID ) );					bufIndex = sizeof( tempID );
			memcpy( &tempCC, tempMessage.GetpAddData() + bufIndex, sizeof( tempCC ) );		bufIndex += sizeof( tempCC );
			WRAP_ENTER_CS( &g_CS, 19 );
			if( g_ThisClient.m_NetSystem.GetID() == tempID )
			{
				g_ThisClient.m_PlayerInfo.GetCharacterInfo().SetClass( tempCC );
			}
			else if( g_AnotherClientList.end() != g_AnotherClientList.find( tempID ) )
			{
				g_AnotherClientList[ tempID ].m_PlayerInfo.GetCharacterInfo().SetClass( tempCC );
			}
			WRAP_LEAVE_CS( &g_CS, 19 );
			break;

		case Matchless::FSTC_TEAM_UPDATE:
			memcpy( &tempID, tempMessage.GetpAddData(), sizeof( tempID ) );							bufIndex = sizeof( tempID );
			memcpy( &tempTeamNo, tempMessage.GetpAddData() + bufIndex, sizeof( tempTeamNo ) );		bufIndex += sizeof( tempTeamNo );
			WRAP_ENTER_CS( &g_CS, 20 );
			if( g_ThisClient.m_NetSystem.GetID() == tempID )
			{
				g_ThisClient.m_PlayerInfo.SetTeamNum( tempTeamNo );
			}
			else if( g_AnotherClientList.end() != g_AnotherClientList.find( tempID ) )
			{
				g_AnotherClientList[ tempID ].m_PlayerInfo.SetTeamNum( tempTeamNo );
			}
			WRAP_LEAVE_CS( &g_CS, 20 );
			break;

		case Matchless::FSTC_MAP_UPDATE:
			WRAP_ENTER_CS( &g_CS, 21 );
			memcpy( &g_CurrentMapKind, tempMessage.GetpAddData(), sizeof( g_CurrentMapKind ) );
			WRAP_LEAVE_CS( &g_CS, 21 );
			break;

		case Matchless::FSTC_GAMEOUT:
			WRAP_ENTER_CS( &g_CS, 22 );
			ChangeAndInformMainStepState( g_ThisClient, Matchless::EMSS_Adjust );
			//if( g_pBackgroundSound )
			//{
			//	g_pBackgroundSound->Stop();
			//}
			WRAP_LEAVE_CS( &g_CS, 22 );
			break;

		case Matchless::FSTC_GAME_MOVE_POSITION:
			if( !g_IsGraphicResourceLoaded )
			{
				break;
			}
			memcpy( &tempID, tempMessage.GetpAddData(), sizeof( tempID ) );							bufIndex = sizeof( tempID );
			memcpy( &tempMtx, tempMessage.GetpAddData() + sizeof( tempID ), sizeof( tempMtx ) );	bufIndex += sizeof( tempMtx );
			for( int i = 0 ; i < 4 ; ++i )
			{
				for( int j = 0 ; j < 4 ; ++j )
				{
					tempD3DXMtx.m[ i ][ j ] = tempMtx.m[ i ][ j ];
				}
			}
			WRAP_ENTER_CS( &g_CS, 23 );
			g_AnotherClientList[ tempID ].m_PlayerInfo.SetTransform( tempMtx );
			g_AnotherCharacterList[ tempID ].SetPosition( D3DXVECTOR3( tempD3DXMtx._41, tempD3DXMtx._42, tempD3DXMtx._43 ) );
			g_AnotherCharacterList[ tempID ].SetTransforMatrix( tempD3DXMtx );
			WRAP_LEAVE_CS( &g_CS, 23 );
			break;

		case Matchless::FSTC_GAME_MOVE_ANIMATION:
			if( !g_IsGraphicResourceLoaded )
			{
				break;
			}
			memcpy( &tempID, tempMessage.GetpAddData(), sizeof( tempID ) );							bufIndex = sizeof( tempID );
			WRAP_ENTER_CS( &g_CS, 24 );
			while( bufIndex < tempMessage.GetAddDataLen() )
			{
				memcpy( &tempAniTrackIndex, tempMessage.GetpAddData() + bufIndex, sizeof( tempAniTrackIndex ) );
				bufIndex += sizeof( tempAniTrackIndex );
				memcpy( &tempAniTrackInfo, tempMessage.GetpAddData() + bufIndex, sizeof( tempAniTrackInfo ) );
				bufIndex += sizeof( tempAniTrackInfo );
				g_AnotherCharacterList[ tempID ].SetCurrentAnimationSet( tempAniTrackIndex, tempAniTrackInfo.m_AniIndex );
				g_AnotherCharacterList[ tempID ].SetAniTrackInfoDesc( tempAniTrackIndex, tempAniTrackInfo.m_TrackDesc );
			}
			WRAP_LEAVE_CS( &g_CS, 24 );
			break;

		case Matchless::FSTC_GAME_SKILL_CASTSTART:
			memcpy( &tempCastTime, tempMessage.GetpAddData(), sizeof( tempCastTime ) );
			WRAP_ENTER_CS( &g_CS, 25 );
			g_IsNowCasting = true;
			g_CastStartTick = g_Timer.GetTick();
			g_CastEndTick = g_CastStartTick + tempCastTime;
			WRAP_LEAVE_CS( &g_CS, 25 );
			break;

		case Matchless::FSTC_GAME_SKILL_APPLY:
			memcpy( &tempCasterID, tempMessage.GetpAddData(), sizeof( tempCasterID ) );							bufIndex = sizeof( tempCasterID );
			memcpy( &tempTargetID, tempMessage.GetpAddData() + bufIndex, sizeof( tempTargetID ) );				bufIndex += sizeof( tempTargetID );
			memcpy( &tempCharacterSkill, tempMessage.GetpAddData() + bufIndex, sizeof( tempCharacterSkill ) );	bufIndex += sizeof( tempCharacterSkill );
			HandleSkillWork( tempCharacterSkill, tempCasterID, tempTargetID );
			break;

		case Matchless::FSTC_GAME_SKILL_CANCEL:
			WRAP_ENTER_CS( &g_CS, 26 );
			g_IsNowCasting = false;
			g_CastStartTick = 0;
			g_CastEndTick = 0;
			WRAP_LEAVE_CS( &g_CS, 26 );
			break;

		case Matchless::FSTC_GAME_SKILL_FAILED:
			break;

		case Matchless::FSTC_GAME_CHAR_UPDATE:
			memcpy( &tempID, tempMessage.GetpAddData(), sizeof( tempID ) );
			memcpy( &tempCDT, tempMessage.GetpAddData() + sizeof( tempID ), sizeof( tempCDT ) );
			memcpy( &tempNumeric, tempMessage.GetpAddData() + sizeof( tempID ) + sizeof( tempCDT ), sizeof( tempNumeric ) );
			WRAP_ENTER_CS( &g_CS, 27 );
			if( g_ThisClient.m_NetSystem.GetID() == tempID )
			{
				g_ThisClient.m_PlayerInfo.GetCharacterInfo().SetData( tempCDT, tempNumeric, 0, 0 );
			}
			else if( g_AnotherClientList.end() != g_AnotherClientList.find( tempID ) )
			{
				g_AnotherClientList[ tempID ].m_PlayerInfo.GetCharacterInfo().SetData( tempCDT, tempNumeric, 0, 0 );
			}
			WRAP_LEAVE_CS( &g_CS, 27 );
			break;

		case Matchless::FSTC_GAMESTART_FAILED:
			WRAP_ENTER_CS( &g_CS, 28 );
			UpdateRoomMasterUI( Matchless::EMSS_Wait, (g_IsGameStartable = false) && g_ThisClient.m_PlayerInfo.IsRoomMaster() );
			WRAP_LEAVE_CS( &g_CS, 28 );
			break;

		case Matchless::FSTC_GAMESTART_SUCCEED:
			WRAP_ENTER_CS( &g_CS, 29 );
			ChangeAndInformMainStepState( g_ThisClient, Matchless::EMSS_Play );
			UpdateRoomMasterUI( Matchless::EMSS_Play, g_ThisClient.m_PlayerInfo.IsRoomMaster() );
			//if( g_pBackgroundSound )
			//{
			//	g_pBackgroundSound->Reset();
			//	g_pBackgroundSound->Play( 0, DSBPLAY_LOOPING, 1 );
			//}
			WRAP_LEAVE_CS( &g_CS, 29 );
			break;

		case Matchless::FSTC_INFORM_CLIENTINFO:
			memcpy( &tempClient, tempMessage.GetpAddData(), sizeof( tempClient ) );
			WRAP_ENTER_CS( &g_CS, 30 );
			if( g_ThisClient.m_NetSystem.GetID() != tempClient.m_NetSystem.GetID() )
			{
				g_AnotherClientList[ tempClient.m_NetSystem.GetID() ] = tempClient;
			}
			else
			{
				g_ThisClient.m_PlayerInfo.SetCharacterInfo( tempClient.m_PlayerInfo.GetCharacterInfo() );
				g_ThisClient.m_PlayerInfo.SetTransform( tempClient.m_PlayerInfo.GetTransform() );
			}
			WRAP_LEAVE_CS( &g_CS, 30 );
			break;

		case Matchless::FSTC_STARTABLE:
			WRAP_ENTER_CS( &g_CS, 31 );
			memcpy( &g_IsGameStartable, tempMessage.GetpAddData(), tempMessage.GetAddDataLen() );
			UpdateRoomMasterUI( Matchless::EMSS_Play, g_IsGameStartable && g_ThisClient.m_PlayerInfo.IsRoomMaster() );
			WRAP_LEAVE_CS( &g_CS, 31 );
			break;

		}
	}


	return 0;
}


int ChangeAndInformMainStepState( Matchless::CClient & aDestModule, const Matchless::EMainStepState & aSrc )
{
	CNetMessage		tempMessage;

	aDestModule.m_PlayerInfo.SetMainStepState( aSrc );
	tempMessage.SendData(  aDestModule.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FCTS_MSS_UPDATE,
							sizeof( aDestModule.m_PlayerInfo.GetMainStepState() ),  (char*)&(aDestModule.m_PlayerInfo.GetMainStepState()) );

	return 0;
}


bool HandleSkillWork( const Matchless::ECharacterSkill aSkillKind, const unsigned int aCasterID, const unsigned int aTargetID )
{
	Matchless::SEffect		tempEffectInfo;
	CSound *				pPlaySound = NULL;
	UINT					tempAniKind;
	Matchless::CClient *	pCaster = NULL;
	unsigned int			tempPlayTime = 60;


	WRAP_ENTER_CS( &g_CS, 32 );
	if( aCasterID == g_ThisClient.m_NetSystem.GetID() )
	{
		pCaster = &g_ThisClient;
	}
	else if( g_AnotherClientList.end() != g_AnotherClientList.find( aCasterID ) )
	{
		pCaster = &(g_AnotherClientList[ aCasterID ]);
	}
	else
	{
		pCaster = NULL;
		WRAP_LEAVE_CS( &g_CS, 32 );
		return	false;
	}
	WRAP_LEAVE_CS( &g_CS, 32 );

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
		WRAP_ENTER_CS( &g_CS, 33 );
		tempEffectInfo.m_StartPosX = pCaster->m_PlayerInfo.GetTransform()._41;
		tempEffectInfo.m_StartPosY = pCaster->m_PlayerInfo.GetTransform()._42;
		tempEffectInfo.m_StartPosZ = pCaster->m_PlayerInfo.GetTransform()._43;
		WRAP_LEAVE_CS( &g_CS, 33 );
		tempEffectInfo.m_StartPosY += (CHARACTER_COMMON_HEIGHT / 2);
		//pPlaySound = g_pMageNormalAttackSound;
		tempAniKind = CHARACTER_ANIINDEX_ATTACK;
		break;

	case Matchless::ECS_Mage_FireBall:
		tempEffectInfo.m_PosType = Matchless::SEffect::EPT_Active;
		tempEffectInfo.m_CharacterID = aTargetID;
		WRAP_ENTER_CS( &g_CS, 34 );
		tempEffectInfo.m_StartPosX = pCaster->m_PlayerInfo.GetTransform()._41;
		tempEffectInfo.m_StartPosY = pCaster->m_PlayerInfo.GetTransform()._42;
		tempEffectInfo.m_StartPosZ = pCaster->m_PlayerInfo.GetTransform()._43;
		WRAP_LEAVE_CS( &g_CS, 34 );
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
	WRAP_ENTER_CS( &g_CS, 35 );
	tempEffectInfo.m_StartTick = g_Timer.GetTick();
	tempEffectInfo.m_EndTick = g_Timer.GetTick() + tempPlayTime;
	tempEffectInfo.m_CurrentTick = tempEffectInfo.m_StartTick;
	g_EffectList.push_back( tempEffectInfo );

	if( pPlaySound )
	{
		pPlaySound->Reset();
		pPlaySound->Play( 0, 0, 10 );
	}
	WRAP_LEAVE_CS( &g_CS, 35 );


	return	true;
}
