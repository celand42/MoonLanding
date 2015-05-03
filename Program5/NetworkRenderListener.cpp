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
   int score = render_manager->get_score();
    const char* score_ascii = GameManager::i_to_a(score);
   string opponents_score = render_manager->networkSendReceive(score_ascii);
   //cout << "Your score: " << score << endl;
   //cout << "Their score: " << opponents_score << endl;
   if(opponents_score != "")
      render_manager->set_score(atoi(opponents_score.c_str()));
   delete[] score_ascii;
   // //int scroll_bar_setting = render_manager->getScrollBarSetting();
   // string buttonPressed = render_manager->getButtonSetting();
   // // if(buttonPressed == "Force")
   // // {
   // //    render_manager->setButtonSetting("");
   // //    cout << buttonPressed << endl;
   // //    exit(0);
   // // }
   // cout << "buttonPressed" << endl;
   // string newButton = render_manager->networkSendReceive(buttonPressed);
   // // if(buttonPressed != "")
   // //   

   

   // cout << "new button" << endl;
   // cout << newButton << endl;
   // if(newButton == "Force")
   // {
   //    render_manager->force(-1);

   // }
   
   
     
   

   return getRenderStatus();
}

