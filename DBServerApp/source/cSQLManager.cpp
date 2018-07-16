
#include	"stdafx.h"
#include	"LibraryDef.h"
#include	"cLog.h"
#include	"cSQLManager.h"
#include	"cMatchlessDBServerException.h"


cSQLManager::cSQLManager()
	: m_hEnv{}, m_hDbc{}, m_hStmt{}
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
	SQLRETURN ret = SQLExecDirect( m_hStmt, (SQLTCHAR*)sCmd.c_str(), SQL_NTS );
	if ( SQL_SUCCESS != ret && SQL_SUCCESS_WITH_INFO != ret )
	{
		return ret;
	}
	SQLCancel( m_hStmt );

	return RETURN_SUCCEED;
}
