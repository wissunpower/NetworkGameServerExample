
#include	"stdafx.h"
#include	"ResourceManager.h"
#include	"cPacket.h"


int GetCharacterResourceName( const Matchless::ECharacterClass & aECC, tstring& sNameOut )
{
	switch( aECC )
	{

	case Matchless::ECC_Breaker:
		sNameOut = TEXT( "Character/Breaker/Character_Breaker.x" );
		break;

	case Matchless::ECC_Defender:
		sNameOut = TEXT( "Character/Defender/Character_Defender.x" );
		break;

	case Matchless::ECC_Mage:
		sNameOut = TEXT( "Character/Mage/Character_Mage.x" );
		break;

	case Matchless::ECC_Healer:
		sNameOut = TEXT( "Character/Healer/Character_Healer.x" );
		break;

	default:
		sNameOut = TEXT( "" );
		break;

	}

	return 0;
}


int InitializeCharacterMesh( DWORD aMeshOptions, LPDIRECT3DDEVICE9 apDevice, LPD3DXLOADUSERDATA apUserDataLoader )
{
	TCHAR	tempPath[ MAX_PATH ];
	TCHAR	tempName[ MAX_PATH / 4 ];


	RetrieveResourceFile( TEXT( "Character/Breaker/Character_Breaker.x" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	g_BreakerMesh.LoadFromX( tempPath, tempName, aMeshOptions, apDevice, apUserDataLoader );
	g_BreakerMesh.SetIdleAnimationSet( CHARACTER_ANIINDEX_IDLE );

	RetrieveResourceFile( TEXT( "Character/Defender/Character_Defender.x" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	g_DefenderMesh.LoadFromX( tempPath, tempName, aMeshOptions, apDevice, apUserDataLoader );
	g_DefenderMesh.SetIdleAnimationSet( CHARACTER_ANIINDEX_IDLE );

	RetrieveResourceFile( TEXT( "Character/Mage/Character_Mage.x" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	g_MageMesh.LoadFromX( tempPath, tempName, aMeshOptions, apDevice, apUserDataLoader );
	g_MageMesh.SetIdleAnimationSet( CHARACTER_ANIINDEX_IDLE );

	RetrieveResourceFile( TEXT( "Character/Healer/Character_Healer.x" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	g_HealerMesh.LoadFromX( tempPath, tempName, aMeshOptions, apDevice, apUserDataLoader );
	g_HealerMesh.SetIdleAnimationSet( CHARACTER_ANIINDEX_IDLE );

	return 0;
}


int DeleteCharacterMesh( void )
{
	g_BreakerMesh.DestroyAll();
	g_DefenderMesh.DestroyAll();
	g_MageMesh.DestroyAll();
	g_HealerMesh.DestroyAll();

	return 0;
}


int CreateGraphicResource( DWORD aMeshOptions, LPDIRECT3DDEVICE9 apDevice, LPD3DXLOADUSERDATA apUserDataLoader )
{
	if( g_IsGraphicResourceLoaded )
	{
		return 0;
	}


	HRESULT		hr;
	bool		tempBool;
	TCHAR		tempPath[ MAX_PATH ];
	TCHAR		tempName[ MAX_PATH / 4 ];
	D3DXMATRIX	tempMtx;
	CNetMessage	tempMessage;


	D3DXCreateSphere( apDevice, 32, 12, 12, &g_pTargetCharacterMark, NULL );


	TCHAR			tempCD[ MAX_PATH ];
	LPD3DXBUFFER	pAttributes;


	RetrieveResourceFile( TEXT( "Character/Teammark_Home.x" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );

	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );

	V_RETURN( D3DXLoadMeshFromX( tempName, D3DXMESH_MANAGED, apDevice, NULL, &pAttributes, NULL, &g_NumHomeTeamMarkMaterial, &g_pHomeTeamMark ) );

	if( g_NumHomeTeamMarkMaterial > 0 )
	{
		D3DXMATERIAL *	pTempMaterials = (D3DXMATERIAL*)pAttributes->GetBufferPointer();

		TCHAR	strTexturePath[ MAX_PATH ];
		TCHAR	tszBuf[ MAX_PATH ];


		g_pHomeTeamMarkMaterial = new D3DMATERIAL9[ g_NumHomeTeamMarkMaterial ];
		g_ppHomeTeamMarkTexture = new LPDIRECT3DTEXTURE9[ g_NumHomeTeamMarkMaterial ];
		if( g_pHomeTeamMarkMaterial == NULL || g_ppHomeTeamMarkTexture == NULL )
		{
			if( g_pHomeTeamMarkMaterial )
			{
				SAFE_DELETE_ARRAY( g_pHomeTeamMarkMaterial );
			}
			if( g_ppHomeTeamMarkTexture )
			{
				SAFE_DELETE_ARRAY( g_ppHomeTeamMarkTexture );
			}

			if( pAttributes )
			{
				pAttributes->Release();
			}
			return	(hr = E_OUTOFMEMORY);
		}

		for( DWORD i = 0 ; i < g_NumHomeTeamMarkMaterial ; ++i )
		{
			g_pHomeTeamMarkMaterial[ i ] = pTempMaterials[ i ].MatD3D;

			g_pHomeTeamMarkMaterial[ i ].Ambient = g_pHomeTeamMarkMaterial[ i ].Diffuse;
			g_ppHomeTeamMarkTexture[ i ] = NULL;

			if( pTempMaterials[ i ].pTextureFilename != NULL && strlen( pTempMaterials[ i ].pTextureFilename ) )
			{
#if defined(UNICODE) | defined(_UNICODE)
				MultiByteToWideChar( CP_ACP, 0, pTempMaterials[ i ].pTextureFilename, -1, tszBuf, MAX_PATH );
#else
				_tcsncpy( tszBuf, g_pHomeTeamMarkMaterial[ i ].pTextureFilename, MAX_PATH );
#endif
				tszBuf[ MAX_PATH - 1 ] = TEXT( '\0' );
				hr = DXUTFindDXSDKMediaFileCch( strTexturePath, MAX_PATH, tszBuf );
				if( FAILED(
					hr = D3DXCreateTextureFromFile( apDevice, strTexturePath, &g_ppHomeTeamMarkTexture[ i ] )
					) )
				{
					g_ppHomeTeamMarkTexture[ i ] = NULL;
				}
			}
		}
	}

	SetCurrentDirectory( tempCD );

	if( pAttributes )
	{
		pAttributes->Release();
	}


	RetrieveResourceFile( TEXT( "Character/Teammark_Away.x" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );

	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );

	V_RETURN( D3DXLoadMeshFromX( tempName, D3DXMESH_MANAGED, apDevice, NULL, &pAttributes, NULL, &g_NumAwayTeamMarkMaterial, &g_pAwayTeamMark ) );

	if( g_NumAwayTeamMarkMaterial > 0 )
	{
		D3DXMATERIAL *	pTempMaterials = (D3DXMATERIAL*)pAttributes->GetBufferPointer();

		TCHAR	strTexturePath[ MAX_PATH ];
		TCHAR	tszBuf[ MAX_PATH ];


		g_pAwayTeamMarkMaterial = new D3DMATERIAL9[ g_NumAwayTeamMarkMaterial ];
		g_ppAwayTeamMarkTexture = new LPDIRECT3DTEXTURE9[ g_NumAwayTeamMarkMaterial ];
		if( g_pAwayTeamMarkMaterial == NULL || g_ppAwayTeamMarkTexture == NULL )
		{
			if( g_pAwayTeamMarkMaterial )
			{
				SAFE_DELETE_ARRAY( g_pAwayTeamMarkMaterial );
			}
			if( g_ppAwayTeamMarkTexture )
			{
				SAFE_DELETE_ARRAY( g_ppAwayTeamMarkTexture );
			}

			if( pAttributes )
			{
				pAttributes->Release();
			}
			return	(hr = E_OUTOFMEMORY);
		}

		for( DWORD i = 0 ; i < g_NumAwayTeamMarkMaterial ; ++i )
		{
			g_pAwayTeamMarkMaterial[ i ] = pTempMaterials[ i ].MatD3D;

			g_pAwayTeamMarkMaterial[ i ].Ambient = g_pAwayTeamMarkMaterial[ i ].Diffuse;
			g_ppAwayTeamMarkTexture[ i ] = NULL;

			if( pTempMaterials[ i ].pTextureFilename != NULL && strlen( pTempMaterials[ i ].pTextureFilename ) )
			{
#if defined(UNICODE) | defined(_UNICODE)
				MultiByteToWideChar( CP_ACP, 0, pTempMaterials[ i ].pTextureFilename, -1, tszBuf, MAX_PATH );
#else
				_tcsncpy( tszBuf, g_pAwayTeamMarkMaterial[ i ].pTextureFilename, MAX_PATH );
#endif
				tszBuf[ MAX_PATH - 1 ] = TEXT( '\0' );
				hr = DXUTFindDXSDKMediaFileCch( strTexturePath, MAX_PATH, tszBuf );
				if( FAILED(
					hr = D3DXCreateTextureFromFile( apDevice, strTexturePath, &g_ppAwayTeamMarkTexture[ i ] )
					) )
				{
					g_ppAwayTeamMarkTexture[ i ] = NULL;
				}
			}
		}
	}

	SetCurrentDirectory( tempCD );

	if( pAttributes )
	{
		pAttributes->Release();
	}


	CreateTerrain( apDevice );


	{
		cMonitor::Owner lock{ g_Monitor };
		if ( 0 == g_CurrentMapKind )
		{
			g_SkyBox.Build( apDevice, TEXT( "SkyBox/SkyBox1/SkyBox_SkyBox1_" ), aMeshOptions );
		}
		else
		{
			g_SkyBox.Build( apDevice, TEXT( "SkyBox/SkyBox2/SkyBox_SkyBox2_" ), aMeshOptions );
		}
	}

	tstring sPath;
	GetCharacterResourceName( g_ThisClient.m_PlayerInfo.GetCharacterInfo().GetClass(), sPath );
	if( !FAILED( RetrieveResourceFile( sPath.c_str(), tempPath, MAX_PATH, tempName, MAX_PATH / 4 ) ) )
	{
		V_RETURN( g_ThisCharacter.LoadFromX( tempPath, tempName, aMeshOptions, apDevice, apUserDataLoader ) );
		g_ThisCharacter.SetIdleAnimationSet( CHARACTER_ANIINDEX_IDLE );
		for( int i = 0 ; i < 4 ; ++i )
		{
			for( int j = 0 ; j < 4 ; ++j )
			{
				tempMtx.m[ i ][ j ] = g_ThisClient.m_PlayerInfo.GetTransform().m[ i ][ j ];
			}
		}
		tempMtx._42 += GetDistanceWithMesh( &tempBool, D3DXVECTOR3( tempMtx._41, tempMtx._42, tempMtx._43 ), D3DXVECTOR3( 0.0f, 1.0f, 0.0f ), g_pTerrain, g_TerrainTransform );
		g_ThisCharacter.SetPosition( D3DXVECTOR3( tempMtx._41, tempMtx._42, tempMtx._43 ) );
		g_ThisCharacter.ResetAxis();
		g_ThisCharacter.SetTransforMatrix( tempMtx );
		if( 0 == g_ThisClient.m_PlayerInfo.GetTeamNum() % 2 )
		{
			g_ThisCharacter.Rotate( 0, 180.0f, 0 );
			g_ThisCharacter.UpdatePerFrame( 0.0, NULL );
		}
		for( int i = 0 ; i < 4 ; ++i )
		{
			for( int j = 0 ; j < 4 ; ++j )
			{
				g_ThisClient.m_PlayerInfo.GetTransform().m[ i ][ j ] = g_ThisCharacter.GetTransforMatrix().m[ i ][ j ];
			}
		}

		cMonitor::Owner lock{ g_Monitor };
		g_IsUpdateMovePosition = true;
		g_IsUpdateMoveAnimation = true;
	}

	{
		cMonitor::Owner lock{ g_Monitor };
		for ( auto cIt = g_AnotherClientList.begin() ; cIt != g_AnotherClientList.end() ; ++cIt )
		{
			GetCharacterResourceName( cIt->second.m_PlayerInfo.GetCharacterInfo().GetClass(), sPath );
			if ( !FAILED( RetrieveResourceFile( sPath.c_str(), tempPath, MAX_PATH, tempName, MAX_PATH / 4 ) ) )
			{
				auto amIt = g_AnotherCharacterList.insert( std::map< unsigned int, CAnimateMesh >::value_type( cIt->first, CAnimateMesh() ) ).first;
				V_RETURN( amIt->second.LoadFromX( tempPath, tempName, aMeshOptions, apDevice, apUserDataLoader ) );
				amIt->second.SetIdleAnimationSet( CHARACTER_ANIINDEX_IDLE );
				amIt->second.MoveOnYAxis(
					GetDistanceWithMesh( &tempBool, amIt->second.GetPosition(), amIt->second.GetYAxis(), g_pTerrain, g_TerrainTransform )
				);
				amIt->second.SetCurrentAnimationSet( 0, CHARACTER_ANIINDEX_IDLE );
				amIt->second.SetCurrentAnimationSet( 1, CHARACTER_ANIINDEX_IDLE );
			}
		}
	}


	g_IsGraphicResourceLoaded = true;

	cOPacket oPacket;
	oPacket.Encode4u( Matchless::FCTS_GAME_MOVE_ALL_REQUEST );
	oPacket.Send( g_ThisClient.m_NetSystem.GetSocket() );

	return 0;
}


int DestroyGraphicResource( void )
{
	if( !g_IsGraphicResourceLoaded )
	{
		return 0;
	}


	g_ThisCharacter.DestroyAll();
	g_AnotherCharacterList.clear();


	g_SkyBox.Release();


	SAFE_DELETE_ARRAY( g_pTerrainMaterial );
	for( DWORD i = 0 ; i < g_NumTerrainMaterial ; ++i )
	{
		SAFE_RELEASE( g_ppTerrainTexture[ i ] );
	}
	SAFE_DELETE_ARRAY( g_ppTerrainTexture );

	if( g_pTerrain )
	{
		g_pTerrain->Release();
	}


	SAFE_DELETE_ARRAY( g_pAwayTeamMarkMaterial );
	for( DWORD i = 0 ; i < g_NumAwayTeamMarkMaterial ; ++i )
	{
		SAFE_RELEASE( g_ppAwayTeamMarkTexture[ i ] );
	}
	SAFE_DELETE_ARRAY( g_ppAwayTeamMarkTexture );

	if( g_pAwayTeamMark )
	{
		g_pAwayTeamMark->Release();
	}


	SAFE_DELETE_ARRAY( g_pHomeTeamMarkMaterial );
	for( DWORD i = 0 ; i < g_NumHomeTeamMarkMaterial ; ++i )
	{
		SAFE_RELEASE( g_ppHomeTeamMarkTexture[ i ] );
	}
	SAFE_DELETE_ARRAY( g_ppHomeTeamMarkTexture );

	if( g_pHomeTeamMark )
	{
		g_pHomeTeamMark->Release();
	}


	if( g_pTargetCharacterMark )
	{
		g_pTargetCharacterMark->Release();
	}


	g_IsGraphicResourceLoaded = false;

	return 0;
}


HRESULT CreateTerrain( IDirect3DDevice9 * pd3dDevice )
{
	HRESULT		hr;
	TCHAR	tempPath[ MAX_PATH ];
	TCHAR	tempName[ MAX_PATH / 4 ];


	TCHAR	tempCD[ MAX_PATH ];
	LPD3DXBUFFER	pAttributes;


	{
		cMonitor::Owner lock{ g_Monitor };
		if ( 0 == g_CurrentMapKind )
		{
			RetrieveResourceFile( TEXT( "Terrain/Terrain1/Terrain_Terrain1.x" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
		}
		else
		{
			RetrieveResourceFile( TEXT( "Terrain/Terrain2/Terrain_Terrain2.x" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
		}
	}


	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );

	V_RETURN( D3DXLoadMeshFromX( tempName, D3DXMESH_MANAGED, pd3dDevice, NULL, &pAttributes, NULL, &g_NumTerrainMaterial, &g_pTerrain ) );

	if( g_NumTerrainMaterial > 0 )
	{
		D3DXMATERIAL *	pTempMaterials = (D3DXMATERIAL*)pAttributes->GetBufferPointer();

		TCHAR	strTexturePath[ MAX_PATH ];
		TCHAR	tszBuf[ MAX_PATH ];


		g_pTerrainMaterial = new D3DMATERIAL9[ g_NumTerrainMaterial ];
		g_ppTerrainTexture = new LPDIRECT3DTEXTURE9[ g_NumTerrainMaterial ];
		if( g_pTerrainMaterial == NULL || g_ppTerrainTexture == NULL )
		{
			if( g_pTerrainMaterial )
			{
				SAFE_DELETE_ARRAY( g_pTerrainMaterial );
			}
			if( g_ppTerrainTexture )
			{
				SAFE_DELETE_ARRAY( g_ppTerrainTexture );
			}

			if( pAttributes )
			{
				pAttributes->Release();
			}
			return	(hr = E_OUTOFMEMORY);
		}

		for( DWORD i = 0 ; i < g_NumTerrainMaterial ; ++i )
		{
			g_pTerrainMaterial[ i ] = pTempMaterials[ i ].MatD3D;

			g_pTerrainMaterial[ i ].Ambient = g_pTerrainMaterial[ i ].Diffuse;
			g_ppTerrainTexture[ i ] = NULL;

			if( pTempMaterials[ i ].pTextureFilename != NULL && strlen( pTempMaterials[ i ].pTextureFilename ) )
			{
#if defined(UNICODE) | defined(_UNICODE)
				MultiByteToWideChar( CP_ACP, 0, pTempMaterials[ i ].pTextureFilename, -1, tszBuf, MAX_PATH );
#else
				_tcsncpy( tszBuf, g_pTerrainMaterial[ i ].pTextureFilename, MAX_PATH );
#endif
				tszBuf[ MAX_PATH - 1 ] = TEXT( '\0' );
				hr = DXUTFindDXSDKMediaFileCch( strTexturePath, MAX_PATH, tszBuf );
				if( FAILED(
					hr = D3DXCreateTextureFromFile( pd3dDevice, strTexturePath, &g_ppTerrainTexture[ i ] )
					) )
				{
					g_ppTerrainTexture[ i ] = NULL;
				}
			}
		}
	}

	SetCurrentDirectory( tempCD );

	if( pAttributes )
	{
		pAttributes->Release();
	}


	D3DXMatrixIdentity( &g_TerrainTransform );
	D3DXMatrixTranslation( &g_TerrainTransform, 0.0f, -100.0f, 0.0f );


	return	hr;
}


void RenderTerrain( IDirect3DDevice9 * pd3dDevice )
{
	if( pd3dDevice == NULL )
	{
		return;
	}


	HRESULT		hr;


	pd3dDevice->SetTransform( D3DTS_WORLD, &g_TerrainTransform );
	pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
	
	for( DWORD i = 0 ; i < g_NumTerrainMaterial ; ++i )
	{
		V( pd3dDevice->SetMaterial( &g_pTerrainMaterial[ i ] ) );
		V( pd3dDevice->SetTexture( 0, g_ppTerrainTexture[ i ] ) );
		V( g_pTerrain->DrawSubset( i ) );
	}
}


HRESULT DrawGameUI( ID3DXSprite * pSprite, ID3DXFont * pFont, const Matchless::EMainStepState aStep )
{
	HRESULT		hr = S_OK;

	const D3DSURFACE_DESC *		pd3dsdBackBuffer = DXUTGetD3D9BackBufferSurfaceDesc();


	V_RETURN( pSprite->Begin( D3DXSPRITE_ALPHABLEND | D3DXSPRITE_SORT_TEXTURE ) );

	{
		cMonitor::Owner lock{ g_Monitor };
		switch ( aStep )
		{

		case Matchless::EMSS_Wait:
			hr = DrawGameWaitUI( pSprite, pFont, pd3dsdBackBuffer );
			break;

		case Matchless::EMSS_Play:
			hr = DrawGamePlayUI( pSprite, pFont, pd3dsdBackBuffer );
			break;

		}
	}


	V_RETURN( pSprite->End() );


	return	hr;
}


HRESULT DrawGameWaitUI( ID3DXSprite * pSprite, ID3DXFont * pFont, const D3DSURFACE_DESC * pBackBufferSurfaceDesc )
{
	HRESULT		hr = S_OK;

	CDXUTTextHelper				txtHelper( pFont, pSprite, 15 );
	LPDIRECT3DTEXTURE9			tempTexture = NULL;
	D3DXVECTOR3					tempPos;
	D3DXVECTOR3					tempPos2;
	float						xUnit;
	float						yUnit;
	float						tempFriendWidth;
	float						tempFriendHeight;
	float						tempEnemyWidth;
	float						tempEnemyHeight;
	TCHAR						tempStr[ 32 ];
	D3DXMATRIX					tempMatrix;


	if( pBackBufferSurfaceDesc )
	{
		xUnit = (float)(pBackBufferSurfaceDesc->Width) / 800.0f;
		yUnit = (float)(pBackBufferSurfaceDesc->Height) / 600.0f;
	}
	else
	{
		xUnit = 1.0f;
		yUnit = 1.0f;
	}

	tempFriendWidth = 26.0f;
	tempEnemyWidth = 587.0f;
	tempFriendHeight = tempEnemyHeight = 145.0f;
	D3DXMatrixIdentity( &tempMatrix );
	tempMatrix._11 = xUnit;
	tempMatrix._22 = yUnit;


	pSprite->SetTransform( &tempMatrix );


	// draw this play information.
	switch( g_ThisClient.m_PlayerInfo.GetCharacterInfo().GetClass() )
	{

	case Matchless::ECC_Breaker:
		tempTexture = g_pTexBreakerEmblem;
		break;

	case Matchless::ECC_Defender:
		tempTexture = g_pTexDefenderEmblem;
		break;

	case Matchless::ECC_Mage:
		tempTexture = g_pTexMageEmblem;
		break;

	case Matchless::ECC_Healer:
		tempTexture = g_pTexHealerEmblem;
		break;

	default:
		tempTexture = NULL;
		break;

	}

	tempPos.x = tempFriendWidth;
	tempPos.y = tempFriendHeight;
	tempPos.z = 0.375f;
	hr = pSprite->Draw( g_pTexAnCharInfoFrame, NULL, NULL, &tempPos, D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );

	tempPos2.x = tempPos.x + 8;
	tempPos2.y = tempPos.y + 6;
	tempPos2.z = 0.0f;
	hr = pSprite->Draw( tempTexture, NULL, NULL, &tempPos2, D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );

	txtHelper.SetInsertionPos( (int)(tempFriendWidth + 64.0f), (int)tempPos.y );
	txtHelper.SetForegroundColor( D3DXCOLOR( 0.0f, 0.0f, 0.0f, 1.0f ) );
	_stprintf( tempStr, TEXT( "My ID : %u" ), g_ThisClient.m_NetSystem.GetID() );
	txtHelper.DrawTextLine( tempStr );

	tempPos.y = tempFriendHeight;
	tempFriendHeight += 60.0f;


	// draw another play information.
	{
		cMonitor::Owner lock{ g_Monitor };
		for ( auto acIter = g_AnotherClientList.begin() ; acIter != g_AnotherClientList.end() ; ++acIter )
		{
			switch ( acIter->second.m_PlayerInfo.GetCharacterInfo().GetClass() )
			{

			case Matchless::ECC_Breaker:
				tempTexture = g_pTexBreakerEmblem;
				break;

			case Matchless::ECC_Defender:
				tempTexture = g_pTexDefenderEmblem;
				break;

			case Matchless::ECC_Mage:
				tempTexture = g_pTexMageEmblem;
				break;

			case Matchless::ECC_Healer:
				tempTexture = g_pTexHealerEmblem;
				break;

			default:
				tempTexture = NULL;
				break;

			}

			if ( g_ThisClient.m_PlayerInfo.GetTeamNum() == acIter->second.m_PlayerInfo.GetTeamNum() )
			{
				tempPos.x = tempFriendWidth;
				tempPos.y = tempFriendHeight;
				tempFriendHeight += 60.0f;
			}
			else
			{
				tempPos.x = tempEnemyWidth;
				tempPos.y = tempEnemyHeight;
				tempEnemyHeight += 60.0f;
			}
			tempPos.z = 0.375f;
			hr = pSprite->Draw( g_pTexAnCharInfoFrame, NULL, NULL, &tempPos, D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );

			tempPos2.x = tempPos.x + 8;
			tempPos2.y = tempPos.y + 6;
			tempPos2.z = 0.0f;
			hr = pSprite->Draw( tempTexture, NULL, NULL, &tempPos2, D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );

			txtHelper.SetInsertionPos( (int)( tempPos.x + 64.0f ), (int)tempPos.y );
			txtHelper.SetForegroundColor( D3DXCOLOR( 0.0f, 0.0f, 0.0f, 1.0f ) );
			_stprintf( tempStr, TEXT( "ID : %u" ), acIter->second.m_NetSystem.GetID() );
			txtHelper.DrawTextLine( tempStr );
		}
	}


	// draw map information.
	switch( g_CurrentMapKind )
	{

	case 0:
		tempTexture = g_pTexTerrain1Emblem;
		break;

	case 1:
		tempTexture = g_pTexTerrain2Emblem;
		break;

	default:
		tempTexture = NULL;
		break;

	}

	tempPos.x = 64.0f;
	tempPos.y = 376.0f;

	hr = pSprite->Draw( tempTexture, NULL, NULL, &tempPos, D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );


	tempPos.x = 12;
	tempPos.y = 100;
	tempPos.z = 0.5f;
	hr = pSprite->Draw( g_pTexHomePartyWord, NULL, NULL, &tempPos, D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );

	tempPos.x = 12;
	tempPos.y = 128;
	tempPos.z = 0.5f;
	hr = pSprite->Draw( g_pTexPartyInfoFrame, NULL, NULL, &tempPos, D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );

	tempPos.x = 573;
	tempPos.y = 100;
	tempPos.z = 0.5f;
	hr = pSprite->Draw( g_pTexAwayPartyWord, NULL, NULL, &tempPos, D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );

	tempPos.x = 573;
	tempPos.y = 128;
	tempPos.z = 0.5f;
	hr = pSprite->Draw( g_pTexPartyInfoFrame, NULL, NULL, &tempPos, D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );


	return	hr;
}


HRESULT DrawGamePlayUI( ID3DXSprite * pSprite, ID3DXFont * pFont, const D3DSURFACE_DESC * pBackBufferSurfaceDesc )
{
	HRESULT		hr = S_OK;
	int			i = 0;

	CDXUTTextHelper				txtHelper( pFont, pSprite, 15 );
	LPDIRECT3DTEXTURE9			tempTexture = NULL;
	LPDIRECT3DTEXTURE9			tempSkillIconTex[ 9 ];
	D3DXVECTOR3					tempPos;
	D3DXVECTOR3					tempPos2;
	float						xUnit;
	float						yUnit;
	float						tempFriendWidth;
	float						tempFriendHeight;
	float						tempEnemyWidth;
	float						tempEnemyHeight;
	TCHAR						tempStr[ 32 ];
	RECT						tempRect;
	D3DXMATRIX					tempMatrix;


//#define		TX( x )		(xUnit * (x))
//#define		TY( y )		(yUnit * (y))
#define		TX( x )		(x)
#define		TY( y )		(y)


	if( pBackBufferSurfaceDesc )
	{
		xUnit = (float)(pBackBufferSurfaceDesc->Width) / 800.0f;
		yUnit = (float)(pBackBufferSurfaceDesc->Height) / 600.0f;
	}
	else
	{
		xUnit = 1.0f;
		yUnit = 1.0f;
	}

	tempFriendWidth = 26.0f;
	tempEnemyWidth = 587.0f;
	tempFriendHeight = tempEnemyHeight = 395.0f;
	D3DXMatrixIdentity( &tempMatrix );
	tempMatrix._11 = xUnit;
	tempMatrix._22 = yUnit;


	tempPos.x = 32.0f;
	tempPos.y = 128.0f;
	tempPos.z = 0.0f;


	pSprite->SetTransform( &tempMatrix );


	// draw another play information.
	{
		cMonitor::Owner lock{ g_Monitor };
		for ( auto acIter = g_AnotherClientList.begin() ; acIter != g_AnotherClientList.end() ; ++acIter )
		{
			switch ( acIter->second.m_PlayerInfo.GetCharacterInfo().GetClass() )
			{

			case Matchless::ECC_Breaker:
				tempTexture = g_pTexBreakerEmblem;
				break;

			case Matchless::ECC_Defender:
				tempTexture = g_pTexDefenderEmblem;
				break;

			case Matchless::ECC_Mage:
				tempTexture = g_pTexMageEmblem;
				break;

			case Matchless::ECC_Healer:
				tempTexture = g_pTexHealerEmblem;
				break;

			default:
				tempTexture = NULL;
				break;

			}

			if ( g_ThisClient.m_PlayerInfo.GetTeamNum() == acIter->second.m_PlayerInfo.GetTeamNum() )
			{
				tempPos.x = TX( tempFriendWidth );
				tempPos.y = TY( tempFriendHeight );
				tempFriendHeight += 60.0f;
			}
			else
			{
				tempPos.x = TX( tempEnemyWidth );
				tempPos.y = TY( tempEnemyHeight );
				tempEnemyHeight += 60.0f;
			}
			tempPos.z = 0.375f;
			hr = pSprite->Draw( g_pTexAnCharInfoFrame, NULL, NULL, &tempPos, D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );

			tempPos2.x = tempPos.x + 50;
			tempPos2.y = tempPos.y + 7;
			tempPos2.z = 0.25f;
			hr = pSprite->Draw( g_pTexAnHealthGauge, NULL, NULL, &tempPos2, D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );

			tempRect.left = 0;
			tempRect.right = (LONG)( (float)g_ImgInfoAnHealthBar.Width / (float)acIter->second.m_PlayerInfo.GetCharacterInfo().GetMaxHealth()  *
				(float)( acIter->second.m_PlayerInfo.GetCharacterInfo().GetCurrentHealth() ) );
			tempRect.top = 0;
			tempRect.bottom = g_ImgInfoHealthGauge.Height;
			tempPos2.x = tempPos.x + 58;
			tempPos2.y = tempPos.y + 10;
			tempPos2.z = 0.125f;
			hr = pSprite->Draw( g_pTexAnHealthBar, &tempRect, NULL, &tempPos2, D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );

			tempPos2.x = tempPos.x + 50;
			tempPos2.y = tempPos.y + 27;
			tempPos2.z = 0.25f;
			hr = pSprite->Draw( g_pTexAnEnergyGauge, NULL, NULL, &tempPos2, D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );

			tempRect.left = 0;
			tempRect.right = (LONG)( (float)g_ImgInfoAnEnergyBar.Width / (float)acIter->second.m_PlayerInfo.GetCharacterInfo().GetMaxEnergy()  *
				(float)( acIter->second.m_PlayerInfo.GetCharacterInfo().GetCurrentEnergy() ) );
			tempRect.top = 0;
			tempRect.bottom = g_ImgInfoAnEnergyBar.Height;
			tempPos2.x = tempPos.x + 58;
			tempPos2.y = tempPos.y + 30;
			tempPos2.z = 0.125f;
			hr = pSprite->Draw( g_pTexAnEnergyBar, &tempRect, NULL, &tempPos2, D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );

			tempPos2.x = tempPos.x + 8;
			tempPos2.y = tempPos.y + 6;
			tempPos2.z = 0.0f;
			hr = pSprite->Draw( tempTexture, NULL, NULL, &tempPos2, D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );

			txtHelper.SetInsertionPos( (int)( tempPos.x + 64.0f ), (int)tempPos.y - 5 );
			txtHelper.SetForegroundColor( D3DXCOLOR( 0.0f, 0.0f, 0.0f, 1.0f ) );
			_stprintf( tempStr, TEXT( "ID : %u" ), acIter->second.m_NetSystem.GetID() );
			txtHelper.DrawTextLine( tempStr );
		}
	}


	tempPos.x = TX( 226 );
	tempPos.y = TY( 458 );
	tempPos.z = 0.0625f;
	hr = pSprite->Draw( g_pTexGaugePanel, NULL, NULL, &tempPos, D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );

	tempPos.x = TX( 303 );
	tempPos.y = TY( 464 );
	tempPos.z = 0.0625f;
	hr = pSprite->Draw( g_pTexSkillPanel, NULL, NULL, &tempPos, D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );

	tempRect.left = 0;
	tempRect.right = g_ImgInfoHealthGauge.Width;
	tempRect.top = (LONG)((float)g_ImgInfoHealthGauge.Height  /  (float)g_ThisClient.m_PlayerInfo.GetCharacterInfo().GetMaxHealth()  *
							(float)(g_ThisClient.m_PlayerInfo.GetCharacterInfo().GetMaxHealth() - g_ThisClient.m_PlayerInfo.GetCharacterInfo().GetCurrentHealth()));
	tempRect.bottom = g_ImgInfoHealthGauge.Height;
	tempPos.x = TX( 250 );
	tempPos.y = TY( 470 ) + (float)tempRect.top;
	tempPos.z = 0.0f;
	hr = pSprite->Draw( g_pTexHealthGauge, &tempRect, NULL, &tempPos, D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );

	tempRect.left = 0;
	tempRect.right = g_ImgInfoEnergyGauge.Width;
	tempRect.top = (LONG)((float)g_ImgInfoEnergyGauge.Height  /  (float)g_ThisClient.m_PlayerInfo.GetCharacterInfo().GetMaxEnergy()  *
							(float)(g_ThisClient.m_PlayerInfo.GetCharacterInfo().GetMaxEnergy() - g_ThisClient.m_PlayerInfo.GetCharacterInfo().GetCurrentEnergy()));
	tempRect.bottom = g_ImgInfoHealthGauge.Height;
	tempPos.x = TX( 296 );
	tempPos.y = TY( 470 ) + (float)tempRect.top;
	tempPos.z = 0.0f;
	hr = pSprite->Draw( g_pTexEnergyGauge, &tempRect, NULL, &tempPos, D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );


	tempPos.x = TX( 12 );
	tempPos.y = TY( 347 );
	tempPos.z = 0.5f;
	hr = pSprite->Draw( g_pTexHomePartyWord, NULL, NULL, &tempPos, D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );

	tempPos.x = TX( 12 );
	tempPos.y = TY( 375 );
	tempPos.z = 0.5f;
	hr = pSprite->Draw( g_pTexPartyInfoFrame, NULL, NULL, &tempPos, D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );

	tempPos.x = TX( 573 );
	tempPos.y = TY( 347 );
	tempPos.z = 0.5f;
	hr = pSprite->Draw( g_pTexAwayPartyWord, NULL, NULL, &tempPos, D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );

	tempPos.x = TX( 573 );
	tempPos.y = TY( 375 );
	tempPos.z = 0.5f;
	hr = pSprite->Draw( g_pTexPartyInfoFrame, NULL, NULL, &tempPos, D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );

	if( g_IsNowCasting  &&
		g_CastStartTick <= g_Timer.GetTick()  &&  g_Timer.GetTick() <= g_CastEndTick )
	{
		tempPos.x = TX( 302 );
		tempPos.y = TY( 425 );
		tempPos.z = 0.125f;
		hr = pSprite->Draw( g_pTexCastBarGauge, NULL, NULL, &tempPos, D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );

		tempRect.left = 0;
		tempRect.right = (LONG)((float)g_ImgCastBarGrowth.Width / (g_CastEndTick - g_CastStartTick) * (g_Timer.GetTick() - g_CastStartTick));
		tempRect.top = 0;
		tempRect.bottom = g_ImgCastBarGrowth.Height;
		tempPos.x = TX( 307 );
		tempPos.y = TY( 430 );
		tempPos.z = 0.0f;
		hr = pSprite->Draw( g_pTexCastBarGrowth, &tempRect, NULL, &tempPos, D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
	}


	switch( g_ThisClient.m_PlayerInfo.GetCharacterInfo().GetClass() )
	{

	case Matchless::ECC_Breaker:
		tempSkillIconTex[ 0 ] = g_pTexBreakerNormalAttackIcon;
		tempSkillIconTex[ 1 ] = g_pTexBreakerRaidIcon;
		tempSkillIconTex[ 2 ] = g_pTexBreakerStunIcon;
		tempSkillIconTex[ 3 ] = g_pTexBreakerArmorBreakIcon;
		tempSkillIconTex[ 4 ] = g_pTexBreakerPowerBreakIcon;
		tempSkillIconTex[ 5 ] = g_pTexBreakerBerserkIcon;
		tempSkillIconTex[ 6 ] = g_pTexBreakerChargeIcon;
		tempSkillIconTex[ 7 ] = g_pTexBreakerSilentIcon;
		tempSkillIconTex[ 8 ] = g_pTexBreakerLastHitIcon;
		break;

	case Matchless::ECC_Defender:
		tempSkillIconTex[ 0 ] = g_pTexDefenderNormalAttackIcon;
		tempSkillIconTex[ 1 ] = g_pTexDefenderStoneSkinIcon;
		tempSkillIconTex[ 2 ] = g_pTexDefenderNaturalIcon;
		tempSkillIconTex[ 3 ] = g_pTexDefenderSacrificeIcon;
		tempSkillIconTex[ 4 ] = g_pTexDefenderAttackBreakIcon;
		tempSkillIconTex[ 5 ] = g_pTexDefenderHeavyBlowIcon;
		tempSkillIconTex[ 6 ] = g_pTexDefenderSpiritLinkIcon;
		tempSkillIconTex[ 7 ] = g_pTexDefenderEarthquakeIcon;
		tempSkillIconTex[ 8 ] = g_pTexDefenderFullThrowIcon;
		break;

	case Matchless::ECC_Mage:
		tempSkillIconTex[ 0 ] = g_pTexMageNormalAttackIcon;
		tempSkillIconTex[ 1 ] = g_pTexMageIceShieldIcon;
		tempSkillIconTex[ 2 ] = g_pTexMageMagicalArmorIcon;
		tempSkillIconTex[ 3 ] = g_pTexMageIceBoltIcon;
		tempSkillIconTex[ 4 ] = g_pTexMageFireBallIcon;
		tempSkillIconTex[ 5 ] = g_pTexMageBlizzardIcon;
		tempSkillIconTex[ 6 ] = g_pTexMageFairyRescueIcon;
		tempSkillIconTex[ 7 ] = g_pTexMageLightningEmissionIcon;
		tempSkillIconTex[ 8 ] = g_pTexMageChainLightningIcon;
		break;

	case Matchless::ECC_Healer:
		tempSkillIconTex[ 0 ] = g_pTexHealerNormalAttackIcon;
		tempSkillIconTex[ 1 ] = g_pTexHealerPerfectIcon;
		tempSkillIconTex[ 2 ] = g_pTexHealerQuickHealIcon;
		tempSkillIconTex[ 3 ] = g_pTexHealerNormalHealIcon;
		tempSkillIconTex[ 4 ] = g_pTexHealerLandBlessingIcon;
		tempSkillIconTex[ 5 ] = g_pTexHealerDispelIcon;
		tempSkillIconTex[ 6 ] = g_pTexHealerFreePromiseIcon;
		tempSkillIconTex[ 7 ] = g_pTexHealerSoulCryIcon;
		tempSkillIconTex[ 8 ] = g_pTexHealerDischargeIcon;
		break;

	default:
		tempSkillIconTex[ 0 ] = NULL;
		tempSkillIconTex[ 1 ] = NULL;
		tempSkillIconTex[ 2 ] = NULL;
		tempSkillIconTex[ 3 ] = NULL;
		tempSkillIconTex[ 4 ] = NULL;
		tempSkillIconTex[ 5 ] = NULL;
		tempSkillIconTex[ 6 ] = NULL;
		tempSkillIconTex[ 7 ] = NULL;
		tempSkillIconTex[ 8 ] = NULL;
		break;

	}

	for( int i = 0 ; i < 9 ; ++i )
	{
		tempPos.x = (float)( TX( 352 + 39 * (i % 5) ) );
		tempPos.y = (float)( TY( 486 + 36 * (i / 5) ) );
		tempPos.z = 0.0f;
		hr = pSprite->Draw( tempSkillIconTex[ i ], NULL, NULL, &tempPos, D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
	}


	i = 0;
	for(  std::list< Matchless::CState >::iterator sIter = g_ThisClient.m_PlayerInfo.GetCharacterInfo().GetStateList().begin()  ;
			sIter != g_ThisClient.m_PlayerInfo.GetCharacterInfo().GetStateList().end()  ;  ++sIter  )
	{
		tempTexture = GetStateIcon( sIter->GetType() );
		tempPos.x = (float)( TX( 750 - (48 * (i % 8)) ) );
		tempPos.y = (float)( TY( 50 + (48 * (i / 8)) ) );
		tempPos.z = 0.0f;
		hr = pSprite->Draw( tempTexture, NULL, NULL, &tempPos, D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f ) );
		++i;
	}


#undef		TX( x )
#undef		TY( y )


	return	hr;
}


LPDIRECT3DTEXTURE9 GetStateIcon( const Matchless::EStateType aType )
{
	LPDIRECT3DTEXTURE9	returnTex = NULL;

	switch( aType )
	{

	case Matchless::EST_Perfect:
		returnTex = g_pTexHealerPerfectIcon;
		break;

	case Matchless::EST_PhyDamInc:
		break;

	case Matchless::EST_PhyDamDec:
		returnTex = g_pTexBreakerPowerBreakIcon;
		break;

	case Matchless::EST_PhyArmInc:
		returnTex = g_pTexDefenderStoneSkinIcon;
		break;

	case Matchless::EST_PhyArmDec:
		returnTex = g_pTexBreakerArmorBreakIcon;
		break;

	case Matchless::EST_MagDamInc:
		break;

	case Matchless::EST_MagDamDec:
		returnTex = g_pTexDefenderAttackBreakIcon;
		break;

	case Matchless::EST_MagArmInc:
		returnTex = g_pTexDefenderNaturalIcon;
		break;

	case Matchless::EST_MagArmDec:
		break;

	case Matchless::EST_Frozen:
		break;

	case Matchless::EST_Faint:
		returnTex = g_pTexBreakerStunIcon;
		break;

	case Matchless::EST_Silence:
		returnTex = g_pTexBreakerSilentIcon;
		break;

	case Matchless::EST_Panic:
		break;

	default:
		returnTex = NULL;
		break;
	}

	return	returnTex;
}
