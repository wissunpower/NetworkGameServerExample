
#include	"stdafx.h"
#include	"CSkyBox.h"



CSkyBox::CSkyBox( void ) : m_IsBuilded( false ), m_pd3dxMesh( NULL ), m_nAttributes( 0 )
{
	for( int i = 0 ; i < 6 ; ++i )
	{
		m_pTextures[ i ] = NULL;
	}
}


CSkyBox::~CSkyBox( void )
{
	Release();
}


bool CSkyBox::Release( void )
{
	if( !m_IsBuilded )
	{
		return	true;
	}

	if( m_pd3dxMesh )
		m_pd3dxMesh->Release();

	for( DWORD i = 0 ; i < m_nAttributes ; ++i )
	{
		if( m_pTextures[ i ] )
			m_pTextures[ i ]->Release();
	}

	m_IsBuilded = false;

	return	true;
}


HRESULT CSkyBox::Build( LPDIRECT3DDEVICE9 apDevice, LPCTSTR aSetName, DWORD aMeshOptions )
{
	if( m_IsBuilded )
	{
		return	S_OK;
	}


	HRESULT		hr;
	tstring		tempStr;
	TCHAR		tempCD[ MAX_PATH ];
	TCHAR		tempPath[ MAX_PATH ];
	TCHAR		tempName[ MAX_PATH / 4 ];
	float		tempUnit = 8192.0f;


	if( FAILED( hr = D3DXCreateMeshFVF( 6 * 2, 4 * 6, aMeshOptions, D3DFVF_XYZ | D3DFVF_TEX1, apDevice, &m_pd3dxMesh ) ) )
	{
		return	hr;
	}


	STexturedVertex *	pVertices = NULL;
	if( FAILED( hr = m_pd3dxMesh->LockVertexBuffer( 0, (LPVOID*)&pVertices ) ) )
	{
		return	hr;
	}
    // Front quad (remember all quads point inward)
    *pVertices++ = STexturedVertex( -tempUnit, +tempUnit, +tempUnit, 0.0f, 0.0f );
    *pVertices++ = STexturedVertex( +tempUnit, +tempUnit, +tempUnit, 1.0f, 0.0f );
    *pVertices++ = STexturedVertex( +tempUnit, -tempUnit, +tempUnit, 1.0f, 1.0f );
    *pVertices++ = STexturedVertex( -tempUnit, -tempUnit, +tempUnit, 0.0f, 1.0f );
    // Back Quad
    *pVertices++ = STexturedVertex( +tempUnit, +tempUnit, -tempUnit, 0.0f, 0.0f );
    *pVertices++ = STexturedVertex( -tempUnit, +tempUnit, -tempUnit, 1.0f, 0.0f );
    *pVertices++ = STexturedVertex( -tempUnit, -tempUnit, -tempUnit, 1.0f, 1.0f );
    *pVertices++ = STexturedVertex( +tempUnit, -tempUnit, -tempUnit, 0.0f, 1.0f );
    // Left Quad
    *pVertices++ = STexturedVertex( -tempUnit, +tempUnit, -tempUnit, 0.0f, 0.0f );
    *pVertices++ = STexturedVertex( -tempUnit, +tempUnit, +tempUnit, 1.0f, 0.0f );
    *pVertices++ = STexturedVertex( -tempUnit, -tempUnit, +tempUnit, 1.0f, 1.0f );
    *pVertices++ = STexturedVertex( -tempUnit, -tempUnit, -tempUnit, 0.0f, 1.0f );
    // Right Quad
    *pVertices++ = STexturedVertex( +tempUnit, +tempUnit, +tempUnit, 0.0f, 0.0f );
    *pVertices++ = STexturedVertex( +tempUnit, +tempUnit, -tempUnit, 1.0f, 0.0f );
    *pVertices++ = STexturedVertex( +tempUnit, -tempUnit, -tempUnit, 1.0f, 1.0f );
    *pVertices++ = STexturedVertex( +tempUnit, -tempUnit, +tempUnit, 0.0f, 1.0f );
    // Top Quad
    *pVertices++ = STexturedVertex( -tempUnit, +tempUnit, -tempUnit, 0.0f, 0.0f );
    *pVertices++ = STexturedVertex( +tempUnit, +tempUnit, -tempUnit, 1.0f, 0.0f );
    *pVertices++ = STexturedVertex( +tempUnit, +tempUnit, +tempUnit, 1.0f, 1.0f );
    *pVertices++ = STexturedVertex( -tempUnit, +tempUnit, +tempUnit, 0.0f, 1.0f );
    // Bottom Quad
    *pVertices++ = STexturedVertex( -tempUnit, -tempUnit, +tempUnit, 0.0f, 0.0f );
    *pVertices++ = STexturedVertex( +tempUnit, -tempUnit, +tempUnit, 1.0f, 0.0f );
    *pVertices++ = STexturedVertex( +tempUnit, -tempUnit, -tempUnit, 1.0f, 1.0f );
    *pVertices++ = STexturedVertex( -tempUnit, -tempUnit, -tempUnit, 0.0f, 1.0f );
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
    *pIndices++ = 4; *pIndices++ = 5; *pIndices++ = 7;
    *pIndices++ = 5; *pIndices++ = 6; *pIndices++ = 7;
    *pIndices++ = 8; *pIndices++ = 9; *pIndices++ = 11;
    *pIndices++ = 9; *pIndices++ = 10; *pIndices++ = 11;
    *pIndices++ = 12; *pIndices++ = 13; *pIndices++ = 15;
    *pIndices++ = 13; *pIndices++ = 14; *pIndices++ = 15;
    *pIndices++ = 16; *pIndices++ = 17; *pIndices++ = 19;
    *pIndices++ = 17; *pIndices++ = 18; *pIndices++ = 19;
    *pIndices++ = 20; *pIndices++ = 21; *pIndices++ = 23;
    *pIndices++ = 21; *pIndices++ = 22; *pIndices++ = 23;
    if( FAILED( hr = m_pd3dxMesh->UnlockIndexBuffer() ) )
	{
		return	hr;
	}


	m_nAttributes = 6;
	DWORD *		pdwAttributes = NULL;
	if( FAILED( hr = m_pd3dxMesh->LockAttributeBuffer( 0, &pdwAttributes ) ) )
	{
		return	hr;
	}
	for( int i = 0 ; i < 6 ; ++i )
	{
		pdwAttributes[ i * 2 ] = pdwAttributes[ i * 2 + 1 ] = i;
	}
    if( FAILED( hr = m_pd3dxMesh->UnlockAttributeBuffer() ) )
	{
		return	hr;
	}


	for( int i = 0 ; i < 6 ; ++i )
	{
		ZeroMemory( &m_Materials[ i ], sizeof( D3DMATERIAL9 ) );
		m_Materials[ i ].Diffuse = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );
		m_Materials[ i ].Ambient = D3DXCOLOR( 1.0f, 1.0f, 1.0f, 1.0f );
	}


	tempStr = aSetName;
	tempStr += TEXT( "Front.tga" );
	RetrieveResourceFile( tempStr.c_str(), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	hr = D3DXCreateTextureFromFileEx(  apDevice,  tempName,  D3DX_DEFAULT,  D3DX_DEFAULT,  0,  0,  D3DFMT_UNKNOWN,  D3DPOOL_MANAGED,
										D3DX_DEFAULT,  D3DX_DEFAULT,  0xFF000000,  NULL,  NULL,  &m_pTextures[ 0 ]  );
	SetCurrentDirectory( tempCD );

	tempStr = aSetName;
	tempStr += TEXT( "Back.tga" );
	RetrieveResourceFile( tempStr.c_str(), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	hr = D3DXCreateTextureFromFileEx(  apDevice,  tempName,  D3DX_DEFAULT,  D3DX_DEFAULT,  0,  0,  D3DFMT_UNKNOWN,  D3DPOOL_MANAGED,
										D3DX_DEFAULT,  D3DX_DEFAULT,  0xFF000000,  NULL,  NULL,  &m_pTextures[ 1 ]  );
	SetCurrentDirectory( tempCD );

	tempStr = aSetName;
	tempStr += TEXT( "Left.tga" );
	RetrieveResourceFile( tempStr.c_str(), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	hr = D3DXCreateTextureFromFileEx(  apDevice,  tempName,  D3DX_DEFAULT,  D3DX_DEFAULT,  0,  0,  D3DFMT_UNKNOWN,  D3DPOOL_MANAGED,
										D3DX_DEFAULT,  D3DX_DEFAULT,  0xFF000000,  NULL,  NULL,  &m_pTextures[ 2 ]  );
	SetCurrentDirectory( tempCD );

	tempStr = aSetName;
	tempStr += TEXT( "Right.tga" );
	RetrieveResourceFile( tempStr.c_str(), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	hr = D3DXCreateTextureFromFileEx(  apDevice,  tempName,  D3DX_DEFAULT,  D3DX_DEFAULT,  0,  0,  D3DFMT_UNKNOWN,  D3DPOOL_MANAGED,
										D3DX_DEFAULT,  D3DX_DEFAULT,  0xFF000000,  NULL,  NULL,  &m_pTextures[ 3 ]  );
	SetCurrentDirectory( tempCD );

	tempStr = aSetName;
	tempStr += TEXT( "Top.tga" );
	RetrieveResourceFile( tempStr.c_str(), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	hr = D3DXCreateTextureFromFileEx(  apDevice,  tempName,  D3DX_DEFAULT,  D3DX_DEFAULT,  0,  0,  D3DFMT_UNKNOWN,  D3DPOOL_MANAGED,
										D3DX_DEFAULT,  D3DX_DEFAULT,  0xFF000000,  NULL,  NULL,  &m_pTextures[ 4 ]  );
	SetCurrentDirectory( tempCD );

	tempStr = aSetName;
	tempStr += TEXT( "Bottom.tga" );
	RetrieveResourceFile( tempStr.c_str(), tempPath, MAX_PATH, tempName, MAX_PATH / 4 );
	GetCurrentDirectory( MAX_PATH, tempCD );
	SetCurrentDirectory( tempPath );
	hr = D3DXCreateTextureFromFileEx(  apDevice,  tempName,  D3DX_DEFAULT,  D3DX_DEFAULT,  0,  0,  D3DFMT_UNKNOWN,  D3DPOOL_MANAGED,
										D3DX_DEFAULT,  D3DX_DEFAULT,  0xFF000000,  NULL,  NULL,  &m_pTextures[ 5 ]  );
	SetCurrentDirectory( tempCD );


	m_IsBuilded = true;


	return	hr;
}


HRESULT CSkyBox::Render( LPDIRECT3DDEVICE9 apDevice, const D3DXVECTOR3 & aPosition )
{
	if( NULL == apDevice )
	{
		return	S_FALSE;
	}


	HRESULT		hr = S_OK;

    D3DXMATRIX mtxWorld;
    D3DXMatrixTranslation(&mtxWorld, aPosition.x, aPosition.y, aPosition.z);
    apDevice->SetTransform(D3DTS_WORLD, &mtxWorld);
    apDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
    apDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
    apDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    apDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
    apDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);
    apDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
    apDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
    apDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);

	for( DWORD i = 0 ; i < 6 ; ++i )
	{
		hr = apDevice->SetMaterial( &m_Materials[ i ] );
		hr =apDevice->SetTexture( 0, m_pTextures[ i ] );
		if( m_pd3dxMesh )
		{
			hr =m_pd3dxMesh->DrawSubset( i );
		}
	}

	apDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
    apDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
    apDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    apDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	return	hr;
}
