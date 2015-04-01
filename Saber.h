#if !defined SABER
#define SABER

#include "GameHeader.h"
#include "Ogre.h"
#include "RenderListener.h"
#include "CSC2110/ListArray.h"
#include "tinyxml.h"

class GameManager;

class Saber
{
	private:
	
	void init();
	
	GameManager* game_manager;
	
	// Animations
	bool saberExtract;
	bool saberLeft;
	bool saberRight;
	bool saberThrust;
	bool animationFinished;
	
	// Sounds
	bool saberOnSound;
	bool saberOffSound;
	bool saberHumSound;
	bool saberSlashSound;
	bool saberThrustSound;
	
	public:
	
	Saber(GameManager* gm);
	virtual ~Saber();
	
	void keyPressed(std::string key);
	
	void processAnimations(float time_step, ListArray<Ogre::AnimationState>* animation_states);
	
	
	
	
	
	
};













#endif