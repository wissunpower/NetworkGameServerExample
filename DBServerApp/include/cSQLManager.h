#pragma once


class cSQLManager
{
public:
	struct INITCONFIG
	{
		tstring		sDNS;
		tstring		sUser;
		tstring		sAuth;
	};

public:
	cSQLManager();
	~cSQLManager();

	int Initialize( const INITCONFIG& config );

	int Shutdown();

	int Execute( const tstring& sCmd );

private:
	SQLHENV		m_hEnv;
	SQLHDBC		m_hDbc;
	SQLHSTMT	m_hStmt;
};
