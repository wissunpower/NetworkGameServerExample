
#include	"DXDeviceHandler.h"



bool CALLBACK IsDeviceAcceptable( D3DCAPS9 * pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed, void * pUserContext )
{
	IDirect3D9 * pD3D = DXUTGetD3D9Object();

	if( FAILED(
		pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType, AdapterFormat,
									D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING, D3DRTYPE_TEXTURE, BackBufferFormat )
		) )
		return false;

	return true;
}


bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings * pDeviceSettings, void * pUserContext )
{
	assert( DXUT_D3D9_DEVICE == pDeviceSettings->ver );


	HRESULT	hr;
	IDirect3D9 * pD3D = DXUTGetD3D9Object();
	D3DCAPS9	caps;

	V( pD3D->GetDeviceCaps( pDeviceSettings->d3d9.AdapterOrdinal,
							pDeviceSettings->d3d9.DeviceType,
							&caps ) );

	// Turn vsync off
	pDeviceSettings->d3d9.pp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	g_SettingsDlg.GetDialogControl()->GetComboBox( DXUTSETTINGSDLG_PRESENT_INTERVAL )->SetEnabled( false );

	// If device doesn't support HW T&L or doesn't support 1.1 vertex shaders in HW
	// then switch to SWVP.
	if( ( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT ) == 0  ||
		caps.VertexShaderVersion < D3DVS_VERSION( 1, 1 ) )
	{
		pDeviceSettings->d3d9.BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

	// If the hardware cannot do vertex blending, use software vertex processing.
	if( caps.MaxVertexBlendMatrices < 2 )
	{
		pDeviceSettings->d3d9.BehaviorFlags = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

	// If using hardware vertex processing, change to mixed vertex processing
	// so there is a fallback.
	if( pDeviceSettings->d3d9.BehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING )
	{
		pDeviceSettings->d3d9.BehaviorFlags = D3DCREATE_MIXED_VERTEXPROCESSING;
	}

	// Debugging vertex shaders requires either REF or software vertex processing
	// and debugging pixel shaders requires REF.
#ifdef	DEBUG_VS
	if( pDeviceSettings->d3d9.DeviceType != D3DDEVTYPE_REF )
	{
		pDeviceSettings->d3d9.BehaviorFlags &= ~D3DCREATE_HARDWARE_VERTEXPROCESSING;
		pDeviceSettings->d3d9.BehaviorFlags &= ~D3DCREATE_PUREDEVICE;
		pDeviceSettings->d3d9.BehaviorFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}
#endif
#ifdef	DEBUG_PS
	pDeviceSettings->d3d9.DeviceType = D3DDEVTYPE_REF;
#endif


	static bool s_bFirstTime = true;

	if( s_bFirstTime )
	{
		s_bFirstTime = false;
		if( pDeviceSettings->d3d9.DeviceType == D3DDEVTYPE_REF )
		{
			DXUTDisplaySwitchingToREFWarning( pDeviceSettings->ver );
		}
	}


	return true;
}


HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9 * pd3dDevice, const D3DSURFACE_DESC * pBackBufferSurfaceDesc, void * pUserContext )
{
	HRESULT				hr;
	TCHAR				tempPath[ MAX_PATH ];
	TCHAR				tempName[ MAX_PATH / 4 ];
	TCHAR				tempCD[ MAX_PATH ];

	D3DXIMAGE_INFO		tempImgInfo;


	//V_RETURN( D3DXCreateSprite( pd3dDevice, &g_pd3dSprite ) );

	CDXUTIMEEditBox::Initialize( DXUTGetHWND() );

	V_RETURN( g_DialogResourceManager.OnD3D9CreateDevice( pd3dDevice ) );
	V_RETURN( g_SettingsDlg.OnD3D9CreateDevice( pd3dDevice ) );

	V_RETURN(
		D3DXCreateFont( pd3dDevice, 16, 0, FW_BOLD, 0, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
						DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, TEXT( "Consolas" ), &g_pFont )
		);


	{
		cMonitor::Owner lock{ g_Monitor };
		InitializeCharacterMesh( D3DXMESH_MANAGED, pd3dDevice, NULL );
		g_StartBackground.Build( pd3dDevice, TEXT( "Background/StartBackground.tga" ), D3DXMESH_MANAGED );
		g_WaitBackground.Build( pd3dDevice, TEXT( "Background/WaitBackground.tga" ), D3DXMESH_MANAGED );
		g_AdjustBackground.Build( pd3dDevice, TEXT( "Background/AdjustBackground.tga" ), D3DXMESH_MANAGED );
		if ( Matchless::EMSS_Play == g_ThisClient.m_PlayerInfo.GetMainStepState() )
		{
			CreateGraphicResource( D3DXMESH_MANAGED, pd3dDevice, NULL );
		}
	}


	CreateEffect( pd3dDevice, D3DXMESH_MANAGED, D3DPOOL_MANAGED );


	D3DDEVICE_CREATION_PARAMETERS	cp;
	pd3dDevice->GetCreationParameters( &cp );
	g_dwBehaviorFlags = cp.BehaviorFlags;


	RetrieveResourceFile( TEXT( "UI/CharacterEmblem/Emblem_Breaker.bmp" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexBreakerEmblem ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/CharacterEmblem/Emblem_Defender.bmp" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexDefenderEmblem ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/CharacterEmblem/Emblem_Mage.bmp" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexMageEmblem ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/CharacterEmblem/Emblem_Healer.bmp" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexHealerEmblem ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/TerrainEmblem/Terrain1Emblem.bmp" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexTerrain1Emblem ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/TerrainEmblem/Terrain2Emblem.bmp" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexTerrain2Emblem ) );
	SetCurrentDirectory( tempCD );


	RetrieveResourceFile( TEXT( "UI/PlayPanel/GaugePanel.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexGaugePanel ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/PlayPanel/SkillPanel.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexSkillPanel ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/PlayPanel/HealthGauge.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, &g_ImgInfoHealthGauge, NULL, &g_pTexHealthGauge ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/PlayPanel/EnergyGauge.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, &g_ImgInfoEnergyGauge, NULL, &g_pTexEnergyGauge ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/PlayPanel/GameLeaveButton.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexGameLeaveButton ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/PlayPanel/GameStartButton.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexGameStartButton ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/PlayPanel/AwayPartyWord.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexAwayPartyWord ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/PlayPanel/HomePartyWord.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexHomePartyWord ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/PlayPanel/PartyInfoFrame.png" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, &tempImgInfo, NULL, &g_pTexPartyInfoFrame ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/PlayPanel/AnCharInfoFrame.png" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexAnCharInfoFrame ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/PlayPanel/AnEnergyBar.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, &g_ImgInfoAnEnergyBar, NULL, &g_pTexAnEnergyBar ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/PlayPanel/AnEnergyGauge.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexAnEnergyGauge ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/PlayPanel/AnHealthBar.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, &g_ImgInfoAnHealthBar, NULL, &g_pTexAnHealthBar ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/PlayPanel/AnHealthGauge.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexAnHealthGauge ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/PlayPanel/CastBarGauge.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, &g_ImgCastBarGauge, NULL, &g_pTexCastBarGauge ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/PlayPanel/CastBarGrowth.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, &g_ImgCastBarGrowth, NULL, &g_pTexCastBarGrowth ) );
	SetCurrentDirectory( tempCD );


	RetrieveResourceFile( TEXT( "UI/SkillIcon/Breaker/NormalAttack.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexBreakerNormalAttackIcon ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/SkillIcon/Breaker/Raid.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexBreakerRaidIcon ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/SkillIcon/Breaker/Stun.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexBreakerStunIcon ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/SkillIcon/Breaker/ArmorBreak.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexBreakerArmorBreakIcon ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/SkillIcon/Breaker/PowerBreak.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexBreakerPowerBreakIcon ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/SkillIcon/Breaker/Berserk.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexBreakerBerserkIcon ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/SkillIcon/Breaker/Charge.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexBreakerChargeIcon ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/SkillIcon/Breaker/Silent.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexBreakerSilentIcon ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/SkillIcon/Breaker/LastHit.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexBreakerLastHitIcon ) );
	SetCurrentDirectory( tempCD );


	RetrieveResourceFile( TEXT( "UI/SkillIcon/Defender/NormalAttack.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexDefenderNormalAttackIcon ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/SkillIcon/Defender/StoneSkin.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexDefenderStoneSkinIcon ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/SkillIcon/Defender/Natural.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexDefenderNaturalIcon ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/SkillIcon/Defender/Sacrifice.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexDefenderSacrificeIcon ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/SkillIcon/Defender/AttackBreak.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexDefenderAttackBreakIcon ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/SkillIcon/Defender/HeavyBlow.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexDefenderHeavyBlowIcon ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/SkillIcon/Defender/SpiritLink.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexDefenderSpiritLinkIcon ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/SkillIcon/Defender/Earthquake.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexDefenderEarthquakeIcon ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/SkillIcon/Defender/FullThrow.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexDefenderFullThrowIcon ) );
	SetCurrentDirectory( tempCD );


	RetrieveResourceFile( TEXT( "UI/SkillIcon/Mage/NormalAttack.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexMageNormalAttackIcon ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/SkillIcon/Mage/IceShield.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexMageIceShieldIcon ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/SkillIcon/Mage/MagicalArmor.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexMageMagicalArmorIcon ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/SkillIcon/Mage/IceBolt.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexMageIceBoltIcon ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/SkillIcon/Mage/FireBall.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexMageFireBallIcon ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/SkillIcon/Mage/Blizzard.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexMageBlizzardIcon ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/SkillIcon/Mage/FairyRescue.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexMageFairyRescueIcon ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/SkillIcon/Mage/ChainLightning.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexMageChainLightningIcon ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/SkillIcon/Mage/LightningEmission.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexMageLightningEmissionIcon ) );
	SetCurrentDirectory( tempCD );


	RetrieveResourceFile( TEXT( "UI/SkillIcon/Healer/NormalAttack.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexHealerNormalAttackIcon ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/SkillIcon/Healer/Perfect.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexHealerPerfectIcon ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/SkillIcon/Healer/QuickHeal.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexHealerQuickHealIcon ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/SkillIcon/Healer/NormalHeal.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexHealerNormalHealIcon ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/SkillIcon/Healer/LandBlessing.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexHealerLandBlessingIcon ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/SkillIcon/Healer/Dispel.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexHealerDispelIcon ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/SkillIcon/Healer/FreePromise.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexHealerFreePromiseIcon ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/SkillIcon/Healer/SoulCry.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexHealerSoulCryIcon ) );
	SetCurrentDirectory( tempCD );

	RetrieveResourceFile( TEXT( "UI/SkillIcon/Healer/Discharge.tga" ), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	V_RETURN( D3DXCreateTextureFromFileEx( pd3dDevice, tempName, D3DX_DEFAULT, D3DX_DEFAULT, 1, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 0x00000001, 0x00000001, 0x00FFFFFF, NULL, NULL, &g_pTexHealerDischargeIcon ) );
	SetCurrentDirectory( tempCD );


	return S_OK;
}


HRESULT CALLBACK OnResetDevice( IDirect3DDevice9 * pd3dDevice, const D3DSURFACE_DESC * pBackBufferSurfaceDesc, void * pUserContext )
{
	HRESULT		hr;


	V_RETURN( D3DXCreateSprite( pd3dDevice, &g_pd3dSprite ) );

	V_RETURN( g_DialogResourceManager.OnD3D9ResetDevice() );
	V_RETURN( g_SettingsDlg.OnD3D9ResetDevice() );

	if( g_pFont )
	{
		V_RETURN( g_pFont->OnResetDevice() );
	}

	V_RETURN( D3DXCreateSprite( pd3dDevice, &g_pTextSprite ) );


	float	fAspect = (float)pBackBufferSurfaceDesc->Width / (float)pBackBufferSurfaceDesc->Height;
	D3DXMATRIX	tempProjTrans;

	// Reset camera
	g_Camera.SetFOV( D3DX_PI / 4 );
	g_Camera.SetAspect( fAspect );
	g_Camera.SetNearClip( 1.0f );
	g_Camera.SetFarClip( 50000.0f );

	tempProjTrans = g_Camera.GetProjectionTransform();
	pd3dDevice->SetTransform( D3DTS_PROJECTION, &tempProjTrans );


	//g_ArcBall.SetWindow( pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height, 0.85f );
	//g_ArcBall.SetTranslationRadius( 374.57941f/*g_fObjectRadius*/ );

	g_SystemUI.SetLocation( pBackBufferSurfaceDesc->Width - 170, 0 );
	g_SystemUI.SetSize( 170, 170 );

	g_GlobalUI.SetLocation( 0, 0 );
	g_GlobalUI.SetSize( pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height );
	if( g_GlobalUI.GetControl( IDC_GLOBAL_NOTICE ) )
	{
		g_GlobalUI.GetControl( IDC_GLOBAL_NOTICE )->SetLocation( pBackBufferSurfaceDesc->Width / 2 - 120, pBackBufferSurfaceDesc->Height / 4 );
		g_GlobalUI.GetControl( IDC_GLOBAL_NOTICE )->SetSize( 240, 60 );
	}

	g_MainStepStartUI.SetLocation( 0, 0 );
	g_MainStepStartUI.SetSize( pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height );
	if( g_MainStepStartUI.GetControl( IDC_START_CONNECTIP ) )
	{
		g_MainStepStartUI.GetControl( IDC_START_CONNECTIP )->SetLocation( pBackBufferSurfaceDesc->Width / 2 - 80, pBackBufferSurfaceDesc->Height / 2 );
		g_MainStepStartUI.GetControl( IDC_START_CONNECTIP )->SetSize( 160, 36 );
	}
	if( g_MainStepStartUI.GetControl( IDC_START_START ) )
	{
		g_MainStepStartUI.GetControl( IDC_START_START )->SetLocation( pBackBufferSurfaceDesc->Width / 2 - 80, pBackBufferSurfaceDesc->Height / 4 * 3 );
		g_MainStepStartUI.GetControl( IDC_START_START )->SetSize( 160, 30 );
	}

	g_MainStepWaitUI.SetLocation( 0, 0 );
	g_MainStepWaitUI.SetSize( pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height );
	if( g_MainStepWaitUI.GetControl( IDC_WAIT_PLAYSTART ) )
	{
		g_MainStepWaitUI.GetControl( IDC_WAIT_PLAYSTART )->SetLocation( 20, 20 );
		g_MainStepWaitUI.GetControl( IDC_WAIT_PLAYSTART )->SetSize( 160, 30 );
	}
	if( g_MainStepWaitUI.GetControl( IDC_WAIT_QUIT ) )
	{
		g_MainStepWaitUI.GetControl( IDC_WAIT_QUIT )->SetLocation( pBackBufferSurfaceDesc->Width - 180, pBackBufferSurfaceDesc->Height - 50 );
		g_MainStepWaitUI.GetControl( IDC_WAIT_QUIT )->SetSize( 160, 30 );
	}
	if( g_MainStepWaitUI.GetControl( IDC_WAIT_CHARSELECT ) )
	{
		g_MainStepWaitUI.GetControl( IDC_WAIT_CHARSELECT )->SetLocation( pBackBufferSurfaceDesc->Width / 2 - 90, pBackBufferSurfaceDesc->Height / 3 * 2 );
		g_MainStepWaitUI.GetControl( IDC_WAIT_CHARSELECT )->SetSize( 180, 30 );
	}
	if( g_MainStepWaitUI.GetControl( IDC_WAIT_TEAMSELECT ) )
	{
		g_MainStepWaitUI.GetControl( IDC_WAIT_TEAMSELECT )->SetLocation( pBackBufferSurfaceDesc->Width / 2 - 90, pBackBufferSurfaceDesc->Height / 4 * 3 );
		g_MainStepWaitUI.GetControl( IDC_WAIT_TEAMSELECT )->SetSize( 180, 30 );
	}
	if( g_MainStepWaitUI.GetControl( IDC_WAIT_MAPSELECT_PREV ) )
	{
		g_MainStepWaitUI.GetControl( IDC_WAIT_MAPSELECT_PREV )->SetLocation( 42, pBackBufferSurfaceDesc->Height - 84 );
		g_MainStepWaitUI.GetControl( IDC_WAIT_MAPSELECT_PREV )->SetSize( 72, 42 );
	}
	if( g_MainStepWaitUI.GetControl( IDC_WAIT_MAPSELECT_NEXT ) )
	{
		g_MainStepWaitUI.GetControl( IDC_WAIT_MAPSELECT_NEXT )->SetLocation( 42 + 100, pBackBufferSurfaceDesc->Height - 84 );
		g_MainStepWaitUI.GetControl( IDC_WAIT_MAPSELECT_NEXT )->SetSize( 72, 42 );
	}


	g_MainStepPlayUI.SetLocation( 0, 0 );
	g_MainStepPlayUI.SetSize( pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height );
	if( g_MainStepPlayUI.GetControl( IDC_PLAY_PLAYQUIT ) )
	{
		g_MainStepPlayUI.GetControl( IDC_PLAY_PLAYQUIT )->SetLocation( 20, 20 );
		g_MainStepPlayUI.GetControl( IDC_PLAY_PLAYQUIT )->SetSize( 160, 30 );
	}

	g_MainStepAdjustUI.SetLocation( 0, 0 );
	g_MainStepAdjustUI.SetSize( pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height );
	if( g_MainStepAdjustUI.GetControl( IDC_ADJUST_PASS ) )
	{
		g_MainStepAdjustUI.GetControl( IDC_ADJUST_PASS )->SetLocation( pBackBufferSurfaceDesc->Width / 2 - 80, pBackBufferSurfaceDesc->Height / 4 * 3 );
		g_MainStepAdjustUI.GetControl( IDC_ADJUST_PASS )->SetSize( 160, 30 );
	}

	g_MainStepEndUI.SetLocation( 0, 0 );
	g_MainStepEndUI.SetSize( pBackBufferSurfaceDesc->Width, pBackBufferSurfaceDesc->Height );

	return S_OK;
}


void CALLBACK OnLostDevice( void * pUserContext )
{
	g_DialogResourceManager.OnD3D9LostDevice();
	g_SettingsDlg.OnD3D9LostDevice();

	if( g_pFont )
	{
		g_pFont->OnLostDevice();
	}

	SAFE_RELEASE( g_pTextSprite );

	SAFE_RELEASE( g_pd3dSprite );
}


void CALLBACK OnDestroyDevice( void * pUserContext )
{
	SAFE_RELEASE( g_pFont );


	DestroyGraphicResource();

	g_StartBackground.Release();
	g_WaitBackground.Release();
	g_AdjustBackground.Release();

	DestroyEffect( pUserContext );

	DeleteCharacterMesh();


	g_SettingsDlg.OnD3D9DestroyDevice();
	g_DialogResourceManager.OnD3D9DestroyDevice();

	CDXUTIMEEditBox::Uninitialize();

	SAFE_RELEASE( g_pTexBreakerEmblem );
	SAFE_RELEASE( g_pTexDefenderEmblem );
	SAFE_RELEASE( g_pTexMageEmblem );
	SAFE_RELEASE( g_pTexHealerEmblem );

	SAFE_RELEASE( g_pTexTerrain1Emblem );
	SAFE_RELEASE( g_pTexTerrain2Emblem );

	SAFE_RELEASE( g_pTexGaugePanel );
	SAFE_RELEASE( g_pTexSkillPanel );
	SAFE_RELEASE( g_pTexHealthGauge );
	SAFE_RELEASE( g_pTexEnergyGauge );

	SAFE_RELEASE( g_pTexGameLeaveButton );
	SAFE_RELEASE( g_pTexGameStartButton );
	SAFE_RELEASE( g_pTexAwayPartyWord );
	SAFE_RELEASE( g_pTexHomePartyWord );
	SAFE_RELEASE( g_pTexPartyInfoFrame );
	SAFE_RELEASE( g_pTexAnCharInfoFrame );
	SAFE_RELEASE( g_pTexAnEnergyBar );
	SAFE_RELEASE( g_pTexAnEnergyGauge );
	SAFE_RELEASE( g_pTexAnHealthBar );
	SAFE_RELEASE( g_pTexAnHealthGauge );

	SAFE_RELEASE( g_pTexCastBarGauge );
	SAFE_RELEASE( g_pTexCastBarGrowth );

	SAFE_RELEASE( g_pTexBreakerNormalAttackIcon );
	SAFE_RELEASE( g_pTexBreakerRaidIcon );
	SAFE_RELEASE( g_pTexBreakerStunIcon );
	SAFE_RELEASE( g_pTexBreakerArmorBreakIcon );
	SAFE_RELEASE( g_pTexBreakerPowerBreakIcon );
	SAFE_RELEASE( g_pTexBreakerBerserkIcon );
	SAFE_RELEASE( g_pTexBreakerChargeIcon );
	SAFE_RELEASE( g_pTexBreakerSilentIcon );
	SAFE_RELEASE( g_pTexBreakerLastHitIcon );

	SAFE_RELEASE( g_pTexDefenderNormalAttackIcon );
	SAFE_RELEASE( g_pTexDefenderStoneSkinIcon );
	SAFE_RELEASE( g_pTexDefenderNaturalIcon );
	SAFE_RELEASE( g_pTexDefenderSacrificeIcon );
	SAFE_RELEASE( g_pTexDefenderAttackBreakIcon );
	SAFE_RELEASE( g_pTexDefenderHeavyBlowIcon );
	SAFE_RELEASE( g_pTexDefenderSpiritLinkIcon );
	SAFE_RELEASE( g_pTexDefenderEarthquakeIcon );
	SAFE_RELEASE( g_pTexDefenderFullThrowIcon );

	SAFE_RELEASE( g_pTexMageNormalAttackIcon );
	SAFE_RELEASE( g_pTexMageIceShieldIcon );
	SAFE_RELEASE( g_pTexMageMagicalArmorIcon );
	SAFE_RELEASE( g_pTexMageIceBoltIcon );
	SAFE_RELEASE( g_pTexMageFireBallIcon );
	SAFE_RELEASE( g_pTexMageBlizzardIcon );
	SAFE_RELEASE( g_pTexMageFairyRescueIcon );
	SAFE_RELEASE( g_pTexMageChainLightningIcon );
	SAFE_RELEASE( g_pTexMageLightningEmissionIcon );

	SAFE_RELEASE( g_pTexHealerNormalAttackIcon );
	SAFE_RELEASE( g_pTexHealerPerfectIcon );
	SAFE_RELEASE( g_pTexHealerQuickHealIcon );
	SAFE_RELEASE( g_pTexHealerNormalHealIcon );
	SAFE_RELEASE( g_pTexHealerLandBlessingIcon );
	SAFE_RELEASE( g_pTexHealerDispelIcon );
	SAFE_RELEASE( g_pTexHealerFreePromiseIcon );
	SAFE_RELEASE( g_pTexHealerSoulCryIcon );
	SAFE_RELEASE( g_pTexHealerDischargeIcon );

	//SAFE_RELEASE( g_pd3dSprite );
}


HRESULT CreateEffect( IDirect3DDevice9 * apDevice, DWORD aMeshOptions, D3DPOOL aPoolType )
{
	HRESULT		hr = S_OK;


	g_BreakerNormalAttackEffect.Build( apDevice, TEXT( "Effect/Skill/Breaker/NormalAttack/esbn" ), aMeshOptions, aPoolType );
	g_BreakerRaidEffect.Build( apDevice, TEXT( "Effect/Skill/Breaker/Raid/esbr" ), aMeshOptions, aPoolType );
	g_BreakerStunEffect.Build( apDevice, TEXT( "Effect/Skill/Breaker/Stun/esbs" ), aMeshOptions, aPoolType, 1, 1 );
	g_BreakerArmorBreakEffect.Build( apDevice, TEXT( "Effect/Skill/Breaker/ArmorBreak/esba" ), aMeshOptions, aPoolType, 1, 1 );
	g_BreakerPowerBreakEffect.Build( apDevice, TEXT( "Effect/Skill/Breaker/PowerBreak/esbp" ), aMeshOptions, aPoolType );
	g_BreakerBerserkEffect.Build( apDevice, TEXT( "Effect/Skill/Breaker/Berserk/esbb" ), aMeshOptions, aPoolType );
	g_BreakerChargeEffect.Build( apDevice, TEXT( "Effect/Skill/Breaker/Charge/esbc" ), aMeshOptions, aPoolType );
	g_BreakerSilentEffect.Build( apDevice, TEXT( "Effect/Skill/Breaker/Silent/esbs" ), aMeshOptions, aPoolType );
	g_BreakerLastHitEffect.Build( apDevice, TEXT( "Effect/Skill/Breaker/LastHit/esbl" ), aMeshOptions, aPoolType, 1, 1 );

	g_DefenderNormalAttackEffect.Build( apDevice, TEXT( "Effect/Skill/Defender/NormalAttack/esdn" ), aMeshOptions, aPoolType );
	g_DefenderStoneSkinEffect.Build( apDevice, TEXT( "Effect/Skill/Defender/StoneSkin/esds" ), aMeshOptions, aPoolType );
	g_DefenderNaturalEffect.Build( apDevice, TEXT( "Effect/Skill/Defender/Natural/esdn" ), aMeshOptions, aPoolType );
	g_DefenderSacrificeEffect.Build( apDevice, TEXT( "Effect/Skill/Defender/Sacrifice/esds" ), aMeshOptions, aPoolType );
	g_DefenderAttackBreakEffect.Build( apDevice, TEXT( "Effect/Skill/Defender/AttackBreak/esda" ), aMeshOptions, aPoolType );
	g_DefenderHeavyBlowEffect.Build( apDevice, TEXT( "Effect/Skill/Defender/HeavyBlow/esdh" ), aMeshOptions, aPoolType );
	g_DefenderSpiritLinkEffect.Build( apDevice, TEXT( "Effect/Skill/Defender/SpiritLink/esds" ), aMeshOptions, aPoolType );
	g_DefenderEarthquakeEffect.Build( apDevice, TEXT( "Effect/Skill/Defender/Earthquake/esde" ), aMeshOptions, aPoolType );
	g_DefenderFullThrowEffect.Build( apDevice, TEXT( "Effect/Skill/Defender/FullThrow/esdf" ), aMeshOptions, aPoolType );

	g_MageNormalAttackEffect.Build( apDevice, TEXT( "Effect/Skill/Mage/NormalAttack/esmn" ), aMeshOptions, aPoolType );
	g_MageIceShieldEffect.Build( apDevice, TEXT( "Effect/Skill/Mage/IceShield/esmi" ), aMeshOptions, aPoolType );
	g_MageMagicalArmorEffect.Build( apDevice, TEXT( "Effect/Skill/Mage/MagicalArmor/esmm" ), aMeshOptions, aPoolType );
	g_MageIceBoltEffect.Build( apDevice, TEXT( "Effect/Skill/Mage/IceBolt/esmi" ), aMeshOptions, aPoolType );
	g_MageFireBallEffect.Build( apDevice, TEXT( "Effect/Skill/Mage/FireBall/esmf" ), aMeshOptions, aPoolType, 1, 1 );
	g_MageBlizzardEffect.Build( apDevice, TEXT( "Effect/Skill/Mage/Blizzard/esmb" ), aMeshOptions, aPoolType );
	g_MageFairyRescueEffect.Build( apDevice, TEXT( "Effect/Skill/Mage/FairyRescue/esmf" ), aMeshOptions, aPoolType );
	g_MageChainLightningEffect.Build( apDevice, TEXT( "Effect/Skill/Mage/ChainLightning/esmc" ), aMeshOptions, aPoolType );
	g_MageLightningEmissionEffect.Build( apDevice, TEXT( "Effect/Skill/Mage/LightningEmission/esml" ), aMeshOptions, aPoolType );

	g_HealerNormalAttackEffect.Build( apDevice, TEXT( "Effect/Skill/Healer/NormalAttack/eshn" ), aMeshOptions, aPoolType );
	g_HealerPerfectEffect.Build( apDevice, TEXT( "Effect/Skill/Healer/Perfect/eshp" ), aMeshOptions, aPoolType, 1, 1 );
	g_HealerQuickHealEffect.Build( apDevice, TEXT( "Effect/Skill/Healer/QuickHeal/eshq" ), aMeshOptions, aPoolType, 1, 1 );
	g_HealerNormalHealEffect.Build( apDevice, TEXT( "Effect/Skill/Healer/NormalHeal/eshn" ), aMeshOptions, aPoolType, 1, 1, 60 );
	g_HealerLandBlessingEffect.Build( apDevice, TEXT( "Effect/Skill/Healer/LandBlessing/eshl" ), aMeshOptions, aPoolType );
	g_HealerDispelEffect.Build( apDevice, TEXT( "Effect/Skill/Healer/Dispel/eshd" ), aMeshOptions, aPoolType );
	g_HealerFreePromiseEffect.Build( apDevice, TEXT( "Effect/Skill/Healer/FreePromise/eshf" ), aMeshOptions, aPoolType );
	g_HealerSoulCryEffect.Build( apDevice, TEXT( "Effect/Skill/Healer/SoulCry/eshs" ), aMeshOptions, aPoolType );
	g_HealerDischargeEffect.Build( apDevice, TEXT( "Effect/Skill/Healer/Discharge/eshd" ), aMeshOptions, aPoolType );


	return	hr;
}


void DestroyEffect( void * pUserContext )
{
	g_BreakerNormalAttackEffect.Release();
	g_BreakerRaidEffect.Release();
	g_BreakerStunEffect.Release();
	g_BreakerArmorBreakEffect.Release();
	g_BreakerPowerBreakEffect.Release();
	g_BreakerBerserkEffect.Release();
	g_BreakerChargeEffect.Release();
	g_BreakerSilentEffect.Release();
	g_BreakerLastHitEffect.Release();

	g_DefenderNormalAttackEffect.Release();
	g_DefenderStoneSkinEffect.Release();
	g_DefenderNaturalEffect.Release();
	g_DefenderSacrificeEffect.Release();
	g_DefenderAttackBreakEffect.Release();
	g_DefenderHeavyBlowEffect.Release();
	g_DefenderSpiritLinkEffect.Release();
	g_DefenderEarthquakeEffect.Release();
	g_DefenderFullThrowEffect.Release();

	g_MageNormalAttackEffect.Release();
	g_MageIceShieldEffect.Release();
	g_MageMagicalArmorEffect.Release();
	g_MageIceBoltEffect.Release();
	g_MageFireBallEffect.Release();
	g_MageBlizzardEffect.Release();
	g_MageFairyRescueEffect.Release();
	g_MageChainLightningEffect.Release();
	g_MageLightningEmissionEffect.Release();

	g_HealerNormalAttackEffect.Release();
	g_HealerPerfectEffect.Release();
	g_HealerQuickHealEffect.Release();
	g_HealerNormalHealEffect.Release();
	g_HealerLandBlessingEffect.Release();
	g_HealerDispelEffect.Release();
	g_HealerFreePromiseEffect.Release();
	g_HealerSoulCryEffect.Release();
	g_HealerDischargeEffect.Release();
}


bool UpdateEffect( void )
{
	cMonitor::Owner lock{ g_Monitor };

	std::list< Matchless::SEffect >::iterator eIter = g_EffectList.begin();

	while( eIter != g_EffectList.end() )
	{
		if( eIter->m_CurrentTick >= eIter->m_EndTick )
		{
			std::list< Matchless::SEffect >::iterator prevEIter = eIter;
			++eIter;
			g_EffectList.erase( prevEIter );
			continue;
		}

		eIter->m_CurrentTick = g_Timer.GetTick();


		std::map< unsigned int, CAnimateMesh >::iterator	acIter;

		switch( eIter->m_PosType )
		{

		case Matchless::SEffect::EPT_Active:
			acIter = g_AnotherCharacterList.find( eIter->m_CharacterID );
			if( acIter != g_AnotherCharacterList.end() )
			{
				eIter->m_EndPosX = acIter->second.GetPosition().x;
				eIter->m_EndPosY = acIter->second.GetPosition().y;
				eIter->m_EndPosZ = acIter->second.GetPosition().z;
			}
			else
			{
				eIter->m_EndPosX = g_ThisCharacter.GetPosition().x;
				eIter->m_EndPosY = g_ThisCharacter.GetPosition().y;
				eIter->m_EndPosZ = g_ThisCharacter.GetPosition().z;
			}
			eIter->m_EndPosY += (CHARACTER_COMMON_HEIGHT / 2);
			break;

		case Matchless::SEffect::EPT_Character:
			acIter = g_AnotherCharacterList.find( eIter->m_CharacterID );
			if( acIter != g_AnotherCharacterList.end() )
			{
				eIter->m_StartPosX = acIter->second.GetPosition().x;
				eIter->m_StartPosY = acIter->second.GetPosition().y;
				eIter->m_StartPosZ = acIter->second.GetPosition().z;
			}
			else
			{
				eIter->m_StartPosX = g_ThisCharacter.GetPosition().x;
				eIter->m_StartPosY = g_ThisCharacter.GetPosition().y;
				eIter->m_StartPosZ = g_ThisCharacter.GetPosition().z;
			}
			eIter->m_StartPosY += (CHARACTER_COMMON_HEIGHT / 2);
			break;

		}

		++eIter;
	}

	return	true;
}


bool RenderEffect( IDirect3DDevice9 * apDevice )
{
	CEffectBillboard *	pTempEffect = NULL;
	D3DXVECTOR3			tempPos;


	for( std::list< Matchless::SEffect >::iterator eIter = g_EffectList.begin() ; eIter != g_EffectList.end() ; ++eIter )
	{
		switch( (Matchless::ECharacterClass)eIter->m_Type2 )
		{

		case Matchless::ECS_Breaker_NormalAttack:
			pTempEffect = (&g_BreakerNormalAttackEffect);
			break;
		case Matchless::ECS_Breaker_Raid:
			//pTempEffect = (&g_BreakerRaidEffect);
			pTempEffect = (&g_DefenderHeavyBlowEffect);
			break;
		case Matchless::ECS_Breaker_Stun:
			pTempEffect = (&g_BreakerStunEffect);
			break;
		case Matchless::ECS_Breaker_ArmorBreak:
			pTempEffect = (&g_BreakerArmorBreakEffect);
			break;
		case Matchless::ECS_Breaker_PowerBreak:
			pTempEffect = (&g_BreakerPowerBreakEffect);
			break;
		case Matchless::ECS_Breaker_Berserk:
			pTempEffect = (&g_BreakerBerserkEffect);
			break;
		case Matchless::ECS_Breaker_Charge:
			pTempEffect = (&g_BreakerChargeEffect);
			break;
		case Matchless::ECS_Breaker_Silent:
			pTempEffect = (&g_BreakerSilentEffect);
			break;
		case Matchless::ECS_Breaker_LastHit:
			pTempEffect = (&g_BreakerLastHitEffect);
			break;

		case Matchless::ECS_Defender_NormalAttack:
			pTempEffect = (&g_DefenderNormalAttackEffect);
			break;
		case Matchless::ECS_Defender_StoneSkin:
			pTempEffect = (&g_DefenderStoneSkinEffect);
			break;
		case Matchless::ECS_Defender_Natural:
			pTempEffect = (&g_DefenderStoneSkinEffect);
			//pTempEffect = (&g_DefenderNaturalEffect);
			break;
		case Matchless::ECS_Defender_Sacrifice:
			pTempEffect = (&g_DefenderSacrificeEffect);
			break;
		case Matchless::ECS_Defender_AttackBreak:
			pTempEffect = (&g_DefenderAttackBreakEffect);
			break;
		case Matchless::ECS_Defender_HeavyBlow:
			pTempEffect = (&g_DefenderHeavyBlowEffect);
			break;
		case Matchless::ECS_Defender_SpiritLink:
			pTempEffect = (&g_DefenderSpiritLinkEffect);
			break;
		case Matchless::ECS_Defender_Earthquake:
			pTempEffect = (&g_DefenderEarthquakeEffect);
			break;
		case Matchless::ECS_Defender_FullThrow:
			pTempEffect = (&g_DefenderFullThrowEffect);
			break;

		case Matchless::ECS_Mage_NormalAttack:
			pTempEffect = (&g_MageNormalAttackEffect);
			break;
		case Matchless::ECS_Mage_IceShield:
			pTempEffect = (&g_MageIceShieldEffect);
			break;
		case Matchless::ECS_Mage_MagicalArmor:
			pTempEffect = (&g_MageMagicalArmorEffect);
			break;
		case Matchless::ECS_Mage_IceBolt:
			pTempEffect = (&g_MageIceBoltEffect);
			break;
		case Matchless::ECS_Mage_FireBall:
			pTempEffect = (&g_MageFireBallEffect);
			break;
		case Matchless::ECS_Mage_Blizzard:
			pTempEffect = (&g_MageBlizzardEffect);
			break;
		case Matchless::ECS_Mage_FairyRescue:
			pTempEffect = (&g_MageFairyRescueEffect);
			break;
		case Matchless::ECS_Mage_ChainLightning:
			pTempEffect = (&g_MageChainLightningEffect);
			break;
		case Matchless::ECS_Mage_LightningEmission:
			pTempEffect = (&g_MageLightningEmissionEffect);
			break;

		case Matchless::ECS_Healer_NormalAttack:
			pTempEffect = (&g_HealerNormalAttackEffect);
			break;
		case Matchless::ECS_Healer_Perfect:
			pTempEffect = (&g_HealerPerfectEffect);
			break;
		case Matchless::ECS_Healer_QuickHeal:
			pTempEffect = (&g_HealerQuickHealEffect);
			break;
		case Matchless::ECS_Healer_NormalHeal:
			pTempEffect = (&g_HealerNormalHealEffect);
			break;
		case Matchless::ECS_Healer_LandBlessing:
			pTempEffect = (&g_HealerLandBlessingEffect);
			break;
		case Matchless::ECS_Healer_Dispel:
			pTempEffect = (&g_HealerDispelEffect);
			break;
		case Matchless::ECS_Healer_FreePromise:
			pTempEffect = (&g_HealerFreePromiseEffect);
			break;
		case Matchless::ECS_Healer_SoulCry:
			pTempEffect = (&g_HealerSoulCryEffect);
			break;
		case Matchless::ECS_Healer_Discharge:
			pTempEffect = (&g_HealerDischargeEffect);
			break;

		default:
			pTempEffect = NULL;
			break;

		}

		switch( eIter->m_PosType )
		{

		case Matchless::SEffect::EPT_Active:
			tempPos.x = eIter->m_StartPosX + (eIter->m_EndPosX - eIter->m_StartPosX) / 60 * (eIter->m_CurrentTick - eIter->m_StartTick);
			tempPos.y = eIter->m_StartPosY + (eIter->m_EndPosY - eIter->m_StartPosY) / 60 * (eIter->m_CurrentTick - eIter->m_StartTick);
			tempPos.z = eIter->m_StartPosZ + (eIter->m_EndPosZ - eIter->m_StartPosZ) / 60 * (eIter->m_CurrentTick - eIter->m_StartTick);
			break;

		case Matchless::SEffect::EPT_Character:
			tempPos.x = eIter->m_StartPosX - (g_Camera.GetLookAt().x * 128);
			tempPos.y = eIter->m_StartPosY - (g_Camera.GetLookAt().y * 128);
			tempPos.z = eIter->m_StartPosZ - (g_Camera.GetLookAt().z * 128);
			break;

		}

		if( NULL != pTempEffect )
		{
			pTempEffect->UpdatePerFrame( (eIter->m_CurrentTick - eIter->m_StartTick) / 2 );
			pTempEffect->Render( apDevice, g_Camera.GetRight(), g_Camera.GetUp(), tempPos );
		}
	}

	return	true;
}
