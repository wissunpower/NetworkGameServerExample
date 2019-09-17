
#ifndef		__CLIENTAPPROOT_H_29j3r_e92u3_u8239__
#define		__CLIENTAPPROOT_H_29j3r_e92u3_u8239__



#include	<list>
#include	<map>

#include	<winsock2.h>
#include	<tchar.h>

#include	"cMonitor.h"
#include	"debugconsole.h"

#include	"ErrorHandler.h"
#include	"MatchlessProtocol.h"

#include	"DXUT.h"

#include	"DXUTgui.h"
#include	"DXUTguiIME.h"
#include	"DXUTcamera.h"
#include	"DXUTSettingsDlg.h"
#include	"SDKmisc.h"
#include	"SDKsound.h"

#include	"CCamera.h"
#include	"CAllocateHierarchy.h"
#include	"CAnimateMesh.h"
#include	"CTerrain.h"
#include	"CSkyBox.h"
#include	"CBackground.h"
#include	"CEffectBillboard.h"

#include	"MatchlessRoot.h"
#include	"MatchlessTimer.h"
#include	"MatchlessCClient.h"
#include	"MatchlessCSKill.h"
#include	"MatchlessSEffect.h"



#if defined(DEBUG) | defined(_DEBUG)
//#define		CS_TESTCODE_ON
#endif

#define		VK_TABLE_MAX	0x0100

const LPCWSTR SERVER_IPADDR = L"127.0.0.1";
#define		IPV4_LENG			16

#define		SCREEN_WIDTH		800
#define		SCREEN_HEIGHT		600

#define		FMOVE_UNIT			275.0f				// Character forward move
#define		BMOVE_UNIT			250.0f				// character backward move

#define		TARGETING_RANGE		2048.0f

#define		GAME_COMMAND_RECYCLE			0.5f
#define		GAME_SKILLCOMMAND_RECYCLE		0.75f


#define		CHARACTER_ANIINDEX_IDLE			4
#define		CHARACTER_ANIINDEX_DIE			0
#define		CHARACTER_ANIINDEX_FMOVE		3
#define		CHARACTER_ANIINDEX_BMOVE		1
#define		CHARACTER_ANIINDEX_ATTACK		2


#define		MAP_KIND_COUNT			2


#define		CHARACTER_COMMON_HEIGHT		240


#define		EFFECTTYPE_SKILL		0


//--------------------------------------------------------------------------------------
// UI control IDs
//--------------------------------------------------------------------------------------
#define		IDC_TOGGLEFULLSCREEN    1
#define		IDC_TOGGLEREF           2
#define		IDC_CHANGEDEVICE        3
#define		IDC_METHOD				5

#define		IDC_GLOBAL_NOTICE			0x1001

#define		IDC_START_START				0x2001
#define		IDC_START_CONNECTIP			0x2002

#define		IDC_WAIT_PLAYSTART			0x3001
#define		IDC_WAIT_QUIT				0x3002
#define		IDC_WAIT_CHARSELECT			0x3003
#define		IDC_WAIT_TEAMSELECT			0x3004
#define		IDC_WAIT_MAPSELECT_PREV		0x3005
#define		IDC_WAIT_MAPSELECT_NEXT		0x3006

#define		IDC_PLAY_PLAYQUIT			0x4001

#define		IDC_ADJUST_PASS				0x5001



//--------------------------------------------------------------------------------------
// Global variables
//--------------------------------------------------------------------------------------
#ifdef	CS_TESTCODE_ON
extern DebugConsole					g_Console;
#endif

extern std::list< int >				g_CSHistory;

extern ID3DXSprite *				g_pd3dSprite;
extern ID3DXFont *					g_pFont;
extern ID3DXSprite *				g_pTextSprite;
extern CD3DArcBall					g_ArcBall;
extern CDXUTDialogResourceManager	g_DialogResourceManager;
extern CD3DSettingsDlg				g_SettingsDlg;
extern CDXUTDialog					g_SystemUI;
extern CDXUTDialog					g_GlobalUI;

extern CDXUTDialog					g_MainStepStartUI;
extern CDXUTDialog					g_MainStepWaitUI;
extern CDXUTDialog					g_MainStepPlayUI;
extern CDXUTDialog					g_MainStepAdjustUI;
extern CDXUTDialog					g_MainStepEndUI;

extern Matchless::Timer				g_Timer;

extern CCamera						g_Camera;

extern tstring						g_Notice;

extern std::wstring					g_ConnectIP;

extern cMonitor											g_Monitor;

extern Matchless::CClient								g_ThisClient;
extern CAnimateMesh										g_ThisCharacter;
extern std::map< unsigned int, Matchless::CClient >		g_AnotherClientList;
extern std::map< unsigned int, CAnimateMesh >			g_AnotherCharacterList;

extern bool												g_IsGameStartable;
extern bool												g_IsGraphicResourceLoaded;


extern bool						g_IsFrameUpdated;
extern bool						g_IsUpdateMovePosition;
extern bool						g_IsUpdateMoveAnimation;
extern unsigned int				g_TargetID;

extern bool						g_IsNowCasting;
extern unsigned int				g_CastStartTick;
extern unsigned int				g_CastEndTick;


extern CAnimateMesh		g_BreakerMesh;
extern CAnimateMesh		g_DefenderMesh;
extern CAnimateMesh		g_MageMesh;
extern CAnimateMesh		g_HealerMesh;

extern CAnimateMesh *	g_pSelectCharacter;


extern LPD3DXMESH				g_pTerrain;
extern D3DXMATRIX				g_TerrainTransform;
extern DWORD					g_NumTerrainMaterial;
extern D3DMATERIAL9 *			g_pTerrainMaterial;
extern LPDIRECT3DTEXTURE9 *		g_ppTerrainTexture;

extern CSkyBox					g_SkyBox;
extern CBackground				g_StartBackground;
extern CBackground				g_WaitBackground;
extern CBackground				g_AdjustBackground;

extern LPD3DXMESH				g_pTargetCharacterMark;

extern LPD3DXMESH				g_pHomeTeamMark;
extern DWORD					g_NumHomeTeamMarkMaterial;
extern D3DMATERIAL9 *			g_pHomeTeamMarkMaterial;
extern LPDIRECT3DTEXTURE9 *		g_ppHomeTeamMarkTexture;

extern LPD3DXMESH				g_pAwayTeamMark;
extern DWORD					g_NumAwayTeamMarkMaterial;
extern D3DMATERIAL9 *			g_pAwayTeamMarkMaterial;
extern LPDIRECT3DTEXTURE9 *		g_ppAwayTeamMarkTexture;


extern std::list< Matchless::SEffect >		g_EffectList;

extern CEffectBillboard						g_BreakerNormalAttackEffect;
extern CEffectBillboard						g_BreakerRaidEffect;
extern CEffectBillboard						g_BreakerStunEffect;
extern CEffectBillboard						g_BreakerArmorBreakEffect;
extern CEffectBillboard						g_BreakerPowerBreakEffect;
extern CEffectBillboard						g_BreakerBerserkEffect;
extern CEffectBillboard						g_BreakerChargeEffect;
extern CEffectBillboard						g_BreakerSilentEffect;
extern CEffectBillboard						g_BreakerLastHitEffect;

extern CEffectBillboard						g_DefenderNormalAttackEffect;
extern CEffectBillboard						g_DefenderStoneSkinEffect;
extern CEffectBillboard						g_DefenderNaturalEffect;
extern CEffectBillboard						g_DefenderSacrificeEffect;
extern CEffectBillboard						g_DefenderAttackBreakEffect;
extern CEffectBillboard						g_DefenderHeavyBlowEffect;
extern CEffectBillboard						g_DefenderSpiritLinkEffect;
extern CEffectBillboard						g_DefenderEarthquakeEffect;
extern CEffectBillboard						g_DefenderFullThrowEffect;

extern CEffectBillboard						g_MageNormalAttackEffect;
extern CEffectBillboard						g_MageIceShieldEffect;
extern CEffectBillboard						g_MageMagicalArmorEffect;
extern CEffectBillboard						g_MageIceBoltEffect;
extern CEffectBillboard						g_MageFireBallEffect;
extern CEffectBillboard						g_MageBlizzardEffect;
extern CEffectBillboard						g_MageFairyRescueEffect;
extern CEffectBillboard						g_MageChainLightningEffect;
extern CEffectBillboard						g_MageLightningEmissionEffect;

extern CEffectBillboard						g_HealerNormalAttackEffect;
extern CEffectBillboard						g_HealerPerfectEffect;
extern CEffectBillboard						g_HealerQuickHealEffect;
extern CEffectBillboard						g_HealerNormalHealEffect;
extern CEffectBillboard						g_HealerLandBlessingEffect;
extern CEffectBillboard						g_HealerDispelEffect;
extern CEffectBillboard						g_HealerFreePromiseEffect;
extern CEffectBillboard						g_HealerSoulCryEffect;
extern CEffectBillboard						g_HealerDischargeEffect;


extern LPDIRECT3DTEXTURE9	g_pTexBreakerEmblem;
extern LPDIRECT3DTEXTURE9	g_pTexDefenderEmblem;
extern LPDIRECT3DTEXTURE9	g_pTexMageEmblem;
extern LPDIRECT3DTEXTURE9	g_pTexHealerEmblem;

extern LPDIRECT3DTEXTURE9	g_pTexTerrain1Emblem;
extern LPDIRECT3DTEXTURE9	g_pTexTerrain2Emblem;

extern LPDIRECT3DTEXTURE9	g_pTexGaugePanel;
extern LPDIRECT3DTEXTURE9	g_pTexSkillPanel;
extern LPDIRECT3DTEXTURE9	g_pTexHealthGauge;
extern LPDIRECT3DTEXTURE9	g_pTexEnergyGauge;

extern LPDIRECT3DTEXTURE9	g_pTexGameLeaveButton;
extern LPDIRECT3DTEXTURE9	g_pTexGameStartButton;
extern LPDIRECT3DTEXTURE9	g_pTexAwayPartyWord;
extern LPDIRECT3DTEXTURE9	g_pTexHomePartyWord;
extern LPDIRECT3DTEXTURE9	g_pTexPartyInfoFrame;
extern LPDIRECT3DTEXTURE9	g_pTexAnCharInfoFrame;
extern LPDIRECT3DTEXTURE9	g_pTexAnEnergyBar;
extern LPDIRECT3DTEXTURE9	g_pTexAnEnergyGauge;
extern LPDIRECT3DTEXTURE9	g_pTexAnHealthBar;
extern LPDIRECT3DTEXTURE9	g_pTexAnHealthGauge;

extern LPDIRECT3DTEXTURE9	g_pTexCastBarGauge;
extern LPDIRECT3DTEXTURE9	g_pTexCastBarGrowth;

extern LPDIRECT3DTEXTURE9		g_pTexBreakerNormalAttackIcon;
extern LPDIRECT3DTEXTURE9		g_pTexBreakerRaidIcon;
extern LPDIRECT3DTEXTURE9		g_pTexBreakerStunIcon;
extern LPDIRECT3DTEXTURE9		g_pTexBreakerArmorBreakIcon;
extern LPDIRECT3DTEXTURE9		g_pTexBreakerPowerBreakIcon;
extern LPDIRECT3DTEXTURE9		g_pTexBreakerBerserkIcon;
extern LPDIRECT3DTEXTURE9		g_pTexBreakerChargeIcon;
extern LPDIRECT3DTEXTURE9		g_pTexBreakerSilentIcon;
extern LPDIRECT3DTEXTURE9		g_pTexBreakerLastHitIcon;

extern LPDIRECT3DTEXTURE9		g_pTexDefenderNormalAttackIcon;
extern LPDIRECT3DTEXTURE9		g_pTexDefenderStoneSkinIcon;
extern LPDIRECT3DTEXTURE9		g_pTexDefenderNaturalIcon;
extern LPDIRECT3DTEXTURE9		g_pTexDefenderSacrificeIcon;
extern LPDIRECT3DTEXTURE9		g_pTexDefenderAttackBreakIcon;
extern LPDIRECT3DTEXTURE9		g_pTexDefenderHeavyBlowIcon;
extern LPDIRECT3DTEXTURE9		g_pTexDefenderSpiritLinkIcon;
extern LPDIRECT3DTEXTURE9		g_pTexDefenderEarthquakeIcon;
extern LPDIRECT3DTEXTURE9		g_pTexDefenderFullThrowIcon;

extern LPDIRECT3DTEXTURE9		g_pTexMageNormalAttackIcon;
extern LPDIRECT3DTEXTURE9		g_pTexMageIceShieldIcon;
extern LPDIRECT3DTEXTURE9		g_pTexMageMagicalArmorIcon;
extern LPDIRECT3DTEXTURE9		g_pTexMageIceBoltIcon;
extern LPDIRECT3DTEXTURE9		g_pTexMageFireBallIcon;
extern LPDIRECT3DTEXTURE9		g_pTexMageBlizzardIcon;
extern LPDIRECT3DTEXTURE9		g_pTexMageFairyRescueIcon;
extern LPDIRECT3DTEXTURE9		g_pTexMageChainLightningIcon;
extern LPDIRECT3DTEXTURE9		g_pTexMageLightningEmissionIcon;

extern LPDIRECT3DTEXTURE9		g_pTexHealerNormalAttackIcon;
extern LPDIRECT3DTEXTURE9		g_pTexHealerPerfectIcon;
extern LPDIRECT3DTEXTURE9		g_pTexHealerQuickHealIcon;
extern LPDIRECT3DTEXTURE9		g_pTexHealerNormalHealIcon;
extern LPDIRECT3DTEXTURE9		g_pTexHealerLandBlessingIcon;
extern LPDIRECT3DTEXTURE9		g_pTexHealerDispelIcon;
extern LPDIRECT3DTEXTURE9		g_pTexHealerFreePromiseIcon;
extern LPDIRECT3DTEXTURE9		g_pTexHealerSoulCryIcon;
extern LPDIRECT3DTEXTURE9		g_pTexHealerDischargeIcon;

extern D3DXIMAGE_INFO		g_ImgInfoHealthGauge;
extern D3DXIMAGE_INFO		g_ImgInfoEnergyGauge;

extern D3DXIMAGE_INFO		g_ImgInfoAnEnergyBar;
extern D3DXIMAGE_INFO		g_ImgInfoAnHealthBar;

extern D3DXIMAGE_INFO		g_ImgCastBarGauge;
extern D3DXIMAGE_INFO		g_ImgCastBarGrowth;


extern unsigned int		g_CurrentMapKind;


extern CSoundManager	g_SoundManager;

extern CSound *			g_pBackgroundSound;

extern CSound *			g_pBreakerNormalAttackSound;

extern CSound *			g_pDefenderNormalAttackSound;

extern CSound *			g_pMageNormalAttackSound;

extern CSound *			g_pHealerNormalAttackSound;
extern CSound *			g_pHealerQuickHealSound;



bool UpdateRoomMasterUI( const Matchless::EMainStepState aState, const bool aOnOff );

bool HandleSkillCommand( const Matchless::ECharacterClass aClass, const unsigned int aSkillKind, const unsigned int aCurrentTarget );

bool LoadSound( void );
bool ReleaseSound( void );


Matchless::ECharacterClass VoidPtrToCharacterClass( const void* src );

unsigned short VoidPtrToShort( const void* src );




#endif		// __CLIENTAPPROOT_H_29j3r_e92u3_u8239__
