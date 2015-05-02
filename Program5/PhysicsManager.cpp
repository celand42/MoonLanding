#include "PhysicsManager.h"
#include "RenderManager.h"
#include "BulletSceneNodeMotionState.h"
#include "BulletDebugDrawer.h"

#include <iostream>
using namespace std;

void PhysicsManager::setGravity(float* values)
{
   dynamics_world->setGravity(btVector3(values[0], values[1], values[2]));
}

void PhysicsManager::applyTorqueImpulse(std::string rigid_body_name, float pitch, float yaw, float roll)
{
   RigidBody* rb_torque = rigid_bodies->tableRetrieve(&rigid_body_name);

   if (rb_torque)
   {
      btRigidBody* bt_rb_torque = rb_torque->getRigidBody();
      //bt_rb_torque->applyCentralImpulse(btVector3(pitch, yaw, roll));
      bt_rb_torque->applyTorqueImpulse(btVector3(pitch, yaw, roll));  //applies the torque all at once
   }
}

//step simulation assumes time values are seconds!
void PhysicsManager::stepPhysicsSimulation(float time_incr)
{
   btScalar time_step(time_incr);
   btScalar fixed_time_step(10.0);  //can lose time if this value is too small

   //updateRigidBodies();  //copy scene node transforms into bullet physics transforms
   dynamics_world->stepSimulation(time_step, fixed_time_step);
   dynamics_world->debugDrawWorld();
}

PhysicsManager::PhysicsManager(RenderManager* rm)
{
   rigid_bodies = new TableAVL<RigidBody, std::string>(&RigidBody::compare_items, &RigidBody::compare_keys);
   compound_shapes = new TableAVL<CompoundShape, std::string>(&CompoundShape::compare_items, &CompoundShape::compare_keys);

   render_manager = rm;
   init();

   BulletDebugDrawer* debug_drawer = new BulletDebugDrawer(rm);
   dynamics_world->setDebugDrawer(debug_drawer);
   dynamics_world->getDebugDrawer()->setDebugMode(1);  //1 will display the collision box
}

PhysicsManager::~PhysicsManager()
{
   AVLTreeIterator<CompoundShape>* compound_shape_iter = compound_shapes->tableIterator();
   while(compound_shape_iter->hasNext())
   {
      CompoundShape* cs = compound_shape_iter->next();
      delete cs;
   }
   delete compound_shape_iter;
   delete compound_shapes;

   //cleanup in the reverse order of creation/initialization
   int num_collision_objects = dynamics_world->getNumCollisionObjects();
   btAlignedObjectArray<btCollisionObject*> rigid_bodies_array = dynamics_world->getCollisionObjectArray();

   for (int i = num_collision_objects - 1; i >= 0; i--)
   {
      btRigidBody* rigid_body = btRigidBody::upcast(rigid_bodies_array[i]);
      dynamics_world->removeCollisionObject(rigid_body);

      delete rigid_body->getMotionState();
      delete rigid_body->getCollisionShape();
      //delete rigid_body;
   }

   AVLTreeIterator<RigidBody>* rigid_body_iter = rigid_bodies->tableIterator();
   while(rigid_body_iter->hasNext())
   {
      RigidBody* rgb = rigid_body_iter->next();
      delete rgb;
   }
   delete rigid_body_iter;
   delete rigid_bodies;

   BulletDebugDrawer* debug_drawer = (BulletDebugDrawer*) dynamics_world->getDebugDrawer();
   delete debug_drawer;

   delete dynamics_world;
   delete solver;
   delete dispatcher;
   delete collision_configuration;
   delete overlapping_pair_cache;

   render_manager = NULL;
}

void PhysicsManager::init()
{
   //btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
   //detects when any two bounding regions intersect
   overlapping_pair_cache = new btDbvtBroadphase();

   //collision configuration manages the allocation of memory for collision objects and for collision detection algorithms. Advanced users can create their own configuration.
   collision_configuration = new btDefaultCollisionConfiguration();

   //use the default collision dispatcher (repository of collision detection algorithms). For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
   dispatcher = new btCollisionDispatcher(collision_configuration);

   //the default constraint solver ensures all bodies in the scene are accurately affected by the scene's pervading motions, collisions, and forces. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
   solver = new btSequentialImpulseConstraintSolver();

   //encapsulates the world simulation
   //the dynamics world interface is how to set important world properties
   dynamics_world = new btDiscreteDynamicsWorld(dispatcher, overlapping_pair_cache, solver, collision_configuration);
}

void PhysicsManager::createRigidBodies()
{
   AVLTreeIterator<CompoundShape>* it = compound_shapes->tableIterator();
   
   btCompoundShape* com_shape = 0;
   
   while(it->hasNext())
   {
      CompoundShape* cs = it->next();
      SceneNodeMotion* snm = render_manager->createSceneNodeMotion(cs->getKey());
      
      BulletSceneNodeMotionState* ms = new BulletSceneNodeMotionState(snm, render_manager);
      
      btScalar* m = new btScalar(cs->getMass());
      
      com_shape = cs->getCompoundShape();
      
      btVector3* li = new btVector3(0,0,0);
      
      com_shape->calculateLocalInertia(*m, *li);
      btRigidBody::btRigidBodyConstructionInfo* rbci = new btRigidBody::btRigidBodyConstructionInfo(*m, ms, com_shape, *li);
      btRigidBody* btrb = new btRigidBody(*rbci);
      btrb->setDamping(.1,.1);
      btrb->setActivationState(DISABLE_DEACTIVATION); 
      RigidBody* rb = new RigidBody(cs->getKey(), btrb);
      rigid_bodies->tableInsert(rb);
      
      dynamics_world->addRigidBody(btrb);
   }
   delete it;
}

void PhysicsManager::createCollisionShape(std::string compound_shape_id, std::string collision_shape, float* collision_shape_params, float mass, float* translation, float* rotation)
{

   btCollisionShape* col_shape = NULL;

   if (collision_shape == "sphere")
   {
      col_shape = new btSphereShape(btScalar(collision_shape_params[0]));
   }
   else if (collision_shape == "cylinder_z")
   {
      col_shape = new btCylinderShapeZ(btVector3(collision_shape_params[0], collision_shape_params[1], collision_shape_params[2]));
   }
   else
   {
      col_shape = new btBoxShape(btVector3(collision_shape_params[0], collision_shape_params[1], collision_shape_params[2]));
   }

   btTransform* tr = new btTransform();
   tr->setIdentity();
   
   tr->setOrigin(btVector3(translation[0],translation[1],translation[2]));
   tr->setRotation(btQuaternion(rotation[0], rotation[1], rotation[2], rotation[3]));
   
   CompoundShape* cs = compound_shapes->tableRetrieve(&compound_shape_id);
   
   if(cs == NULL)
   {
      compound_shapes->tableInsert(new CompoundShape(compound_shape_id, new btCompoundShape()));
      cs = compound_shapes->tableRetrieve(&compound_shape_id);
   }

   cs->addChildShape(tr, col_shape, mass);
}

/*
void PhysicsManager::createRigidBody(SceneNodeMotion* scene_node_motion, std::string rigid_body_id, std::string collision_shape, float* collision_shape_params, float _mass)
{
   //create a dynamic rigid body
   btCollisionShape* col_shape = NULL;

   //the constructor takes the half extents

   if (collision_shape == "sphere")
   {
      col_shape = new btSphereShape(btScalar(collision_shape_params[0]));
   }
   else if (collision_shape == "cylinder_x")
   {
      col_shape = new btCylinderShapeX(btVector3(collision_shape_params[0], collision_shape_params[1], collision_shape_params[2]));
   }
   else
   {
      col_shape = new btBoxShape(btVector3(collision_shape_params[0], collision_shape_params[1], collision_shape_params[2]));
   }

   //the constructor copies the scene node transform into the bullet transform
   BulletSceneNodeMotionState* motion_state = new BulletSceneNodeMotionState(scene_node_motion, render_manager);

   btScalar mass(_mass);
   btVector3 local_inertia(0,0,0);  //values filled in next, based on the mass

   col_shape->calculateLocalInertia(mass, local_inertia); //fill in inertia values

   btRigidBody::btRigidBodyConstructionInfo rb_info(mass, motion_state, col_shape, local_inertia);
   btRigidBody* bt_rigid_body = new btRigidBody(rb_info);
   bt_rigid_body->setDamping(0.5,0.5);  //stops the forces, linear and angular (i.e. air resistance)
   bt_rigid_body->setActivationState(DISABLE_DEACTIVATION); //prevent rigid body from sleeping

   RigidBody* rigid_body = new RigidBody(rigid_body_id, bt_rigid_body);
   rigid_bodies->tableInsert(rigid_body);

   dynamics_world->addRigidBody(bt_rigid_body);
}
*/

//rigid bodies may have been moved by the user, so the physics manager must be informed of this
void PhysicsManager::updateRigidBodies()
{
   int num_collision_objects = dynamics_world->getNumCollisionObjects();
   btAlignedObjectArray<btCollisionObject*> rigid_bodies = dynamics_world->getCollisionObjectArray();

   for (int i = num_collision_objects - 1; i >= 0; i--)
   {
      btRigidBody* rigid_body = btRigidBody::upcast(rigid_bodies[i]);
      BulletSceneNodeMotionState* motion_state = (BulletSceneNodeMotionState*) rigid_body->getMotionState();

      btTransform current_transform;
      motion_state->copyNodeTransformIntoBulletTransform();
      motion_state->getWorldTransform(current_transform);
      rigid_body->setWorldTransform(current_transform);
   }
}

