#if !defined DEBUG_DRAW
#define DEBUG_DRAW

#include "LinearMath/btIDebugDraw.h"
#include <iostream>

#include "btBulletDynamicsCommon.h"

struct DebugDrawer;
class RenderManager;

class OgreBulletDebugDrawer : public btIDebugDraw
{
   private:
      // Ogre::SceneManager* scene_manager;
      // Ogre::ManualObject* manual_object;
      // Ogre::SceneNode* manual_object_node;
      RenderManager* render_manager;
      DebugDrawer* db;
      int debug_mode;
      int* count;
		
   public:
      //These are the only two methods we need to implement
      OgreBulletDebugDrawer(RenderManager* rm);
      ~OgreBulletDebugDrawer();
      virtual void drawLine(const btVector3& from,const btVector3& to,const btVector3& color);
		
      //These don't need to be implemented unless we want to draw more complex debug objects
      virtual void drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color);
      virtual void reportErrorWarning(const char* warningString);
      virtual void draw3dText(const btVector3& location,const char* textString);
		
      //Set how to draw the debug infor
      virtual void setDebugMode(int debug_mode);
		
      //Not absolutely necessary but implemented for convenience, tells the btDynamicsWorld how to draw the debug info
      virtual int getDebugMode() const;
		
      void clearLines();
};

#endif
