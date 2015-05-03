#include "RenderManager.h"
#include "GUIManager.h"
#include "ScriptManager.h"
#include "PhysicsManager.h"

#include "GameManager.h"
#include "AnimationRenderListener.h"
#include "InputRenderListener.h"
#include "PhysicsRenderListener.h"
#include "NetworkRenderListener.h"
#include "Saber.h"

#include <iostream>
using namespace std;

using namespace Ogre;


int RenderManager::get_score()
{
   return saber->getScore();
}
void RenderManager::set_score(int value)
{
   cout <<"CURRENT SCORE MOTHER FUCKER" <<    value << endl;
   gui_manager->updateScore(value, false);
}
string RenderManager::networkSendReceive(string message_send)
{
   return game_manager->networkSendReceive(message_send);
}

void RenderManager::setGravity(string gravity_str)
{
   if (gravity_str != "")
   {
cout << "changing gravity" << endl;
      float gravity[3];
      GameManager::parseFloats(gravity_str, gravity);
      physics_manager->setGravity(gravity);
   }
   else
   {
cout << "not changing gravity" << endl;
   }
}

void RenderManager::stepPhysicsSimulation(float elapsed_time)
{
   physics_manager->stepPhysicsSimulation(elapsed_time);
}

void RenderManager::applyTorqueImpulse(std::string name, float pitch, float yaw, float roll)
{
   physics_manager->applyTorqueImpulse(name, pitch, yaw, roll);
}

void RenderManager::setOrientation(SceneNodeMotion* scene_node_motion, double w, double x, double y, double z)
{
   Ogre::SceneNode* scene_node = scene_node_motion->scene_node_motion;
   scene_node->setOrientation(w, x, y, z);
}

void RenderManager::setPosition(SceneNodeMotion* scene_node_motion, double x, double y, double z)
{
   Ogre::SceneNode* scene_node = scene_node_motion->scene_node_motion;
   scene_node->setPosition(x, y, z);
}

float* RenderManager::getOrientation(SceneNodeMotion* scene_node_motion)
{
   Ogre::SceneNode* scene_node = scene_node_motion->scene_node_motion;

   Ogre::Quaternion q = scene_node->getOrientation();
   Ogre::Real w = q.w;
   Ogre::Real x = q.x;
   Ogre::Real y = q.y;
   Ogre::Real z = q.z;

   float* rot = new float[4];
   rot[3] = w;
   rot[0] = x;
   rot[1] = y;
   rot[2] = z;
   return rot;
}

float* RenderManager::getPosition(SceneNodeMotion* scene_node_motion)
{
   ASSERT(scene_node_motion);

   Ogre::SceneNode* scene_node = scene_node_motion->scene_node_motion;

   Ogre::Vector3 pos = scene_node->getPosition();
   float* pos_ = new float[3];
   pos_[0] = pos.x;
   pos_[1] = pos.y;
   pos_[2] = pos.z;
   return pos_;
}

void RenderManager::executeRotateScript(string script_file_name, string script_function_name, string object_name, int degrees)
{
   try
   {
      selected_node = scene_manager->getSceneNode(object_name);
   }
   catch (Ogre::Exception& e)
   {
      game_manager->logComment(e.what());
      ASSERT(false);
   }

   const Quaternion& current_quat = selected_node->getOrientation();
   const int num_inputs = 5;
   const int num_outputs = 4;

   const char** inputs = new const char*[num_inputs];
   inputs[0] = GameManager::f_to_a(current_quat.w);
   inputs[1] = GameManager::f_to_a(current_quat.x);
   inputs[2] = GameManager::f_to_a(current_quat.y);
   inputs[3] = GameManager::f_to_a(current_quat.z);
   inputs[4] = GameManager::f_to_a(degrees);             //amount to rotate in degrees

   char** outputs = new char*[num_outputs];

   int output_len = 20;
   for (int i = 0; i < num_outputs; i++)
   {
      outputs[i] = new char[output_len + 1];
   }

   script_manager->executeScript(script_file_name, script_function_name, num_inputs, num_outputs, inputs, outputs);

   float w = GameManager::a_to_f(outputs[0]);
   float x = GameManager::a_to_f(outputs[1]);
   float y = GameManager::a_to_f(outputs[2]);
   float z = GameManager::a_to_f(outputs[3]);
cout << w << endl;
cout << x << endl;
cout << y << endl;
cout << z << endl;
   selected_node->setOrientation(w, x, y, z);

   for (int i = 0; i < num_inputs; i++)
   {
      delete[] inputs[i];
   }

   for (int i = 0; i < num_outputs; i++)
   {
      delete[] outputs[i];
   }

   delete[] inputs;
   delete[] outputs;
}



void RenderManager::executeSlashScript(string script_file_name, string script_function_name, string object_name, int counter)
{
   int degrees = 90;
 
  
   const int num_inputs = 1;
   const int num_outputs = 3;

   const char** inputs = new const char*[num_inputs];
   inputs[0] = GameManager::f_to_a(counter);
   
   char** outputs = new char*[num_outputs];

   int output_len = 20;
   for (int i = 0; i < num_outputs; i++)
   {
      outputs[i] = new char[output_len + 1];
   }

  
   script_manager->executeScript(script_file_name, script_function_name, num_inputs, num_outputs, inputs, outputs);
   
   Ogre::SceneNode* sn;
  
   //Ogre::SceneNode* sn = scene_manager->getSceneNode("BallTransformNode ");
   try
   {

      sn = scene_manager->getSceneNode(object_name + "Node");
      Vector3 t = sn->getPosition();
      sn->setPosition(atof(outputs[0]), atof(outputs[1]), atof(outputs[2]));
      sn->setOrientation(90,0,180,0);

      physics_manager->resetBall(object_name,t.x, atof(outputs[1]), atof(outputs[2]));
     

   }
   catch (Ogre::Exception& e)
   {
      game_manager->logComment(e.what());
      ASSERT(false);
   }
  for(int i = 0; i < num_outputs; i++)
  {
      //cout << outputs[i] << endl;
  }
   for (int i = 0; i < num_inputs; i++)
   {
      delete[] inputs[i];
   }

   for (int i = 0; i < num_outputs; i++)  //why does this delete crash?
   {
      delete[] outputs[i];
   }
   //scene_manager->destroySceneNode(node);

   delete[] inputs;
   delete[] outputs;
   

  
}

void RenderManager::executeForceScript(string script_file_name, string script_function_name, string object_name, int counter)
{
   int degrees = 90;
   
  
   const int num_inputs = 1;
   const int num_outputs = 3;

   const char** inputs = new const char*[num_inputs];
   inputs[0] = GameManager::f_to_a(counter);
   
   char** outputs = new char*[num_outputs];

   int output_len = 20;
   for (int i = 0; i < num_outputs; i++)
   {
      outputs[i] = new char[output_len + 1];
   }

  
   script_manager->executeScript(script_file_name, script_function_name, num_inputs, num_outputs, inputs, outputs);
   
   Ogre::SceneNode* sn;
   
   try
   {
      //if(user !=1)
         physics_manager->applyForce(object_name, atof(outputs[0]), atof(outputs[1]), atof(outputs[2]));
     
   }
   catch (Ogre::Exception& e)
   {
      game_manager->logComment(e.what());
      ASSERT(false);
   }

   for (int i = 0; i < num_inputs; i++)
   {
      delete[] inputs[i];
   }

   for (int i = 0; i < num_outputs; i++)  //why does this delete crash?
   {
      delete[] outputs[i];
   }

   delete[] inputs;
   delete[] outputs;
   

  
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

void RenderManager::force()
{
   SceneNode::ChildNodeIterator it = scene_manager->getRootSceneNode()->getChildIterator();
   SceneNode* node;
   string name;
   int counter = 0;
	while (it.hasMoreElements())
	{	
		node = dynamic_cast<Ogre::SceneNode*>(it.getNext());
		name=node->getName();
		Vector3 nodePos = node->getPosition();
			
		name = name.substr(0, name.size()-4) ;	// Removes "Node" from object name

		if (name != "Saber")
		{
			executeForceScript("assets/lua_scripts/placement.lua","force",name, counter);
			counter++;
		}

	}
	
}

void RenderManager::playAudio(uint32 audio_id, uint32 num_repeats)
{
	game_manager->playAudio(audio_id, num_repeats);
}

void RenderManager::setSelectedNode(std::string item)
{
   try
   {
	  saber->resetAnimation();
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
   gui_manager->mousePressed(mouse_x, mouse_y, game_mouse);
}

void RenderManager::mouseReleased(uint32 mouse_x, uint32 mouse_y, uint32 game_mouse)
{
   gui_manager->mouseReleased(mouse_x, mouse_y, game_mouse);
}

void RenderManager::mouseMoved(uint32 mouse_x, uint32 mouse_y, int mouse_rel_x, int mouse_rel_y)
{
/*
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
*/
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
      scene_manager->clearScene();  //pretty much destroys everything

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

   animation_render_listener = NULL;
   input_render_listener = NULL;
   physics_render_listener = NULL;

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
      window = root->initialise(true, "Game Engine Programming");  

      //can create BSP or octree scene managers
      scene_manager = root->createSceneManager(Ogre::ST_GENERIC, "Default Scene Manager");
      window->getCustomAttribute("WINDOW", &window_handle);

      //the Ogre viewport corresponds to a clipping region into which the contents of the camera view will be rendered in the window on each frame
      //by default, the size of the viewport matches the size of the window, but the viewport can be cropped
      //the camera represents a view into an existing scene and the viewport represents a region into which an existing camera will render its contents
      camera = scene_manager->createCamera("Camera");

      //z-order (for possible overlapping), left, top, width, height
      viewport = window->addViewport(camera, 0, 0, 0, 1.0, 1.0);  //assign a camera to a viewport (can have many cameras and viewports in a single window)
      viewport->setBackgroundColour(Ogre::ColourValue(0,0,0));

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
   saber = new Saber(gm);
   init();

   //register the listener
   //the listener is notified before and after each frame
   animation_states = new ListArray<Ogre::AnimationState>();

   animation_render_listener = new AnimationRenderListener(this);
   root->addFrameListener(animation_render_listener);

   input_render_listener = new InputRenderListener(this);
   root->addFrameListener(input_render_listener);

   gui_manager = new GUIManager(this);
   script_manager = new ScriptManager();
   physics_manager = new PhysicsManager(this);

   physics_render_listener = new PhysicsRenderListener(this);
   root->addFrameListener(physics_render_listener);

   network_render_listener = new NetworkRenderListener(this);
   root->addFrameListener(network_render_listener);
}

RenderManager::~RenderManager()
{
   delete physics_manager;
   physics_manager = NULL;

   delete script_manager;
   script_manager = NULL;
   delete gui_manager;
   gui_manager = NULL;

   delete animation_states;  //the ListArray containing the animations

   delete animation_render_listener;
   delete input_render_listener;
   delete physics_render_listener;
   //delete network_render_listener;

   game_manager = NULL;

   stopRendering();

   window->removeAllViewports();
   window->destroy();
   window = NULL;

   delete root;
   root = NULL;
}

void RenderManager::clearManualObject(SceneNodeManual* snm)
{
   SceneNode* scene_node_manual = snm->scene_node_manual;
   ManualObject* manual_object = (ManualObject*) scene_node_manual->getAttachedObject("Manual_Object");
   manual_object->clear();
}

SceneNodeManual* RenderManager::createManualObject()
{
   SceneNodeManual* scene_node_manual = new SceneNodeManual;
   	
   ManualObject* manual_object = scene_manager->createManualObject("Manual_Object");
   manual_object->setDynamic(true);
   static const char* mat_name = "OgreBulletCollisionsDebugDefault";
   Ogre::MaterialPtr manual_object_material = Ogre::MaterialManager::getSingleton().getDefaultSettings()->clone(mat_name);
   manual_object_material->setReceiveShadows(false); 
   manual_object_material->getTechnique(0)->setLightingEnabled(false); 
   SceneNode* manual_object_node = scene_manager->getRootSceneNode()->createChildSceneNode();
   manual_object_node->attachObject(manual_object);

   scene_node_manual->scene_node_manual = manual_object_node;
   return scene_node_manual;
}

void RenderManager::drawLine(float* from, float* to, float* color, SceneNodeManual* snm)
{
   SceneNode* scene_node_manual = snm->scene_node_manual;
   ManualObject* manual_object = (ManualObject*) scene_node_manual->getAttachedObject("Manual_Object");
   manual_object->begin("OgreBulletCollisionsDebugDefault", Ogre::RenderOperation::OT_LINE_LIST);
   manual_object->position(Vector3(from[0], from[1], from[2]));
   manual_object->colour(ColourValue(color[0], color[1], color[2]));
   manual_object->position(Vector3(to[0], to[1], to[2]));
   manual_object->colour(ColourValue(color[0], color[1], color[2]));
   manual_object->end();
}

void RenderManager::destroySceneNodeMotion(SceneNodeMotion* snm)
{
   free(snm);
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
   input_render_listener->stopRendering();
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
	if (saber->gameFinished())	// GAME OVER
		stopRendering();
	
   saber->processAnimations(time_step, animation_states);
   
   
   SceneNode::ChildNodeIterator it = scene_manager->getRootSceneNode()->getChildIterator();
   SceneNode* node;
   string name;
   int counter = 0;
   
   try
   {
		while (it.hasMoreElements())
		{	
			node = dynamic_cast<Ogre::SceneNode*>(it.getNext());
			name=node->getName();
			Vector3 nodePos = node->getPosition();
				
			name = name.substr(0, name.size()-4) ;	// Removes "Node" from object name
				
			if (nodePos.y < -25)
			{
				executeSlashScript("assets/lua_scripts/placement.lua","placement",name, counter);	// Moves off screen Falcon back to top
				counter++;
			}

		}
 
   }
   catch (Ogre::Exception& e)
   {
      game_manager->logComment(e.what());
      ASSERT(false);
   }
   
	physics_manager->updateRigidBodies();
	
	gui_manager->updateScore(saber->getScore(), true);
}

void RenderManager::increaseScore()
{
	saber->increaseScore();
}

void RenderManager::resetAnimation()
{
	saber->resetAnimation();
}

void RenderManager::keyPressed(std::string game_key)
{
   if (game_key == "ESCAPE")
   {
	 saber->keyPressed(game_key);  
     //stopRendering();
   }
   
   else
   {
	   cout<<game_key<<endl;
	   saber->keyPressed(game_key);
   }
}

SceneNodeMotion* RenderManager::createSceneNodeMotion(std::string scene_node_id)
{
   SceneNodeMotion* scene_node_motion = (SceneNodeMotion*) malloc(sizeof(SceneNodeMotion));
   scene_node_motion->scene_node_motion = scene_manager->getSceneNode(scene_node_id);
   return scene_node_motion;
}

void RenderManager::addSceneNodeAnimation(TiXmlNode* animation_node_xml, SceneNode* animation_node, std::string animation_name_text, float* values)
{
   std::string animation_seconds_text = GameManager::textFromChildNode(animation_node_xml, "seconds");

   Ogre::Animation* animation = scene_manager->createAnimation(animation_name_text, GameManager::parseInt(animation_seconds_text));
   animation->setInterpolationMode(Ogre::Animation::IM_SPLINE);

   //indicate that we want a scene node to be animated (affecting everything lower than it in the scene graph)
   Ogre::NodeAnimationTrack* animation_track = animation->createNodeTrack(1, animation_node);

   TiXmlNode* keyframes_xml = animation_node_xml->FirstChild("keyframes");
   for(TiXmlNode* keyframe_xml = keyframes_xml->FirstChild("keyframe"); keyframe_xml; keyframe_xml = keyframe_xml->NextSibling())
   {  
      std::string keyframe_xml_time = GameManager::textFromChildNode(keyframe_xml, "time");
      float time = GameManager::parseFloat(keyframe_xml_time);

      std::string keyframe_xml_translation = GameManager::textFromChildNode(keyframe_xml, "translation");
      GameManager::parseFloats(keyframe_xml_translation, values);
      Vector3 vt(values[0], values[1], values[2]);

      std::string keyframe_xml_rotation = GameManager::textFromChildNode(keyframe_xml, "rotation");
      GameManager::parseFloats(keyframe_xml_rotation, values);
      Vector3 vr(values[1], values[2], values[3]);
      Quaternion q(Degree(values[0]), vr);

      Ogre::TransformKeyFrame* animation_key = animation_track->createNodeKeyFrame(time); 
      animation_key->setTranslate(vt);
      animation_key->setRotation(q);
   }

   Ogre::AnimationState* animation_state = scene_manager->createAnimationState(animation_name_text);
   animation_state->setEnabled(true);
   animation_state->setLoop(true);

   animation_states->add(animation_state);
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
        
		TiXmlElement* gravity_settings = scene->FirstChildElement("gravity");
		TiXmlElement* gravity_x = gravity_settings->FirstChildElement("x");
		TiXmlElement* gravity_y = gravity_settings->FirstChildElement("y");
		TiXmlElement* gravity_z = gravity_settings->FirstChildElement("z");
		
		float vals[3];
		vals[0] = atof(gravity_x->GetText());
		vals[1] = atof(gravity_y->GetText());
		vals[2] = atof(gravity_z->GetText());
		
        physics_manager->setGravity(vals);
		
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
			//TiXmlElement* transformName = element->FirstChildElement("name")->GetText();
			
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
				cout << "HI: " << name << endl;
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
        
		
		TiXmlElement* physics = element->FirstChildElement("physics");	
		
		if (physics)
		{
			 TiXmlElement* collision_shape = physics->FirstChildElement("collision_shape");
			 string coll = collision_shape->GetText();
			 
			 if (coll == "compound")	// Compound shape
			 {
				 TiXmlElement* numShapesElement = physics->FirstChildElement("num_shapes");
				 int numShapes = 0;
				 numShapes = atoi(numShapesElement->GetText());
				 string* coll_Shapes = new string[numShapes];
				 double** coll_collParams = new double *[numShapes];
				 double** coll_posParams = new double *[numShapes];
				 for (int count = 0; count < numShapes; count++)
				 {
					 coll_collParams[count] = new double[3];
					 coll_posParams[count] = new double[3];
				 }
				 
				 int count = 0;
				 
				 for(TiXmlElement* shape = physics->FirstChildElement("shape"); shape != NULL; shape = shape->NextSiblingElement("shape"))
				 {
					 cout<< shape->GetText() << endl;
					 coll_Shapes[count] = shape->GetText();
					 
					TiXmlElement* collision_parameters = shape->NextSiblingElement("collision_parameters");
					TiXmlElement* collision_x = collision_parameters->FirstChildElement("x");
					TiXmlElement* collision_y = collision_parameters->FirstChildElement("y");
					TiXmlElement* collision_z = collision_parameters->FirstChildElement("z");
					
					coll_collParams[count][0] = atof(collision_x->GetText());
					coll_collParams[count][1] = atof(collision_y->GetText());
					coll_collParams[count][2] = atof(collision_z->GetText());
					
					TiXmlElement* position_parameters = shape->NextSiblingElement("position_parameters");
					TiXmlElement* coll_position_x = position_parameters->FirstChildElement("x");
					TiXmlElement* coll_position_y = position_parameters->FirstChildElement("y");
				    TiXmlElement* coll_position_z = position_parameters->FirstChildElement("z");
					
					coll_posParams[count][0] = atof(coll_position_x->GetText());
					coll_posParams[count][1] = atof(coll_position_y->GetText());
					coll_posParams[count][2] = atof(coll_position_z->GetText());
					
							
					
					//std::string child_physics_mass = GameManager::textFromChildNode(physics, "mass");
					//float mass = GameManager::parseFloat(child_physics_mass);

					count++;	 
				 }
				
				 TiXmlElement* mass = physics->FirstChildElement("mass");	
				
				 SceneNodeMotion* scene_node_motion = (SceneNodeMotion*) malloc(sizeof(SceneNodeMotion));
				 scene_node_motion->scene_node_motion = node;
				 physics_manager->createCompoundRigidBody(scene_node_motion, name, coll_Shapes, coll_collParams, coll_posParams, atof(mass->GetText()), numShapes);
				 
			 }

			 else	// Single shape
			 {

				 TiXmlElement* collision_parameters = physics->FirstChildElement("collision_parameters");
				 TiXmlElement* collision_x = collision_parameters->FirstChildElement("x");
				 TiXmlElement* collision_y = collision_parameters->FirstChildElement("y");
				 TiXmlElement* collision_z = collision_parameters->FirstChildElement("z");
				//std::string child_physics_collision_shape = GameManager::textFromChildNode(scene_graph_child_physics, "collision_shape");
				//std::string child_physics_collision_parameters = GameManager::textFromChildNode(physics, "collision_parameters");

				double* collision_shape_params = new double[3];
				collision_shape_params[0] = atof(collision_x->GetText());
				collision_shape_params[1] = atof(collision_y->GetText());
				collision_shape_params[2] = atof(collision_z->GetText());
				
				TiXmlElement* mass = physics->FirstChildElement("mass");			
				
				//std::string child_physics_mass = GameManager::textFromChildNode(physics, "mass");
				//float mass = GameManager::parseFloat(child_physics_mass);

				SceneNodeMotion* scene_node_motion = (SceneNodeMotion*) malloc(sizeof(SceneNodeMotion));
				scene_node_motion->scene_node_motion = node;
				physics_manager->createSimpleRigidBody(scene_node_motion, name, collision_shape->GetText(), collision_shape_params, atof(mass->GetText()));
			}
		}
		
		
		
		
		
		
        TiXmlElement* child = element->FirstChildElement("child");
        
        if (child)
		{
			processScene(child, node);
		}
            
    }
}

