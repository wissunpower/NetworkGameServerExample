
#ifndef		__RESOURCEMANAGER_H_f93jf_923j9_394jh__
#define		__RESOURCEMANAGER_H_f93jf_923j9_394jh__



#include	"ClientAppRoot.h"



int InitializeCharacterMesh( DWORD aMeshOptions, LPDIRECT3DDEVICE9 apDevice, LPD3DXLOADUSERDATA apUserDataLoader );
int DeleteCharacterMesh( void );

int CreateGraphicResource( DWORD aMeshOptions, LPDIRECT3DDEVICE9 apDevice, LPD3DXLOADUSERDATA apUserDataLoader );
int DestroyGraphicResource( void );



HRESULT CreateTerrain( IDirect3DDevice9 * pd3dDevice );
void RenderTerrain( IDirect3DDevice9 * pd3dDevice );


HRESULT DrawGameUI( ID3DXSprite * pSprite, ID3DXFont * pFont, const Matchless::EMainStepState aMSS );

HRESULT DrawGameWaitUI( ID3DXSprite * pSprite, ID3DXFont * pFont, const D3DSURFACE_DESC * pBackBufferSurfaceDesc );
HRESULT DrawGamePlayUI( ID3DXSprite * pSprite, ID3DXFont * pFont, const D3DSURFACE_DESC * pBackBufferSurfaceDesc );

LPDIRECT3DTEXTURE9 GetStateIcon( const Matchless::EStateType aType );



#endif		// __RESOURCEMANAGER_H_f93jf_923j9_394jh__
