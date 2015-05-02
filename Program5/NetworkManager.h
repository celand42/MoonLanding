#if !defined NETWORK_MANAGER
#define NETWORK_MANAGER

#include "GameHeader.h"

#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/SocketAddress.h"
#include "Poco/Runnable.h"
#include "Poco/Thread.h"

#include <string>
#include <iostream>
using namespace std;

class GameManager;

class NetworkManager : public Poco::Runnable
{
   private:
      Poco::Net::StreamSocket* client_socket;
      Poco::Net::SocketAddress* socket_address;
      Poco::Thread* game_thread;

      char* network_buffer;
      int network_buffer_length;
      bool network_buffer_status;

      GameManager* game_manager;
      void networkPrepareSend(string message_send);

   public:
      NetworkManager(GameManager* gm, uint32 buffer_size);
      virtual ~NetworkManager();
      void createSocketAddress(string ip_address, uint32 port);

      virtual void run();

      string networkSendReceive(string message_send);
      bool checkNetworkStatus();
};

#endif
