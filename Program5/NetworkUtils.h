#if !defined NETWORK_UTILS
#define NETWORK_UTILS

#include <string>

class NetworkUtils
{
   public:
      static int bufferLength(const char* buffer);
      static void clearBuffer(char* buffer, int buffer_size);
      static int a_to_i(const char* str);
      static float a_to_f(const char* str);
      static const char* i_to_a(int number);
      static const char* f_to_a(float number);
};

#endif
