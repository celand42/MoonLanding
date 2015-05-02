#if !defined (BULLET_SCENE_NODE_MOTION_STATE)
#define BULLET_SCENE_NODE_MOTION_STATE

#include "btBulletDynamicsCommon.h"

struct SceneNodeMotion;
class RenderManager;

class BulletSceneNodeMotionState : public btMotionState 
{
   private:
      RenderManager* render_manager;
      SceneNodeMotion* scene_node_motion;
      btTransform bullet_transform;

   public:
      BulletSceneNodeMotionState(SceneNodeMotion* node, RenderManager* render_manager);
      virtual ~BulletSceneNodeMotionState();

      //required btMotionState methods
      virtual void getWorldTransform(btTransform& worldTrans) const;
      virtual void setWorldTransform(const btTransform& worldTrans);

      void copyBulletTransformIntoNodeTransform();
      void copyNodeTransformIntoBulletTransform();
};

#endif
