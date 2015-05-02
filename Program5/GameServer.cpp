#include "Poco/Net/ServerSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Net/TCPServer.h"
#include "Poco/Net/TCPServerConnection.h"
#include "Poco/Net/TCPServerConnectionFactory.h"

using Poco::Net::ServerSocket;
using Poco::Net::SocketAddress;
using Poco::Net::TCPServer;
using Poco::Net::TCPServerConnection;
using Poco::Net::TCPServerConnectionFactory;
using Poco::Net::TCPServerConnectionFactoryImpl;

#include "GameConnection.h"
#include <iostream>
using namespace std;

int main()
{
   SocketAddress socket_addr("localhost", 6789);
   ServerSocket server_socket(socket_addr);

   TCPServer tcp_server(new TCPServerConnectionFactoryImpl<GameConnection>(), server_socket);
   tcp_server.start();
   cout << "here " << endl;
   //need an infinite loop here to keep the server running
   while(true)
   {
      
   }

   tcp_server.stop();
}