#include "GameManager.h"
#include "RenderManager.h"
#include "ResourceManager.h"
#include "LogManager.h"
#include "InputManager.h"
#include "Saber.h"

#include <stdlib.h>  //atoi, atof (actual include not needed?)

Saber::Saber(GameManager* gm)
{
	game_manager = gm;
	score = 0;
	init();
}

Saber::~Saber()
{
	game_manager = NULL;
}

void Saber::init()
{
	saberExtract = true;
	saberLeft = false;
	saberRight = false;
	saberThrust = false;
	animationFinished = true;
	
	saberOnSound = true;
	saberOffSound = true;
	saberHumSound = true;
	saberSlashSound = true;
	saberThrustSound = true;
	
	resetAnimate = false;
	
	gameOver = false;
	gameOverTimer = 0.0;
}

void Saber::keyPressed(std::string key)
{
	if (key == "ESCAPE")
	{
		if(animationFinished)
		{
			animationFinished = false;
			
			if(!saberExtract)
				saberExtract = true;
			else
				saberExtract = false;
		}
	}
	
	else if (key == "A" || key == "LEFT")
	{
		if (!saberRight)
			saberLeft = true;
	}
   
   else if (key == "D" || key == "RIGHT")
   {
	   if (!saberLeft)
			saberRight = true;
   }
   
   else if (key == "W" || key == "UP")
	   saberThrust = true;
}


void Saber::processAnimations(float time_step, ListArray<Ogre::AnimationState>* animation_states)
{
	if (gameOver)
	{
		gameOverTimer += time_step;
	}
	
   // Animation State Indices
   // 1: Hilt
   // 2: Swing Left
   // 3: Swing Right
   // 4: Thrust
   // 5: Blade
   
   if (resetAnimate)
   {
		resetAnimate = false;
		animation_states->get(1)->setTimePosition(0);
		animation_states->get(5)->setTimePosition(0);
		saberOnSound = true;
   }
   
   // Animate hilt flick and blade extension
   if (saberExtract)
   {   
		if (animation_states->get(1)->getTimePosition() + time_step < animation_states->get(1)->getLength())
			animation_states->get(1)->addTime(time_step);

		if (animation_states->get(1)->getTimePosition() > 0.35 &&
			animation_states->get(5)->getTimePosition() < 0.5)
		{
			if (saberOnSound)
			{
				saberOnSound = false;
				saberOffSound = true;
				game_manager->playAudio(21,1);	// Saber on noise
			}
			
			animation_states->get(5)->addTime(time_step);
		}
		
		if (animation_states->get(5)->getTimePosition() > 0.5)
		{
			animation_states->get(5)->setTimePosition(0.5);
			animationFinished = true;
		}

		
   }
   
   // Animate blade retraction
	else
	{		
		if (animation_states->get(5)->getTimePosition() >= 0.5)
		{
			if (saberOffSound)
			{
				saberOnSound = true;
				saberOffSound = false;
				game_manager->playAudio(22,1);	// Saber off noise
			}
			
			animation_states->get(5)->addTime(time_step);
		}
		
		if (animation_states->get(1)->getTimePosition() > 0.99)
			animation_states->get(1)->addTime(time_step);
		
		if (animation_states->get(5)->getTimePosition() < 0.5)
		{
			animation_states->get(5)->setTimePosition(0);
			animationFinished = true;	
			gameOver = true;
		}
	}
	
	// Swing left
	if (saberLeft)
	{
		if (saberSlashSound && saberExtract && animationFinished)
		{
			saberSlashSound = false;
			game_manager->playAudio(23,1);	// Saber slash noise
		}
		
		if (animation_states->get(2)->getTimePosition() + time_step < animation_states->get(2)->getLength())
		{
			animation_states->get(2)->addTime(time_step);
		}
		
		else
		{
				saberLeft = false;
				saberSlashSound = true;
				animation_states->get(2)->setTimePosition(0);
		}
	}
	
	// Swing right
	else if (saberRight)
	{
		if (saberSlashSound && saberExtract && animationFinished)
		{
			saberSlashSound = false;
			game_manager->playAudio(23,1);	// Saber slash noise
		}
		
		if (animation_states->get(3)->getTimePosition() + time_step < animation_states->get(3)->getLength())
			animation_states->get(3)->addTime(time_step);
		
		else 
		{
			saberSlashSound = true;
			saberRight = false;
			animation_states->get(3)->setTimePosition(0);
		}		
	}
	
	// Thrust blade
	else if (saberThrust)
	{
		if (saberThrustSound && saberExtract && animationFinished)
		{
			saberThrustSound = false;
			game_manager->playAudio(24,1);	// Saber stab noise
			game_manager->playAudio(25,1);	// Wilhelm noise
		}
		
		if (animation_states->get(4)->getTimePosition() + time_step < animation_states->get(4)->getLength())
			animation_states->get(4)->addTime(time_step);
		
		else 
		{
				saberThrust = false;
				saberThrustSound = true;
				animation_states->get(4)->setTimePosition(0);
		}		
	}
}

void Saber::resetAnimation()
{
	resetAnimate = true;
}

bool Saber::gameFinished()
{
	return gameOverTimer > 1;
}

void Saber::increaseScore()
{
	score += 10;
}

int Saber::getScore()
{
	return score;
}