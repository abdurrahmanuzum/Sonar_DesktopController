#pragma once

#include <iostream>
#include <string>
#include <WS2tcpip.h>
#include "globals.h"

#pragma comment ( lib, "ws2_32.lib" )

#define BUFFER_SIZE 8

class TCPClient
{
	public:
		TCPClient();
		~TCPClient();

		bool init();
		bool connectto( const std::string ip_address, const int port );
		int  senddata( std::string data );
		int  getdata( std::string& buffer );

	private:
		WSAData		m_data;
		WORD		m_var;
		SOCKET		m_sock;
		sockaddr_in m_hint;
};

