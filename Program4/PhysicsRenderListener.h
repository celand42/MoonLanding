#if !defined PHYSICS_RENDER_LISTENER
#define PHYSICS_RENDER_LISTENER

#include "RenderListener.h"

class PhysicsRenderListener: public RenderListener
{
   private:

   public:
      PhysicsRenderListener(RenderManager* render_manager);
      virtual ~PhysicsRenderListener();

      //called as a new frame begins
      bool frameStarted(const Ogre::FrameEvent& event);

};

#endif
