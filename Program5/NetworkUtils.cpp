#include "NetworkUtils.h"

#include <stdlib.h>

#include <sstream>
#include <iostream>
using namespace std;

//length does NOT include the null terminator
int NetworkUtils::bufferLength(const char* buffer)
{
   int count = 0;
   while (buffer[count] != 0)
   {
      count++;
   }
   return count;
}

void NetworkUtils::clearBuffer(char* buffer, int buffer_size)
{
   for (int i = 0; i < buffer_size; i++)
   {
      buffer[i] = 0;
   }
}

int NetworkUtils::a_to_i(const char* str)
{
   return atoi(str);
}

float NetworkUtils::a_to_f(const char* str)
{
   return atof(str);
}

const char* NetworkUtils::i_to_a(int number)
{
   stringstream out;
   out << number;

   string orig_text = out.str();
   int len = orig_text.length();
   char* text = new char[len+1];
   for (int i = 0; i <= len; i++)  //copy over null terminator
   {
      text[i] = orig_text[i];
   }
   return text;
}

const char* NetworkUtils::f_to_a(float number)
{
   stringstream out;
   out << number;

   string orig_text = out.str();
   int len = orig_text.length();
   char* text = new char[len+1];
   for (int i = 0; i <= len; i++)  //copy over null terminator
   {
      text[i] = orig_text[i];
   }
   return text;
}
