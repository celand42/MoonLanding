#include "GameManager.h"

#include <iostream>
using namespace std;

int main(int argc, char** argv)
{
   GameManager* game_manager = NULL;

   if (argv[1] == NULL)
   {
      cout << "No scene file specified." << endl;
   }
   else
   {
      game_manager = GameManager::getGameManager(argv[1]);  //singleton
   }

   return 0;
}
