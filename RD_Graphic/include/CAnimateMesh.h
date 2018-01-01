
#ifndef		CANIMATEMESH_H_vj923_v9wj3_934jg
#define		CANIMATEMESH_H_vj923_v9wj3_934jg



#include	<map>

#include	"RD_GraphicRoot.h"
#include	"CAllocateHierarchy.h"



struct SAniTrackInfo
{
	D3DXTRACK_DESC	m_TrackDesc;
	UINT			m_AniIndex;
	LPCSTR			m_AniName;
};



class CAnimateMesh
{

public:

	CAnimateMesh( void );
	~CAnimateMesh( void );
	CAnimateMesh( const CAnimateMesh & src );
	CAnimateMesh & operator=( const CAnimateMesh & src );

	// Get Member Function
	const LPD3DXFRAME GetFrameRoot( void )									{ return m_pFrameRoot; }
	ID3DXAnimationController * GetAnimateController( void )					{ return m_pAnimateController; }
	const bool IsAnimationContinue( void ) const							{ return m_bAnimationContinue; }
	const UINT GetIdleAnimationSet( void ) const							{ return m_IdleAnimationSet; }
	const std::map< UINT, LPD3DXANIMATIONSET > & GetAnimationSetMap( void )	{ return m_AnimationSetMap; }
	const std::map< UINT, SAniTrackInfo > & GetAniTrackMap( void ) const	{ return m_AniTrackMap; }
	bool GetAniTrackInfo( UINT aTrackIndex, SAniTrackInfo & aInfoOut );
	const D3DXVECTOR3 & GetXAxis( void ) const								{ return m_XAxis; }
	const D3DXVECTOR3 & GetYAxis( void ) const								{ return m_YAxis; }
	const D3DXVECTOR3 & GetZAxis( void ) const								{ return m_ZAxis; }
	const D3DXVECTOR3 & GetPosition( void ) const							{ return m_Position; }
	const D3DXMATRIX & GetTransforMatrix( void ) const						{ return m_TransforMatrix; }

	// Set Member Function
	void SetAnimationContinue( bool abOnOff )							{ m_bAnimationContinue = abOnOff; }
	void SetIdleAnimationSet( UINT aIndex );
	void SetCurrentAnimationSet( UINT aTrackIndex, UINT aAniIndex );
	void SetAniTrackInfoDesc( UINT aTrackIndex, const D3DXTRACK_DESC & aDesc );
	void SetPosition( const D3DXVECTOR3 & aPoint )						{ m_Position = aPoint; }
	void SetTransforMatrix( const D3DXMATRIX & aMatrix )				{ m_TransforMatrix = aMatrix; }

	void ResetAxis( void );

	HRESULT LoadFromX( LPCTSTR aFilePath, LPCTSTR aFileName, DWORD aMeshOptions, LPDIRECT3DDEVICE9 apDevice, LPD3DXLOADUSERDATA apUserDataLoader );
	HRESULT DestroyAll( void );
	HRESULT DestroyFrameRoot( void );
	HRESULT DestroyAnimateController( void );

	HRESULT UpdatePerFrame( DOUBLE aTimeDelta, LPD3DXANIMATIONCALLBACKHANDLER pCallbackHandler );
	void Render( LPDIRECT3DDEVICE9 apDevice );

	bool MoveOnXAxis( float aDistance );
	bool MoveOnYAxis( float aDistance );
	bool MoveOnZAxis( float aDistance );

	bool Rotate( float aX, float aY, float aZ );


private:

	bool						m_bSetFrameRoot;
	LPD3DXFRAME					m_pFrameRoot;

	bool						m_bSetAnimateController;
	ID3DXAnimationController *	m_pAnimateController;

	bool						m_bAnimationContinue;
	UINT						m_IdleAnimationSet;
	std::map< UINT, LPD3DXANIMATIONSET >	m_AnimationSetMap;		// < AnimationSet index, AnimationSet reference >
	std::map< UINT, SAniTrackInfo >			m_AniTrackMap;			// < Animation Track index, track information >

	D3DXVECTOR3					m_XAxis;
	D3DXVECTOR3					m_YAxis;
	D3DXVECTOR3					m_ZAxis;

	D3DXVECTOR3					m_Position;

	// For world transfor
	D3DXMATRIX					m_TransforMatrix;

};



HRESULT CopyFrameTree( const D3DXFRAME * const pSrcFrameCurrent, LPD3DXFRAME & pDestFrameCurrent );
HRESULT SetupBoneMatrixPointersOnMesh( LPD3DXMESHCONTAINER pMeshContainer, LPD3DXFRAME pReferFrameRoot );
HRESULT SetupBoneMatrixPointers( LPD3DXFRAME pFrameRoot, LPD3DXFRAME pFrameCurrent );
void UpdateFrameMatrices( LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix );
void DrawFrame( IDirect3DDevice9 * pd3dDevice, LPD3DXFRAME pFrame );
void DrawMeshContainer( IDirect3DDevice9 * pd3dDevice, LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase );

HRESULT UpdateHierarchyBoundingBoxInfo( LPD3DXFRAME pFrameCurrent, LPD3DXVECTOR3 pMin, LPD3DXVECTOR3 pMax );




#endif		// CANIMATEMESH_H_vj923_v9wj3_934jg
