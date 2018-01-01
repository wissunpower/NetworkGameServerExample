
#include	"CAllocateHierarchy.h"



bool			g_bUseSoftwareVP;
METHOD			g_SkinningMethod = D3DNONINDEXED;
D3DXMATRIX *	g_pBoneMatrices = NULL;
UINT			g_NumBoneMatricesMax = 0;



HRESULT AllocateName( LPCSTR Name, LPSTR * pNewName )
{
	UINT	cbLength;

	if( Name != NULL )
	{
		cbLength = (UINT)strlen( Name ) + 1;
		*pNewName = new CHAR[ cbLength ];
		if( *pNewName == NULL )
			return E_OUTOFMEMORY;
		memcpy( *pNewName, Name, cbLength * sizeof( CHAR ) );
	}
	else
	{
		*pNewName = NULL;
	}

	return S_OK;
}


HRESULT GenerateSkinnedMesh( IDirect3DDevice9 * pd3dDevice, D3DXMESHCONTAINER_DERIVED * pMeshContainer )
{
	HRESULT		hr = S_OK;
	D3DCAPS9	d3dCaps;
	pd3dDevice->GetDeviceCaps( &d3dCaps );

	if( pMeshContainer->pSkinInfo == NULL )
	{
		return	hr;
	}

	g_bUseSoftwareVP = false;

	SAFE_RELEASE( pMeshContainer->MeshData.pMesh );
	SAFE_RELEASE( pMeshContainer->pBoneCombinationBuf );


	if( g_SkinningMethod == D3DNONINDEXED )
	{
		hr = pMeshContainer->pSkinInfo->ConvertToBlendedMesh( pMeshContainer->pOrigMesh, D3DXMESH_MANAGED | D3DXMESHOPT_VERTEXCACHE,
																pMeshContainer->pAdjacency, NULL, NULL, NULL,
																&pMeshContainer->NumInfl, &pMeshContainer->NumAttributeGroups,
																&pMeshContainer->pBoneCombinationBuf, &pMeshContainer->MeshData.pMesh );
		if( FAILED( hr ) )
		{
			return	hr;
		}

		LPD3DXBONECOMBINATION rgBoneCombinations =
			reinterpret_cast<LPD3DXBONECOMBINATION>( pMeshContainer->pBoneCombinationBuf->GetBufferPointer() );

		for( pMeshContainer->iAttributeSW = 0 ; pMeshContainer->iAttributeSW < pMeshContainer->NumAttributeGroups ; ++pMeshContainer->iAttributeSW )
		{
			DWORD	cInfl = 0;

			for( DWORD iInfl = 0 ; iInfl < pMeshContainer->NumInfl ; ++iInfl )
			{
				if( rgBoneCombinations[ pMeshContainer->iAttributeSW ].BoneId[ iInfl ] != UINT_MAX )
				{
					++cInfl;
				}
			}

			if( cInfl > d3dCaps.MaxVertexBlendMatrices )
			{
				break;
			}
		}

		if( pMeshContainer->iAttributeSW < pMeshContainer->NumAttributeGroups )
		{
			LPD3DXMESH	pMeshTemp;

			hr = pMeshContainer->MeshData.pMesh->CloneMeshFVF( D3DXMESH_SOFTWAREPROCESSING | pMeshContainer->MeshData.pMesh->GetOptions(),
																pMeshContainer->MeshData.pMesh->GetFVF(), pd3dDevice, &pMeshTemp );
			if( FAILED( hr ) )
			{
				return	hr;
			}

			pMeshContainer->MeshData.pMesh->Release();
			pMeshContainer->MeshData.pMesh = pMeshTemp;
			pMeshTemp = NULL;
		}
	}
	else if( g_SkinningMethod == D3DINDEXED )
	{
		DWORD	NumMaxFaceInfl;
		DWORD	Flags = D3DXMESHOPT_VERTEXCACHE;

		LPDIRECT3DINDEXBUFFER9	pIB;
		hr = pMeshContainer->pOrigMesh->GetIndexBuffer( &pIB );
		if( FAILED( hr ) )
		{
			return	hr;
		}

		hr = pMeshContainer->pSkinInfo->GetMaxFaceInfluences( pIB, pMeshContainer->pOrigMesh->GetNumFaces(), &NumMaxFaceInfl );
		pIB->Release();
		if( FAILED( hr ) )
		{
			return	hr;
		}

		NumMaxFaceInfl = min( NumMaxFaceInfl, 12 );

		if( d3dCaps.MaxVertexBlendMatrixIndex + 1 < NumMaxFaceInfl )
		{
			pMeshContainer->NumPaletteEntries = min( 256, pMeshContainer->pSkinInfo->GetNumBones() );
			pMeshContainer->UseSoftwareVP = true;
			g_bUseSoftwareVP = true;
			Flags |= D3DXMESH_SYSTEMMEM;
		}
		else
		{
			pMeshContainer->NumPaletteEntries = min( (d3dCaps.MaxVertexBlendMatrixIndex + 1) / 2, pMeshContainer->pSkinInfo->GetNumBones() );
			pMeshContainer->UseSoftwareVP = false;
			Flags |= D3DXMESH_MANAGED;
		}

		hr = pMeshContainer->pSkinInfo->ConvertToIndexedBlendedMesh( pMeshContainer->pOrigMesh, Flags, pMeshContainer->NumPaletteEntries,
																		pMeshContainer->pAdjacency, NULL, NULL, NULL,
																		&pMeshContainer->NumInfl, &pMeshContainer->NumAttributeGroups,
																		&pMeshContainer->pBoneCombinationBuf, &pMeshContainer->MeshData.pMesh );
		if( FAILED( hr ) )
		{
			return	hr;
		}
	}
	else if( g_SkinningMethod == SOFTWARE )
	{
		hr = pMeshContainer->pOrigMesh->CloneMeshFVF( D3DXMESH_MANAGED, pMeshContainer->pOrigMesh->GetFVF(), pd3dDevice,
														&pMeshContainer->MeshData.pMesh );
		if( FAILED( hr ) )
		{
			return	hr;
		}

		hr = pMeshContainer->MeshData.pMesh->GetAttributeTable( NULL, &pMeshContainer->NumAttributeGroups );
		if( FAILED( hr ) )
		{
			return	hr;
		}

		delete	[]	pMeshContainer->pAttributeTable;
		pMeshContainer->pAttributeTable = new D3DXATTRIBUTERANGE[ pMeshContainer->NumAttributeGroups ];
		if( pMeshContainer->pAttributeTable == NULL )
		{
			return	(hr = E_OUTOFMEMORY);
		}

		hr = pMeshContainer->MeshData.pMesh->GetAttributeTable( pMeshContainer->pAttributeTable, NULL );
		if( FAILED( hr ) )
		{
			return	hr;
		}

		if( g_NumBoneMatricesMax < pMeshContainer->pSkinInfo->GetNumBones() )
		{
			g_NumBoneMatricesMax = pMeshContainer->pSkinInfo->GetNumBones();

			delete	[]	g_pBoneMatrices;
			g_pBoneMatrices = new D3DXMATRIX[ g_NumBoneMatricesMax ];
			if( g_pBoneMatrices == NULL )
			{
				return	(hr = E_OUTOFMEMORY);
			}
		}
	}
	else
	{
		hr = E_INVALIDARG;
	}


	return	hr;
}


void UpdateSkinningMethod( LPD3DXFRAME pFrameBase )
{
	D3DXFRAME_DERIVED *	pFrame = (D3DXFRAME_DERIVED*)pFrameBase;
	D3DXMESHCONTAINER_DERIVED *	pMeshContainer;

	pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pFrame->pMeshContainer;

	while( pMeshContainer != NULL )
	{
		GenerateSkinnedMesh( DXUTGetD3D9Device(), pMeshContainer );

		pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainer->pNextMeshContainer;
	}

	if( pFrame->pFrameSibling != NULL )
	{
		UpdateSkinningMethod( pFrame->pFrameSibling );
	}

	if( pFrame->pFrameFirstChild != NULL )
	{
		UpdateSkinningMethod( pFrame->pFrameFirstChild );
	}
}


HRESULT CAllocateHierarchy::CreateFrame( LPCSTR Name, LPD3DXFRAME * ppNewFrame )
{
	HRESULT		hr = S_OK;
	D3DXFRAME_DERIVED *	pFrame;

	*ppNewFrame = NULL;

	pFrame = new D3DXFRAME_DERIVED;
	if( pFrame == NULL )
	{
		return	(hr = E_OUTOFMEMORY);
	}

	hr = AllocateName( Name, &pFrame->Name );
	if( FAILED( hr ) )
	{
		delete	pFrame;
		return	hr;
	}

	D3DXMatrixIdentity( &pFrame->TransformationMatrix );
	D3DXMatrixIdentity( &pFrame->CombinedTransformationMatrix );

	pFrame->pMeshContainer = NULL;
	pFrame->pFrameSibling = NULL;
	pFrame->pFrameFirstChild = NULL;

	*ppNewFrame = pFrame;
	pFrame = NULL;


	return	hr;
}


HRESULT CAllocateHierarchy::CreateMeshContainer( LPCSTR Name,
												CONST D3DXMESHDATA * pMeshData,
												CONST D3DXMATERIAL * pMaterials,
												CONST D3DXEFFECTINSTANCE * pEffectInstances,
												DWORD NumMaterials,
												CONST DWORD * pAdjacency,
												LPD3DXSKININFO pSkinInfo,
												LPD3DXMESHCONTAINER * ppNewMeshContainer )
{
	HRESULT		hr;
	D3DXMESHCONTAINER_DERIVED *	pMeshContainer = NULL;
	UINT		NumFaces;
	UINT		iMaterial;
	UINT		iBone,	cBones;
	LPDIRECT3DDEVICE9	pd3dDevice = NULL;

	LPD3DXMESH	pMesh = NULL;


	*ppNewMeshContainer = NULL;

	if( pMeshData->Type != D3DXMESHTYPE_MESH )
	{
		return	(hr = E_FAIL);
	}

	pMesh = pMeshData->pMesh;

	if( pMesh->GetFVF() == 0 )
	{
		return	(hr = E_FAIL);
	}

	pMeshContainer = new D3DXMESHCONTAINER_DERIVED;
	if( pMeshContainer == NULL )
	{
		return	(hr = E_OUTOFMEMORY);
	}
	memset( pMeshContainer, 0, sizeof( D3DXMESHCONTAINER_DERIVED ) );

	hr = AllocateName( Name, &pMeshContainer->Name );
	if( FAILED( hr ) )
	{
		if( pMeshContainer != NULL )
		{
			DestroyMeshContainer( pMeshContainer );
		}
		return	hr;
	}

	pMesh->GetDevice( &pd3dDevice );
	NumFaces = pMesh->GetNumFaces();

	if( !( pMesh->GetFVF() & D3DFVF_NORMAL ) )
	{
		pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;
		hr = pMesh->CloneMeshFVF( pMesh->GetOptions(), pMesh->GetFVF() | D3DFVF_NORMAL, pd3dDevice, &pMeshContainer->MeshData.pMesh );
		if( FAILED( hr ) )
		{
			SAFE_RELEASE( pd3dDevice );
			if( pMeshContainer != NULL )
			{
				DestroyMeshContainer( pMeshContainer );
			}
			return	hr;
		}

		pMesh = pMeshContainer->MeshData.pMesh;

		D3DXComputeNormals( pMesh, NULL );
	}
	else
	{
		pMeshContainer->MeshData.pMesh = pMesh;
		pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

		pMesh->AddRef();
	}


	pMeshContainer->NumMaterials = max( 1, NumMaterials );
	pMeshContainer->pMaterials = new D3DXMATERIAL[ pMeshContainer->NumMaterials ];
	pMeshContainer->ppTextures = new LPDIRECT3DTEXTURE9[ pMeshContainer->NumMaterials ];
	pMeshContainer->pAdjacency = new DWORD[ NumFaces * 3 ];
	if( (pMeshContainer->pAdjacency == NULL) || (pMeshContainer->pMaterials == NULL) )
	{
		SAFE_RELEASE( pd3dDevice );
		if( pMeshContainer != NULL )
		{
			DestroyMeshContainer( pMeshContainer );
		}
		return	(hr = E_OUTOFMEMORY);
	}

	memcpy( pMeshContainer->pAdjacency, pAdjacency, sizeof( DWORD ) * NumFaces * 3 );
	memset( pMeshContainer->ppTextures, 0, sizeof( LPDIRECT3DTEXTURE9 ) * pMeshContainer->NumMaterials );

	if( NumMaterials > 0 )
	{
		memcpy( pMeshContainer->pMaterials, pMaterials, sizeof( D3DXMATERIAL ) * NumMaterials );

		for( iMaterial = 0 ; iMaterial < NumMaterials ; ++iMaterial )
		{
			if( pMeshContainer->pMaterials[ iMaterial ].pTextureFilename != NULL )
			{
				TCHAR	strTexturePath[ MAX_PATH ];
				TCHAR	tszBuf[ MAX_PATH ];

#if defined(UNICODE) | defined(_UNICODE)
				MultiByteToWideChar( CP_ACP, 0, pMeshContainer->pMaterials[ iMaterial ].pTextureFilename, -1, tszBuf, MAX_PATH );
#else
				_tcsncpy( tszBuf, pMeshContainer->pMaterials[ iMaterial ].pTextureFilename, MAX_PATH );
#endif
				tszBuf[ MAX_PATH - 1 ] = TEXT( '\0' );
				DXUTFindDXSDKMediaFileCch( strTexturePath, MAX_PATH, tszBuf );
				if( FAILED(
					D3DXCreateTextureFromFile( pd3dDevice, strTexturePath, &pMeshContainer->ppTextures[ iMaterial ] )
					) )
				{
					pMeshContainer->ppTextures[ iMaterial ] = NULL;
				}
				pMeshContainer->pMaterials[ iMaterial ].pTextureFilename = NULL;
			}
		}
	}
	else
	{
		pMeshContainer->pMaterials[ 0 ].pTextureFilename = NULL;
		memset( &pMeshContainer->pMaterials[ 0 ].MatD3D, 0, sizeof( D3DMATERIAL9 ) );
		pMeshContainer->pMaterials[ 0 ].MatD3D.Diffuse.r = 0.5f;
		pMeshContainer->pMaterials[ 0 ].MatD3D.Diffuse.g = 0.5f;
		pMeshContainer->pMaterials[ 0 ].MatD3D.Diffuse.b = 0.5f;
		pMeshContainer->pMaterials[ 0 ].MatD3D.Specular = pMeshContainer->pMaterials[ 0 ].MatD3D.Diffuse;
	}

	if( pSkinInfo != NULL )
	{
		pMeshContainer->pSkinInfo = pSkinInfo;
		pSkinInfo->AddRef();

		pMeshContainer->pOrigMesh = pMesh;
		pMesh->AddRef();

		cBones = pSkinInfo->GetNumBones();
		pMeshContainer->pBoneOffsetMatrices = new D3DXMATRIX[ cBones ];
		if( pMeshContainer->pBoneOffsetMatrices == NULL )
		{
			SAFE_RELEASE( pd3dDevice );
			if( pMeshContainer != NULL )
			{
				DestroyMeshContainer( pMeshContainer );
			}
			return	(hr = E_OUTOFMEMORY);
		}

		for( iBone = 0 ; iBone < cBones ; ++iBone )
		{
			pMeshContainer->pBoneOffsetMatrices[ iBone ] = *( pMeshContainer->pSkinInfo->GetBoneOffsetMatrix( iBone ) );
		}

		hr = GenerateSkinnedMesh( pd3dDevice, pMeshContainer );
		if( FAILED( hr ) )
		{
			SAFE_RELEASE( pd3dDevice );
			if( pMeshContainer != NULL )
			{
				DestroyMeshContainer( pMeshContainer );
			}
			return	hr;
		}
	}

	*ppNewMeshContainer = pMeshContainer;
	pMeshContainer = NULL;


	SAFE_RELEASE( pd3dDevice );
	if( pMeshContainer != NULL )
	{
		DestroyMeshContainer( pMeshContainer );
	}


	return	hr;
}


HRESULT CAllocateHierarchy::DestroyFrame( LPD3DXFRAME pFrameToFree )
{
	SAFE_DELETE_ARRAY( pFrameToFree->Name );
	SAFE_DELETE( pFrameToFree );
	return S_OK;
}


HRESULT CAllocateHierarchy::DestroyMeshContainer( LPD3DXMESHCONTAINER pMeshContainerBase )
{
	UINT	iMaterial;
	D3DXMESHCONTAINER_DERIVED *	pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;

	SAFE_DELETE_ARRAY( pMeshContainer->Name );
	SAFE_DELETE_ARRAY( pMeshContainer->pAdjacency );
	SAFE_DELETE_ARRAY( pMeshContainer->pMaterials );
	SAFE_DELETE_ARRAY( pMeshContainer->pBoneOffsetMatrices );

	if( pMeshContainer->ppTextures != NULL )
	{
		for( iMaterial = 0 ; iMaterial < pMeshContainer->NumMaterials ; ++iMaterial )
		{
			SAFE_RELEASE( pMeshContainer->ppTextures[ iMaterial ] );
		}
	}

	SAFE_DELETE_ARRAY( pMeshContainer->pAttributeTable );

	SAFE_DELETE_ARRAY( pMeshContainer->ppTextures );
	SAFE_DELETE_ARRAY( pMeshContainer->ppBoneMatrixPtrs );
	SAFE_RELEASE( pMeshContainer->pBoneCombinationBuf );
	SAFE_RELEASE( pMeshContainer->MeshData.pMesh );
	SAFE_RELEASE( pMeshContainer->pSkinInfo );
	SAFE_RELEASE( pMeshContainer->pOrigMesh );
	SAFE_DELETE( pMeshContainer );

	return S_OK;
}
