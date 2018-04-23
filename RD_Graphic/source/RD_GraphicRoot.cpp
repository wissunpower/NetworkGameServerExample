
#include	"stdafx.h"
#include	"RD_GraphicRoot.h"



DWORD						g_dwBehaviorFlags;



HRESULT RetrieveResourceFile( const tstring& sSrc, tstring& sPath, tstring& sName )
{
	if( sSrc.empty() )
	{
		return	E_INVALIDARG;
	}


	HRESULT					hr = S_OK;
	std::vector< tchar >	tempStr( MAX_PATH );

	hr = DXUTFindDXSDKMediaFileCch( tempStr.data(), static_cast<int>( tempStr.size() ), sSrc.c_str() );
	if( FAILED( hr ) )
	{
		return	hr;
	}

	tstring sTempPath = tempStr.data();
	auto nLastSlash = sTempPath.find_last_of( _T( '/' ) );

	if( tstring::npos != nLastSlash )
	{
		sPath = sTempPath.substr( 0, nLastSlash );
		sName = sTempPath.substr( nLastSlash + 1, sTempPath.size() - ( nLastSlash + 1 ) );
	}
	else
	{
		sPath = _T( "." );
		sName = tempStr.data();
	}

	return	hr;
}


float GetDistanceWithMesh( bool * apHit, const D3DXVECTOR3 & aPosition, const D3DXVECTOR3 & aDirection,
						  const LPD3DXBASEMESH apMesh, const D3DXMATRIX & aMeshTransform )
{
	D3DXVECTOR3		tempPosition;
	D3DXMATRIX		tempTM;
	D3DXVECTOR3		countDirection( -aDirection.x, -aDirection.y, -aDirection.z );
	BOOL			bFind;
	float			fDistance = 0.0f;
	float			fCountDistance = 0.0f;


	D3DXMatrixInverse( &tempTM, NULL, &aMeshTransform );
	D3DXMatrixTranspose( &tempTM, &tempTM );

	tempPosition.x = tempTM._11 * aPosition.x + tempTM._12 * aPosition.y + tempTM._13 * aPosition.z + tempTM._14 * 1;
	tempPosition.y = tempTM._21 * aPosition.x + tempTM._22 * aPosition.y + tempTM._23 * aPosition.z + tempTM._24 * 1;
	tempPosition.z = tempTM._31 * aPosition.x + tempTM._32 * aPosition.y + tempTM._33 * aPosition.z + tempTM._34 * 1;


	/*
	D3DXIntersect( apMesh, &tempPosition, &aDirection, &bFind, NULL, NULL, NULL, &fDistance, NULL, NULL );
	D3DXIntersect( apMesh, &tempPosition, &countDirection, &bCountFind, NULL, NULL, NULL, &fCountDistance, NULL, NULL );
	fCountDistance *= (-1.0f);

	(*apHit) = bFind || bCountFind;
	if( bFind && bCountFind )
	{
		fDistance = ( fDistance < fCountDistance ) ? fDistance : fCountDistance;
	}
	else if( bFind )
	{
		fDistance = fDistance;
	}
	else if( bCountFind )
	{
		fDistance = fCountDistance;
	}
	else
	{
		fDistance = 0.0f;
	}
	*/


	(*apHit) = true;
	D3DXIntersect( apMesh, &tempPosition, &aDirection, &bFind, NULL, NULL, NULL, &fDistance, NULL, NULL );
	if( !bFind )
	{
		D3DXIntersect( apMesh, &tempPosition, &countDirection, &bFind, NULL, NULL, NULL, &fDistance, NULL, NULL );
		if( !bFind )
		{
			(*apHit) = false;
			fDistance = 0.0f;
		}
		else
		{
			fDistance *= (-1.0f);
		}
	}


	return	( fDistance );
}
