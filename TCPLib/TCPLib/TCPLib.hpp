#if !defined( GUARD_MY_LIB )
#define GUARD_MY_LIB

#include <WinSock2.h>
#include <iostream>
#include <string>
#include <vector>

#pragma comment(lib, "Ws2_32.lib")

class tcp
{
private:
	//Invalid _sockAddress is -1 for a failure
	sockaddr_in			_sockAddress;
	SOCKET				_hSocket;
	SOCKET				_hAccepted;
	std::vector<SOCKET> _clientSockets;
public:

	//Initialize WSA
	int WSASStartup( WORD version, LPWSADATA wsaData)
	{
		int iResult = WSAStartup( version, wsaData );
		if( iResult != 0 ){
			std::cerr << "WSAStartup failed: " << iResult << std::endl;
			return EXIT_FAILURE;
		}

		return 0;
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

		//Connect all of the clients to individual sockets
		//Then push all of the sockets to individual threads for later processing
		_hAccepted = SOCKET_ERROR;

		while( _clientSockets.size() != 2 )
		{
			_hAccepted = accept( _hSocket, NULL, NULL );
			if ( _hAccepted != SOCKET_ERROR )
			{
				_clientSockets.push_back( _hAccepted );
				std::cout << "Client connected" << std::endl;
			}
		}
	}

	void connect_socket()
	{
		if( connect( _hSocket, (SOCKADDR*)&_sockAddress, sizeof( _sockAddress ) ) == SOCKET_ERROR )	{
			std::cerr << "Failed to connect" << std::endl;
		}
		else
			std::cout << "Client connected" << std::endl;
	}

	template <typename T>
	void clientMessage( T msg )
	{
		//unsigned const int MAX = 256;
		//char buf[MAX];
		//strcpy_s( buf, "Hello" );

		//int bytesSent = send( _hSocket, buf, strlen( buf ) + 1, 0 );
		//cout << "Sent: " << bytesSent << " bytes" << endl;

		//	//Returns how many bytes it has gotten
		//int byteRecv = recv( _hSocket, buf, //assigned buffer
		//	MAX, //size of buffer
		//	0 );

		//cout << "Recieved " << byteRecv << " bytes" << endl;
		//cout << "Msg: " << buf << endl;


		send( _hSocket, reinterpret_cast<char *>( &msg ), sizeof( msg ), 0 );
		std::cout << "Sent: " << msg << std::endl;

		bool isPrime = false;
		recv( _hSocket, reinterpret_cast<char *>( &isPrime ), sizeof( isPrime ), 0 );
		if( isPrime )
			std::cout << msg << " is a Prime Number!" << std::endl;
		else
			std::cout << msg << " is a not a Prime Number!" << std::endl;
	}

	//template <typename T>
	void serverMessage()
	{
		//To recieve data
		//unsigned const int MAX = 256;
		//char buf[MAX];

		////Returns how many bytes it has gotten
		//int byteRecv = recv( _hAccepted, buf, //assigned buffer
		//	MAX, //size of buffer
		//	0 );

		//std::cout << "Recieved " << byteRecv << " bytes" << std::endl;
		//std::cout << "Msg: " << buf << std::endl;

		//std::string str = reinterpret_cast<char *>(buf );
		//str += " + server";
		//strcpy_s( buf, str.c_str() );
		//int bytesSent = send( _hAccepted, buf, strlen( buf ) + 1, 0 );
		//std::cout << "Sent: " << bytesSent << " bytes" << std::endl;

		//Change it to 0 and it works but does not send the message back
		//Seems like the memory allocated for the msg is being inproperly accessed by the WindSock2 functions.

		int iResult = 0;
		do {

			int n = 0;
			for ( unsigned i = 0; i < _clientSockets.size(); ++i )
			{
				iResult = recv( _clientSockets[i], reinterpret_cast<char *>( &n ), sizeof( n ), 0 );
				std::cout << "Recieved: " << n << std::endl;

				bool isPrime = isPrimeNumber(n);

				send( _clientSockets[i], reinterpret_cast<char *>( &isPrime ), sizeof( isPrime ), 0 );
				if( isPrime )
					std::cout << "Sent: " << n << " is a Prime Number!" << std::endl;
				else
					std::cout << "Sent: " << n << " is a not a Prime Number!" << std::endl;
			}


		}
		while( iResult > 0 );


	}

	bool isPrimeNumber( int num ){
		bool prime = true;
		for(int i = 3; i <= num; i++){
			prime = true;
			for(int n = 2; n <= i - 1; n++){
				if(i % n == 0){
					prime = false;
				}
			}
		}
		return prime;
	}

	~tcp()
	{
		if ( _clientSockets.size() != 0 )
		{
			for( unsigned i = 0; i < _clientSockets.size(); ++i )
				closesocket( _clientSockets[i] );
		}
		/*if ( _hAccepted != INVALID_SOCKET)
		closesocket( _hAccepted );*/
		if ( _hSocket != INVALID_SOCKET)
			closesocket( _hSocket );

		WSACleanup();
	}

};

#endif