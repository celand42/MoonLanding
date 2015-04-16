#include "ScriptManager.h"
#include "RenderManager.h"

#include <iostream>

ScriptManager::ScriptManager(RenderManager* rm)
{
   render_manager = rm;
   L = luaL_newstate();
   luaL_openlibs(L);
}

ScriptManager::~ScriptManager()
{
   lua_close(L);
   render_manager = NULL;
}

/*
void ScriptManager::executeScript(std::string& script_name, std::string& function_name, int num_params, int num_results, double* params, double* results) 
{
   //pass an array of parameters, num parameters, an array of results, num results

   //change this to dofile instead
   luaL_dofile(L, script_name.c_str());
   //luaL_loadfile(L, script_name.c_str());
   //lua_pcall(L, 0, 0, 0);   //priming run?!  this has to be here!!

   lua_getglobal (L, function_name.c_str());
   for (int i = 0; i < num_params; i++)
   {
      lua_pushnumber(L, params[i]);
   }
   lua_pcall(L, num_params, num_results, 0);  //number of arguments, number of results
   
   //remember pop is in the reverse order
   for (int i = num_results - 1; i >= 0; i--)
   {
      results[i] = lua_tonumber(L, -1);
      lua_pop(L, 1);
   }
}
*/

void ScriptManager::executeScript(std::string& script_file_name, std::string& script_function_name, int num_inputs, int num_outputs, const char** inputs, char** outputs) 
{
   
   luaL_dofile(L, script_file_name.c_str());
   lua_getglobal (L, script_function_name.c_str());

   for (int i = 0; i < num_inputs; i++)
   {
      lua_pushstring(L, inputs[i]);
   }

   lua_pcall(L, num_inputs, num_outputs, 0);
   
   //remember pop is in the reverse order
   for (int i = num_outputs - 1; i >= 0; i--)
   {
      const char* temp = lua_tostring(L, -1);
      strcpy(outputs[i], temp);
      lua_pop(L, 1);
   }
}
