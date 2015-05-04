#include "AIManager.h"
#include "ScriptManager.h"
#include "NetworkUtils.h"

AIManager::AIManager(GameManager* gm)
{
   game_manager = gm;
   script_manager = new ScriptManager();
}

AIManager::~AIManager()
{
   delete script_manager;
   script_manager = NULL;
   game_manager = NULL;
}
char AIManager::executeAutoPilot(float x, float y, int your_score, int opponent_score)
{

   cout << x << " XXXXXXX" << endl;
   cout << y << " YYYYYYYY" << endl;

   string script_file_name = "assets/lua_scripts/placement.lua";
   string script_function_name = "statistics";
   int num_inputs = 3;
   const char** inputs = new const char*[num_inputs];
   cout << "here" << endl;
   inputs[0] = NetworkUtils::f_to_a((float)your_score/(float)(your_score + opponent_score));
   inputs[1] = NetworkUtils::f_to_a(x);
   inputs[2] = NetworkUtils::f_to_a(y);
   //cout << inputs[0] << endl;
   int num_outputs = 1;
   
   char** outputs = new char*[num_outputs];

   for (int i = 0; i < num_outputs; i++)
   {
      outputs[i] = new char[21];
   }
   //script_manager->executeScript(script_file_name, script_file_name, num_inputs, num_outputs, inputs, outputs );

   for (int i = 0; i < num_inputs; i++)
   {
      delete[] inputs[i];
   }

   delete[] inputs;

   //cout << outputs[0] << endl;
   for (int i = 0; i < num_outputs; i++)
   {
      delete[] outputs[i];
   }

   delete[] outputs;


   // if (y <= 20 && y >= 10)
   // {
   //    if (x < 0)
   //       return 'L';
   //    else
   //       return 'R';
   // }

   return 'N';
}

void AIManager::executeMotionScript(float* r_i, float* r_f, float* v_i, float* v_f, float* gravity, float t, float* a_req)
{
   string script_file_name = "assets/lua_scripts/equations_of_motion_secant_method_root_finding.lua";
   string script_function_name = "solveEquationsOfMotion";

   int num_inputs = 16;
   int num_outputs = 6;

   const char** inputs = new const char*[16];
   inputs[0] = NetworkUtils::f_to_a(r_i[0]);
   inputs[1] = NetworkUtils::f_to_a(r_i[1]);
   inputs[2] = NetworkUtils::f_to_a(r_i[2]);

   inputs[3] = NetworkUtils::f_to_a(r_f[0]);
   inputs[4] = NetworkUtils::f_to_a(r_f[1]);
   inputs[5] = NetworkUtils::f_to_a(r_f[2]);

   inputs[6] = NetworkUtils::f_to_a(v_i[0]);
   inputs[7] = NetworkUtils::f_to_a(v_i[1]);
   inputs[8] = NetworkUtils::f_to_a(v_i[2]);

   inputs[9] = NetworkUtils::f_to_a(v_f[0]);
   inputs[10] = NetworkUtils::f_to_a(v_f[1]);
   inputs[11] = NetworkUtils::f_to_a(v_f[2]);

   inputs[12] = NetworkUtils::f_to_a(gravity[0]);
   inputs[13] = NetworkUtils::f_to_a(gravity[1]);
   inputs[14] = NetworkUtils::f_to_a(gravity[2]);

   inputs[15] = NetworkUtils::f_to_a(t);

   char** outputs = new char*[num_outputs];
   int output_len = 20;
   for (int i = 0; i < num_outputs; i++)
   {
      outputs[i] = new char[output_len + 1];
   }

   script_manager->executeScript(script_file_name, script_function_name, num_inputs, num_outputs, inputs, outputs);

   for (int i = 0; i < num_inputs; i++)
   {
      delete[] inputs[i];
   }

   delete[] inputs;

   a_req[0] = NetworkUtils::a_to_f(outputs[0]);
   a_req[1] = NetworkUtils::a_to_f(outputs[1]);
   a_req[2] = NetworkUtils::a_to_f(outputs[2]);
   a_req[3] = NetworkUtils::a_to_f(outputs[3]);
   a_req[4] = NetworkUtils::a_to_f(outputs[4]);
   a_req[5] = NetworkUtils::a_to_f(outputs[5]);

   for (int i = 0; i < num_outputs; i++)
   {
      delete[] outputs[i];
   }

   delete[] outputs;
}

void AIManager::executeFindPathScript(string vertices_file, string edges_file, string start_vertex, string end_vertex)
{

   string script_file_name = "assets/lua_scripts/find_path.lua";
   string script_function_name = "findPath";
   
   int num_inputs = 4;
   int num_outputs = 5;  //this must be exact

   const char** files = new const char*[num_inputs];
   files[0] = vertices_file.c_str();
   files[1] = edges_file.c_str();
   files[2] = start_vertex.c_str();
   files[3] = end_vertex.c_str();

   char** path = new char*[num_outputs];
   for (int i = 0; i < num_outputs; i++)
   {
      path[i] = new char[40];
	  for(int j = 0; j < 40; j++) path[i][j]=0;
   }

   script_manager->executeScript(script_file_name, script_function_name, num_inputs, num_outputs, files, path);

   for (int i = 0; i < num_outputs; i++)
   {
      cout << path[i] << endl;
   }

    for (int i = 0; i < num_outputs; i++)
   {
      delete[] path[i];
   }

   delete[] files;
   delete[] path;
}

