#if !defined AI_MANAGER
#define AI_MANAGER

#include "GameHeader.h"

#include <string>
#include <iostream>
using namespace std;

class GameManager;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
class ScriptManager;

class AIManager
{
   private:
      GameManager* game_manager;
      ScriptManager* script_manager;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                     

   public:
      AIManager(GameManager* gm);
      virtual ~AIManager();

      void executeMotionScript(float* r_i, float* r_f, float* v_i, float* v_f, float* gravity, float t, float* a_req);
	  void executeFindPathScript(string vertices_file, string edges_file, string start_vertex, string end_vertex);
};

#endif
