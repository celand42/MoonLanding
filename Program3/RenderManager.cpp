#include "RenderManager.h"
#include "GUIManager.h"
//#include "ScriptManager.h"
#include "GameManager.h"
#include "AnimationRenderListener.h"
#include "InputRenderListener.h"

#include <iostream>
using namespace std;

using namespace Ogre;

void RenderManager::executeScript(string file_name, string script_name, string object_name)
{
/*
   try
   {
      selected_node = scene_manager->getSceneNode(object_name);
   }
   catch (Ogre::Exception& e)
   {
      game_manager->logComment(e.what());
      ASSERT(false);
   } 

   double input[3];
   input[0] = 0;
   input[1] = 0;
   input[2] = 0;

   double output[3];

   script_manager->executeScript(file_name, script_name, 3, 3, input, output);

   selected_node->pitch(Degree(output[0]));
   selected_node->yaw(Degree(output[1]));
   selected_node->roll(Degree(output[2]));
*/
}

void RenderManager::logComment(std::string comment_message)
{
   game_manager->logComment(comment_message);
}
void RenderManager::talk()
{
	srand(time(NULL));
	int random = rand() % 6 + 40;
	game_manager->playAudio(random, 1);
}
void RenderManager::setSelectedNode(std::string item)
{
   try
   {
      //selected_node = scene_manager->getSceneNode(item);
	  game_manager->resetAnimation();
      Entity* entity = scene_manager->getEntity("BladeMesh");
      entity->setMaterialName(item);
   }
   catch (Ogre::Exception& e)
   {
      game_manager->logComment(e.what());
      ASSERT(false);
   }
}

void RenderManager::joystickAxisMoved(int* amount)
{
    //It should be noted that most axis inputs range between ~-32720 and ~32720
    //Control stick axis tend to equal +-~500 when they are idle (as in not beig touched)
    //Triggers start at one extreme and end at the other as they are pulled down.

    Camera* camera = scene_manager->getCamera("Camera");
    const Vector3& camera_pos = camera->getPosition();
    const Vector3& camera_dir = camera->getDirection();

    float sticks[6];
    for (int i = 0; i < 6; i++)
    {
       if (abs(amount[i]) < 5000) //dead zone
       {
          amount[i] = 0;
       }

       sticks[i] = amount[i]/10000.0f;
    }

                                 //left stick n/s            left stick e/w
    float x_pos = camera_pos.x + (camera_dir.x*-sticks[0]) + (camera_dir.z*-sticks[1]);
    float y_pos = camera_pos.y;  //doesn't change
    float z_pos = camera_pos.z + (camera_dir.z*-sticks[0]) - (camera_dir.x*-sticks[1]);

    float yaw = atan2(camera_dir.z, camera_dir.x);   //current yaw
    yaw = yaw + sticks[3]/60.0f;  //right stick e/w
   
    float x_dir = cos(yaw);
    float y_dir = camera_dir.y - sticks[2]/60.0f;  //right stick n/s
    float z_dir = sin(yaw);
   
    Vector3 dir_vector = Vector3(x_dir, y_dir, z_dir);  
    dir_vector.normalise();

    Vector3 pos_vector = Vector3(x_pos, y_pos, z_pos);
      
    camera->setDirection(dir_vector);
    camera->setPosition(pos_vector);
}

void RenderManager::updateAudio()
{
   game_manager->updateAudio();
}

void RenderManager::mousePressed(uint32 mouse_x, uint32 mouse_y, uint32 game_mouse)
{
	//Entity* entity = scene_manager->getEntity("BladeMesh");
	//entity->setMaterialName("Silver");
	//cout<<"CLICK"<<endl;
   gui_manager->mousePressed(mouse_x, mouse_y, game_mouse);
}

void RenderManager::mouseReleased(uint32 mouse_x, uint32 mouse_y, uint32 game_mouse)
{
   gui_manager->mouseReleased(mouse_x, mouse_y, game_mouse);
}

void RenderManager::mouseMoved(uint32 mouse_x, uint32 mouse_y, int mouse_rel_x, int mouse_rel_y)
{
   if (selected_node)
   {
      //get root's (one) child and change its rotation based on mouse movement
      //SceneNode* root = scene_manager->getRootSceneNode();
      //SceneNode* scene_parent_node = (SceneNode*) root->getChild(0);
      const Quaternion& current_quat = selected_node->getOrientation();

      Vector3 x_axis(1, 0, 0);
      Vector3 y_axis(0, 1, 0);
      Vector3 z_axis(0, 0, 1);
      int x_degree = mouse_rel_x;
      int y_degree = mouse_rel_y;

      Quaternion x_quat(Degree(y_degree), x_axis);
      Quaternion y_quat(Degree(x_degree), y_axis);
      Quaternion z_quat(Degree(x_degree), z_axis);

      Quaternion new_quat = x_quat*y_quat*current_quat;  //quaternions to the right are applied first
      //scene_parent_node->rotate(new_quat);  //here, new_quat is applied to the right
      selected_node->setOrientation(new_quat);
   }

   gui_manager->mouseMoved(mouse_x, mouse_y);
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

      gui_manager->loadResourceGroup(group_name);
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
      //need to destroy the scene graph as well
      scene_manager->clearScene();
      //root->removeAndDestroyAllChildren();
      gui_manager->unloadResourceGroup();

      ResourceGroupManager& rgm = ResourceGroupManager::getSingleton();
      //rgm.clearResourceGroup(group_name);
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
   selected_node = NULL;

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
	  viewport->setBackgroundColour(Ogre::ColourValue(.33, .33, .33, 1));

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
   root->addFrameListener(render_listener);
   render_listener = new InputRenderListener(this);
   root->addFrameListener(render_listener);

   gui_manager = new GUIManager(this);
   //script_manager = new ScriptManager(this);
}

RenderManager::~RenderManager()
{
   //delete script_manager;
   //script_manager = NULL;
   delete gui_manager;
   gui_manager = NULL;

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
	game_manager->processAnimations(time_step, animation_states);
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
				
				Animation* animate = scene_manager->createAnimation(animationName->GetText(), atof(animationLength->GetText()));	// Second parameter is number of keyframes
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

