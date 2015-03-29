#include "RenderManager.h"
#include "GameManager.h"
#include "AnimationRenderListener.h"
#include "InputRenderListener.h"

#include <iostream>
using namespace std;

using namespace Ogre;

void RenderManager::joystickAxisMoved(std::string axis, int amount)
{
   amount = amount/10000;

   //get root's (one) child and change its rotation based on mouse movement
   //SceneNode* root = scene_manager->getRootSceneNode();
   //SceneNode* scene_parent_node = (SceneNode*) root->getChild(0);
   //const Quaternion& current_quat = scene_parent_node->getOrientation();

   Camera* camera = scene_manager->getCamera("Camera");
   const Quaternion& current_quat = camera->getOrientation();
   const Vector3& current_pos = camera->getPosition();
   const Vector3& current_dir = camera->getDirection();

   Vector3 x_axis(1, 0, 0);
   Vector3 y_axis(0, 1, 0);
   Vector3 z_axis(0, 0, 1);

   int degree = amount;  

   if (axis == "R_NS")
   {
      Quaternion quat(Degree(degree), x_axis);
      Quaternion new_quat = quat*current_quat;  //quaternions to the right are applied first
      //scene_parent_node->rotate(new_quat);  //here, new_quat is applied to the right
      //scene_parent_node->setOrientation(new_quat);
      camera->setOrientation(new_quat);
   }
   else if (axis == "R_EW")
   {
      Quaternion quat(Degree(degree), y_axis);
      Quaternion new_quat = quat*current_quat;
      //scene_parent_node->rotate(new_quat);
      //scene_parent_node->setOrientation(new_quat);
      camera->setOrientation(new_quat);
   }

   //for movement, move in the direction that the camera is pointing, but only in the xz-plane
   //use current_dir to update the camera position
   else if (axis == "L_NS")
   {
      //move in the direction the camera is pointing
      Vector3 move = amount * current_dir;
      Vector3 new_pos = current_pos + move;
      camera->setPosition(new_pos);
   }
   else if (axis == "L_EW") 
   {
/*
      Vector3 move(amount, 0, 0);
      Vector3 new_pos = current_pos + move;
      camera->setPosition(new_pos);
*/
   }
}

void RenderManager::mouseMoved(uint32 mouse_x, uint32 mouse_y, int mouse_rel_x, int mouse_rel_y)
{
   //get root's (one) child and change its rotation based on mouse movement
   SceneNode* root = scene_manager->getRootSceneNode();
   SceneNode* scene_parent_node = (SceneNode*) root->getChild(0);
   const Quaternion& current_quat = scene_parent_node->getOrientation();
	
   Vector3 x_axis(1, 0, 0);
   //Vector3 y_axis(0, 1, 0);
   //Vector3 z_axis(0, 0, 1);
   //int x_degree = mouse_rel_x;
   int y_degree = mouse_rel_y;

   Quaternion x_quat(Degree(y_degree), x_axis);
   //Quaternion y_quat(Degree(x_degree), y_axis);
   //Quaternion z_quat(Degree(x_degree), z_axis);
   Quaternion new_quat = x_quat*current_quat;  //quaternions to the right are applied first
   //scene_parent_node->rotate(new_quat);  //here, new_quat is applied to the right
   scene_parent_node->setOrientation(new_quat);
}

void RenderManager::checkForInput(float time_step)
{
   game_manager->checkForInput(time_step);
}

void RenderManager::initialiseResourceGroup(std::string group_name)
{
   try
   {
      ResourceGroupManager& rgm = ResourceGroupManager::getSingleton();
      rgm.initialiseResourceGroup(group_name);  //pre-load the resources located in the specific paths (parse scripts)
      game_manager->logComment("Scripts parsed successfully.");
   }
   catch (Ogre::Exception& e)
   {
      game_manager->logComment(e.what());
      ASSERT(false);
   }
}

void RenderManager::loadResourceGroup(std::string group_name)
{
   try
   {
      ResourceGroupManager& rgm = ResourceGroupManager::getSingleton();
      rgm.loadResourceGroup(group_name, true, true);  //load the resources in the specific paths
      game_manager->logComment("Meshes loaded successfully.");
   }
   catch (Ogre::Exception& e)
   {
      game_manager->logComment(e.what());
      ASSERT(false);
   }
}

void RenderManager::unloadResourceGroup(std::string group_name)
{
   try
   {
      ResourceGroupManager& rgm = ResourceGroupManager::getSingleton();
      rgm.destroyResourceGroup(group_name);   //completely remove resource information (could use clear instead of destroy)
      game_manager->logComment("Resources unloaded successfully.");
   }
   catch (Ogre::Exception& e)
   {
      game_manager->logComment(e.what());
      ASSERT(false);
   }
}

void RenderManager::addPathResource(std::string file_name, std::string path_type, std::string group_name)
{
   ResourceGroupManager& rgm = ResourceGroupManager::getSingleton();
   rgm.addResourceLocation(file_name, path_type, group_name);  //"file_name" is the path
}
      
void RenderManager::addMeshResource(std::string file_name, std::string mesh_str, std::string group_name)
{
   ResourceGroupManager& rgm = ResourceGroupManager::getSingleton();
   rgm.declareResource(file_name, mesh_str, group_name);
}

void RenderManager::init()
{
   root = NULL;
   window = NULL;
   scene_manager = NULL;
   render_listener = NULL;
   animationFinished = true;

   try
   {
      root = OGRE_NEW Ogre::Root("","");    //resource/config files go here (we aren't using them)
      root->loadPlugin("RenderSystem_GL");  //prepares external dlls for later use

      Ogre::RenderSystem* render_system = root->getRenderSystemByName("OpenGL Rendering Subsystem"); //just returns a pointer to an uninialized render system
      if (!render_system)
      {
         //log that OpenGL is not available (probably can't continue)
         return;
      }

      root->setRenderSystem(render_system);
      //manually set configuration options
      render_system->setConfigOption("Full Screen", "No");
      render_system->setConfigOption("Video Mode", "800 x 600 @ 32-bit colour");

      //initialize render system
      //automatically create the window and give it a title
      window = root->initialise(true, "Anakin's Dark Side");  

      //can create BSP or octree scene managers
      scene_manager = root->createSceneManager(Ogre::ST_GENERIC, "Default Scene Manager");
      window->getCustomAttribute("WINDOW", &window_handle);

      //the Ogre viewport corresponds to a clipping region into which the contents of the camera view will be rendered in the window on each frame
      //by default, the size of the viewport matches the size of the window, but the viewport can be cropped
      //the camera represents a view into an existing scene and the viewport represents a region into which an existing camera will render its contents
      camera = scene_manager->createCamera("Camera");

      //z-order (for possible overlapping), left, top, width, height
      viewport = window->addViewport(camera, 0, 0, 0, 1.0, 1.0);  //assign a camera to a viewport (can have many cameras and viewports in a single window)
      viewport->setBackgroundColour(Ogre::ColourValue(.3, .3, .61, 1));

      float actual_width = Ogre::Real(viewport->getActualWidth());
      float actual_height = Ogre::Real(viewport->getActualHeight());
      float aspect_ratio = actual_width/actual_height;
      camera->setAspectRatio(aspect_ratio);

/*
      Ogre::Camera* camera2 = scene_manager->createCamera("Camera 2");
      camera2->setPosition(Ogre::Vector3(0, 9.99, 0.01));
      camera2->lookAt(Ogre::Vector3(0, 0, 0));
      camera2->setNearClipDistance(2);
      camera2->setFarClipDistance(50);
      Ogre::Viewport* viewport2 = window->addViewport(camera2, 1, 0.5, 0, 0.5, 1.0);  //assign a camera to a viewport (can have many cameras and viewports in a single window)
      viewport2->setBackgroundColour(Ogre::ColourValue(0.1,0.1,0.1));
      camera2->setAspectRatio(aspect_ratio);
*/
   }

   catch (Ogre::Exception& e)
   {
      game_manager->logComment(e.what());
      ASSERT(false);
   }
}

RenderManager::RenderManager(GameManager* gm)
{
   game_manager = gm;
   init();

   //register the listener
   //the listener is notified before and after each frame
   animation_states = new ListArray<Ogre::AnimationState>();
   render_listener = new AnimationRenderListener(this);
   //render_listener = new InputRenderListener(this);
   root->addFrameListener(render_listener);
   render_listener = new InputRenderListener(this);
   root->addFrameListener(render_listener);
}

RenderManager::~RenderManager()
{
   delete animation_states;  //the ListArray containing the animations

   game_manager = NULL;

   stopRendering();
   delete render_listener;

   scene_manager->destroyAllCameras();
   scene_manager->clearScene();

   window->removeAllViewports();
   window->destroy();
   window = NULL;

   delete root;
   root = NULL;
}

size_t RenderManager::getRenderWindowHandle()
{
   return window_handle;
}

int RenderManager::getRenderWindowWidth()
{
   return viewport->getActualWidth();
}

int RenderManager::getRenderWindowHeight()
{
   return viewport->getActualHeight();
}

void RenderManager::stopRendering()
{
   render_listener->stopRendering();
}

void RenderManager::startRendering()
{
   root->startRendering();
}

void RenderManager::setTimeSinceLastFrame(Ogre::Real tslf)
{
   time_since_last_frame = tslf;
}

Ogre::RenderWindow* RenderManager::getRenderWindow()
{
   return window;
}

Ogre::SceneManager* RenderManager::getSceneManager()
{
   return scene_manager;
}

void RenderManager::processAnimations(float time_step)
{
	//cout<<"HI"<< endl;
   //ListArrayIterator<Ogre::AnimationState>* anim_iter = animation_states->iterator();
   
   // Animation State Indices
   // 1: Hilt
   // 2: Swing Left
   // 3: Swing Right
   // 4: Thrust
   // 5: Blade
   
   // Animate hilt flick and blade extension
   if (game_manager->saberOn())
   {   
		if (animation_states->get(1)->getTimePosition() < 0.99)
			animation_states->get(1)->addTime(time_step);

		if (animation_states->get(1)->getTimePosition() > 0.35 &&
			animation_states->get(5)->getTimePosition() < 0.5)
		{
			animation_states->get(5)->addTime(time_step);
		}
		
		if (animation_states->get(5)->getTimePosition() > 0.5)
			animationFinished = true;

		
		//animation_states->get(1)->addTime(time_step);		
   }
   
   // Animate blade retraction
	else
	{
		if (animation_states->get(5)->getTimePosition() > 0.5)
			animation_states->get(5)->addTime(time_step);
		
		if (animation_states->get(1)->getTimePosition() > 0.99)
			animation_states->get(1)->addTime(time_step);
		
		if (animation_states->get(5)->getTimePosition() < 0.5)
			animationFinished = true;
	}
	
	// Swing left
	if (game_manager->swingLeft())
	{
		if (animation_states->get(2)->getTimePosition() < 0.99)
			animation_states->get(2)->addTime(time_step);
		
		else
		{
				game_manager->finishLeft();
				animation_states->get(2)->setTimePosition(0);
		}
	}
	
	// Swing right
	else if (game_manager->swingRight())
	{
		if (animation_states->get(3)->getTimePosition() < 0.99)
			animation_states->get(3)->addTime(time_step);
		
		else 
		{
				game_manager->finishRight();
				animation_states->get(3)->setTimePosition(0);
		}		
	}
	
	// Thrust blade
	else if (game_manager->saberStab())
	{
		if (animation_states->get(4)->getTimePosition() < 0.99)
			animation_states->get(4)->addTime(time_step);
		
		else 
		{
				game_manager->finishStab();
				animation_states->get(4)->setTimePosition(0);
		}		
	}
	
	//animation_states->get(1)->addTime(time_step);

   //cout << animation_states->get(1)->getTimePosition() << endl;
   
   //while(anim_iter->hasNext())
   //{
   //   Ogre::AnimationState* animation_state = anim_iter->next();
   //   animation_state->addTime(time_step);
   //}
   //delete anim_iter;
}



void RenderManager::createScene(string fileName)
{
    TiXmlDocument sceneDoc(fileName.c_str());
    
    if (sceneDoc.LoadFile())
    {
		// Grabs root element
		TiXmlElement* scene = sceneDoc.RootElement();
		std::string scope_text = GameManager::textFromChildNode(scene, "scope");
        game_manager->loadResources(scope_text);
		
        // Creates camera
        TiXmlElement* camera_settings = scene->FirstChildElement("camera");
        
        // Sets camera position
        TiXmlElement* camera_position = camera_settings->FirstChildElement("position");
		TiXmlElement* position_x = camera_position->FirstChildElement("x");
		TiXmlElement* position_y = camera_position->FirstChildElement("y");
		TiXmlElement* position_z = camera_position->FirstChildElement("z");
		Vector3 pos (atof(position_x->GetText()), 
					 atof(position_y->GetText()), 
					 atof(position_z->GetText()));
        camera->setPosition(pos);
        
        // Sets camera direction
        TiXmlElement* camera_direction = camera_settings->FirstChildElement("direction");
		TiXmlElement* c_direction_x = camera_direction->FirstChildElement("x");
		TiXmlElement* c_direction_y = camera_direction->FirstChildElement("y");
		TiXmlElement* c_direction_z = camera_direction->FirstChildElement("z");
		Vector3 c_dir (atof(c_direction_x->GetText()), 
					   atof(c_direction_y->GetText()), 
					   atof(c_direction_z->GetText()));
        camera->lookAt(c_dir);
        
        // Sets camera near clip distance
        TiXmlElement* camera_near_clip = camera_settings->FirstChildElement("near_clip");
        camera->setNearClipDistance(atoi(camera_near_clip->GetText()));
        
        // Sets camera far clip distance
        TiXmlElement* camera_far_clip = camera_settings->FirstChildElement("far_clip");
        camera->setFarClipDistance(atoi(camera_far_clip->GetText()));
        
        // Creates and sets up light
        TiXmlElement* light_settings = scene->FirstChildElement("light");
        Ogre::Light* light = scene_manager->createLight("Light");
        
        // Sets light type
        light->setType(Ogre::Light::LT_DIRECTIONAL);
        
        // Sets light color
        TiXmlElement* light_color = light_settings->FirstChildElement("color");
		TiXmlElement* light_r = light_color->FirstChildElement("r");
		TiXmlElement* light_g = light_color->FirstChildElement("g");
		TiXmlElement* light_b = light_color->FirstChildElement("b");
        light->setDiffuseColour(atof(light_r->GetText()),
                                atof(light_g->GetText()),
                                atof(light_b->GetText()));
        
        // Sets light direction
        TiXmlElement* light_direction = light_settings->FirstChildElement("direction");
		TiXmlElement* l_direction_x = light_direction->FirstChildElement("x");
		TiXmlElement* l_direction_y = light_direction->FirstChildElement("y");
		TiXmlElement* l_direction_z = light_direction->FirstChildElement("z");
		Vector3 l_dir (atof(l_direction_x->GetText()), 
					   atof(l_direction_y->GetText()), 
					   atof(l_direction_z->GetText()));
        light->setDirection(l_dir);
        
        // Process rest of scene
        processScene(scene->FirstChildElement("root"), scene_manager->getRootSceneNode());
    }
}


void RenderManager::processScene(TiXmlElement* elements, Ogre::SceneNode* parent_node)
{
    for (TiXmlElement* element = elements->FirstChildElement(); element != NULL; element = element->NextSiblingElement())
    {
        string name = element->FirstChildElement("name")->GetText();
        string type = element->Value();
        
        Ogre::SceneNode* node = scene_manager->createSceneNode(name + "Node");
        parent_node->addChild(node);
        
        if (type == "transform")
        {
            TiXmlElement* scale = element->FirstChildElement("scale");
            TiXmlElement* rotate = element->FirstChildElement("rotate");
            TiXmlElement* translate = element->FirstChildElement("translate");
            
            if (scale)
			{
				TiXmlElement* scale_x = scale->FirstChildElement("x");
				TiXmlElement* scale_y = scale->FirstChildElement("y");
				TiXmlElement* scale_z = scale->FirstChildElement("z");
				Vector3 s (atof(scale_x->GetText()), 
						   atof(scale_y->GetText()), 
					       atof(scale_z->GetText()));
				
                node->scale(s);
			}
			
            if (rotate)
			{                
				TiXmlElement* rotate_w = rotate->FirstChildElement("w");
				TiXmlElement* rotate_x = rotate->FirstChildElement("x");
				TiXmlElement* rotate_y = rotate->FirstChildElement("y");
				TiXmlElement* rotate_z = rotate->FirstChildElement("z");
				Degree d  (atof(rotate_w->GetText()));
				Vector3 r (atof(rotate_x->GetText()), 
						   atof(rotate_y->GetText()), 
					       atof(rotate_z->GetText()));
				node->rotate(Quaternion(d, r));
			}
				
            if (translate)
			{
				TiXmlElement* translate_x = translate->FirstChildElement("x");
				TiXmlElement* translate_y = translate->FirstChildElement("y");
				TiXmlElement* translate_z = translate->FirstChildElement("z");
				Vector3 t (atof(translate_x->GetText()), 
						   atof(translate_y->GetText()), 
					       atof(translate_z->GetText()));
                node->translate(t);
			}
        }
        else if (type == "animation")
        {
			TiXmlElement* animationName = element->FirstChildElement("name");
			//TiXmlElement* animationLength = element->FirstChildElement("length");
			
			Vector3 v(-.7071,.7071,0);
            //Animation* animate = scene_manager->createAnimation(animationName->GetText(), atoi(animationLength->GetText()));	// Second parameter is number of keyframes
			//animate->setInterpolationMode(Animation::IM_LINEAR);
			
			//NodeAnimationTrack* track = animate->createNodeTrack(1, node);
			
			//TransformKeyFrame* key;
			
			//int count = 1;
			
			for (TiXmlElement* keyFrames = element->FirstChildElement("keyframes"); keyFrames != NULL; keyFrames = keyFrames->NextSiblingElement())
			{
				string str = keyFrames->Value();
				
				if (str == "child")
					break;
				
				
				
				TiXmlElement* animationName = keyFrames->FirstChildElement("name");
				cout << animationName->GetText() << endl;
				TiXmlElement* animationLength = keyFrames->FirstChildElement("length");
				
				Animation* animate = scene_manager->createAnimation(animationName->GetText(), atoi(animationLength->GetText()));	// Second parameter is number of keyframes
				animate->setInterpolationMode(Animation::IM_LINEAR);
			
				NodeAnimationTrack* track = animate->createNodeTrack(1, node);
			
				TransformKeyFrame* key;
				
			
				for (TiXmlElement* kF = keyFrames->FirstChildElement("key"); kF != NULL; kF = kF->NextSiblingElement())
				{	
					TiXmlElement* time = kF->FirstChildElement("time");
					
					key = track->createNodeKeyFrame(atof(time->GetText()));

					TiXmlElement* scale = kF->FirstChildElement("scale");
					TiXmlElement* rotate = kF->FirstChildElement("rotate");
					TiXmlElement* translate = kF->FirstChildElement("translate");
					
					if(scale)
					{
						TiXmlElement* scale_x = scale->FirstChildElement("x");
						TiXmlElement* scale_y = scale->FirstChildElement("y");
						TiXmlElement* scale_z = scale->FirstChildElement("z");
						Vector3 s (atof(scale_x->GetText()), 
								   atof(scale_y->GetText()), 
								   atof(scale_z->GetText()));
						key->setScale(s);
					}
					
					if(rotate)
					{
						TiXmlElement* rotate_w = rotate->FirstChildElement("w");
						TiXmlElement* rotate_x = rotate->FirstChildElement("x");
						TiXmlElement* rotate_y = rotate->FirstChildElement("y");
						TiXmlElement* rotate_z = rotate->FirstChildElement("z");
						Degree d  (atof(rotate_w->GetText()));
						Vector3 r (atof(rotate_x->GetText()), 
								   atof(rotate_y->GetText()), 
								   atof(rotate_z->GetText()));
						Quaternion q(d, r);
						key->setRotation(q);									
					}
					
					if(translate)
					{
						TiXmlElement* translate_x = translate->FirstChildElement("x");
						TiXmlElement* translate_y = translate->FirstChildElement("y");
						TiXmlElement* translate_z = translate->FirstChildElement("z");
						Vector3 t (atof(translate_x->GetText()), 
								   atof(translate_y->GetText()), 
								   atof(translate_z->GetText()));
						key->setTranslate(t);		
					}
					
					
				}
				
			
			//TransformKeyFrame* key = track->createNodeKeyFrame(0);
			//Ogre::Quaternion q1(Degree(-10), Vector3(0,0,1));
			//key->setTranslate(Vector3(-1,0,0));
			//key->setRotation(q1);		
			
			//key = track->createNodeKeyFrame(1);
			//Ogre::Quaternion q2(Degree(-10), Vector3(0,0,1));
			//key->setTranslate(Vector3(0,0,0));
			//key->setTranslate(Vector3(-5,0,0));
			//key->setRotation(q1);
			
				//count++;
				//cout<< "HIT " +  << endl;
				
				AnimationState* animate_state = scene_manager->createAnimationState(animationName->GetText());
				animate_state->setEnabled(true);
				animate_state->setLoop(true);
			
				animation_states->add(animate_state);
				//count++;
				//cout<< count << endl;
			}
			
        }
        else if (type == "entity")
        {
            TiXmlElement* mesh = element->FirstChildElement("mesh");
            TiXmlElement* material = element->FirstChildElement("material");
            
            Entity* entity = scene_manager->createEntity(name, mesh->GetText());
            entity->setMaterialName(material->GetText());
			
            node->attachObject(entity);
        }
        
        TiXmlElement* child = element->FirstChildElement("child");
        
        if (child)
		{
			processScene(child, node);
		}
            
    }
}



bool RenderManager::animationDone()
{
	return this->animationFinished;
}

void RenderManager::newAnimation()
{
	this->animationFinished = false;
}