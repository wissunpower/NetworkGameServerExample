#pragma once


#include	"MatchlessCClient.h"


namespace MatchlessServer
{
	class CClient : public Matchless::CClient, public std::enable_shared_from_this< CClient >
	{

	public:
		CClient() = default;

		CClient( const CClient& ) = delete;
		CClient& operator=( const CClient& ) = delete;

	private:
		mutable std::recursive_mutex	m_lock;

	};
}
