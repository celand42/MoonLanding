#include "AnimationRenderListener.h"
#include "RenderManager.h"

#include <iostream>
using namespace std;

AnimationRenderListener::AnimationRenderListener(RenderManager* rm) : RenderListener(rm)
{
}

AnimationRenderListener::~AnimationRenderListener()
{
}

//Ogre notifies this class when frames are processed
//as long as this method returns true, the game will continue (automatically done by Ogre)
bool AnimationRenderListener::frameStarted(const Ogre::FrameEvent& event) 
{
   float time_step = event.timeSinceLastFrame;
   //this is the memory address of the object invoked to call the method where I am
   getRenderManager()->processAnimations(time_step);
   return getRenderStatus();
}

//bool AnimationRenderListener::frameEnded(const Ogre::FrameEvent& event) {return getRenderStatus();}
//bool AnimationRenderListener::frameRenderingQueued(const Ogre::FrameEvent& event) {return getRenderStatus();}
