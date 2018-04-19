
#include	"stdafx.h"
#include	"CEffectBillboard.h"



#define		SIZE_UNIT		128



CEffectBillboard::CEffectBillboard( void ) : m_IsBuilded( false )
{
	D3DXMatrixIdentity( &m_TransformMatrix );
}


CEffectBillboard::~CEffectBillboard( void )
{
	Release();
}


CEffectBillboard::CEffectBillboard( const CEffectBillboard & src )
{
}


CEffectBillboard & CEffectBillboard::operator=( const CEffectBillboard & src )
{
	if( this != (&src) )
	{
	}

	return	(*this);
}


bool CEffectBillboard::Release( void )
{
	if( !m_IsBuilded )
	{
		return	true;
	}

	SAFE_RELEASE( m_pd3dxMesh );

	for( std::list< IDirect3DTexture9* >::iterator tIter = m_TexList.begin() ; tIter != m_TexList.end() ; ++tIter )
	{
		SAFE_RELEASE( (*tIter) );
	}
	m_TexList.clear();

	m_IsBuilded = false;

	return	true;
}


HRESULT CEffectBillboard::Build(  IDirect3DDevice9 * apDevice,  LPCTSTR aSetName,  DWORD aMeshOptions,  D3DPOOL aPoolType,
									int aSize,  int aStartFrame,  int aFrameNum  )
{
	if( m_IsBuilded )
	{
		return	S_OK;
	}

	if( 1 > aSize )
	{
		aSize = 1;
	}
	else if( 8 < aSize )
	{
		aSize = 8;
	}


	HRESULT		hr = S_OK;
	int			panelWidth = SIZE_UNIT * aSize;
	int			panelHeight = SIZE_UNIT * aSize;
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
    *pVertices++ = STexturedVertex( -(float)panelWidth, +(float)panelHeight, 0.0f, 0.0f, 0.0f);
    *pVertices++ = STexturedVertex( +(float)panelWidth, +(float)panelHeight, 0.0f, 1.0f, 0.0f);
    *pVertices++ = STexturedVertex( +(float)panelWidth, -(float)panelHeight, 0.0f, 1.0f, 1.0f);
    *pVertices++ = STexturedVertex( -(float)panelWidth, -(float)panelHeight, 0.0f, 0.0f, 1.0f);
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


	tstring			sPath;
	tstring			sName;

	for( int i = aStartFrame ; i < (aFrameNum + aStartFrame) ; ++i )
	{
		IDirect3DTexture9 *		tempTex;

		tstringstream	tss;
		tss << aSetName;
		tss.width( 4 );
		tss.fill( _T( '0' ) );
		tss << i << _T( ".tga" );
		sPath = tss.str();
		RetrieveResourceFile( sPath, sPath, sName );
		GetCurrentDirectory( MAX_PATH, tempCD );
		SetCurrentDirectory( sPath.c_str() );

		hr = D3DXCreateTextureFromFileEx(  apDevice,  sName.c_str(),  D3DX_DEFAULT,  D3DX_DEFAULT,  0,  0,  D3DFMT_UNKNOWN,  aPoolType,
											D3DX_DEFAULT,  D3DX_DEFAULT,  0xFF000000,  NULL,  NULL,  &tempTex  );

		//D3DXCreateTextureFromFile( apDevice, tempName, &tempTex );

		m_TexList.push_back( tempTex );

		SetCurrentDirectory( tempCD );
	}

	m_CurrentTexIter = m_TexList.begin();


	m_IsBuilded = true;


	return	hr;
}


HRESULT CEffectBillboard::UpdatePerFrame( DOUBLE aTimeDelta, bool abUpdate )
{
	HRESULT		hr = S_OK;


	if( abUpdate )
	{
		++m_CurrentTexIter;

		if( m_TexList.end() == m_CurrentTexIter )
		{
			m_CurrentTexIter = m_TexList.begin();
		}
	}

	return	hr;
}


HRESULT CEffectBillboard::UpdatePerFrame( const unsigned int aIndex )
{
	HRESULT		hr = S_OK;


	if( m_TexList.size() > aIndex )
	{
		int i = 0;
		for( std::list< IDirect3DTexture9* >::iterator tIter = m_TexList.begin()  ;
			tIter != m_TexList.end()  &&  i < m_TexList.size()  ;
			++tIter, ++i )
		{
			if( aIndex == i )
			{
				m_CurrentTexIter = tIter;
				break;
			}
		}
	}

	return	hr;
}


HRESULT CEffectBillboard::Render( IDirect3DDevice9 * apDevice, const D3DXVECTOR3 & aRight, const D3DXVECTOR3 & aUp, const D3DXVECTOR3 & aPos )
{
	if( NULL == apDevice )
	{
		return	S_FALSE;
	}


	HRESULT		hr = S_OK;
	D3DXVECTOR3		tempLookAt;
	static int		tempCount = 0;


	D3DXVec3Cross( &tempLookAt, &aRight, &aUp );

	m_TransformMatrix._11 = aRight.x;		m_TransformMatrix._12 = aRight.y;		m_TransformMatrix._13 = aRight.z;
	m_TransformMatrix._21 = aUp.x;			m_TransformMatrix._22 = aUp.y;			m_TransformMatrix._23 = aUp.z;
	m_TransformMatrix._31 = tempLookAt.x;	m_TransformMatrix._32 = tempLookAt.y;	m_TransformMatrix._33 = tempLookAt.z;
	m_TransformMatrix._41 = aPos.x;
	m_TransformMatrix._42 = aPos.y;
	m_TransformMatrix._43 = aPos.z;
	apDevice->SetTransform( D3DTS_WORLD, &m_TransformMatrix );

	apDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
    apDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );

	apDevice->SetRenderState( D3DRS_LIGHTING, false );
	apDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, true );
    apDevice->SetRenderState( D3DRS_ZWRITEENABLE, false );
	apDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
	apDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

	if( m_TexList.end() != m_CurrentTexIter )
	{
		hr = apDevice->SetTexture( 0, (*m_CurrentTexIter) );
	}
	if( m_pd3dxMesh )
	{
		hr = m_pd3dxMesh->DrawSubset( 0 );
	}

    apDevice->SetRenderState( D3DRS_LIGHTING, true );
	apDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, false );
	apDevice->SetRenderState( D3DRS_ZWRITEENABLE, true );

	return	hr;
}
