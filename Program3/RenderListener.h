#if !defined RENDER_LISTENER
#define RENDER_LISTENER

#include "Ogre.h"
class RenderManager;

class RenderListener: public Ogre::FrameListener
{
   private:
      RenderManager* render_manager;
      bool render;

   public:
      RenderListener(RenderManager* render_manager);
      virtual ~RenderListener();

      //called as a new frame begins
      virtual bool frameStarted(const Ogre::FrameEvent& event);

      //called after the back buffer is flipped and the scene is presented to the display
      virtual bool frameRenderingQueued(const Ogre::FrameEvent& event);

      //called after the scene has rendered but before the back buffer is drawn
      virtual bool frameEnded(const Ogre::FrameEvent& event);

      void stopRendering();
      bool getRenderStatus();

   protected:

      RenderManager* getRenderManager();
};

#endif
