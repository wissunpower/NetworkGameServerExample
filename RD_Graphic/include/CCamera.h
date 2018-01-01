
#ifndef		CCAMERA_H_vh8w3_bj9w3_bj89s
#define		CCAMERA_H_vh8w3_bj9w3_bj89s


#include	"RD_GraphicRoot.h"


class CCamera
{

public:

	CCamera( void );
	~CCamera( void );

	// Get Member Function
	const D3DXVECTOR3 & GetPosition( void ) const	{ return m_Position; }
	const D3DXVECTOR3 & GetRight( void ) const		{ return m_Right; }
	const D3DXVECTOR3 & GetLookAt( void ) const		{ return m_LookAt; }
	const D3DXVECTOR3 & GetUp( void ) const			{ return m_Up; }
	const D3DXMATRIX & GetViewTransform( void );

	float GetFOV( void ) const						{ return m_FOV; }
	float GetAspect( void ) const					{ return m_Aspect; }
	float GetNearClip( void ) const					{ return m_NearClip; }
	float GetFarClip( void ) const					{ return m_FarClip; }
	const D3DXMATRIX & GetProjectionTransform( void );

	const D3DLIGHT9 & GetLight( void ) const		{ return m_Light; }

	// Set Member Function
	void SetPosition( const D3DXVECTOR3 & aValue )	{ m_Position = aValue; }
	void SetLookAt( const D3DXVECTOR3 & aValue )	{ m_LookAt = aValue; }
	void SetUp( const D3DXVECTOR3 & aValue )		{ m_Up = aValue; }

	void SetFOV( float aValue )						{ m_FOV = aValue; }
	void SetAspect( float aValue )					{ m_Aspect = aValue; }
	void SetNearClip( float aValue )				{ m_NearClip = aValue; }
	void SetFarClip( float aValue )					{ m_FarClip = aValue; }

	void SetLight( const D3DLIGHT9 & aValue )		{ m_Light = aValue; }

	bool UpdateLight( D3DLIGHTTYPE aType = D3DLIGHT_POINT, float aRange = 1000.0f );

	bool MoveOnXAxis( float aDistance );
	bool MoveOnYAxis( float aDistance );
	bool MoveOnZAxis( float aDistance );

	bool Rotate( float aX, float aY, float aZ );


private:

	D3DXVECTOR3		m_Position;
	D3DXVECTOR3		m_Right;
	D3DXVECTOR3		m_LookAt;
	D3DXVECTOR3		m_Up;
	D3DXMATRIX		m_ViewTrans;

	float			m_FOV;
	float			m_Aspect;
	float			m_NearClip;
	float			m_FarClip;
	D3DXMATRIX		m_ProjectionTrans;

	D3DLIGHT9		m_Light;

};



#endif		// CCAMERA_H_vh8w3_bj9w3_bj89s
