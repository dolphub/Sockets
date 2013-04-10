#if !defined( GUARD_MY_LIB )
#define GUARD_MY_LIB

#include <WinSock2.h>
#include <iostream>
#include <string>

#pragma comment(lib, "Ws2_32.lib")

class tcp
{
private:
	//Invalid _sockAddress is -1 for a failure
	sockaddr_in			_sockAddress;
	SOCKET				_hSocket;
	SOCKET				_hAccepted;
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
		_hAccepted = SOCKET_ERROR;

		//Connect all of the clients to individual sockets
		//Then push all of the sockets to individual threads for later processing
		while( _hAccepted == SOCKET_ERROR )
			_hAccepted = accept( _hSocket, NULL, NULL );

		std::cout << "Client connected" << std::endl;
	}

	void connect_socket( )
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
		send( _hSocket, reinterpret_cast<char *>( &msg ), sizeof( msg ), 0 );
		std::cout << "Sent: " << msg << std::endl;

		recv( _hSocket, reinterpret_cast<char *>( &msg ), sizeof( msg ), 0 );
		std::cout << "Recieved: " << msg << std::endl;
	}

	//template <typename T>
	void serverMessage( )
	{
		//Change it to 0 and it works but does not send the message back
		//Seems like the memory allocated for the msg is being inproperly accessed by the WindSock2 functions.
		std::string msg = "";
		recv( _hAccepted, reinterpret_cast<char *>( &msg ), sizeof( msg ), 0 );
		std::cout << "Recieved Sent: " << msg << std::endl;

		//DoStuff();
		std::string str = " server change";
		//msg += str;

		send( _hAccepted, reinterpret_cast<char *>( &msg ), sizeof( msg ), 0 );
		std::cout << "Message Sent: " << msg << std::endl;
	}

	~tcp()
	{
		closesocket( _hAccepted );
		closesocket( _hSocket );
		WSACleanup();
	}

};

#endif