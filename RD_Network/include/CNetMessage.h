
#ifndef		__CNETMESSAGE_H_8fje3_b8dj3_g9w3g__
#define		__CNETMESSAGE_H_8fje3_b8dj3_g9w3g__



#include	"RD_NetworkRoot.h"



class CNetMessage
{

public:

	CNetMessage( void );
	~CNetMessage( void );
	CNetMessage( const CNetMessage & src );
	CNetMessage & operator=( const CNetMessage & src );

	const unsigned int GetType( void ) const;
	const unsigned int GetAddDataLen( void ) const;
	const char * const GetpAddData( void ) const;

	void SetType( const unsigned int aType );

	void SetData( const char* buf, const size_t len );

	int SendData( SOCKET socket, int aFlags, unsigned int aType, unsigned int aAddDataLen, const char * const apAddData );
	int ReceiveData( SOCKET socket, int aFlags );


protected:

	unsigned int	m_type;
	unsigned int	m_addDataLen;
	char *			m_pAddData;

};



#endif		// __CNETMESSAGE_H_8fje3_b8dj3_g9w3g__
