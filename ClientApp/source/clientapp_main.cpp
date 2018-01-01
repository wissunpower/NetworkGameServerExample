
#include	"ClientAppRoot.h"

#include	"NetworkHandler.h"
#include	"DXDeviceHandler.h"


float	tempFloat = 0.0f;

void RenderText( void )
{
	const D3DSURFACE_DESC *	pd3dsdBackBuffer = DXUTGetD3D9BackBufferSurfaceDesc();
	CDXUTTextHelper	txtHelper( g_pFont, g_pTextSprite, 16 );

	txtHelper.Begin();
	txtHelper.SetInsertionPos( 5, 70 );
	txtHelper.SetForegroundColor( D3DXCOLOR( 0.0f, 0.0f, 0.0f, 1.0f ) );
	txtHelper.DrawTextLine( DXUTGetFrameStats( DXUTIsVsyncEnabled() ) );
	txtHelper.DrawFormattedTextLine( TEXT( "%f" ), tempFloat );
	//txtHelper.DrawTextLine( DXUTGetDeviceStats() );

	//txtHelper.SetForegroundColor( D3DXCOLOR( 0.75f, 1.0f, 0.0f, 1.0f ) );


	txtHelper.End();
}


void ProcessUserInput( float fElapsedTime )
{
	WRAP_ENTER_CS( &g_CS, 1 );
	if( !DXUTIsActive()  ||  Matchless::EMSS_Play != g_ThisClient.m_PlayerInfo.GetMainStepState()  ||  !g_IsGraphicResourceLoaded  ||
		0 >= g_ThisClient.m_PlayerInfo.GetCharacterInfo().GetCurrentHealth()  ||
		g_ThisClient.m_PlayerInfo.GetCharacterInfo().FindState( Matchless::EST_Faint ) )
	{
		WRAP_LEAVE_CS( &g_CS, 1 );
		return;
	}
	WRAP_LEAVE_CS( &g_CS, 1 );


	static float	fElapsedTimeForTargeting = 0.0f;		// temporary
	static float	fElapsedTimeForAction = 0.0f;			// temporary
	static UINT		preAnimationSet;						// temporary
	BYTE			tempKeyTable[ VK_TABLE_MAX ];
	D3DXVECTOR3		tempCountThisLookAt( -(g_ThisCharacter.GetZAxis().x), -(g_ThisCharacter.GetZAxis().y), -(g_ThisCharacter.GetZAxis().z) );
	unsigned int	tempNumKey = 0;
	bool			tempBool = false;
	float			tempHeight = 0.0f;


	fElapsedTimeForTargeting += fElapsedTime;
	fElapsedTimeForAction += fElapsedTime;


	if( GetKeyboardState( tempKeyTable ) )
	{
		float tempRotate = 0.0f;
		float tempXMoveDistance = 0.0f;
		float tempYMoveDistance = 0.0f;
		float tempZMoveDistance = 0.0f;

		//if( tempKeyTable[ 'N' ] & 0xF0 )
		//	tempXMoveDistance = -1.0f;

		//if( tempKeyTable[ 'M' ] & 0xF0 )
		//	tempXMoveDistance = 1.0f;

		if( tempKeyTable[ VK_PRIOR ] & 0xF0 )
			tempYMoveDistance = 1.0f;

		if( tempKeyTable[ VK_NEXT ] & 0xF0 )
			tempYMoveDistance = -1.0f;

		if( tempKeyTable[ VK_UP ] & 0xF0 )
		{
			tempZMoveDistance = -FMOVE_UNIT * fElapsedTime;
			if( CHARACTER_ANIINDEX_FMOVE != preAnimationSet )
			{
				g_IsUpdateMoveAnimation = true;
			}
			g_ThisCharacter.SetCurrentAnimationSet( 0, preAnimationSet = CHARACTER_ANIINDEX_FMOVE );
		}

		if( tempKeyTable[ VK_DOWN ] & 0xF0 )
		{
			tempZMoveDistance = BMOVE_UNIT * fElapsedTime;
			if( CHARACTER_ANIINDEX_BMOVE != preAnimationSet )
			{
				g_IsUpdateMoveAnimation = true;
			}
			g_ThisCharacter.SetCurrentAnimationSet( 0, preAnimationSet = CHARACTER_ANIINDEX_BMOVE );
		}
		else if( !(tempKeyTable[ VK_UP ] & 0xF0) )
		{
			if( g_ThisCharacter.GetIdleAnimationSet() != preAnimationSet )
			{
				g_IsUpdateMoveAnimation = true;
			}
			g_ThisCharacter.SetCurrentAnimationSet( 0, preAnimationSet = g_ThisCharacter.GetIdleAnimationSet() );
		}

		if( tempKeyTable[ VK_LEFT ] & 0xF0 )
			tempRotate = -2.0f;

		if( tempKeyTable[ VK_RIGHT ] & 0xF0 )
			tempRotate = 2.0f;


		if( tempKeyTable[ VK_TAB ] & 0xF0  &&  GAME_COMMAND_RECYCLE <= fElapsedTimeForTargeting )		// Handle targeting command
		{
			fElapsedTimeForTargeting = 0.0f;

			if( !g_AnotherCharacterList.empty() )
			{
				std::map< unsigned int, CAnimateMesh >::iterator	amIt;
				std::map< unsigned int, CAnimateMesh >::iterator	amItOrg;

				if( 0 == g_TargetID  ||
					( g_AnotherCharacterList.end() == (amIt = g_AnotherCharacterList.find( g_TargetID )) && g_ThisClient.m_NetSystem.GetID() != g_TargetID ) )
				{
					amItOrg = g_AnotherCharacterList.end();
					amIt = g_AnotherCharacterList.begin();
				}
				else
				{
					amItOrg = amIt;
					if( g_AnotherCharacterList.end() == amIt )
					{
						amIt = g_AnotherCharacterList.begin();
					}
					else
					{
						++amIt;
					}
				}

				while( amItOrg != amIt )
				{
					D3DXVECTOR3		tempVector;

					if( g_AnotherCharacterList.end() == amIt )
					{
						g_TargetID = g_ThisClient.m_NetSystem.GetID();
						break;
					}
					else
					{
						D3DXVec3Subtract( &tempVector, &(amIt->second.GetPosition()), &(g_ThisCharacter.GetPosition()) );
						float	tempLength = D3DXVec3Length( &tempVector );

						if( TARGETING_RANGE >= tempLength )
						{
							g_TargetID = amIt->first;
							break;
						}
					}

					if( g_AnotherCharacterList.end() == amIt )
					{
						amIt = g_AnotherCharacterList.begin();
					}
					else
					{
						++amIt;
					}
				}
			}
			else
			{
				g_TargetID = g_ThisClient.m_NetSystem.GetID();
			}
		}


		if( tempRotate != 0.0f )
		{
			g_ThisCharacter.Rotate( 0, tempRotate, 0 );
		}

		if( 0.0f != tempXMoveDistance  ||  0.0f != tempZMoveDistance )
		{
			D3DXVECTOR3		tempPosVec( g_ThisCharacter.GetPosition() );

			tempPosVec.x += (g_ThisCharacter.GetXAxis().x * tempXMoveDistance + g_ThisCharacter.GetZAxis().x * tempZMoveDistance);
			tempPosVec.y += (g_ThisCharacter.GetXAxis().y * tempXMoveDistance + g_ThisCharacter.GetZAxis().y * tempZMoveDistance);
			tempPosVec.z += (g_ThisCharacter.GetXAxis().z * tempXMoveDistance + g_ThisCharacter.GetZAxis().z * tempZMoveDistance);

			tempFloat = tempHeight = GetDistanceWithMesh(  &tempBool,  tempPosVec,  g_ThisCharacter.GetYAxis(),
												g_pTerrain,  g_TerrainTransform  );

			if( tempBool && tempHeight < (fElapsedTime * 512.0f) )
			{
				g_ThisCharacter.MoveOnXAxis( tempXMoveDistance );
				g_ThisCharacter.MoveOnYAxis( tempHeight );
				g_ThisCharacter.MoveOnZAxis( tempZMoveDistance );
			}
			else
			{
				tempXMoveDistance = 0.0f;
				tempZMoveDistance = 0.0f;
			}
		}

		if( 0.0f != tempYMoveDistance )
		{
			g_ThisCharacter.MoveOnYAxis( tempYMoveDistance );
		}

		if( 0.0f != tempXMoveDistance  ||  0.0f != tempZMoveDistance  ||  0.0f != tempRotate )
		{
			Matchless::SMatrix4		tempMatrix;

			WRAP_ENTER_CS( &g_CS, 2 );
			for( int i = 0 ; i < 4 ; ++i )
			{
				for( int j = 0 ; j < 4 ; ++j )
				{
					tempMatrix.m[ i ][ j ] = g_ThisCharacter.GetTransforMatrix().m[ i ][ j ];
				}
			}
			g_ThisClient.m_PlayerInfo.SetTransform( tempMatrix );
			g_IsUpdateMovePosition = true;
			WRAP_LEAVE_CS( &g_CS, 2 );
		}


		if( GAME_SKILLCOMMAND_RECYCLE <= fElapsedTimeForAction  &&  (
			( (tempNumKey = 0x31) && (tempKeyTable[ 0x31 ] & 0xF0) )  ||
			( (tempNumKey = 0x32) && (tempKeyTable[ 0x32 ] & 0xF0) )  ||
			( (tempNumKey = 0x33) && (tempKeyTable[ 0x33 ] & 0xF0) )  ||
			( (tempNumKey = 0x34) && (tempKeyTable[ 0x34 ] & 0xF0) )  ||
			( (tempNumKey = 0x35) && (tempKeyTable[ 0x35 ] & 0xF0) )  ||
			( (tempNumKey = 0x36) && (tempKeyTable[ 0x36 ] & 0xF0) )  ||
			( (tempNumKey = 0x37) && (tempKeyTable[ 0x37 ] & 0xF0) )  ||
			( (tempNumKey = 0x38) && (tempKeyTable[ 0x38 ] & 0xF0) )  ||
			( (tempNumKey = 0x39) && (tempKeyTable[ 0x39 ] & 0xF0) )  ||
			( (tempNumKey = 0x30) && (tempKeyTable[ 0x30 ] & 0xF0) )
			) )
		{
			fElapsedTimeForAction = 0.0f;
			HandleSkillCommand( g_ThisClient.m_PlayerInfo.GetCharacterInfo().GetClass(), tempNumKey - 0x30, g_TargetID );
		}
	}
}


void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void * pUserContext )
{
	g_Timer.Advance( fElapsedTime );


	if( g_Timer.IsWakeUp()  &&  g_Timer.GetTick() )
	{
		ProcessUserInput( (float)g_Timer.GetFramePeriod() );
	}


	g_GlobalUI.GetStatic( IDC_GLOBAL_NOTICE )->SetText( g_Notice );


	D3DXVECTOR3			tempCameraLookAt;
	IDirect3DDevice9 *	pd3dDevice = DXUTGetD3D9Device();

	D3DXMATRIX	tempViewTrans = g_Camera.GetViewTransform();
	pd3dDevice->SetTransform( D3DTS_VIEW, &tempViewTrans );


	WRAP_ENTER_CS( &g_CS, 3 );
	if( Matchless::EMSS_Play == g_ThisClient.m_PlayerInfo.GetMainStepState()  &&  !g_IsGraphicResourceLoaded )		// Load game resource
	{
		CreateGraphicResource( D3DXMESH_MANAGED, pd3dDevice, NULL );
	}
	else if( Matchless::EMSS_Play != g_ThisClient.m_PlayerInfo.GetMainStepState()  &&  g_IsGraphicResourceLoaded )	// Release game resource
	{
		DestroyGraphicResource();
	}
	WRAP_LEAVE_CS( &g_CS, 3 );


	SAniTrackInfo											tempAniTrackInfo;
	std::map< UINT, LPD3DXANIMATIONSET >::const_iterator	asIter;

	float		tempYRate = 0.0f;
	float		tempZRate = 0.0f;
	D3DXMATRIX	tempMatrix;


	WRAP_ENTER_CS( &g_CS, 4 );

	switch( g_ThisClient.m_PlayerInfo.GetCharacterInfo().GetClass() )
	{

	case Matchless::ECC_Breaker:
		g_pSelectCharacter = &g_BreakerMesh;
		break;

	case Matchless::ECC_Defender:
		g_pSelectCharacter = &g_DefenderMesh;
		break;

	case Matchless::ECC_Mage:
		g_pSelectCharacter = &g_MageMesh;
		break;

	case Matchless::ECC_Healer:
		g_pSelectCharacter = &g_HealerMesh;
		break;

	default:
		g_pSelectCharacter = NULL;
		break;

	}

	if( g_pSelectCharacter )
	{
		tempYRate = 0.0f;
		tempZRate = -900.0f;

		g_pSelectCharacter->SetCurrentAnimationSet( 0, CHARACTER_ANIINDEX_IDLE );
		g_pSelectCharacter->GetAniTrackInfo( 1, tempAniTrackInfo );
		tempAniTrackInfo.m_TrackDesc.Enable = FALSE;
		g_pSelectCharacter->SetAniTrackInfoDesc( 1, tempAniTrackInfo.m_TrackDesc );

		g_pSelectCharacter->UpdatePerFrame( fElapsedTime, NULL );

		g_Camera.SetPosition(
			D3DXVECTOR3(
			g_pSelectCharacter->GetPosition().x + (g_pSelectCharacter->GetYAxis().x * tempYRate) + (g_pSelectCharacter->GetZAxis().x * tempZRate),
			g_pSelectCharacter->GetPosition().y + (g_pSelectCharacter->GetYAxis().y * tempYRate) + (g_pSelectCharacter->GetZAxis().y * tempZRate),
			g_pSelectCharacter->GetPosition().z + (g_pSelectCharacter->GetYAxis().z * tempYRate) + (g_pSelectCharacter->GetZAxis().z * tempZRate)
			)
			);

		tempCameraLookAt.x = g_pSelectCharacter->GetPosition().x - g_Camera.GetPosition().x;
		tempCameraLookAt.y = (g_pSelectCharacter->GetPosition().y - g_Camera.GetPosition().y) / 2;
		tempCameraLookAt.z = g_pSelectCharacter->GetPosition().z - g_Camera.GetPosition().z;
		D3DXVec3Normalize( &tempCameraLookAt, &tempCameraLookAt );
		g_Camera.SetLookAt( tempCameraLookAt );
		g_Camera.SetUp( g_pSelectCharacter->GetYAxis() );

		g_IsFrameUpdated = false;
	}


	switch( g_ThisClient.m_PlayerInfo.GetMainStepState() )
	{

	case Matchless::EMSS_Start:
		break;

	case Matchless::EMSS_Wait:
		break;

	case Matchless::EMSS_Play:
		if( g_IsGraphicResourceLoaded )
		{
			tempYRate = 600.0f;
			tempZRate = 1500.0f;

			if( g_ThisCharacter.GetAniTrackInfo( 1, tempAniTrackInfo )  &&
				g_ThisCharacter.GetAnimationSetMap().end() != (asIter = g_ThisCharacter.GetAnimationSetMap().find( tempAniTrackInfo.m_AniIndex ))  &&
				tempAniTrackInfo.m_TrackDesc.Position + (DOUBLE)fElapsedTime >= asIter->second->GetPeriod() )
			{
				tempAniTrackInfo.m_TrackDesc.Enable = FALSE;
				tempAniTrackInfo.m_TrackDesc.Position = 0.0;
				g_ThisCharacter.SetAniTrackInfoDesc( 1, tempAniTrackInfo.m_TrackDesc );
				g_IsUpdateMoveAnimation = true;
			}

			for(  std::map< unsigned int, CAnimateMesh >::iterator amIter = g_AnotherCharacterList.begin()  ;
				amIter != g_AnotherCharacterList.end()  ;  ++amIter  )
			{
				amIter->second.UpdatePerFrame( fElapsedTime, NULL );
			}

			g_ThisCharacter.UpdatePerFrame( fElapsedTime, NULL );

			if( g_Timer.IsWakeUp() )
			{
				UpdateEffect();
			}

			g_Camera.SetPosition(
				D3DXVECTOR3(
				g_ThisCharacter.GetPosition().x /*+ (g_ArcBall.GetRotationMatrix()->_13 * 500)*/+ (g_ThisCharacter.GetYAxis().x * tempYRate) + (g_ThisCharacter.GetZAxis().x * tempZRate),
				g_ThisCharacter.GetPosition().y /*+ (g_ArcBall.GetRotationMatrix()->_23 * 500)*/+ (g_ThisCharacter.GetYAxis().y * tempYRate) + (g_ThisCharacter.GetZAxis().y * tempZRate),
				g_ThisCharacter.GetPosition().z /*+ (g_ArcBall.GetRotationMatrix()->_33 * 500)*/+ (g_ThisCharacter.GetYAxis().z * tempYRate) + (g_ThisCharacter.GetZAxis().z * tempZRate)
				)
				);

			tempCameraLookAt.x = g_ThisCharacter.GetPosition().x - g_Camera.GetPosition().x;
			tempCameraLookAt.y = (g_ThisCharacter.GetPosition().y - g_Camera.GetPosition().y) / 2;
			tempCameraLookAt.z = g_ThisCharacter.GetPosition().z - g_Camera.GetPosition().z;
			D3DXVec3Normalize( &tempCameraLookAt, &tempCameraLookAt );
			g_Camera.SetLookAt( tempCameraLookAt );
			g_Camera.SetUp( g_ThisCharacter.GetYAxis() );

			g_IsFrameUpdated = true;
		}

		if( g_IsUpdateMovePosition )
		{
			char					buf[ 512 ];
			unsigned int			bufIndex = 0;
			CNetMessage				tempMessage;
			Matchless::SMatrix4		tempMatrix;


			WRAP_ENTER_CS( &g_CS, 5 );
			tempMatrix = g_ThisClient.m_PlayerInfo.GetTransform();

			memcpy( buf, &tempMatrix, sizeof( tempMatrix ) );				bufIndex = sizeof( tempMatrix );

			tempMessage.SendData( g_ThisClient.m_NetSystem.GetSocket(), 0, (unsigned int)Matchless::FCTS_GAME_MOVE_POSITION, bufIndex, buf );

			g_IsUpdateMovePosition = false;
			WRAP_LEAVE_CS( &g_CS, 5 );
		}
		if( g_IsUpdateMoveAnimation )
		{
			char					buf[ 512 ];
			unsigned int			bufIndex = 0;
			CNetMessage				tempMessage;
			std::map< UINT, SAniTrackInfo >::const_iterator		atIter;


			WRAP_ENTER_CS( &g_CS, 6 );

			for( atIter = g_ThisCharacter.GetAniTrackMap().begin()  ;  atIter != g_ThisCharacter.GetAniTrackMap().end()  ;  ++atIter )
			{
				memcpy( buf + bufIndex, &(atIter->first), sizeof( atIter->first ) );	bufIndex += sizeof( atIter->first );
				memcpy( buf + bufIndex, &(atIter->second), sizeof( atIter->second ) );	bufIndex += sizeof( atIter->second );
			}

			tempMessage.SendData( g_ThisClient.m_NetSystem.GetSocket(), 0, (unsigned int)Matchless::FCTS_GAME_MOVE_ANIMATION, bufIndex, buf );

			g_IsUpdateMoveAnimation = false;
			WRAP_LEAVE_CS( &g_CS, 6 );
		}
		break;

	case Matchless::EMSS_Adjust:
		break;

	}
	WRAP_LEAVE_CS( &g_CS, 4 );

}


void CALLBACK OnFrameRender( IDirect3DDevice9 * pd3dDevice, double fTime, float fElapsedTime, void * pUserContext )
{
	HRESULT			hr;
	D3DXMATRIX		tempMtx;


	if( g_SettingsDlg.IsActive() )
	{
		g_SettingsDlg.OnRender( fElapsedTime );
		return;
	}

	V( pd3dDevice->Clear( 0L, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB( 0, 66, 75, 121 ), 1.0f, 0L ) );


	g_Camera.UpdateLight();

	V( pd3dDevice->SetLight( 0, &g_Camera.GetLight() ) );
	V( pd3dDevice->LightEnable( 0, TRUE ) );

	if( SUCCEEDED( pd3dDevice->BeginScene() ) )
	{
		switch( g_ThisClient.m_PlayerInfo.GetMainStepState() )
		{

		case Matchless::EMSS_Start:
			g_StartBackground.Render( pd3dDevice, D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) );
			V( g_MainStepStartUI.OnRender( fElapsedTime ) );
			break;
			
		case Matchless::EMSS_Wait:
			g_WaitBackground.Render( pd3dDevice, D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) );
			if( g_pSelectCharacter )
			{
				g_pSelectCharacter->Render( pd3dDevice );
			}
			V( g_MainStepWaitUI.OnRender( fElapsedTime ) );
			break;

		case Matchless::EMSS_Play:
			if( !g_IsGraphicResourceLoaded  ||  !g_IsFrameUpdated )
			{
				break;
			}
			for(  std::map< unsigned int, CAnimateMesh >::iterator amIter = g_AnotherCharacterList.begin()  ;
				amIter != g_AnotherCharacterList.end()  ;  ++amIter  )
			{
				amIter->second.Render( pd3dDevice );

				if( amIter->first == g_TargetID )
				{
					D3DXMatrixIdentity( &tempMtx );
					tempMtx._41 = amIter->second.GetPosition().x;
					tempMtx._42 = amIter->second.GetPosition().y + 256.0f;
					tempMtx._43 = amIter->second.GetPosition().z;

					pd3dDevice->SetTransform( D3DTS_WORLD, &tempMtx );
					pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

					V( g_pTargetCharacterMark->DrawSubset( 0 ) );
				}

				D3DXMatrixIdentity( &tempMtx );
				tempMtx._11 = 2.0f;
				tempMtx._22 = 2.0f;
				tempMtx._33 = 2.0f;
				tempMtx._41 = amIter->second.GetPosition().x + (amIter->second.GetXAxis().x * 96.0f) + (amIter->second.GetYAxis().x * 192.0f);
				tempMtx._42 = amIter->second.GetPosition().y + (amIter->second.GetXAxis().y * 96.0f) + (amIter->second.GetYAxis().y * 192.0f);
				tempMtx._43 = amIter->second.GetPosition().z + (amIter->second.GetXAxis().z * 96.0f) + (amIter->second.GetYAxis().z * 192.0f);

				pd3dDevice->SetTransform( D3DTS_WORLD, &tempMtx );

				WRAP_ENTER_CS( &g_CS, 7 );
				if( g_AnotherClientList[ amIter->first ].m_PlayerInfo.GetTeamNum() % 2 )
				{
					for( DWORD i = 0 ; i < g_NumHomeTeamMarkMaterial ; ++i )
					{
						V( pd3dDevice->SetMaterial( &g_pHomeTeamMarkMaterial[ i ] ) );
						V( pd3dDevice->SetTexture( 0, g_ppHomeTeamMarkTexture[ i ] ) );
						V( g_pHomeTeamMark->DrawSubset( i ) );
					}
				}
				else
				{
					for( DWORD i = 0 ; i < g_NumAwayTeamMarkMaterial ; ++i )
					{
						V( pd3dDevice->SetMaterial( &g_pAwayTeamMarkMaterial[ i ] ) );
						V( pd3dDevice->SetTexture( 0, g_ppAwayTeamMarkTexture[ i ] ) );
						V( g_pAwayTeamMark->DrawSubset( i ) );
					}
				}
				WRAP_LEAVE_CS( &g_CS, 7 );
			}

			g_SkyBox.Render( pd3dDevice, g_ThisCharacter.GetPosition() );
			RenderTerrain( pd3dDevice );

			g_ThisCharacter.Render( pd3dDevice );
			if( g_ThisClient.m_NetSystem.GetID() == g_TargetID )
			{
				D3DXMatrixIdentity( &tempMtx );
				tempMtx._41 = g_ThisCharacter.GetPosition().x;
				tempMtx._42 = g_ThisCharacter.GetPosition().y + 256.0f;
				tempMtx._43 = g_ThisCharacter.GetPosition().z;

				pd3dDevice->SetTransform( D3DTS_WORLD, &tempMtx );
				pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

				V( g_pTargetCharacterMark->DrawSubset( 0 ) );
			}

			D3DXMatrixIdentity( &tempMtx );
			tempMtx._11 = 2.0f;
			tempMtx._22 = 2.0f;
			tempMtx._33 = 2.0f;
			tempMtx._41 = g_ThisCharacter.GetPosition().x + (g_ThisCharacter.GetXAxis().x * 96.0f) + (g_ThisCharacter.GetYAxis().x * 192.0f);
			tempMtx._42 = g_ThisCharacter.GetPosition().y + (g_ThisCharacter.GetXAxis().y * 96.0f) + (g_ThisCharacter.GetYAxis().y * 192.0f);
			tempMtx._43 = g_ThisCharacter.GetPosition().z + (g_ThisCharacter.GetXAxis().z * 96.0f) + (g_ThisCharacter.GetYAxis().z * 192.0f);

			pd3dDevice->SetTransform( D3DTS_WORLD, &tempMtx );

			WRAP_ENTER_CS( &g_CS, 8 );
			if( g_ThisClient.m_PlayerInfo.GetTeamNum() % 2 )
			{
				for( DWORD i = 0 ; i < g_NumHomeTeamMarkMaterial ; ++i )
				{
					V( pd3dDevice->SetMaterial( &g_pHomeTeamMarkMaterial[ i ] ) );
					V( pd3dDevice->SetTexture( 0, g_ppHomeTeamMarkTexture[ i ] ) );
					V( g_pHomeTeamMark->DrawSubset( i ) );
				}
			}
			else
			{
				for( DWORD i = 0 ; i < g_NumAwayTeamMarkMaterial ; ++i )
				{
					V( pd3dDevice->SetMaterial( &g_pAwayTeamMarkMaterial[ i ] ) );
					V( pd3dDevice->SetTexture( 0, g_ppAwayTeamMarkTexture[ i ] ) );
					V( g_pAwayTeamMark->DrawSubset( i ) );
				}
			}
			WRAP_LEAVE_CS( &g_CS, 8 );


			RenderEffect( pd3dDevice );

			V( g_MainStepPlayUI.OnRender( fElapsedTime ) );
			g_IsFrameUpdated = false;
			break;

		case Matchless::EMSS_Adjust:
			g_AdjustBackground.Render( pd3dDevice, D3DXVECTOR3( 0.0f, 0.0f, 0.0f ) );
			V( g_MainStepAdjustUI.OnRender( fElapsedTime ) );
			break;

		case Matchless::EMSS_End:
			V( g_MainStepEndUI.OnRender( fElapsedTime ) );
			break;

		}


#if defined(DEBUG) | defined(_DEBUG)
		RenderText();
#endif

		V( g_SystemUI.OnRender( fElapsedTime ) );
#if defined(DEBUG) | defined(_DEBUG)
		V( g_GlobalUI.OnRender( fElapsedTime ) );
#endif


		DrawGameUI( g_pd3dSprite, g_pFont, &g_CS, g_ThisClient.m_PlayerInfo.GetMainStepState() );


		V( pd3dDevice->EndScene() );
	}
}


LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, bool * pbNoFurtherProcessing, void * pUserContext )
{
	*pbNoFurtherProcessing = CDXUTIMEEditBox::StaticMsgProc( hWnd, uMsg, wParam, lParam );
	if( *pbNoFurtherProcessing )
	{
		return 0;
	}
	*pbNoFurtherProcessing = g_DialogResourceManager.MsgProc( hWnd, uMsg, wParam, lParam );
	if( *pbNoFurtherProcessing )
	{
		return 0;
	}

	if( g_SettingsDlg.IsActive() )
	{
		g_SettingsDlg.MsgProc( hWnd, uMsg, wParam, lParam );
		return 0;
	}

	*pbNoFurtherProcessing = g_SystemUI.MsgProc( hWnd, uMsg, wParam, lParam );
	if( *pbNoFurtherProcessing )
	{
		return 0;
	}

	*pbNoFurtherProcessing = g_GlobalUI.MsgProc( hWnd, uMsg, wParam, lParam );
	if( *pbNoFurtherProcessing )
	{
		return 0;
	}

	g_ArcBall.HandleMessages( hWnd, uMsg, wParam, lParam );


	switch( g_ThisClient.m_PlayerInfo.GetMainStepState() )
	{

	case Matchless::EMSS_Start:
		(*pbNoFurtherProcessing) = g_MainStepStartUI.MsgProc( hWnd, uMsg, wParam, lParam );
		break;

	case Matchless::EMSS_Wait:
		(*pbNoFurtherProcessing) = g_MainStepWaitUI.MsgProc( hWnd, uMsg, wParam, lParam );
		break;

	case Matchless::EMSS_Play:
		(*pbNoFurtherProcessing) = g_MainStepPlayUI.MsgProc( hWnd, uMsg, wParam, lParam );
		break;

	case Matchless::EMSS_Adjust:
		(*pbNoFurtherProcessing) = g_MainStepAdjustUI.MsgProc( hWnd, uMsg, wParam, lParam );
		break;

	case Matchless::EMSS_End:
		(*pbNoFurtherProcessing) = g_MainStepEndUI.MsgProc( hWnd, uMsg, wParam, lParam );
		break;

	}

	if( *pbNoFurtherProcessing )
	{
		return 0;
	}


	return 0;
}


void CALLBACK KeyboardProc( UINT nChar, bool bKeyDown, bool bAltDown, void * pUserContext )
{
	// Delegate to ProcessUserInput() function
	_asm nop

	return;
}


void CALLBACK OnGUIEvent( UINT nEvent, int nControlID, CDXUTControl * pControl, void * pUserContext )
{
	SOCKADDR_IN		serveraddr;
	METHOD			NewSkinningMethod;
	CNetMessage		tempMessage;
	DWORD			ThreadID;

	bool			bCreateSocketFailed;
	bool			bConnectToServerFailed;
	bool			bReceiveLoginDataFailed;
	bool			bLoginFailed;

	unsigned short int			tempTN;
	Matchless::ECharacterClass	tempCharacterClass;


	switch( nControlID )
	{

	case IDC_TOGGLEFULLSCREEN:
		DXUTToggleFullScreen();
		break;

	//case IDC_TOGGLEREF:
	//	DXUTToggleREF();
	//	break;

	//case IDC_CHANGEDEVICE:
	//	g_SettingsDlg.SetActive( !g_SettingsDlg.IsActive() );
	//	break;

	//case IDC_METHOD:
	//	NewSkinningMethod = (METHOD)(size_t)( (CDXUTComboBox*)pControl )->GetSelectedData();
	//	if( g_SkinningMethod != NewSkinningMethod )
	//	{
	//		g_SkinningMethod = NewSkinningMethod;

	//		for(  std::map< unsigned int, CAnimateMesh >::iterator amIter = g_AnotherCharacterList.begin()  ;
	//			  amIter != g_AnotherCharacterList.end()  ;  ++amIter  )
	//		{
	//			UpdateSkinningMethod( amIter->second.GetFrameRoot() );
	//		}

	//		UpdateSkinningMethod( g_ThisCharacter.GetFrameRoot() );
	//	}
	//	break;

	case IDC_START_CONNECTIP:
		switch( nEvent )
		{

		case EVENT_EDITBOX_STRING:
			break;

		case EVENT_EDITBOX_CHANGE:
			WideCharToMultiByte( CP_ACP, 0, ((CDXUTEditBox*)pControl)->GetText(), -1, g_ConnectIP, IPV4_LENG, NULL, FALSE );
			break;

		}
		break;

	case IDC_START_START:
		g_ThisClient.m_NetSystem.SetSocket( socket( AF_INET, SOCK_STREAM, 0 ) );
		ZeroMemory( &serveraddr, sizeof( serveraddr ) );
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_port = htons( MATCHLESS_SERVER_PORT );
		serveraddr.sin_addr.s_addr = inet_addr( g_ConnectIP );
		if( (bCreateSocketFailed =
				(INVALID_SOCKET == g_ThisClient.m_NetSystem.GetSocket()))  ||
			(bConnectToServerFailed =
				(SOCKET_ERROR == connect( g_ThisClient.m_NetSystem.GetSocket(), (SOCKADDR*)&serveraddr, sizeof( serveraddr ) )))  ||
			(bReceiveLoginDataFailed =
				(SOCKET_ERROR == tempMessage.ReceiveData( g_ThisClient.m_NetSystem.GetSocket(), 0 )))  ||
			(bLoginFailed =
				(Matchless::FSTC_LOGIN_FAILED == (Matchless::ENetMessageType)tempMessage.GetType())) )		// Failed connect to server
		{
			closesocket( g_ThisClient.m_NetSystem.GetSocket() );

			if( bCreateSocketFailed )
			{
				_tcscpy( g_Notice, TEXT( "Failed create network socket." ) );
			}
			else if( bConnectToServerFailed )
			{
				_tcscpy( g_Notice, TEXT( "Failed connect to server." ) );
			}
			else if( bReceiveLoginDataFailed )
			{
				_tcscpy( g_Notice, TEXT( "Failed receive login data." ) );
			}
			else if( bLoginFailed )
			{
				_tcscpy( g_Notice, TEXT( "Failed login." ) );
			}
			else
			{
				_tcscpy( g_Notice, TEXT( "Occur unknown error." ) );
			}
		}
		else																								// Succeed connect to server
		{
			unsigned int				bufIndex;
			unsigned int				tempID;
			unsigned short int			tempTeamNo;
			bool						tempbRM;
			Matchless::ECharacterClass	tempCC;

			memcpy( &tempID, tempMessage.GetpAddData(), sizeof( tempID ) );								bufIndex = sizeof( tempID );
			memcpy( &tempTeamNo, tempMessage.GetpAddData() + bufIndex, sizeof( tempTeamNo ) );			bufIndex += sizeof( tempTeamNo );
			memcpy( &tempbRM, tempMessage.GetpAddData() + bufIndex, sizeof( tempbRM ) );				bufIndex += sizeof( tempbRM );
			memcpy( &tempCC, tempMessage.GetpAddData() + bufIndex, sizeof( tempCC ) );					bufIndex += sizeof( tempCC );
			g_ThisClient.m_NetSystem.SetID( tempID );
			g_ThisClient.m_PlayerInfo.SetTeamNum( tempTeamNo );
			g_ThisClient.m_PlayerInfo.SetbRoomMaster( tempbRM );
			g_ThisClient.m_PlayerInfo.GetCharacterInfo().SetClass( tempCC );

			ChangeAndInformMainStepState( g_ThisClient, Matchless::EMSS_Wait );
			UpdateRoomMasterUI( Matchless::EMSS_Wait, tempbRM );
			((CDXUTComboBox*)g_MainStepWaitUI.GetControl( IDC_WAIT_CHARSELECT ))->SetSelectedByData( (void*)tempCC );

			if( NULL == CreateThread( NULL, 0, NetReceiveProcess, (LPVOID)g_ThisClient.m_NetSystem.GetSocket(), 0, &ThreadID ) )
			{
				_tcscpy( g_Notice, TEXT( "Failed create thread for receive network data" ) );
			}

			_tcscpy( g_Notice, TEXT( "Not found." ) );

#ifdef	CS_TESTCODE_ON
			g_Console.Output( "Succeed Server Connection\n" );
#endif

		}
		break;

	case IDC_WAIT_PLAYSTART:
		tempMessage.SendData( g_ThisClient.m_NetSystem.GetSocket(), 0, (unsigned int)Matchless::FCTS_GAMESTART_REQUEST, 0, NULL );
		break;

	case IDC_WAIT_QUIT:
		tempMessage.SendData( g_ThisClient.m_NetSystem.GetSocket(), 0, (unsigned int)Matchless::FCTS_LOGOUT_INFORM, 0, NULL );
		DXUTShutdown();
		break;

	case IDC_WAIT_CHARSELECT:
		WRAP_ENTER_CS( &g_CS, 9 );
		g_ThisClient.m_PlayerInfo.GetCharacterInfo().SetClass( (Matchless::ECharacterClass)(unsigned int)((CDXUTComboBox*)pControl)->GetSelectedData() );
		tempCharacterClass = g_ThisClient.m_PlayerInfo.GetCharacterInfo().GetClass();
		tempMessage.SendData(  g_ThisClient.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FCTS_CHARCLASS_UPDATE,
								sizeof( tempCharacterClass ),  (char*)&tempCharacterClass  );
		WRAP_LEAVE_CS( &g_CS, 9 );
		break;

	case IDC_WAIT_TEAMSELECT:
		WRAP_ENTER_CS( &g_CS, 10 );
		g_ThisClient.m_PlayerInfo.SetTeamNum( tempTN = (unsigned short int)((CDXUTComboBox*)pControl)->GetSelectedData() );
		tempMessage.SendData(  g_ThisClient.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FCTS_TEAM_UPDATE,
								sizeof( tempTN ),  (char*)&tempTN  );
		WRAP_LEAVE_CS( &g_CS, 10 );
		break;
		
	case IDC_WAIT_MAPSELECT_PREV:
		WRAP_ENTER_CS( &g_CS, 11 );
		g_CurrentMapKind = (0 == g_CurrentMapKind) ? MAP_KIND_COUNT - 1 : g_CurrentMapKind - 1 ;
		tempMessage.SendData(  g_ThisClient.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FCTS_MAP_UPDATE,
								sizeof( g_CurrentMapKind ),  (char*)&g_CurrentMapKind  );
		WRAP_LEAVE_CS( &g_CS, 11 );
		break;

	case IDC_WAIT_MAPSELECT_NEXT:
		WRAP_ENTER_CS( &g_CS, 12 );
		g_CurrentMapKind = ((MAP_KIND_COUNT - 1) == g_CurrentMapKind) ? 0 : g_CurrentMapKind + 1 ;
		tempMessage.SendData(  g_ThisClient.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FCTS_MAP_UPDATE,
								sizeof( g_CurrentMapKind ),  (char*)&g_CurrentMapKind  );
		WRAP_LEAVE_CS( &g_CS, 12 );
		break;

	case IDC_PLAY_PLAYQUIT:
		tempMessage.SendData( g_ThisClient.m_NetSystem.GetSocket(), 0, (unsigned int)Matchless::FCTS_GAMEOUT_REQUEST, 0, NULL );
		break;

	case IDC_ADJUST_PASS:
		WRAP_ENTER_CS( &g_CS, 13 );
		ChangeAndInformMainStepState( g_ThisClient, Matchless::EMSS_Wait );
		tempCharacterClass = g_ThisClient.m_PlayerInfo.GetCharacterInfo().GetClass();
		tempMessage.SendData(  g_ThisClient.m_NetSystem.GetSocket(),  0,  (unsigned int)Matchless::FCTS_CHARCLASS_UPDATE,
								sizeof( tempCharacterClass ),  (char*)&tempCharacterClass  );
		WRAP_LEAVE_CS( &g_CS, 13 );
		break;

	}
}


int InitApp( void )
{
	Matchless::SetSampleCharacterInfo();
	Matchless::SetSampleSkillInfo();


	g_ThisClient.m_NetSystem.SetID( 0 );
	g_ThisClient.m_PlayerInfo.SetMainStepState( Matchless::EMSS_Start );
	g_ThisClient.m_PlayerInfo.SetbRoomMaster( false );

	_tcscpy( g_Notice, TEXT( "Not found." ) );
	strncpy( g_ConnectIP, SERVER_IPADDR, IPV4_LENG );


	// Initialize dialog object
	g_SettingsDlg.Init( &g_DialogResourceManager );
	g_SystemUI.Init( &g_DialogResourceManager );
	g_GlobalUI.Init( &g_DialogResourceManager );

	g_MainStepStartUI.Init( &g_DialogResourceManager );
	g_MainStepStartUI.SetCallback( OnGUIEvent );
	g_MainStepStartUI.SetFont( 1, TEXT( "Comic Sans MS" ), 15, FW_NORMAL );
	g_MainStepStartUI.SetFont( 2, TEXT( "Courier New" ), 10, FW_NORMAL );

	g_MainStepWaitUI.Init( &g_DialogResourceManager );
	g_MainStepWaitUI.SetCallback( OnGUIEvent );

	g_MainStepPlayUI.Init( &g_DialogResourceManager );
	g_MainStepPlayUI.SetCallback( OnGUIEvent );

	g_MainStepAdjustUI.Init( &g_DialogResourceManager );
	g_MainStepAdjustUI.SetCallback( OnGUIEvent );

	g_MainStepEndUI.Init( &g_DialogResourceManager );
	g_MainStepEndUI.SetCallback( OnGUIEvent );


	// Initialize control member object in g_SystemUI & g_GlobalUI dialog
	g_SystemUI.SetCallback( OnGUIEvent );	int iY = 10;
	g_SystemUI.AddButton( IDC_TOGGLEFULLSCREEN, TEXT( "Toggle full screen" ), 35, iY, 125, 22 );
	//g_SystemUI.AddButton( IDC_TOGGLEREF, TEXT( "Toggle REF (F3)" ), 35, iY += 24, 125, 22, VK_F3 );
	//g_SystemUI.AddButton( IDC_CHANGEDEVICE, TEXT( "Change device (F2)" ), 35, iY += 24, 125, 22, VK_F2 );

	g_GlobalUI.SetCallback( OnGUIEvent );
	//g_GlobalUI.AddComboBox( IDC_METHOD, SCREEN_WIDTH / 2, 10, 230, 24, TEXT( 'S' ) );
	//g_GlobalUI.GetComboBox( IDC_METHOD )->AddItem( TEXT( "Fixed function non-indexed (s)kinning" ), (void*)D3DNONINDEXED );
	//g_GlobalUI.GetComboBox( IDC_METHOD )->AddItem( TEXT( "Fixed function indexed (s)kinning" ), (void*)D3DINDEXED );
	//g_GlobalUI.GetComboBox( IDC_METHOD )->AddItem( TEXT( "Software (s)kinning" ), (void*)SOFTWARE );
	g_GlobalUI.AddStatic( IDC_GLOBAL_NOTICE, g_Notice, SCREEN_WIDTH / 2 - 120, SCREEN_HEIGHT / 4, 240, 60 );
	g_GlobalUI.GetStatic( IDC_GLOBAL_NOTICE )->SetTextColor( D3DCOLOR_ARGB( 255, 255, 255, 0 ) );
	g_GlobalUI.GetControl( IDC_GLOBAL_NOTICE )->GetElement( 0 )->dwTextFormat = DT_CENTER | DT_VCENTER | DT_WORDBREAK;


	CDXUTIMEEditBox *	pIMEEdit;


	// Initialize control member object in g_MainStepStartUI dialog
	CDXUTIMEEditBox::InitDefaultElements( &g_MainStepStartUI );
	if(
		SUCCEEDED( CDXUTIMEEditBox::CreateIMEEditBox( &g_MainStepStartUI, IDC_START_CONNECTIP, TEXT( SERVER_IPADDR ), SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2, 160, 36, false, &pIMEEdit ) )
		)
	{
		g_MainStepStartUI.AddControl( pIMEEdit );
		pIMEEdit->GetElement( 0 )->iFont = 1;
		pIMEEdit->GetElement( 1 )->iFont = 1;
		pIMEEdit->GetElement( 9 )->iFont = 1;
		pIMEEdit->GetElement( 0 )->TextureColor.Init( D3DCOLOR_ARGB( 128, 255, 255, 255 ) );
		pIMEEdit->SetBorderWidth( 7 );
		pIMEEdit->SetTextColor( D3DCOLOR_ARGB( 255, 64, 64, 64 ) );
		pIMEEdit->SetCaretColor( D3DCOLOR_ARGB( 255, 64, 64, 64 ) );
		pIMEEdit->SetSelectedTextColor( D3DCOLOR_ARGB( 255, 255, 255, 255 ) );
		pIMEEdit->SetSelectedBackColor( D3DCOLOR_ARGB( 255, 40, 72, 72 ) );
	}
	g_MainStepStartUI.AddButton( IDC_START_START, TEXT( "START" ), SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 4 * 3, 160, 30, TEXT( 'S' ) );


	// Initialize control member object in g_MainStepWaitUI dialog
	g_MainStepWaitUI.AddButton( IDC_WAIT_PLAYSTART, TEXT( "PLAY START" ), 20, 20, 160, 30, TEXT( 'S' ) );
	g_MainStepWaitUI.AddButton( IDC_WAIT_QUIT, TEXT( "QUIT" ), SCREEN_WIDTH - 180, SCREEN_HEIGHT - 50, 160, 30, TEXT( 'Q' ) );

	CDXUTComboBox *		pComboBox;
	g_MainStepWaitUI.AddComboBox( IDC_WAIT_CHARSELECT, SCREEN_WIDTH / 2 - 90, SCREEN_HEIGHT / 3 * 2, 180, 30, TEXT( 'N' ), false, &pComboBox );
	if( pComboBox )
	{
		pComboBox->SetDropHeight( 60 );
		pComboBox->AddItem( TEXT( "Breaker" ), (void*)Matchless::ECC_Breaker );
		pComboBox->AddItem( TEXT( "Defender" ), (void*)Matchless::ECC_Defender );
		pComboBox->AddItem( TEXT( "Mage" ), (void*)Matchless::ECC_Mage );
		pComboBox->AddItem( TEXT( "Healer" ), (void*)Matchless::ECC_Healer );
	}
	g_ThisClient.m_PlayerInfo.GetCharacterInfo().SetClass(
		(Matchless::ECharacterClass)(unsigned int)g_MainStepWaitUI.GetComboBox( IDC_WAIT_CHARSELECT )->GetSelectedData()
		);

	g_MainStepWaitUI.AddComboBox( IDC_WAIT_TEAMSELECT, SCREEN_WIDTH / 2 - 90, SCREEN_HEIGHT / 4 * 3, 180, 30, TEXT( 'T' ), false, &pComboBox );
	if( pComboBox )
	{
		pComboBox->SetDropHeight( 30 );
		pComboBox->AddItem( TEXT( "1 Team" ), (void*)1 );
		pComboBox->AddItem( TEXT( "2 Team" ), (void*)2 );
	}
	g_ThisClient.m_PlayerInfo.GetCharacterInfo().SetClass(
		(Matchless::ECharacterClass)(unsigned int)g_MainStepWaitUI.GetComboBox( IDC_WAIT_TEAMSELECT )->GetSelectedData()
		);

	g_MainStepWaitUI.AddButton( IDC_WAIT_MAPSELECT_PREV, TEXT( "PREV" ), 42, SCREEN_HEIGHT - 84, 72, 42 );
	g_MainStepWaitUI.AddButton( IDC_WAIT_MAPSELECT_NEXT, TEXT( "NEXT" ), 42 + 100, SCREEN_HEIGHT - 84, 72, 42 );


	// Initialize control member object in g_MainStepPlayUI dialog
	g_MainStepPlayUI.AddButton( IDC_PLAY_PLAYQUIT, TEXT( "PLAY QUIT" ), 20, 20, 160, 30, TEXT( 'Q' ) );


	// Initialize control member object in g_MainStepAdjustUI dialog
	g_MainStepAdjustUI.AddButton( IDC_ADJUST_PASS, TEXT( "PASS" ), SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 4 * 3, 160, 30, TEXT( 'P' ) );

	g_ThisCharacter.MoveOnYAxis( 3000.0f );


	return 0;
}


int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{

#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif


	InitializeCriticalSection( &g_CS );

	WSADATA		wsa;
	if( 0 != WSAStartup( MAKEWORD( 2, 2 ), &wsa ) )
	{
		return -1;
	}


	DXUTSetCallbackD3D9DeviceAcceptable( IsDeviceAcceptable );
	DXUTSetCallbackDeviceChanging( ModifyDeviceSettings );
	DXUTSetCallbackD3D9DeviceCreated( OnCreateDevice );
	DXUTSetCallbackD3D9DeviceReset( OnResetDevice );
	DXUTSetCallbackD3D9DeviceLost( OnLostDevice );
	DXUTSetCallbackD3D9DeviceDestroyed( OnDestroyDevice );

	DXUTSetCallbackFrameMove( OnFrameMove );
	DXUTSetCallbackD3D9FrameRender( OnFrameRender );

	DXUTSetCallbackMsgProc( MsgProc );

	//DXUTSetCallbackKeyboard( KeyboardProc );		// Delegate to ProcessUserInput() function


	InitApp();

	DXUTInit( true, true );
	DXUTSetHotkeyHandling( true, true, true );
	DXUTSetCursorSettings( true, true );
	DXUTCreateWindow( TEXT( "Matchless" ) );

	LoadSound();

	DXUTCreateDevice( true, SCREEN_WIDTH, SCREEN_HEIGHT );

	DXUTMainLoop();

	delete	[]	g_pBoneMatrices;


	ReleaseSound();


	closesocket( g_ThisClient.m_NetSystem.GetSocket() );

	WSACleanup();

	//DeleteCriticalSection( &g_CS );


	return DXUTGetExitCode();
}