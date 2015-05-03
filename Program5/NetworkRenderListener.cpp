#include "NetworkRenderListener.h"
#include "RenderManager.h"
#include "GameManager.h"

#include <iostream>
using namespace std;

NetworkRenderListener::NetworkRenderListener(RenderManager* rm) : RenderListener(rm)
{
}

NetworkRenderListener::~NetworkRenderListener()
{
}

//Ogre notifies this class when frames are processed
//as long as this method returns true, the game will continue (automatically done by Ogre)
bool NetworkRenderListener::frameStarted(const Ogre::FrameEvent& event) 
{
   float time_step = event.timeSinceLastFrame;
   RenderManager* render_manager = getRenderManager();

   //int scroll_bar_setting = render_manager->getScrollBarSetting();
	//cout << scroll_bar_setting << endl;
   //const char* scroll_bar_ascii = GameManager::i_to_a(scroll_bar_setting);
   //string gravity_str("0, ");
   //gravity_str.append(scroll_bar_ascii);
   //delete[] scroll_bar_ascii;
   //gravity_str.append(", 0");

   //string gravity_str_ = render_manager->networkSendReceive(gravity_str);
	//cout << gravity_str_.c_str() << " gravity " << endl;
   //render_manager->setGravity(gravity_str_);

   return getRenderStatus();
}

