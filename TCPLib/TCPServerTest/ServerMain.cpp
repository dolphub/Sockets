#include <TCPLib.hpp>
using namespace std;

int main() {
	cout << "Server..." << endl;

	//Initialize WSA
	tcp _tcp;

	WSADATA wsaData;
	_tcp.WSASStartup( MAKEWORD(2,2), &wsaData );

	_tcp.CreateSocket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

	_tcp.SocketAddress( "127.0.0.1", 23152 );

	//Need to remove the second parameter
	_tcp.bind_socket( 1, 0 );

	//Need to get the server to stay online until the client has gone offline
	_tcp.serverMessage();

	//How do I stop the server from closing before all of the clients have been closed
	cout << "Exit" << endl;
}