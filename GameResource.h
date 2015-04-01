#if !defined GAME_RESOURCE
#define GAME_RESOURCE

enum GameResourceType{PATH, MESH, AUDIO};

#include "GameHeader.h"
#include <string>

class GameResource
{
   protected:
      uint32 resource_id;  //unique identifier
      std::string resource_group_name;
      std::string resource_file_name;
      GameResourceType resource_type;

   public:
      GameResource(uint32 id, std::string group_name, std::string file_name, GameResourceType type);
      virtual ~GameResource();

      virtual void load() = 0;
      virtual void unload() = 0;

      uint32 getResourceID();
      std::string getResourceGroupName();
      std::string getResourceFileName();
      GameResourceType getResourceType();
};

#endif