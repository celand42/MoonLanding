#if !defined INPUT_RENDER_LISTENER
#define INPUT_RENDER_LISTENER

#include "RenderListener.h"

class InputRenderListener: public RenderListener
{
   private:

   public:
      InputRenderListener(RenderManager* render_manager);
      virtual ~InputRenderListener();

      //called as a new frame begins
      bool frameStarted(const Ogre::FrameEvent& event);

};

#endif
