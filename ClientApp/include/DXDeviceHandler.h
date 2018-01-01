
#ifndef		__DXDEVICEHANDLER_H_Fj923_fheuw_f8230__
#define		__DXDEVICEHANDLER_H_Fj923_fheuw_f8230__



#include	"ClientAppRoot.h"

#include	"ResourceManager.h"



bool CALLBACK IsDeviceAcceptable( D3DCAPS9 * pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat, bool bWindowed, void * pUserContext );
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings * pDeviceSettings, void * pUserContext );
HRESULT CALLBACK OnCreateDevice( IDirect3DDevice9 * pd3dDevice, const D3DSURFACE_DESC * pBackBufferSurfaceDesc, void * pUserContext );
HRESULT CALLBACK OnResetDevice( IDirect3DDevice9 * pd3dDevice, const D3DSURFACE_DESC * pBackBufferSurfaceDesc, void * pUserContext );
void CALLBACK OnLostDevice( void * pUserContext );
void CALLBACK OnDestroyDevice( void * pUserContext );

HRESULT CreateEffect( IDirect3DDevice9 * apDevice, DWORD aMeshOptions, D3DPOOL aPoolType );
void DestroyEffect( void * pUserContext );

bool UpdateEffect( void );
bool RenderEffect( IDirect3DDevice9 * apDevice );



#endif		__DXDEVICEHANDLER_H_Fj923_fheuw_f8230__
