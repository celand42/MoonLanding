#include "GameConnection.h"
#include "NetworkUtils.h"
#include <string>
#include <iostream>
using namespace std;

#include <stdlib.h>

//CSC2110::Random* GameConnection::rand = new CSC2110::Random();
int GameConnection::num_connections = 0;
StreamSocket** GameConnection::connections = new StreamSocket*[2];

//constructor must have this one parameter for the factory to create it
GameConnection::GameConnection(const StreamSocket& s): TCPServerConnection(s) 
{
   //a new GameConnection is created every connection
}

GameConnection::~GameConnection()
{
}

void GameConnection::run()
{
   //variables must be local to the run method (instance variables crash)
   char server_buffer[101];
   StreamSocket& server_socket = socket();

   int active_thread_id = GameConnection::num_connections;
   int other_thread_id = 1 - active_thread_id;

   GameConnection::connections[active_thread_id] = &server_socket;
   GameConnection::num_connections = GameConnection::num_connections + 1;

   //wait here until all the player connections have been made
   while(GameConnection::num_connections < 2)
   {
      Poco::Thread::sleep(1000);
   }

cout << GameConnection::num_connections << endl;
   StreamSocket* active_player = GameConnection::connections[active_thread_id];
   StreamSocket* other_player = GameConnection::connections[other_thread_id];

   while(true)
   {
      Poco::Thread::sleep(500);
cout << "connection started id: " << active_thread_id << endl;
      NetworkUtils::clearBuffer(server_buffer, 101);

      int num_bytes_received = 0;
      while (true)
      {
         num_bytes_received += active_player->receiveBytes(&server_buffer[num_bytes_received], (101) - num_bytes_received);
         if (server_buffer[num_bytes_received - 1] == 0) break;  //found the null terminator
      }
cout << "num bytes received: " << num_bytes_received << endl;
cout << server_buffer << endl;
      int num_bytes_sent = other_player->sendBytes(server_buffer, 101);  //send the null terminator
cout << "num bytes sent: " << num_bytes_sent << endl;
cout << "connection finished id: " << active_thread_id << endl;
   }
}
