
#ifndef		__CBACKGROUND_H_fjh90_892hf_fj9w0__
#define		__CBACKGROUND_H_fjh90_892hf_fj9w0__



#include	"RD_GraphicRoot.h"



class CBackground
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

	CBackground( void );
	~CBackground( void );

	// Get Member Function
	const D3DXMATRIX & GetTransformMatrix( void ) const			{ return m_TransformMatrix; }

	// Set Member Function
	void SetTransformMatrix( const D3DXMATRIX & aMatrix )		{ m_TransformMatrix = aMatrix; }

	bool Release( void );

	HRESULT Build( IDirect3DDevice9 * apDevice, LPCTSTR aFileName, DWORD aMeshOptions );
	HRESULT Render( IDirect3DDevice9 * apDevice, const D3DXVECTOR3 & aPosition );


private:

	bool					m_IsBuilded;
	LPD3DXMESH				m_pd3dxMesh;

	D3DXMATRIX				m_TransformMatrix;

	D3DMATERIAL9			m_Materials;
	IDirect3DTexture9 *		m_pTextures;

};



#endif		// __CBACKGROUND_H_fjh90_892hf_fj9w0__
