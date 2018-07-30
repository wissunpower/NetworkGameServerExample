#pragma once


#include "cConnection.h"


class cServerConnectionManager
{
public:
	~cServerConnectionManager();

	using CONN_MAP = std::multiset< std::shared_ptr< cConnection > >;

	void SetConnectConfig( const INITCONFIG& initConfig );

	int CreateConnection( INITCONFIG& initConfig, const DWORD dwInitialConnectionCount );
	int IncreaseConnectionPool( const DWORD dwCount = 1 );

	bool AddConnection( const std::shared_ptr< cConnection >& pConnection );
	bool RemoveConnection( const std::shared_ptr< cConnection >& pConnection );
	inline int __fastcall GetConnectionCnt()
	{ return static_cast<int>( m_conConnection.size() ); }

	void BroadCast_Chat( const tstring& szIP, const tstring& szChatMsg );

protected:
	CONN_MAP					m_conConnectionPool;
	CONN_MAP					m_conConnection;
	std::recursive_mutex		m_lock;

	INITCONFIG					m_connectConfig;
};
