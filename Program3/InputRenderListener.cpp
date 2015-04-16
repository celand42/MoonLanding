#include "InputRenderListener.h"
#include "RenderManager.h"

#include <iostream>
using namespace std;

InputRenderListener::InputRenderListener(RenderManager* rm) : RenderListener(rm)
{
}

InputRenderListener::~InputRenderListener()
{
}

//Ogre notifies this class when frames are processed
//as long as this method returns true, the game will continue (automatically done by Ogre)
bool InputRenderListener::frameStarted(const Ogre::FrameEvent& event) 
{
   float time_step = event.timeSinceLastFrame;

   getRenderManager()->checkForInput(time_step);
   getRenderManager()->updateAudio();

   return getRenderStatus();
}

