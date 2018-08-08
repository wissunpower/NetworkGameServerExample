
#include	"stdafx.h"
#include	"LibraryDef.h"
#include	"cPacket.h"
#include	"cSingleton.h"
#include	"cNetMessageHandler_Impl.h"
#include	"cSQLManager.h"


int cLogCommonWriteHandler::OnProcess( cConnection& connection, cIPacket& iPacket )
{
	tm tmWhen = iPacket.DecodeTM();
	unsigned int nLogType = iPacket.Decode4u();
	tstring sLog = iPacket.DecodeStr( tstring{} );

	std::vector< char > vStr( 40 );
	std::strftime( vStr.data(), vStr.size(), "%F %T", &tmWhen );

	tstring sSQLCmd{};
	sSQLCmd = _T( "INSERT INTO [dbo].[LogCommon] ([Time],[Type],[Log]) VALUES ('" );
	sSQLCmd += tstring{ vStr.begin(), vStr.end() }.c_str();
	sSQLCmd += _T( "', " );
	sSQLCmd += wsp::to( nLogType );
	sSQLCmd += _T( ", '" );
	sSQLCmd += sLog;
	sSQLCmd += _T( "')" );
	cSingleton< cSQLManager >::Get()->Execute( sSQLCmd );

	return RETURN_SUCCEED;
}
