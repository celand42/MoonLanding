#if !defined ANIMATION_RENDER_LISTENER
#define ANIMATION_RENDER_LISTENER

#include "RenderListener.h"

class AnimationRenderListener: public RenderListener
{
   private:

   public:
      AnimationRenderListener(RenderManager* render_manager);
      virtual ~AnimationRenderListener();

      //called as a new frame begins
      bool frameStarted(const Ogre::FrameEvent& event);

};

#endif
