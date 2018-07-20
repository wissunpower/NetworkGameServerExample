#pragma once


class cSQLManager
{
public:
	static constexpr int DISPLAY_MAX = 50;
	static constexpr int DISPLAY_FORMAT_EXTRA = 3;  // Per column extra display bytes (| <data> )
	static constexpr tchar* DISPLAY_FORMAT = _T( "%c %*.*s " );
	static constexpr tchar* DISPLAY_FORMAT_C = _T( "%c %-*.*s " );
	static constexpr int NULL_SIZE = 6;   // <NULL>
	static constexpr tchar PIPE	= _T( '|' );

	struct INITCONFIG
	{
		tstring		sDNS;
		tstring		sUser;
		tstring		sAuth;
	};

	struct BINDING
	{
		SQLSMALLINT					nDisplaySize;
		std::vector< tchar >		vBuffer;
		SQLLEN						indPtr;
		bool						bChar;
		//std::shared_ptr< BINDING >	sNext;			
	};

public:
	cSQLManager();
	~cSQLManager();

	int Initialize( const INITCONFIG& config );

	int Shutdown();

	int Execute( const tstring& sCmd );

private:
	void HandleDiagnosticRecord( SQLHANDLE hHandle, SQLSMALLINT hType, RETCODE nRetCode );
	void DisplayResults( HSTMT hStmt, SQLSMALLINT nCols );
	void AllocateBindings( HSTMT hStmt, SQLSMALLINT nCols, std::list< std::shared_ptr< BINDING > >& lBinding, SQLSMALLINT* pDisplay );
	void DisplayTitles( HSTMT hStmt, DWORD nDisplaySize, const std::list< std::shared_ptr< BINDING > >& lBinding );
	void SetConsole( DWORD dwDisplaySize, bool bInvert );

private:
	SQLHENV		m_hEnv;
	SQLHDBC		m_hDbc;
	SQLHSTMT	m_hStmt;

	std::recursive_mutex	m_lock;

	int			m_nConsoleHeight;
};
