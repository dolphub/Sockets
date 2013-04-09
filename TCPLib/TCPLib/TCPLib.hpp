#if !defined( __TCP_LIB )
#include <WinSock2.h>
#include <iostream>

class tcp
{
private:
	//Invalid _sockAddress is -1 for a failure
	sockaddr_in			_sockAddress;
	SOCKET				_hSocket;
	SOCKET				_hAccepted;
public:
	
	//Initialize WSA
	int WSASStartup( int i, int x, WSAData wsaData)
	{
		int iResult = WSAStartup( MAKEWORD(i,x), &wsaData );
		if( iResult != 0 ){
			std::cerr << "WSAStartup failed: " << iResult << std::endl;
			return EXIT_FAILURE;
		}
	}

	//Create the Socket
	void CreateSocket( int sockaddr, int type, int protoType )
	{
		
		_hSocket = socket( sockaddr, type, protoType );
		if( _hSocket == INVALID_SOCKET )
		{
			std::cerr << "Error: socket(): " << WSAGetLastError() << std::endl;
			_sockAddress.sin_family = -1;
		}
		else
			_sockAddress.sin_family = sockaddr;
	}

	//Fill in the rest of the socket address information for creation
	void SocketAddress( const char * ipAddress, u_short hton )
	{
		_sockAddress.sin_addr.s_addr = inet_addr(ipAddress);
		_sockAddress.sin_port = htons(hton);
	}

	//Bind the client to the server
	//I dont know what the value is
	void bind_socket( int value, int bufferSize )
	{
		if( bind( _hSocket, (SOCKADDR*)&_sockAddress, sizeof( _sockAddress ) ) == SOCKET_ERROR ) {
			std::cerr << "Failed to bind" << std::endl;
			//exitCode = EXIT_FAILURE;
		}

		if( listen( _hSocket, value ) )
		{
			std::cerr << "Failed to listen" << std::endl;
			//exitCode = EXIT_FAILURE;
		}

		std::cout << "Waiting for a connetion" << std::endl;
		_hAccepted = SOCKET_ERROR;

		//Connect all of the clients to individual sockets
		//Then push all of the sockets to individual threads for later processing
		while( _hAccepted == SOCKET_ERROR )
			_hAccepted = accept( _hSocket, NULL, NULL );

		std::cout << "Client connected" << std::endl;
	}

	template <typename T>
	void recieveMessage( T msg )
	{
		unsigned const int MAX = 256;
		char buf[MAX];

		//Returns how many bytes it has gotten
		int byteRecv = recv( _hAccepted, buf, //assigned buffer
			MAX, //size of buffer
			0 );

		std::cout << "Recieved " << byteRecv << " bytes" << std::endl;
		std::cout << "Msg: " << buf << std::endl;

		//strcpy( buf, "Recieved" );
		//int bytesSent = send( _hAccepted, buf, strlen( buf ) + 1, 0 );
		//std::cout << "Sent: " << bytesSent << " bytes" << std::endl;

		recv( _hAccepted, reinterpret_cast<T*>( &msg ), sizeof( msg ), 0 );
	}

	template <typename T>
	void sendMessage( T msg )
	{
		send( _hAccepted, reinterpret_cast<T*>( &msg ), sizeof( msg ), 0 );
	}

	~tcp()
	{
		closesocket( _hSocket );
		WSACleanup();
	}

};

#endif