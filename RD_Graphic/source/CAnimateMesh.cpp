
#include	"stdafx.h"
#include	"CAnimateMesh.h"



CAnimateMesh::CAnimateMesh( void ) : m_bSetFrameRoot( false ), m_pFrameRoot( NULL ),
										m_bSetAnimateController( false ), m_pAnimateController( NULL ),
										m_bAnimationContinue( false ), m_IdleAnimationSet( 0 ),
										m_XAxis( 1, 0, 0 ), m_YAxis( 0, 1, 0 ), m_ZAxis( 0, 0, 1 ), m_Position( 0, 0, 0 )
{
	D3DXMatrixIdentity( &m_TransforMatrix );
}


CAnimateMesh::~CAnimateMesh( void )
{
	DestroyAll();
}


CAnimateMesh::CAnimateMesh( const CAnimateMesh & src ) : m_bSetFrameRoot( false ), m_bSetAnimateController( false ),
															m_XAxis( src.m_XAxis ), m_YAxis( src.m_YAxis ), m_ZAxis( src.m_ZAxis ),
															m_bAnimationContinue( src.m_bAnimationContinue ),
															m_IdleAnimationSet( src.m_IdleAnimationSet ),
															m_Position( 0, 0, 0 ), m_TransforMatrix( src.m_TransforMatrix )
{
	// copy m_pFrameRoot
	DestroyFrameRoot();
	if( src.m_bSetFrameRoot  &&  !FAILED( CopyFrameTree( src.m_pFrameRoot, m_pFrameRoot ) )  &&
		!FAILED( SetupBoneMatrixPointers( m_pFrameRoot, m_pFrameRoot ) ) )
	{
		m_bSetFrameRoot = true;
	}

	// copy m_pAnimateController
	DestroyAnimateController();
	if( src.m_bSetAnimateController  &&  src.m_pAnimateController != NULL  &&
		!FAILED( src.m_pAnimateController->CloneAnimationController(  src.m_pAnimateController->GetMaxNumAnimationOutputs(),
																		src.m_pAnimateController->GetMaxNumAnimationSets(),
																		src.m_pAnimateController->GetMaxNumTracks(),
																		src.m_pAnimateController->GetMaxNumEvents(),
																		&m_pAnimateController ) ) )
	{
		m_bSetAnimateController = true;


		LPD3DXANIMATIONSET	tempAS;

		for( UINT i = 0 ; i < m_pAnimateController->GetNumAnimationSets() ; ++i )
		{
			if( !FAILED( m_pAnimateController->GetAnimationSet( i, &tempAS ) ) )
			{
				m_AnimationSetMap.insert( std::map< UINT, LPD3DXANIMATIONSET >::value_type( i, tempAS ) );
			}
		}


		m_AniTrackMap = src.m_AniTrackMap;

		for( UINT i = 0 ; i < m_pAnimateController->GetMaxNumTracks() ; ++i )
		{
			if( m_AnimationSetMap.end() != m_AnimationSetMap.find( m_AniTrackMap[ i ].m_AniIndex ) )
			{
				m_pAnimateController->SetTrackAnimationSet( i, m_AnimationSetMap[ m_AniTrackMap[ i ].m_AniIndex ] );
				m_pAnimateController->SetTrackDesc( i, &(m_AniTrackMap[ i ].m_TrackDesc) );
			}
		}
	}
}


CAnimateMesh & CAnimateMesh::operator=( const CAnimateMesh & src )
{
	if( this != (&src) )
	{
		// copy m_pFrameRoot
		DestroyFrameRoot();
		if( src.m_bSetFrameRoot  &&  !FAILED( CopyFrameTree( src.m_pFrameRoot, m_pFrameRoot ) )  &&
			!FAILED( SetupBoneMatrixPointers( m_pFrameRoot, m_pFrameRoot ) ) )
		{
			m_bSetFrameRoot = true;
		}

		// copy m_pAnimateController
		DestroyAnimateController();
		if( src.m_bSetAnimateController  &&  src.m_pAnimateController != NULL  &&
			!FAILED( src.m_pAnimateController->CloneAnimationController(  src.m_pAnimateController->GetMaxNumAnimationOutputs(),
																			src.m_pAnimateController->GetMaxNumAnimationSets(),
																			src.m_pAnimateController->GetMaxNumTracks(),
																			src.m_pAnimateController->GetMaxNumEvents(),
																			&m_pAnimateController ) ) )
		{
			m_bSetAnimateController = true;


			LPD3DXANIMATIONSET	tempAS;

			for( UINT i = 0 ; i < m_pAnimateController->GetNumAnimationSets() ; ++i )
			{
				if( !FAILED( m_pAnimateController->GetAnimationSet( i, &tempAS ) ) )
				{
					m_AnimationSetMap.insert( std::map< UINT, LPD3DXANIMATIONSET >::value_type( i, tempAS ) );
				}
			}


			m_AniTrackMap = src.m_AniTrackMap;

			for( UINT i = 0 ; i < m_pAnimateController->GetMaxNumTracks() ; ++i )
			{
				if( m_AnimationSetMap.end() != m_AnimationSetMap.find( m_AniTrackMap[ i ].m_AniIndex ) )
				{
					m_pAnimateController->SetTrackAnimationSet( i, m_AnimationSetMap[ m_AniTrackMap[ i ].m_AniIndex ] );
					m_pAnimateController->SetTrackDesc( i, &(m_AniTrackMap[ i ].m_TrackDesc) );
				}
			}
		}

		m_bAnimationContinue = src.m_bAnimationContinue;
		m_IdleAnimationSet = src.m_IdleAnimationSet;

		m_XAxis = src.m_XAxis;
		m_YAxis = src.m_YAxis;
		m_ZAxis = src.m_ZAxis;

		m_Position = src.m_Position;

		// copy m_TransforMatrix
		m_TransforMatrix = src.m_TransforMatrix;
	}

	return	(*this);
}


bool CAnimateMesh::GetAniTrackInfo( UINT aTrackIndex, SAniTrackInfo & aInfoOut )
{
	std::map< UINT, SAniTrackInfo >::iterator		tIter = m_AniTrackMap.find( aTrackIndex );

	if( m_AniTrackMap.end() != tIter )
	{
		m_pAnimateController->GetTrackDesc( aTrackIndex, &(tIter->second.m_TrackDesc) );
		aInfoOut = tIter->second;
		return	true;
	}

	return	false;
}


void CAnimateMesh::SetIdleAnimationSet( UINT aIndex )
{
	if( m_AnimationSetMap.size() > aIndex )
	{
		m_IdleAnimationSet = aIndex;
	}
}


void CAnimateMesh::SetCurrentAnimationSet( UINT aTrackIndex, UINT aAniIndex )
{
	if( NULL != m_pAnimateController )
	{
		if( m_AnimationSetMap.size() > aAniIndex  &&
			m_AniTrackMap.end() != m_AniTrackMap.find( aTrackIndex )  &&  m_AniTrackMap[ aTrackIndex ].m_AniIndex != aAniIndex )
		{
			m_AniTrackMap[ aTrackIndex ].m_AniIndex = aAniIndex;
			m_pAnimateController->SetTrackAnimationSet( aTrackIndex, m_AnimationSetMap[ m_AniTrackMap[aTrackIndex].m_AniIndex ] );
		}
		m_pAnimateController->SetTrackEnable( aTrackIndex, TRUE );
	}
}


void CAnimateMesh::SetAniTrackInfoDesc( UINT aTrackIndex, const D3DXTRACK_DESC & aDesc )
{
	if( m_AniTrackMap.end() != m_AniTrackMap.find( aTrackIndex ) )
	{
		m_AniTrackMap[ aTrackIndex ].m_TrackDesc = aDesc;
		m_pAnimateController->SetTrackDesc( aTrackIndex, &(m_AniTrackMap[ aTrackIndex ].m_TrackDesc) );
	}
}


void CAnimateMesh::ResetAxis( void )
{
	m_XAxis = D3DXVECTOR3( 1, 0, 0 );
	m_YAxis = D3DXVECTOR3( 0, 1, 0 );
	m_ZAxis = D3DXVECTOR3( 0, 0, 1 );
}


HRESULT CAnimateMesh::LoadFromX( LPCTSTR aFilePath, LPCTSTR aFileName, DWORD aMeshOptions, LPDIRECT3DDEVICE9 apDevice, LPD3DXLOADUSERDATA apUserDataLoader )
{
	if( aFilePath == NULL  ||  aFileName == NULL )
	{
		return	E_INVALIDARG;
	}


	HRESULT				hr = S_OK;
	CAllocateHierarchy	tempAllocate;
	TCHAR				tempCD[ MAX_PATH ];

	DestroyAll();

	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( aFilePath );
	hr = D3DXLoadMeshHierarchyFromX( aFileName, aMeshOptions, apDevice, &tempAllocate, apUserDataLoader, &m_pFrameRoot, &m_pAnimateController );
	SetCurrentDirectory( tempCD );
	if( !FAILED( hr )  &&  !FAILED( SetupBoneMatrixPointers( m_pFrameRoot, m_pFrameRoot ) ) )
	{
		m_bSetFrameRoot = true;
	}

	if( m_pAnimateController != NULL )
	{
		m_bSetAnimateController = true;

		LPD3DXANIMATIONSET	tempAS;
		SAniTrackInfo		tempTrackInfo;

		for( UINT i = 0 ; i < m_pAnimateController->GetNumAnimationSets() ; ++i )
		{
			if( !FAILED( m_pAnimateController->GetAnimationSet( i, &tempAS ) ) )
			{
				m_AnimationSetMap.insert( std::map< UINT, LPD3DXANIMATIONSET >::value_type( i, tempAS ) );
			}
		}

		for( UINT i = 0 ; i < m_pAnimateController->GetMaxNumTracks() ; ++i )
		{
			tempTrackInfo.m_AniIndex = m_IdleAnimationSet;

			if( m_AnimationSetMap.size() > m_IdleAnimationSet )
			{
				m_pAnimateController->SetTrackAnimationSet( i, m_AnimationSetMap[ tempTrackInfo.m_AniIndex ] );
			}

			m_pAnimateController->GetTrackDesc( i, &(tempTrackInfo.m_TrackDesc) );

			tempTrackInfo.m_TrackDesc.Enable = TRUE;
			tempTrackInfo.m_TrackDesc.Weight = 1.0f / m_pAnimateController->GetMaxNumTracks();
			tempTrackInfo.m_TrackDesc.Speed = 1.0f;
			m_pAnimateController->SetTrackDesc( i, &(tempTrackInfo.m_TrackDesc) );

			m_AniTrackMap.insert( std::map< UINT, SAniTrackInfo >::value_type( i, tempTrackInfo ) );
		}
	}
	else
	{
		m_bSetAnimateController = false;
	}

	return	hr;
}


HRESULT CAnimateMesh::DestroyAll( void )
{
	HRESULT	hr;

	hr = DestroyFrameRoot();
	DestroyAnimateController();

	return	hr;
}


HRESULT CAnimateMesh::DestroyFrameRoot( void )
{
	if( !m_bSetFrameRoot )
	{
		return	S_OK;
	}

	HRESULT	hr;
	CAllocateHierarchy	tempAllocate;
	
	hr = D3DXFrameDestroy( m_pFrameRoot, &tempAllocate );
	if( !FAILED( hr ) )
	{
		m_bSetFrameRoot = false;
	}

	return	hr;
}


HRESULT CAnimateMesh::DestroyAnimateController( void )
{
	if( !m_bSetAnimateController )
	{
		return	S_OK;
	}

	SAFE_RELEASE( m_pAnimateController );
	m_AnimationSetMap.clear();
	m_AniTrackMap.clear();
	m_bSetAnimateController = false;

	return	S_OK;
}


HRESULT CAnimateMesh::UpdatePerFrame( DOUBLE aTimeDelta, LPD3DXANIMATIONCALLBACKHANDLER pCallbackHandler )
{
	D3DXVECTOR3		countYAxis( m_YAxis.x, -m_YAxis.y, m_YAxis.z );
	HRESULT			hr = S_OK;


	m_TransforMatrix._41 = m_Position.x;
	m_TransforMatrix._42 = m_Position.y;
	m_TransforMatrix._43 = m_Position.z;


	if( !m_bSetFrameRoot  &&  !m_bSetAnimateController )
	{
		return	hr;
	}


	UpdateFrameMatrices( m_pFrameRoot, &m_TransforMatrix );


	if( m_pAnimateController != NULL )
	{
		hr = m_pAnimateController->AdvanceTime( aTimeDelta, pCallbackHandler );
	}


	return	hr;
}


void CAnimateMesh::Render( LPDIRECT3DDEVICE9 apDevice )
{
	if( !m_bSetFrameRoot )
	{
		return;
	}

	DrawFrame( apDevice, m_pFrameRoot );
}


bool CAnimateMesh::MoveOnXAxis( float aDistance )
{
	D3DXVECTOR3	tempXAxis;

	D3DXVec3Normalize( &tempXAxis, &m_XAxis );

	m_TransforMatrix._41 = (m_Position.x += (tempXAxis.x * aDistance));
	m_TransforMatrix._42 = (m_Position.y += (tempXAxis.y * aDistance));
	m_TransforMatrix._43 = (m_Position.z += (tempXAxis.z * aDistance));

	return	true;
}


bool CAnimateMesh::MoveOnYAxis( float aDistance )
{
	D3DXVECTOR3	tempYAxis;

	D3DXVec3Normalize( &tempYAxis, &m_YAxis );

	m_TransforMatrix._41 = (m_Position.x += (tempYAxis.x * aDistance));
	m_TransforMatrix._42 = (m_Position.y += (tempYAxis.y * aDistance));
	m_TransforMatrix._43 = (m_Position.z += (tempYAxis.z * aDistance));

	return	true;
}


bool CAnimateMesh::MoveOnZAxis( float aDistance )
{
	D3DXVECTOR3	tempZAxis;

	D3DXVec3Normalize( &tempZAxis, &m_ZAxis );

	m_TransforMatrix._41 = (m_Position.x += (tempZAxis.x * aDistance));
	m_TransforMatrix._42 = (m_Position.y += (tempZAxis.y * aDistance));
	m_TransforMatrix._43 = (m_Position.z += (tempZAxis.z * aDistance));

	return	true;
}


bool CAnimateMesh::Rotate( float aX, float aY, float aZ )
{
	D3DXMATRIX	tempMatrix;

	D3DXMatrixIdentity( &tempMatrix );

	if( aX != 0.0f )
	{
		D3DXMatrixRotationAxis( &tempMatrix, &m_XAxis, D3DXToRadian( aX ) );
		D3DXVec3TransformNormal( &m_ZAxis, &m_ZAxis, &tempMatrix );
		D3DXVec3TransformNormal( &m_YAxis, &m_YAxis, &tempMatrix );
		D3DXVec3TransformNormal( &m_XAxis, &m_XAxis, &tempMatrix );
	}

	if( aY != 0.0f )
	{
		D3DXMatrixRotationAxis( &tempMatrix, &m_YAxis, D3DXToRadian( aY ) );
		D3DXVec3TransformNormal( &m_ZAxis, &m_ZAxis, &tempMatrix );
		D3DXVec3TransformNormal( &m_YAxis, &m_YAxis, &tempMatrix );
		D3DXVec3TransformNormal( &m_XAxis, &m_XAxis, &tempMatrix );
	}

	if( aZ != 0.0f )
	{
		D3DXMatrixRotationAxis( &tempMatrix, &m_ZAxis, D3DXToRadian( aZ ) );
		D3DXVec3TransformNormal( &m_ZAxis, &m_ZAxis, &tempMatrix );
		D3DXVec3TransformNormal( &m_YAxis, &m_YAxis, &tempMatrix );
		D3DXVec3TransformNormal( &m_XAxis, &m_XAxis, &tempMatrix );
	}

	D3DXMatrixMultiply( &m_TransforMatrix, &m_TransforMatrix, &tempMatrix );

	D3DXVec3Normalize( &m_ZAxis, &m_ZAxis );
	D3DXVec3Cross( &m_XAxis, &m_YAxis, &m_ZAxis );
	D3DXVec3Normalize( &m_XAxis, &m_XAxis );
	D3DXVec3Cross( &m_YAxis, &m_ZAxis, &m_XAxis );
	D3DXVec3Normalize( &m_YAxis, &m_YAxis );

	return	true;
}



HRESULT CopyFrameTree( const D3DXFRAME * const pSrcFrameCurrent, LPD3DXFRAME & pDestFrameCurrent )
{
	if( pSrcFrameCurrent == NULL )
	{
		pDestFrameCurrent = NULL;
	}
	else
	{
		HRESULT		hr;
		CAllocateHierarchy	tempAllocate;

		hr = tempAllocate.CreateFrame( pSrcFrameCurrent->Name, &pDestFrameCurrent );
		if( FAILED( hr ) )
		{
			return	hr;
		}

		D3DXFRAME_DERIVED *	pSrcFrameCurrentEx = (D3DXFRAME_DERIVED*)pSrcFrameCurrent;
		D3DXFRAME_DERIVED *	pDestFrameCurrentEx = (D3DXFRAME_DERIVED*)pDestFrameCurrent;

		memcpy(  &pDestFrameCurrentEx->TransformationMatrix,  &pSrcFrameCurrentEx->TransformationMatrix,
				sizeof(pSrcFrameCurrent->TransformationMatrix) );
		memcpy(  &pDestFrameCurrentEx->CombinedTransformationMatrix,  &pSrcFrameCurrentEx->CombinedTransformationMatrix,
				sizeof(pSrcFrameCurrentEx->CombinedTransformationMatrix) );

		if( pSrcFrameCurrentEx->pMeshContainer != NULL )
		{
			D3DXMESHDATA		tempMesh;
			LPD3DXMESH			tempSrcMesh;
			LPDIRECT3DDEVICE9	pd3dDevice = NULL;
			D3DXMESHCONTAINER_DERIVED *	pSrcMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pSrcFrameCurrentEx->pMeshContainer;

			tempMesh.Type = pSrcMeshContainer->MeshData.Type;
			tempSrcMesh = pSrcMeshContainer->pOrigMesh;
			tempSrcMesh->GetDevice( &pd3dDevice );
			hr = tempSrcMesh->CloneMeshFVF( tempSrcMesh->GetOptions(), tempSrcMesh->GetFVF(), pd3dDevice, &tempMesh.pMesh );
			SAFE_RELEASE( pd3dDevice );
			if( FAILED( hr ) )
			{
				return	hr;
			}

			hr = tempAllocate.CreateMeshContainer(  pSrcMeshContainer->Name,  &tempMesh,  pSrcMeshContainer->pMaterials,  NULL,
													pSrcMeshContainer->NumMaterials,  pSrcMeshContainer->pAdjacency,
													pSrcMeshContainer->pSkinInfo,  &pDestFrameCurrentEx->pMeshContainer  );
			SAFE_RELEASE( tempMesh.pMesh );
			if( FAILED( hr ) )
			{
				return	hr;
			}
		}

		if( pSrcFrameCurrentEx->pFrameSibling != NULL )
		{
			hr = CopyFrameTree( pSrcFrameCurrentEx->pFrameSibling, pDestFrameCurrent->pFrameSibling );
			if( FAILED( hr ) )
			{
				return	hr;
			}
		}

		if( pSrcFrameCurrentEx->pFrameFirstChild != NULL )
		{
			hr = CopyFrameTree( pSrcFrameCurrentEx->pFrameFirstChild, pDestFrameCurrent->pFrameFirstChild );
			if( FAILED( hr ) )
			{
				return	hr;
			}
		}
	}

	return	S_OK;
}


HRESULT SetupBoneMatrixPointersOnMesh( LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pReferFrameRoot )
{
	if( pMeshContainerBase == NULL  ||  pReferFrameRoot == NULL )
	{
		return	E_INVALIDARG;
	}


	UINT				iBone, cBones;
	D3DXFRAME_DERIVED *	pFrame;

	D3DXMESHCONTAINER_DERIVED *	pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;

	if( pMeshContainer->pSkinInfo != NULL )
	{
		cBones = pMeshContainer->pSkinInfo->GetNumBones();

		pMeshContainer->ppBoneMatrixPtrs = new D3DXMATRIX*[ cBones ];
		if( pMeshContainer->ppBoneMatrixPtrs == NULL )
		{
			return	E_OUTOFMEMORY;
		}

		for( iBone = 0 ; iBone < cBones ; ++iBone )
		{
			pFrame = (D3DXFRAME_DERIVED*)D3DXFrameFind( pReferFrameRoot, pMeshContainer->pSkinInfo->GetBoneName( iBone ) );
			if( pFrame == NULL )
			{
				return	E_FAIL;
			}

			pMeshContainer->ppBoneMatrixPtrs[ iBone ] = &pFrame->CombinedTransformationMatrix;
		}
	}

	return S_OK;
}


HRESULT SetupBoneMatrixPointers( LPD3DXFRAME pFrameRoot, LPD3DXFRAME pFrameCurrent )
{
	if( pFrameRoot == NULL  ||  pFrameCurrent == NULL )
	{
		return	E_INVALIDARG;
	}


	HRESULT	hr;

	if( pFrameCurrent->pMeshContainer != NULL )
	{
		hr = SetupBoneMatrixPointersOnMesh( pFrameCurrent->pMeshContainer, pFrameRoot );
		if( FAILED( hr ) )
		{
			return	hr;
		}
	}

	if( pFrameCurrent->pFrameSibling != NULL )
	{
		hr = SetupBoneMatrixPointers( pFrameRoot, pFrameCurrent->pFrameSibling );
		if( FAILED( hr ) )
		{
			return	hr;
		}
	}

	if( pFrameCurrent->pFrameFirstChild != NULL )
	{
		hr = SetupBoneMatrixPointers( pFrameRoot, pFrameCurrent->pFrameFirstChild );
		if( FAILED( hr ) )
		{
			return	hr;
		}
	}

	return S_OK;
}


void UpdateFrameMatrices( LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix )
{
	D3DXFRAME_DERIVED *	pFrame = (D3DXFRAME_DERIVED*)pFrameBase;

	if( pParentMatrix != NULL )
	{
		D3DXMatrixMultiply( &pFrame->CombinedTransformationMatrix, &pFrame->TransformationMatrix, pParentMatrix );
	}
	else
	{
		pFrame->CombinedTransformationMatrix = pFrame->TransformationMatrix;
	}

	if( pFrame->pFrameSibling != NULL )
	{
		UpdateFrameMatrices( pFrame->pFrameSibling, pParentMatrix );
	}

	if( pFrame->pFrameFirstChild != NULL )
	{
		UpdateFrameMatrices( pFrame->pFrameFirstChild, &pFrame->CombinedTransformationMatrix );
	}
}


void DrawFrame( IDirect3DDevice9 * pd3dDevice, LPD3DXFRAME pFrame )
{
	LPD3DXMESHCONTAINER	pMeshContainer;

	pMeshContainer = pFrame->pMeshContainer;
	while( pMeshContainer != NULL )
	{
		DrawMeshContainer( pd3dDevice, pMeshContainer, pFrame );

		pMeshContainer = pMeshContainer->pNextMeshContainer;
	}

	if( pFrame->pFrameSibling != NULL )
	{
		DrawFrame( pd3dDevice, pFrame->pFrameSibling );
	}

	if( pFrame->pFrameFirstChild != NULL )
	{
		DrawFrame( pd3dDevice, pFrame->pFrameFirstChild );
	}
}


void DrawMeshContainer( IDirect3DDevice9 * pd3dDevice, LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase )
{
	HRESULT					hr;
	D3DXMESHCONTAINER_DERIVED *	pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;
	D3DXFRAME_DERIVED *		pFrame = (D3DXFRAME_DERIVED*)pFrameBase;
	UINT					iMaterial;
	UINT					NumBlend;
	UINT					iAttrib;
	DWORD					AttribIdPrev;
	LPD3DXBONECOMBINATION	pBoneComb;

	UINT			iMatrixIndex;
	UINT			iPaletteEntry;
	D3DXMATRIX		matTemp;
	D3DCAPS9		d3dCaps;
	pd3dDevice->GetDeviceCaps( &d3dCaps );

	if( pMeshContainer->pSkinInfo != NULL )
	{
		if( g_SkinningMethod == D3DNONINDEXED )
		{
			AttribIdPrev = UNUSED32;
			pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>( pMeshContainer->pBoneCombinationBuf->GetBufferPointer() );

			for( iAttrib = 0 ; iAttrib < pMeshContainer->NumAttributeGroups ; ++iAttrib )
			{
				NumBlend = 0;
				for( DWORD i = 0 ; i < pMeshContainer->NumInfl ; ++i )
				{
					if( pBoneComb[ iAttrib ].BoneId[ i ] != UINT_MAX )
					{
						NumBlend = i;
					}
				}

				if( d3dCaps.MaxVertexBlendMatrices >= NumBlend + 1 )
				{
					for( DWORD i = 0 ; i < pMeshContainer->NumInfl ; ++i )
					{
						iMatrixIndex = pBoneComb[ iAttrib ].BoneId[ i ];
						if( iMatrixIndex != UINT_MAX )
						{
							D3DXMatrixMultiply( &matTemp, &pMeshContainer->pBoneOffsetMatrices[ iMatrixIndex ],
												pMeshContainer->ppBoneMatrixPtrs[ iMatrixIndex ] );
							V( pd3dDevice->SetTransform( D3DTS_WORLDMATRIX( i ), &matTemp ) );
						}
					}

					V( pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND, NumBlend ) );
					pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

					if( (AttribIdPrev != pBoneComb[ iAttrib ].AttribId) || (AttribIdPrev == UNUSED32) )
					{
						V( pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[ pBoneComb[ iAttrib ].AttribId ].MatD3D ) );
						V( pd3dDevice->SetTexture( 0, pMeshContainer->ppTextures[ pBoneComb[ iAttrib ].AttribId ] ) );
						AttribIdPrev = pBoneComb[ iAttrib ].AttribId;
					}

					V( pMeshContainer->MeshData.pMesh->DrawSubset( iAttrib ) );
				}
			}

			if( pMeshContainer->iAttributeSW < pMeshContainer->NumAttributeGroups )
			{
				AttribIdPrev = UNUSED32;
				V( pd3dDevice->SetSoftwareVertexProcessing( TRUE ) );
				for( iAttrib = pMeshContainer->iAttributeSW ; iAttrib < pMeshContainer->NumAttributeGroups ; ++iAttrib )
				{
					NumBlend = 0;
					for( DWORD i = 0 ; i < pMeshContainer->NumInfl ; ++i )
					{
						if( pBoneComb[ iAttrib ].BoneId[ i ] != UINT_MAX )
						{
							NumBlend = i;
						}
					}

					if( d3dCaps.MaxVertexBlendMatrices < NumBlend + 1 )
					{
						for( DWORD i = 0 ; i < pMeshContainer->NumInfl ; ++i )
						{
							iMatrixIndex = pBoneComb[ iAttrib ].BoneId[ i ];
							if( iMatrixIndex != UINT_MAX )
							{
								D3DXMatrixMultiply( &matTemp, &pMeshContainer->pBoneOffsetMatrices[ iMatrixIndex ],
													pMeshContainer->ppBoneMatrixPtrs[ iMatrixIndex ] );
								V( pd3dDevice->SetTransform( D3DTS_WORLDMATRIX( i ), &matTemp ) );
							}
						}

						V( pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND, NumBlend ) );
						pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

						if( (AttribIdPrev != pBoneComb[ iAttrib ].AttribId) || (AttribIdPrev == UNUSED32) )
						{
							V( pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[ pBoneComb[ iAttrib ].AttribId ].MatD3D ) );
							V( pd3dDevice->SetTexture( 0, pMeshContainer->ppTextures[ pBoneComb[ iAttrib ].AttribId ] ) );
							AttribIdPrev = pBoneComb[ iAttrib ].AttribId;
						}

						V( pMeshContainer->MeshData.pMesh->DrawSubset( iAttrib ) );
					}
				}
				V( pd3dDevice->SetSoftwareVertexProcessing( FALSE ) );
			}
			V( pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND, 0 ) );
			pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );
		}
		else if( g_SkinningMethod == D3DINDEXED )
		{
			if( pMeshContainer->UseSoftwareVP )
			{
				if( g_dwBehaviorFlags & D3DCREATE_HARDWARE_VERTEXPROCESSING )
					return;

				V( pd3dDevice->SetSoftwareVertexProcessing( TRUE ) );
			}

			if( pMeshContainer->NumInfl == 1 )
			{
				V( pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND, D3DVBF_0WEIGHTS ) );
			}
			else
			{
				V( pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND, pMeshContainer->NumInfl - 1 ) );
			}

			if( pMeshContainer->NumInfl )
			{
				V( pd3dDevice->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, TRUE ) );
			}
			pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

			pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>( pMeshContainer->pBoneCombinationBuf->GetBufferPointer() );
			for( iAttrib = 0 ; iAttrib < pMeshContainer->NumAttributeGroups ; ++iAttrib )
			{
				for( iPaletteEntry = 0 ; iPaletteEntry < pMeshContainer->NumPaletteEntries ; ++iPaletteEntry )
				{
					iMatrixIndex = pBoneComb[ iAttrib ].BoneId[ iPaletteEntry ];
					if( iMatrixIndex != UINT_MAX )
					{
						D3DXMatrixMultiply( &matTemp, &pMeshContainer->pBoneOffsetMatrices[ iMatrixIndex ],
											pMeshContainer->ppBoneMatrixPtrs[ iMatrixIndex ] );
						V( pd3dDevice->SetTransform( D3DTS_WORLDMATRIX( iPaletteEntry ), &matTemp ) );
					}
				}

				V( pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[ pBoneComb[ iAttrib ].AttribId ].MatD3D ) );
				V( pd3dDevice->SetTexture( 0, pMeshContainer->ppTextures[ pBoneComb[ iAttrib ].AttribId ] ) );
				V( pMeshContainer->MeshData.pMesh->DrawSubset( iAttrib ) );
			}

			V( pd3dDevice->SetRenderState( D3DRS_INDEXEDVERTEXBLENDENABLE, FALSE ) );
			V( pd3dDevice->SetRenderState( D3DRS_VERTEXBLEND, 0 ) );
			pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

			if( pMeshContainer->UseSoftwareVP )
			{
				V( pd3dDevice->SetSoftwareVertexProcessing( FALSE ) );
			}
		}
		else if( g_SkinningMethod == SOFTWARE )
		{
			D3DXMATRIX	Identity;
			DWORD	cBones = pMeshContainer->pSkinInfo->GetNumBones();
			DWORD	iBone;
			PBYTE	pbVerticesSrc;
			PBYTE	pbVerticesDest;

			for( iBone = 0 ; iBone < cBones ; ++iBone )
			{
				D3DXMatrixMultiply( &g_pBoneMatrices[ iBone ], &pMeshContainer->pBoneOffsetMatrices[ iBone ], pMeshContainer->ppBoneMatrixPtrs[ iBone ] );
			}

			D3DXMatrixIdentity( &Identity );
			V( pd3dDevice->SetTransform( D3DTS_WORLD, &Identity ) );

			V( pMeshContainer->pOrigMesh->LockVertexBuffer( D3DLOCK_READONLY, (LPVOID*)&pbVerticesSrc ) );
			V( pMeshContainer->MeshData.pMesh->LockVertexBuffer( 0, (LPVOID*)&pbVerticesDest ) );

			pMeshContainer->pSkinInfo->UpdateSkinnedMesh( g_pBoneMatrices, NULL, pbVerticesSrc, pbVerticesDest );

			V( pMeshContainer->pOrigMesh->UnlockVertexBuffer() );
			V( pMeshContainer->MeshData.pMesh->UnlockVertexBuffer() );

			for( iAttrib = 0 ; iAttrib < pMeshContainer->NumAttributeGroups ; ++iAttrib )
			{
				V( pd3dDevice->SetMaterial(
					&(pMeshContainer->pMaterials[ pMeshContainer->pAttributeTable[ iAttrib ].AttribId ].MatD3D) )
					);
				V( pd3dDevice->SetTexture( 0, pMeshContainer->ppTextures[ pMeshContainer->pAttributeTable[ iAttrib ].AttribId ] ) );
				V( pMeshContainer->MeshData.pMesh->DrawSubset( pMeshContainer->pAttributeTable[ iAttrib ].AttribId ) );
			}
		}
		else
		{
			return;
		}
	}
	else
	{
		V( pd3dDevice->SetTransform( D3DTS_WORLD, &pFrame->CombinedTransformationMatrix ) );

		for( iMaterial = 0 ; iMaterial < pMeshContainer->NumMaterials ; ++iMaterial )
		{
			V( pd3dDevice->SetMaterial( &pMeshContainer->pMaterials[ iMaterial ].MatD3D ) );
			V( pd3dDevice->SetTexture( 0, pMeshContainer->ppTextures[ iMaterial ] ) );
			V( pMeshContainer->MeshData.pMesh->DrawSubset( iMaterial ) );
		}
	}
}


HRESULT UpdateHierarchyBoundingBoxInfo( LPD3DXFRAME pFrameCurrent, LPD3DXVECTOR3 pMin, LPD3DXVECTOR3 pMax )
{
	if( pFrameCurrent == NULL  ||  pMin == NULL  ||  pMax == NULL )
	{
		return	E_INVALIDARG;
	}


	HRESULT		hr = S_OK;


	if( pFrameCurrent->pFrameFirstChild )
	{
		UpdateHierarchyBoundingBoxInfo( pFrameCurrent->pFrameFirstChild, pMin, pMax );
	}

	if( pFrameCurrent->pFrameSibling )
	{
		UpdateHierarchyBoundingBoxInfo( pFrameCurrent->pFrameSibling, pMin, pMax );
	}


	if( pFrameCurrent->pMeshContainer )
	{
		for( D3DXMESHCONTAINER_DERIVED * pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pFrameCurrent->pMeshContainer  ;
				pMeshContainer  ;  pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainer->pNextMeshContainer  )
		{
			if( pMeshContainer->pOrigMesh )
			{
				D3DXVECTOR3 *	pVertices = NULL;
				D3DXVECTOR3		tempMin;
				D3DXVECTOR3		tempMax;

				pMeshContainer->pOrigMesh->LockVertexBuffer( 0, (LPVOID*)&pVertices );
				hr = D3DXComputeBoundingBox(  pVertices,
												pMeshContainer->pOrigMesh->GetNumVertices(),
												pMeshContainer->pOrigMesh->GetNumBytesPerVertex(),
												&tempMin, &tempMax  );
				pMeshContainer->pOrigMesh->UnlockVertexBuffer();

				if( !FAILED( hr ) )
				{
					if( pMin->x == 0.0f  &&  pMin->y == 0.0f  &&  pMin->z == 0.0f )
						(*pMin) = tempMin;
					else
						D3DXVec3Minimize( pMin, pMin, &tempMin );

					if( pMax->x == 0.0f  &&  pMax->y == 0.0f  &&  pMax->z == 0.0f )
						(*pMax) = tempMax;
					else
						D3DXVec3Maximize( pMax, pMax, &tempMax );
				}
			}
		}
	}


	return	hr;
}
