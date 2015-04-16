#include "RenderListener.h"
#include "RenderManager.h"

#include <iostream>
using namespace std;

RenderListener::RenderListener(RenderManager* rm)
{
   render_manager = rm;
   render = true;
}

RenderListener::~RenderListener()
{
   render_manager = NULL;
}

void RenderListener::stopRendering()
{
   render = false;
}

bool RenderListener::getRenderStatus()
{
   return render;
}

RenderManager* RenderListener::getRenderManager()
{
   return render_manager;
}

//Ogre notifies this class when frames are processed
//as long as this method returns true, the game will continue (automatically done by Ogre)

//called as a new frame begins
bool RenderListener::frameStarted(const Ogre::FrameEvent& event) {return getRenderStatus();};

//called after the back buffer is flipped and the scene is presented to the display
bool RenderListener::frameRenderingQueued(const Ogre::FrameEvent& event) {return getRenderStatus();};

//called after the scene has rendered but before the back buffer is drawn
bool RenderListener::frameEnded(const Ogre::FrameEvent& event) {return getRenderStatus();};



