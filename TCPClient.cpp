#include "TCPClient.h"



TCPClient::TCPClient()
{
	m_var = MAKEWORD( 2, 2 );
	m_sock = 0; //WARNING
}



TCPClient::~TCPClient()
{
	WSACleanup();

	if ( m_sock != INVALID_SOCKET )
	{
		closesocket( m_sock ); 
	}
}



bool TCPClient::init()
{
	//Initialize winsock
	if ( WSAStartup( m_var, &m_data ) )
	{
		std::cerr << "Failed to create sokcet. ERROR: " << WSAGetLastError() << std::endl;
		return false;
	}


	//Create socket
	m_sock = socket( AF_INET, SOCK_STREAM, 0 );
	if ( m_sock == INVALID_SOCKET )
	{
		std::cerr << "Failed to create socket. ERROR: " << WSAGetLastError() << std::endl;
		return false;
	}

	return true;
}



bool TCPClient::connectto( const std::string ip_address, const int port )
{
	//Fill in a hint structure
	m_hint.sin_family = AF_INET;
	m_hint.sin_port = htons( port );
	inet_pton( AF_INET, ip_address.c_str(), &m_hint.sin_addr );


	//Connect to server
	if ( connect( m_sock, (sockaddr*)&m_hint, sizeof(m_hint) ) == SOCKET_ERROR )
	{
		std::cerr << "Failed to connect server. ERROR: " << WSAGetLastError() << std::endl;
		return false;
	}

	return true;
}



int TCPClient::senddata( std::string data )
{
	int code = send( m_sock, data.c_str(), data.size(), 0 );

	if ( code == SOCKET_ERROR )
	{
		std::cerr << "An error occurred when sending. Error code: " << WSAGetLastError() << std::endl;
	}

	return code;
}



int TCPClient::getdata( std::string& buffer )
{
	char holder[8];

	int bytes_received = recv( m_sock, holder, 8, 0 );

	if (  bytes_received <= 0 )
	{
		std::cerr << "An error occurred when reading. Error code: " << WSAGetLastError() << std::endl;
		return -1;
	}

	buffer = holder;
	return 0;
}






















