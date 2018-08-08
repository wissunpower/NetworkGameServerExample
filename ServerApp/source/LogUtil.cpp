
#include	"stdafx.h"
#include	"LibraryDef.h"
#include	"LogUtil.h"
#include	"cPacket.h"
#include	"MatchlessProtocol.h"
#include	"cSingleton.h"
#include	"cDBServer.h"


int InitLog()
{
	sLogConfig LogConfig;
	LogConfig.s_eLogFileType = eLogFileType::FILETYPE_TEXT;
	//LogConfig.s_hWnd = hWnd;
	LogConfig.s_sLogFileName = _T( "ServerApp" );

	//LogConfig.s_vLogInfoTypes[ static_cast<size_t>( eLogStorageType::STORAGE_OUTPUTWND ) ] = eLogInfoType::LOG_ALL;
	LogConfig.s_vLogInfoTypes[ static_cast<size_t>( eLogStorageType::STORAGE_FILE ) ] = eLogInfoType::LOG_ALL;

	if ( !INIT_LOG( LogConfig ) )
	{
		return 1;
	}

	return RETURN_SUCCEED;
}

int CloseLog()
{
	CLOSE_LOG();

	return RETURN_SUCCEED;
}

int WriteLog( const std::initializer_list< tstring >& sl, const std::initializer_list< eLogInfoType >& tl /*= { eLogInfoType::LOG_INFO_LOW }*/ )
{
	tstring sLog;
	for ( auto s : sl )
	{
		sLog += s;
	}

	eLogInfoType eType { eLogInfoType::LOG_NONE };
	for ( auto t : tl )
	{
		eType = eType | t;
	}

	_tprintf( _T( "%s\n" ), sLog.c_str() );
	LOG( eType, sl );


	auto tpNow = std::chrono::system_clock::now();
	auto tt = std::chrono::system_clock::to_time_t( tpNow );
	tm tmNow;
	localtime_s( &tmNow, &tt );

	cOPacket oPacket;
	oPacket.Encode4u( Matchless::FSTD_LOGCOMMON_WRITE );
	oPacket.EncodeTM( tmNow );
	oPacket.Encode4u( static_cast<unsigned int>( eType ) );
	oPacket.EncodeStr( sLog );
	oPacket.Send( cSingleton< Matchless::cDBServer >::Get()->GetSocket() );

	return RETURN_SUCCEED;
}

int WriteLog( const tstring& s, const std::initializer_list< eLogInfoType >& tl /*= { eLogInfoType::LOG_INFO_LOW }*/ )
{
	return WriteLog( { s }, tl );
}
