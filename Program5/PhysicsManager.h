#if !defined PHYSICS_MANAGER
#define PHYSICS_MANAGER

#include "btBulletDynamicsCommon.h"
#include "RigidBody.h"
#include "CompoundShape.h"
#include "CSC2110/TableAVL.h"
#include <string>

struct SceneNodeMotion;
class RenderManager;

class PhysicsManager
{
   private:
      RenderManager* render_manager;
      TableAVL<RigidBody, std::string>* rigid_bodies;
      TableAVL<CompoundShape, std::string>* compound_shapes;

      //collision detection
      btBroadphaseInterface* overlapping_pair_cache;
      btDefaultCollisionConfiguration* collision_configuration;
      btCollisionDispatcher* dispatcher;

      btSequentialImpulseConstraintSolver* solver;
      btDiscreteDynamicsWorld* dynamics_world;

      void init();

   public:
      PhysicsManager(RenderManager* rm);
      virtual ~PhysicsManager();

      void createCollisionShape(std::string physics_object_id, std::string collision_shape, float* collision_shape_params, float mass, float* translation, float* rotation);
      void createRigidBodies();

      void updateRigidBodies();
      //void createRigidBody(SceneNodeMotion* scene_node, std::string rigid_body_id, std::string collision_shape, float* collision_shape_params, float _mass);
      void stepPhysicsSimulation(float time_incr);

      void applyTorqueImpulse(std::string name, float pitch, float yaw, float roll);
      void setGravity(float* values);

};

#endif
