
#include	"CCamera.h"


CCamera::CCamera( void ) : m_Position( 0, 0, 0 ), m_Right( 1, 0, 0 ), m_LookAt( 0, 0, 1 ), m_Up( 0, 1, 0 ),
							m_FOV( D3DX_PI ), m_Aspect( 1 ), m_NearClip( 1.01f ), m_FarClip( 10000.0f )
{
	D3DXMatrixIdentity( &m_ViewTrans );
	D3DXMatrixIdentity( &m_ProjectionTrans );

	ZeroMemory( &m_Light, sizeof( D3DLIGHT9 ) );
}


CCamera::~CCamera( void )
{
}


const D3DXMATRIX & CCamera::GetViewTransform( void )
{
	//D3DXMatrixLookAtLH( &m_ViewTrans, &m_Position, &m_LookAt, &m_Up );


	//m_LookAt	=	D3DXVECTOR3( m_ViewTrans._13, m_ViewTrans._23, m_ViewTrans._33 );
	//m_Up		=	D3DXVECTOR3( m_ViewTrans._12, m_ViewTrans._22, m_ViewTrans._32 );
	//m_Right		=	D3DXVECTOR3( m_ViewTrans._11, m_ViewTrans._21, m_ViewTrans._31 );

	D3DXVec3Normalize( &m_LookAt, &m_LookAt );
	D3DXVec3Cross( &m_Right, &m_Up, &m_LookAt );
	D3DXVec3Normalize( &m_Right, &m_Right );
	D3DXVec3Cross( &m_Up, &m_LookAt, &m_Right );
	D3DXVec3Normalize( &m_Up, &m_Up );

	m_ViewTrans._11 = m_Right.x;   m_ViewTrans._12 = m_Up.x;   m_ViewTrans._13 = m_LookAt.x;
	m_ViewTrans._21 = m_Right.y;   m_ViewTrans._22 = m_Up.y;   m_ViewTrans._23 = m_LookAt.y;
	m_ViewTrans._31 = m_Right.z;   m_ViewTrans._32 = m_Up.z;   m_ViewTrans._33 = m_LookAt.z;
	m_ViewTrans._41 =- D3DXVec3Dot( &m_Position, &m_Right );
	m_ViewTrans._42 =- D3DXVec3Dot( &m_Position, &m_Up );
	m_ViewTrans._43 =- D3DXVec3Dot( &m_Position, &m_LookAt );

	return	m_ViewTrans;
}


const D3DXMATRIX & CCamera::GetProjectionTransform( void )
{
	D3DXMatrixPerspectiveFovLH( &m_ProjectionTrans, (FLOAT)m_FOV, (FLOAT)m_Aspect, (FLOAT)m_NearClip, (FLOAT)m_FarClip );

	return	m_ProjectionTrans;
}


bool CCamera::UpdateLight( D3DLIGHTTYPE aType, float aRange )
{
	ZeroMemory( &m_Light, sizeof( D3DLIGHT9 ) );


	D3DXVECTOR3		tempDirection;


	m_Light.Type = aType;

	m_Light.Diffuse.a = 1.0f;
	m_Light.Diffuse.r = 1.0f;
	m_Light.Diffuse.g = 1.0f;
	m_Light.Diffuse.b = 1.0f;
	m_Light.Ambient.a = 1.0f;
	m_Light.Ambient.r = 1.0f;
	m_Light.Ambient.g = 1.0f;
	m_Light.Ambient.b = 1.0f;
	m_Light.Specular.a = 1.0f;
	m_Light.Specular.r = 1.0f;
	m_Light.Specular.g = 1.0f;
	m_Light.Specular.b = 1.0f;

	m_Light.Attenuation0 = 1.0f;

	tempDirection.x = m_LookAt.x - m_Up.x;
	tempDirection.y = m_LookAt.y - m_Up.y;
	tempDirection.z = m_LookAt.z - m_Up.z;
	D3DXVec3Normalize( (D3DXVECTOR3*)&m_Light.Direction, &tempDirection );

	m_Light.Position.x = m_Position.x;
	m_Light.Position.y = static_cast<float>( m_Position.y + ( tan( m_FOV / 2 ) * m_NearClip ) );
	m_Light.Position.z = m_Position.z;

	m_Light.Range = aRange;

	return	true;
}


bool CCamera::MoveOnXAxis( float aDistance )
{
	D3DXVECTOR3	tempRight;

	D3DXVec3Normalize( &tempRight, &m_Right );

	m_Position.x += (tempRight.x * aDistance);
	m_Position.y += (tempRight.y * aDistance);
	m_Position.z += (tempRight.z * aDistance);

	return	true;
}


bool CCamera::MoveOnYAxis( float aDistance )
{
	D3DXVECTOR3	tempUp;

	D3DXVec3Normalize( &tempUp, &m_Up );

	m_Position.x += (tempUp.x * aDistance);
	m_Position.y += (tempUp.y * aDistance);
	m_Position.z += (tempUp.z * aDistance);

	return	true;
}


bool CCamera::MoveOnZAxis( float aDistance )
{
	D3DXVECTOR3	tempLookAt;

	D3DXVec3Normalize( &tempLookAt, &m_LookAt );

	m_Position.x += (tempLookAt.x * aDistance);
	m_Position.y += (tempLookAt.y * aDistance);
	m_Position.z += (tempLookAt.z * aDistance);

	return	true;
}


bool CCamera::Rotate( float aX, float aY, float aZ )
{
	D3DXMATRIX	tempMatrix;

	if( aX != 0.0f )
	{
		D3DXMatrixRotationAxis( &tempMatrix, &m_Right, D3DXToRadian( aX ) );
		D3DXVec3TransformNormal( &m_LookAt, &m_LookAt, &tempMatrix );
		D3DXVec3TransformNormal( &m_Up, &m_Up, &tempMatrix );
		D3DXVec3TransformNormal( &m_Right, &m_Right, &tempMatrix );
	}

	if( aY != 0.0f )
	{
		D3DXMatrixRotationAxis( &tempMatrix, &m_Up, D3DXToRadian( aY ) );
		D3DXVec3TransformNormal( &m_LookAt, &m_LookAt, &tempMatrix );
		D3DXVec3TransformNormal( &m_Up, &m_Up, &tempMatrix );
		D3DXVec3TransformNormal( &m_Right, &m_Right, &tempMatrix );
	}

	if( aZ != 0.0f )
	{
		D3DXMatrixRotationAxis( &tempMatrix, &m_LookAt, D3DXToRadian( aZ ) );
		D3DXVec3TransformNormal( &m_LookAt, &m_LookAt, &tempMatrix );
		D3DXVec3TransformNormal( &m_Up, &m_Up, &tempMatrix );
		D3DXVec3TransformNormal( &m_Right, &m_Right, &tempMatrix );
	}

	D3DXVec3Normalize( &m_LookAt, &m_LookAt );
	D3DXVec3Cross( &m_Right, &m_Up, &m_LookAt );
	D3DXVec3Normalize( &m_Right, &m_Right );
	D3DXVec3Cross( &m_Up, &m_LookAt, &m_Right );
	D3DXVec3Normalize( &m_Up, &m_Up );

	return	true;
}
