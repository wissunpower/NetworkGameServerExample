
#ifndef		__CEFFECTBILLBOARD_H_fja02_qnox8_281jd__
#define		__CEFFECTBILLBOARD_H_fja02_qnox8_281jd__



#include	<list>

#include	"RD_GraphicRoot.h"



class CEffectBillboard
{

public:

	const static DWORD ebFVF = D3DFVF_XYZ | /*D3DFVF_NORMAL |*/ D3DFVF_TEX1;

	struct SEBVertex
	{
		D3DXVECTOR3		pos;
		D3DXVECTOR3		normal;
		D3DXVECTOR2		tex1;
	};


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

	CEffectBillboard( void );
	~CEffectBillboard( void );
	CEffectBillboard( const CEffectBillboard & src );
	CEffectBillboard & operator=( const CEffectBillboard & src );

	bool Release( void );

	HRESULT Build( IDirect3DDevice9 * apDevice, LPCTSTR aSetName, DWORD aMeshOptions, D3DPOOL aPoolType,
					int aSize = 1, int aStartFrame = 0, int aFrameNum = 30 );
	HRESULT UpdatePerFrame( DOUBLE aTimeDelta, bool abUpdate );
	HRESULT UpdatePerFrame( const unsigned int aIndex );
	HRESULT Render( IDirect3DDevice9 * apDevice, const D3DXVECTOR3 & aRight, const D3DXVECTOR3 & aUp, const D3DXVECTOR3 & aPos );


private:

	bool					m_IsBuilded;
	LPD3DXMESH				m_pd3dxMesh;

	D3DXMATRIX				m_TransformMatrix;

	std::list< IDirect3DTexture9* >				m_TexList;
	std::list< IDirect3DTexture9* >::iterator	m_CurrentTexIter;

};



#endif		// __CEFFECTBILLBOARD_H_fja02_qnox8_281jd__
