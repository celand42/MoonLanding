#if !defined GAME_CONNECTION
#define GAME_CONNECTION

#include "Poco/Net/StreamSocket.h"
#include "Poco/Net/TCPServerConnection.h"
#include "Poco/Thread.h"

#include "Random.h"

using Poco::Net::StreamSocket;
using Poco::Net::TCPServerConnection;

class GameConnection : public TCPServerConnection
{
   private:
      //static CSC2110::Random* rand;
      static int num_connections;
      static StreamSocket** connections;

   public:
      GameConnection(const StreamSocket& s);
      virtual ~GameConnection();

      virtual void run();
};

#endif