
#ifndef		__CSKYBOX_H_fj902_932jf_9e023__
#define		__CSKYBOX_H_fj902_932jf_9e023__



#include	"RD_GraphicRoot.h"



class CSkyBox
{

public:

	struct STexturedVertex
	{
		STexturedVertex( void ) : m_Position( 0.0f, 0.0f, 0.0f ), m_UV( 0.0f, 0.0f ) {}
		STexturedVertex( float x, float y, float z, float u, float v ) : m_Position( x, y, z ), m_UV( u, v ) {}
		STexturedVertex( const D3DXVECTOR3 & aPosition, const D3DXVECTOR2 & aUV ) : m_Position( aPosition ), m_UV( aUV ) {}

		D3DXVECTOR3		m_Position;
		D3DXVECTOR2		m_UV;
	};


public:

	CSkyBox( void );
	~CSkyBox( void );

	bool Release( void );

	HRESULT Build( LPDIRECT3DDEVICE9 apDevice, LPCTSTR aSetName, DWORD aMeshOptions );
	HRESULT Render( LPDIRECT3DDEVICE9 apDevice, const D3DXVECTOR3 & aPosition );


private:

	bool				m_IsBuilded;
	LPD3DXMESH			m_pd3dxMesh;
	DWORD				m_nAttributes;

	D3DMATERIAL9		m_Materials[ 6 ];
	LPDIRECT3DTEXTURE9	m_pTextures[ 6 ];

};



#endif		// __CSKYBOX_H_fj902_932jf_9e023__
