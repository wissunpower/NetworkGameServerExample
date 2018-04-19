
#include	"stdafx.h"
#include	"CBackground.h"



CBackground::CBackground( void ) : m_IsBuilded( false ), m_pd3dxMesh( NULL ), m_pTextures( NULL )
{
	D3DXMatrixIdentity( &m_TransformMatrix );
}


CBackground::~CBackground( void )
{
	Release();
}


bool CBackground::Release( void )
{
	if( !m_IsBuilded )
	{
		return	true;
	}

	SAFE_RELEASE( m_pd3dxMesh );

	SAFE_RELEASE( m_pTextures );

	m_IsBuilded = false;

	return	true;
}


HRESULT CBackground::Build( IDirect3DDevice9 * apDevice, LPCTSTR aFileName, DWORD aMeshOptions )
{
	if( m_IsBuilded )
	{
		return	S_OK;
	}


	HRESULT		hr;
	TCHAR		tempCD[ MAX_PATH ];


	if( FAILED( hr = D3DXCreateMeshFVF( 2, 4, aMeshOptions, D3DFVF_XYZ | D3DFVF_TEX1, apDevice, &m_pd3dxMesh ) ) )
	{
		return	hr;
	}


	STexturedVertex *	pVertices = NULL;
	if( FAILED( hr = m_pd3dxMesh->LockVertexBuffer( 0, (LPVOID*)&pVertices ) ) )
	{
		return	hr;
	}
    // Front quad (remember all quads point inward)
    *pVertices++ = STexturedVertex(-497.0f, +374.0f, 0.0f, 0.0f, 0.0f);
    *pVertices++ = STexturedVertex(+497.0f, +374.0f, 0.0f, 1.0f, 0.0f);
    *pVertices++ = STexturedVertex(+497.0f, -374.0f, 0.0f, 1.0f, 1.0f);
    *pVertices++ = STexturedVertex(-497.0f, -374.0f, 0.0f, 0.0f, 1.0f);
    if( FAILED( hr = m_pd3dxMesh->UnlockVertexBuffer() ) )
	{
		return	hr;
	}


	USHORT *	pIndices = NULL;
	if( FAILED( hr = m_pd3dxMesh->LockIndexBuffer( 0, (LPVOID*)&pIndices ) ) )
	{
		return	hr;
	}
    *pIndices++ = 0; *pIndices++ = 1; *pIndices++ = 3;
    *pIndices++ = 1; *pIndices++ = 2; *pIndices++ = 3;
    if( FAILED( hr = m_pd3dxMesh->UnlockIndexBuffer() ) )
	{
		return	hr;
	}


	DWORD *		pdwAttributes = NULL;
	if( FAILED( hr = m_pd3dxMesh->LockAttributeBuffer( 0, &pdwAttributes ) ) )
	{
		return	hr;
	}
	pdwAttributes[ 0 ] = pdwAttributes[ 1 ] = 0;
	if( FAILED( hr = m_pd3dxMesh->UnlockAttributeBuffer() ) )
	{
		return	hr;
	}


	ZeroMemory( &m_Materials, sizeof( D3DMATERIAL9 ) );
	m_Materials.Diffuse = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );
	m_Materials.Ambient = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );


	tstring		sPath;
	tstring		sName;
	RetrieveResourceFile( aFileName, sPath, sName );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( sPath.c_str() );
	hr = D3DXCreateTextureFromFileEx(  apDevice,  sName.c_str(),  D3DX_DEFAULT,  D3DX_DEFAULT,  0,  0,  D3DFMT_UNKNOWN,  D3DPOOL_MANAGED,
										D3DX_DEFAULT,  D3DX_DEFAULT,  0xFF000000,  NULL,  NULL,  &m_pTextures  );
	SetCurrentDirectory( tempCD );


	m_IsBuilded = true;


	return	hr;
}


HRESULT CBackground::Render( IDirect3DDevice9 * apDevice, const D3DXVECTOR3 & aPosition )
{
	if( NULL == apDevice )
	{
		return	S_FALSE;
	}


	HRESULT		hr = S_OK;

    //D3DXMatrixTranslation( &m_TransformMatrix, aPosition.x, aPosition.y, aPosition.z );
    apDevice->SetTransform(D3DTS_WORLD, &m_TransformMatrix);
    apDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    apDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    apDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    apDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    apDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
    apDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
    apDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
    apDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

	hr = apDevice->SetMaterial( &m_Materials );
	hr = apDevice->SetTexture( 0, m_pTextures );
	if( m_pd3dxMesh )
	{
		hr =m_pd3dxMesh->DrawSubset( 0 );
	}

	apDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
    apDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
    apDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    apDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	return	hr;
}
