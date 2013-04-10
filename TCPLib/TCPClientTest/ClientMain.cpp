#include <TCPLib.hpp>
using namespace std;

int main() {

	cout << "Client..." << endl;

	//Initialize WSA
	tcp _tcp;

	WSADATA wsaData;
	_tcp.WSASStartup( MAKEWORD(2,2), &wsaData );

	_tcp.CreateSocket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

	_tcp.SocketAddress( "127.0.0.1", 23152 );

	_tcp.connect_socket();

	string str = "Client";

	_tcp.clientMessage(str);

	cout << "Exit" << endl;
}