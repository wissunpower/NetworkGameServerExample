
#include "stdafx.h"
#include "cServerConnectionManager.h"
#include "cLog.h"


cServerConnectionManager::~cServerConnectionManager()
{
	m_conConnection.clear();
}

void cServerConnectionManager::SetConnectConfig( const INITCONFIG& initConfig )
{
	m_connectConfig = initConfig;
}

int cServerConnectionManager::CreateConnection( INITCONFIG& initConfig, const DWORD dwInitialConnectionCount )
{
	SetConnectConfig( initConfig );

	auto dwCount = ( dwInitialConnectionCount < 1 ) ? 1 : dwInitialConnectionCount;
	IncreaseConnectionPool( dwCount );

	return RETURN_SUCCEED;
}

int cServerConnectionManager::IncreaseConnectionPool( const DWORD dwCount /*= 1*/ )
{
	std::lock_guard< std::recursive_mutex > guard { m_lock };

	const DWORD nCurPoolSize = static_cast<DWORD>( m_conConnectionPool.size() );

	for ( DWORD i = nCurPoolSize ; i < nCurPoolSize + dwCount ; ++i )
	{
		m_connectConfig.nIndex = i;
		std::shared_ptr< cConnection > pConnection { new cConnection };
		if ( !pConnection || pConnection->CreateConnection( m_connectConfig ) == false )
			return 1;

		m_conConnectionPool.insert( std::move( pConnection ) );
	}

	return RETURN_SUCCEED;
}

bool cServerConnectionManager::AddConnection( const std::shared_ptr< cConnection >& pConnection )
{
	std::lock_guard< std::recursive_mutex > guard { m_lock };

	auto conn_it = m_conConnection.find( pConnection );
	if ( conn_it != m_conConnection.end() )
	{
		LOG( eLogInfoType::LOG_INFO_NORMAL, tstring{ _T( "SYSTEM | cConnectionManager::AddConnection() | index[ " ) } + wsp::to( pConnection->GetIndex() ) + _T( " ]는 이미 Connection map 에 있습니다." ) );
		return false;
	}

	m_conConnection.insert( std::move( pConnection ) );

	// 추가될 접속을 위한 pool 증가
	if ( m_conConnection.size() == m_conConnectionPool.size() )
	{
		auto ret = IncreaseConnectionPool();
		if ( RETURN_SUCCEED != ret )
		{
			LOG( eLogInfoType::LOG_ERROR_HIGH, tstring{ _T( "SYSTEM | cConnectionManager::AddConnection() | Connection 예비 메모리 확보에 실패하였습니다. ( error code : " ) } + wsp::to( ret ) + _T( " )" ) );
		}
	}

	return true;
}

bool cServerConnectionManager::RemoveConnection( const std::shared_ptr< cConnection >& pConnection )
{
	std::lock_guard< std::recursive_mutex > guard { m_lock };

	auto conn_it = m_conConnection.find( pConnection );
	if ( conn_it == m_conConnection.end() )
	{
		LOG( eLogInfoType::LOG_INFO_NORMAL, tstring{ _T( "SYSTEM | cConnectionManager::RemoveConnection() | index[ " ) } + wsp::to( pConnection->GetIndex() ) + _T( " ]는 Connection map 에 없습니다." ) );
		return false;
	}

	m_conConnection.erase( pConnection );

	return true;
}

void cServerConnectionManager::BroadCast_Chat( const tstring& szIP, const tstring& szChatMsg )
{
}
