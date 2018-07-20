
#include	"stdafx.h"
#include	"LibraryDef.h"
#include	"cLog.h"
#include	"cSQLManager.h"
#include	"cMatchlessDBServerException.h"


#define TRYODBC(h, ht, x)   {   RETCODE rc = x;\
                                if (rc != SQL_SUCCESS) \
                                { \
                                    HandleDiagnosticRecord (h, ht, rc); \
                                } \
                                if (rc == SQL_ERROR) \
                                { \
                                    _ftprintf(stderr, _T( "Error in " #x "\n" )); \
                                }  \
                            }


cSQLManager::cSQLManager()
	: m_hEnv{}, m_hDbc{}, m_hStmt{}
	, m_nConsoleHeight{ 80 }
{
	// Allocate environment handle
	if ( SQLAllocHandle( SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnv ) )
	{
		WriteLog( _T( "Failed SQL Environment handle allocate : SQLAllocHandle()" ), { eLogInfoType::LOG_ERROR_HIGH } );
		throw cMatchlessDBServerException_FailedSQLHandleAllocate { "Environment handle", __FILE__, __LINE__ };
	}
}

cSQLManager::~cSQLManager()
{
	if ( m_hEnv )
	{
		SQLFreeHandle( SQL_HANDLE_ENV, m_hEnv );
	}
}

int cSQLManager::Initialize( const INITCONFIG& config )
{
	SQLRETURN ret = SQL_SUCCESS;

	// Set the ODBC version environment attribute
	if ( SQLSetEnvAttr( m_hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER*)SQL_OV_ODBC3, 0 ) )
	{
		WriteLog( _T( "Failed Set the ODBC version environment attribute : SQLSetEnvAttr()" ), { eLogInfoType::LOG_ERROR_HIGH } );
		return 1;
	}

	// Allocate connection handle
	if ( SQLAllocHandle( SQL_HANDLE_DBC, m_hEnv, &m_hDbc ) )
	{
		WriteLog( _T( "Failed Allocate connection handle : SQLAllocHandle()" ), { eLogInfoType::LOG_ERROR_HIGH } );
		return 2;
	}

	// Set login timeout to 5 seconds
	SQLSetConnectAttr( m_hDbc, SQL_LOGIN_TIMEOUT, (SQLPOINTER)5, 0 );

	// Connect to data source
	ret = SQLConnect( m_hDbc, (SQLTCHAR*)config.sDNS.c_str(), static_cast<SQLSMALLINT>( config.sDNS.length() ),
		(SQLTCHAR*)config.sUser.c_str(), static_cast<SQLSMALLINT>( config.sUser.length() ), (SQLTCHAR*)config.sAuth.c_str(), static_cast<SQLSMALLINT>( config.sAuth.length() ) );
	if ( SQL_SUCCESS != ret && SQL_SUCCESS_WITH_INFO != ret )
	{
		WriteLog( _T( "Failed Connect to data source : SQLConnect()" ), { eLogInfoType::LOG_ERROR_HIGH } );
		return 3;
	}

	// Allocate statement handle
	if ( SQLAllocHandle( SQL_HANDLE_STMT, m_hDbc, &m_hStmt ) )
	{
		WriteLog( _T( "Failed Allocate statement handle : SQLAllocHandle()" ), { eLogInfoType::LOG_ERROR_HIGH } );
		return 4;
	}

	return RETURN_SUCCEED;
}

int cSQLManager::Shutdown()
{
	if ( m_hStmt )
	{
		SQLFreeHandle( SQL_HANDLE_STMT, m_hStmt );
	}

	if ( m_hDbc )
	{
		SQLDisconnect( m_hDbc );
		SQLFreeHandle( SQL_HANDLE_DBC, m_hDbc );
	}

	return RETURN_SUCCEED;
}

int cSQLManager::Execute( const tstring& sCmd )
{
	std::lock_guard< std::recursive_mutex >	guard { m_lock };

	SQLRETURN ret = SQLExecDirect( m_hStmt, (SQLTCHAR*)sCmd.c_str(), SQL_NTS );

	switch ( ret )
	{

	case SQL_SUCCESS_WITH_INFO:
		{
			HandleDiagnosticRecord( m_hStmt, SQL_HANDLE_STMT, ret );
			// fall through
		}

	case SQL_SUCCESS:
		{
			SQLSMALLINT nCols = 0;

			TRYODBC( m_hStmt, SQL_HANDLE_STMT, SQLNumResultCols( m_hStmt, &nCols ) );

			if ( nCols > 0 )
			{
				DisplayResults( m_hStmt, nCols );
			}
			else
			{
				SQLLEN nRowCount;

				TRYODBC( m_hStmt, SQL_HANDLE_STMT, SQLRowCount( m_hStmt, &nRowCount ) );

				if ( nRowCount >= 0 )
				{
					WriteLog( wsp::to( nRowCount ) + ( nRowCount == 1 ? _T( " row" ) : _T( " rows" ) ) + _T( " affected" ) );
				}
			}
		}
		break;

	case SQL_ERROR:
		{
			HandleDiagnosticRecord( m_hStmt, SQL_HANDLE_STMT, ret );
		}
		break;

	default:
		WriteLog( tstring{ _T( "Unexpected return code : " ) } + wsp::to( ret ), { eLogInfoType::LOG_ERROR_HIGH } );
		break;

	}

	TRYODBC( m_hStmt, SQL_HANDLE_STMT, SQLCancel( m_hStmt ) );

	return RETURN_SUCCEED;
}

void cSQLManager::HandleDiagnosticRecord( SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE nRetCode )
{
	SQLSMALLINT	iRec = 0;
	SQLINTEGER	iError;
	std::vector< tchar >	vMessage( 1000 );
	std::vector< tchar >	vState( SQL_SQLSTATE_SIZE + 1 );

	if ( nRetCode == SQL_INVALID_HANDLE )
	{
		WriteLog( _T( "Invalid Handle!" ), { eLogInfoType::LOG_ERROR_HIGH } );
		return;
	}

	while ( SQL_SUCCESS == SQLGetDiagRec( hType, hHandle, ++iRec, vState.data(), &iError, vMessage.data(), static_cast<SQLSMALLINT>( vMessage.size() ), (SQLSMALLINT*)nullptr ) )
	{
		tstring sState{ vState.begin(), vState.begin() + 5 };
		if ( tstring{ _T( "01004" ) } != sState )
		{
			auto nStateLen = _tcslen( sState.c_str() );
			if ( nStateLen < 5 )
			{
				sState = tstring( 5 - nStateLen, _T( ' ' ) ) + sState;
			}
			WriteLog( tstring{ _T( "[" ) } + sState + _T( "] " ) + tstring{ vMessage.begin(), vMessage.end() }.c_str() + _T( " (" ) + wsp::to( iError ) + _T( ")" ), { eLogInfoType::LOG_ERROR_HIGH } );
		}
	}
}

void cSQLManager::DisplayResults( HSTMT hStmt, SQLSMALLINT nCols )
{
	std::list< std::shared_ptr< BINDING > >	lBinding;
	SQLSMALLINT								nDisplaySize;
	RETCODE									nRetCode = SQL_SUCCESS;
	int										iCount = 0;

	// Allocate memory for each column
	AllocateBindings( hStmt, nCols, lBinding, &nDisplaySize );

	// Set the display mode and write the titles
	DisplayTitles( hStmt, nDisplaySize + 1, lBinding );

	bool bNoData = false;

	// Fetch and display the data
	do
	{
		// Fetch a row
		if ( iCount++ >= m_nConsoleHeight - 2 )
		{
			int		nInputChar;
			bool	bEnterReceived = false;

			while ( !bEnterReceived )
			{
				_tprintf( _T( "              " ) );
				SetConsole( nDisplaySize + 2, true );
				_tprintf( _T( "   Press ENTER to continue, Q to quit ( Height : %hd )" ), m_nConsoleHeight );
				SetConsole( nDisplaySize + 2, false );

				nInputChar = _getch();
				_tprintf( _T( "\n" ) );
				if ( ( nInputChar == 'Q' ) || ( nInputChar == 'q' ) )
				{
					// Clean up the allocated buffers
					lBinding.clear();
					return;
				}
				else if ( '\r' == nInputChar )
				{
					bEnterReceived = true;
				}
			}

			iCount = 1;
			DisplayTitles( hStmt, nDisplaySize + 1, lBinding );
		}

		TRYODBC( hStmt, SQL_HANDLE_STMT, nRetCode = SQLFetch( hStmt ) );

		if ( nRetCode == SQL_NO_DATA_FOUND )
		{
			bNoData = true;
		}
		else
		{
			// Display the data. ( Ignore truncations )
			for ( auto it = lBinding.begin() ; it != lBinding.end() ; ++it )
			{
				if ( nullptr == (*it) )
				{
					continue;
				}

				auto pThis = (*it);
				if ( SQL_NULL_DATA != pThis->indPtr )
				{
					_tprintf( pThis->bChar ? DISPLAY_FORMAT_C : DISPLAY_FORMAT, PIPE, pThis->nDisplaySize, pThis->nDisplaySize, tstring{ pThis->vBuffer.begin(), pThis->vBuffer.end() }.c_str() );
				}
				else
				{
					_tprintf( DISPLAY_FORMAT_C, PIPE, pThis->nDisplaySize, pThis->nDisplaySize, _T( "<NULL>" ) );
				}
			}
			_tprintf( _T( " %c\n" ), PIPE );
		}
	} while ( !bNoData );

	SetConsole( nDisplaySize + 2, true );
	_tprintf( _T( "%*.*s" ), nDisplaySize + 2, nDisplaySize + 2, _T( " " ) );
	SetConsole( nDisplaySize + 2, false );
	_tprintf( _T( "\n" ) );

	// Clean up the allocated buffers
	lBinding.clear();
}

void cSQLManager::AllocateBindings( HSTMT hStmt, SQLSMALLINT nCols, std::list< std::shared_ptr< BINDING > >& lBinding, SQLSMALLINT* pDisplay )
{
	SQLSMALLINT					nCol;
	std::shared_ptr< BINDING >	pThis = nullptr;
	std::shared_ptr< BINDING >	pLast = nullptr;
	SQLLEN						cchDisplay, ssType;
	SQLSMALLINT					cchColumnNameLength;

	*pDisplay = 0;
	lBinding.clear();

	for ( nCol = 1 ; nCol <= nCols ; ++nCol )
	{
		pThis.reset( new BINDING );
		lBinding.push_back( pThis );

		TRYODBC( hStmt, SQL_HANDLE_STMT, SQLColAttribute( hStmt, nCol, SQL_DESC_DISPLAY_SIZE, nullptr, 0, nullptr, &cchDisplay ) );

		TRYODBC( hStmt, SQL_HANDLE_STMT, SQLColAttribute( hStmt, nCol, SQL_DESC_CONCISE_TYPE, nullptr, 0, nullptr, &ssType ) );

		pThis->bChar = ( ssType == SQL_CHAR || ssType == SQL_VARCHAR || ssType == SQL_LONGVARCHAR );

		if ( cchDisplay > DISPLAY_MAX )
			cchDisplay = DISPLAY_MAX;

		pThis->vBuffer.resize( cchDisplay + 1 );

		TRYODBC( hStmt, SQL_HANDLE_STMT, SQLBindCol( hStmt, nCol, SQL_C_TCHAR, (SQLPOINTER)pThis->vBuffer.data(), pThis->vBuffer.size() * sizeof( tchar ), &pThis->indPtr ) );

		TRYODBC( hStmt, SQL_HANDLE_STMT, SQLColAttribute( hStmt, nCol, SQL_DESC_NAME, nullptr, 0, &cchColumnNameLength, nullptr ) );

		pThis->nDisplaySize = std::max( (SQLSMALLINT)cchDisplay, cchColumnNameLength );
		if ( pThis->nDisplaySize < NULL_SIZE )
			pThis->nDisplaySize = NULL_SIZE;

		*pDisplay += pThis->nDisplaySize + DISPLAY_FORMAT_EXTRA;
	}

	return;
}

void cSQLManager::DisplayTitles( HSTMT hStmt, DWORD nDisplaySize, const std::list< std::shared_ptr< BINDING > >& lBinding )
{
	std::vector< tchar >	vTitle( DISPLAY_MAX );
	SQLSMALLINT				nCol = 1;

	SetConsole( nDisplaySize + 2, true );

	for ( const auto& pBind : lBinding )
	{
		TRYODBC( hStmt, SQL_HANDLE_STMT, SQLColAttribute( hStmt, nCol++, SQL_DESC_NAME, vTitle.data(), static_cast<SQLSMALLINT>( vTitle.size() * sizeof( tchar ) ), nullptr, nullptr ) );

		_tprintf( DISPLAY_FORMAT_C, PIPE, pBind->nDisplaySize, pBind->nDisplaySize, tstring{ vTitle.begin(), vTitle.end() }.c_str() );
	}

	_tprintf( _T( " %c" ), PIPE );
	SetConsole( nDisplaySize + 2, false );
	_tprintf( _T( "\n" ) );
}

void cSQLManager::SetConsole( DWORD dwDisplaySize, bool bInvert )
{
	HANDLE						hConsole;
	CONSOLE_SCREEN_BUFFER_INFO	csbInfo;

	hConsole = GetStdHandle( STD_OUTPUT_HANDLE );
	if ( INVALID_HANDLE_VALUE != hConsole )
	{
		if ( GetConsoleScreenBufferInfo( hConsole, &csbInfo ) )
		{
			if ( csbInfo.dwSize.X < static_cast<SHORT>( dwDisplaySize ) )
			{
				csbInfo.dwSize.X = static_cast<SHORT>( dwDisplaySize );
				SetConsoleScreenBufferSize( hConsole, csbInfo.dwSize );
			}

			m_nConsoleHeight = csbInfo.dwSize.Y;
		}

		if ( bInvert )
		{
			SetConsoleTextAttribute( hConsole, static_cast<WORD>( csbInfo.wAttributes | BACKGROUND_BLUE ) );
		}
		else
		{
			SetConsoleTextAttribute( hConsole, static_cast<WORD>( csbInfo.wAttributes & ~(BACKGROUND_BLUE) ) );
		}
	}
}
