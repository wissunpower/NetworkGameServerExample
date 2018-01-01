
#ifndef		CALLOCATEHIERARCHY_H_gj893_98hw3_f923j
#define		CALLOCATEHIERARCHY_H_gj893_98hw3_f923j



#include	"RD_GraphicRoot.h"



enum	METHOD
{
	D3DNONINDEXED,
	D3DINDEXED,
	SOFTWARE,
	NONE
};



struct D3DXFRAME_DERIVED : public D3DXFRAME
{
	D3DXMATRIX		CombinedTransformationMatrix;
};


struct D3DXMESHCONTAINER_DERIVED : public D3DXMESHCONTAINER
{
	LPDIRECT3DTEXTURE9 *	ppTextures;

	LPD3DXMESH				pOrigMesh;
	LPD3DXATTRIBUTERANGE	pAttributeTable;
	DWORD					NumAttributeGroups;
	DWORD					NumInfl;
	LPD3DXBUFFER			pBoneCombinationBuf;
	D3DXMATRIX **			ppBoneMatrixPtrs;
	D3DXMATRIX *			pBoneOffsetMatrices;
	DWORD					NumPaletteEntries;
	bool					UseSoftwareVP;
	DWORD					iAttributeSW;
};


class CAllocateHierarchy : public ID3DXAllocateHierarchy
{

public:

	STDMETHOD( CreateFrame )( THIS_ LPCSTR Name, LPD3DXFRAME * ppNewFrame );
	STDMETHOD( CreateMeshContainer )( THIS_ LPCSTR Name,
											CONST D3DXMESHDATA * pMeshData,
											CONST D3DXMATERIAL * pMaterials,
											CONST D3DXEFFECTINSTANCE * pEffectInstances,
											DWORD NumMaterials,
											CONST DWORD * pAdjacency,
											LPD3DXSKININFO pSkinInfo,
											LPD3DXMESHCONTAINER * ppNewMeshContainer );
	STDMETHOD( DestroyFrame )( THIS_ LPD3DXFRAME pFrameToFree );
	STDMETHOD( DestroyMeshContainer )( THIS_ LPD3DXMESHCONTAINER pMeshContainerBase );

	CAllocateHierarchy( void )
	{
	}
	~CAllocateHierarchy( void )
	{
	}

};



extern bool				g_bUseSoftwareVP;
extern METHOD			g_SkinningMethod;
extern D3DXMATRIX *		g_pBoneMatrices;
extern UINT				g_NumBoneMatricesMax;



HRESULT AllocateName( LPCSTR Name, LPSTR* pNewName );
HRESULT GenerateSkinnedMesh( IDirect3DDevice9 * pd3dDevice, D3DXMESHCONTAINER_DERIVED * pMeshContainer );
void UpdateSkinningMethod( LPD3DXFRAME pFrameBase );



#endif		// CALLOCATEHIERARCHY_H_gj893_98hw3_f923j
