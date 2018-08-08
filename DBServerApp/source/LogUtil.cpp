
#include	"stdafx.h"
#include	"LibraryDef.h"
#include	"LogUtil.h"


int InitLog()
{
	sLogConfig LogConfig;
	LogConfig.s_eLogFileType = eLogFileType::FILETYPE_TEXT;
	//LogConfig.s_hWnd = hWnd;
	LogConfig.s_sLogFileName = _T( "DBServerApp" );

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

	return RETURN_SUCCEED;
}

int WriteLog( const tstring& s, const std::initializer_list< eLogInfoType >& tl /*= { eLogInfoType::LOG_INFO_LOW }*/ )
{
	return WriteLog( { s }, tl );
}
