#if !defined NETWORK_RENDER_LISTENER
#define NETWORK_RENDER_LISTENER

#include "RenderListener.h"

class NetworkRenderListener: public RenderListener
{
   private:

   public:
      NetworkRenderListener(RenderManager* render_manager);
      virtual ~NetworkRenderListener();

      //called as a new frame begins
      bool frameStarted(const Ogre::FrameEvent& event);

};

#endif
