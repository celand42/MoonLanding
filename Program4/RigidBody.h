#if !defined (RIGID_BODY)
#define RIGID_BODY

#include "btBulletDynamicsCommon.h"
#include <string>

class RigidBody
{
   private:
      std::string rigid_body_id;
      btRigidBody* rigid_body;

   public:
      RigidBody(std::string id, btRigidBody* rb);
      virtual ~RigidBody();

      btRigidBody* getRigidBody();
      std::string getKey();
      static int compare_items(RigidBody* rgb1, RigidBody* rgb2);
      static int compare_keys(std::string* key, RigidBody* rgb);
};

#endif
