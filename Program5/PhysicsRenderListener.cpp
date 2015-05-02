#include "PhysicsRenderListener.h"
#include "RenderManager.h"

#include <iostream>
using namespace std;

PhysicsRenderListener::PhysicsRenderListener(RenderManager* rm) : RenderListener(rm)
{
}

PhysicsRenderListener::~PhysicsRenderListener()
{
}

//Ogre notifies this class when frames are processed
//as long as this method returns true, the game will continue (automatically done by Ogre)
bool PhysicsRenderListener::frameStarted(const Ogre::FrameEvent& event) 
{
   float time_step = event.timeSinceLastFrame;
   RenderManager* render_manager = getRenderManager();

   render_manager->stepPhysicsSimulation(time_step);
   return getRenderStatus();
}

