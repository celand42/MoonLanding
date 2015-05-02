#include "NetworkManager.h"
#include "NetworkUtils.h"

void NetworkManager::run()
{
   network_buffer_status = false;

   int num_bytes_sent = client_socket->sendBytes(network_buffer, network_buffer_length + 1);
   NetworkUtils::clearBuffer(network_buffer, network_buffer_length + 1);

   int num_bytes_received = 0;
   while (true)
   {
      num_bytes_received += client_socket->receiveBytes(&network_buffer[num_bytes_received], (network_buffer_length+1)-num_bytes_received);
      if (network_buffer[num_bytes_received - 1] == 0) break;  //found the null terminator
   }

   network_buffer_status = true;
}

void NetworkManager::networkPrepareSend(string message_send)
{
   NetworkUtils::clearBuffer(network_buffer, network_buffer_length + 1);
   const char* message_send_char = message_send.c_str();
   uint32 message_send_length = message_send.length();

   uint32 len = network_buffer_length;
   if (message_send_length < len)
   {
      len = message_send_length;
   }

   for (int i = 0; i < len; i++)
   {
      network_buffer[i] = message_send_char[i];
   }
}

//check this every frame for data from the server
string NetworkManager::networkSendReceive(string message_send)
{
   if (game_thread == NULL)  //get started the first time
   {
      networkPrepareSend(message_send);  //copies the string contents into the network buffer
      game_thread = new Poco::Thread();
      game_thread->start(*this);

      string message_receive("");
      return message_receive;
   }

   if (!game_thread->isRunning() && network_buffer_status)
   {
      string message_receive(network_buffer);
      networkPrepareSend(message_send);  //copies the string contents into the network buffer

      delete game_thread;
      game_thread = new Poco::Thread();
      game_thread->start(*this);

      return message_receive;
   }

   else
   {
      string message_receive("");  //the empty string indicates complete information has not been obtained
      return message_receive;
   }
}

NetworkManager::NetworkManager(GameManager* gm, uint32 buffer_length)
{
   game_manager = gm;

   network_buffer_length = buffer_length;
   network_buffer = new char[network_buffer_length + 1];
   network_buffer_status = false;
}

void NetworkManager::createSocketAddress(string ip_address, uint32 port)
{
   socket_address = new Poco::Net::SocketAddress(ip_address, port);
   client_socket = new Poco::Net::StreamSocket(*socket_address);
   game_thread = NULL;
   network_buffer_status = false;
}

bool NetworkManager::checkNetworkStatus()
{
   return network_buffer_status;
}

NetworkManager::~NetworkManager()
{
   if (game_thread) delete game_thread;
   if (client_socket)
   {
      client_socket->close();
      delete client_socket;
      client_socket = NULL;

      delete socket_address;
      socket_address = NULL;
   }

   delete[] network_buffer;
   game_manager = NULL;
}

