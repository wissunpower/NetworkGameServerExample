// DBServerApp.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//

#include	"stdafx.h"

#include	"LibraryDef.h"
#include	"cException.h"
#include	"cLog.h"
#include	"CMiniDump.h"
#include	"cSingleton.h"
#include	"cSQLManager.h"


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


int Startup()
{
	CMiniDump::Begin();

	if ( InitLog() )
	{
		throw cException_FailedToCreateObject{ "Log", __FILE__, __LINE__ };
	}


	int nReturnCode = RETURN_SUCCEED;

	cSQLManager::INITCONFIG		sqlConfig;
	sqlConfig.sDNS = _T( "MatchlessGame" );
	sqlConfig.sUser = _T( "sa" );
	sqlConfig.sAuth = _T( "root" );
	if ( nReturnCode = cSingleton< cSQLManager >::Get()->Initialize( sqlConfig ) )
	{
		WriteLog( { _T( "Failed to SQL Manager Initialize : cSQLManager::Initialize(), Code Num : " ), wsp::to( nReturnCode ) }, { eLogInfoType::LOG_ERROR_HIGH } );
		throw cException_FailedToCreateObject{ "SQL Manager", __FILE__, __LINE__ };
	}

	return RETURN_SUCCEED;
}

int Run()
{
	tstring sSQLCmd{};
	sSQLCmd = _T( "INSERT INTO [dbo].[LogCommon] ([Time],[Type],[Log]) VALUES (GETDATE(), 0, 'For Test~!')" );
	cSingleton< cSQLManager >::Get()->Execute( sSQLCmd );

	return RETURN_SUCCEED;
}

int Cleanup()
{
	cSingleton< cSQLManager >::Get()->Shutdown();

	CloseLog();

	CMiniDump::End();

	return RETURN_SUCCEED;
}


int main()
try {
	if ( Startup() )
	{
		WriteLog( _T( "Failed DBServerApp Startup()" ), { eLogInfoType::LOG_ERROR_HIGH } );
		return 1;
	}

	if ( Run() )
	{
		WriteLog( _T( "Occur Error while DBServerApp Run()" ), { eLogInfoType::LOG_ERROR_HIGH } );
		return 10000;
	}

	Cleanup();

    return RETURN_SUCCEED;
}
catch ( const cException& e ) {
	tstring notice;

	notice = wsp::to( e.what() );

	WriteLog( notice, { eLogInfoType::LOG_ERROR_HIGH } );

	return 0xa0000003;
}
catch ( const std::runtime_error& e ) {
	tstring notice;

	notice = wsp::to( e.what() );

	WriteLog( notice, { eLogInfoType::LOG_ERROR_HIGH } );

	return 0xa0000002;
}
catch ( const std::exception& e ) {
	tstring notice;

	notice = wsp::to( e.what() );

	WriteLog( notice, { eLogInfoType::LOG_ERROR_HIGH } );

	return 0xa0000001;
}
catch ( ... ) {
	WriteLog( _T( "Unknown Exception" ), { eLogInfoType::LOG_ERROR_HIGH } );

	return 0xa0000000;
}
