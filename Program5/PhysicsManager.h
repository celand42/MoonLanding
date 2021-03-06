#if !defined PHYSICS_MANAGER
#define PHYSICS_MANAGER

#include "btBulletDynamicsCommon.h"
#include "RigidBody.h"
#include "CompoundShape.h"
#include "CSC2110/TableAVL.h"
#include <string>

struct SceneNodeMotion;
class RenderManager;

struct CollisionDetect : public btCollisionWorld::ContactResultCallback
{
	RenderManager* render_manager;
	bool sound;
	
	CollisionDetect(RenderManager* rm);
	~CollisionDetect();
	
	btScalar addSingleResult(btManifoldPoint& cp,
        const btCollisionObjectWrapper* colObj0Wrap,
        int partId0,
        int index0,
        const btCollisionObjectWrapper* colObj1Wrap,
        int partId1,
        int index1);
		
	
	
};

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
	  void createSimpleRigidBody(SceneNodeMotion* scene_node, std::string rigid_body_id, std::string collision_shape, double* collision_shape_params, float _mass);
	  void createCompoundRigidBody(SceneNodeMotion* scene_node, std::string rigid_body_id, std::string* collision_shape, double** collision_shape_params, double** collision_pos_params,  float _mass, int numShapes);
      

      void updateRigidBodies();
      //void createRigidBody(SceneNodeMotion* scene_node, std::string rigid_body_id, std::string collision_shape, float* collision_shape_params, float _mass);
      void stepPhysicsSimulation(float time_incr);

      void applyTorqueImpulse(std::string name, float pitch, float yaw, float roll);
	  
	  void applyTorque(std::string name, float pitch, float yaw, float roll);
      void setGravity(float* values);
      void resetBall(std::string name, float x, float y, float z);
	  
	  void applyForce(std::string name, float x, float y, float z);
	  

};

#endif
