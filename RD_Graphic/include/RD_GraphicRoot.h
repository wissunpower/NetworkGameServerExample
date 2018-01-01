
#ifndef		__RD_GRAPHICROOT_H_vjh92_92j39_f9w3r__
#define		__RD_GRAPHICROOT_H_vjh92_92j39_f9w3r__



#include	<tchar.h>
#include	<d3dx9.h>

#include	"DXUT.h"

#include	"SDKmisc.h"



extern	DWORD			g_dwBehaviorFlags;



HRESULT RetrieveResourceFile( LPCTSTR const aSrcStr, LPTSTR aPath, int aPathLen, LPTSTR aName, int aNameLen );
float GetDistanceWithMesh( bool * apHit, const D3DXVECTOR3 & aPosition, const D3DXVECTOR3 & aDirection,
							const LPD3DXBASEMESH apMesh, const D3DXMATRIX & aMeshTransform );




#endif		// __RD_GRAPHICROOT_H_vjh92_92j39_f9w3r__
