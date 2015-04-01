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

void RenderManager::setSelectedNode(std::string item)
{
   try
   {
      selected_node = scene_manager->getSceneNode(item);
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







/*
void RenderManager::buildSceneFromXML(std::string file_name)
{
   TiXmlDocument doc(file_name.c_str());

   if (doc.LoadFile())
   {
      TiXmlNode* scene_node = doc.FirstChild("scene");

      if (scene_node)
      {
         std::string scope_text = GameManager::textFromChildNode(scene_node, "scope");
         game_manager->loadResources(scope_text);

         float values[4];

         TiXmlNode* camera_node = scene_node->FirstChild("camera");

         std::string camera_position_text = GameManager::textFromChildNode(camera_node, "position");
         GameManager::parseFloats(camera_position_text, values);
         Vector3 camera_position(values[0], values[1], values[2]);
         camera->setPosition(camera_position);

         std::string camera_look_at_text = GameManager::textFromChildNode(camera_node, "look_at");
         GameManager::parseFloats(camera_look_at_text, values);
         Vector3 camera_look(values[0], values[1], values[2]);
         camera->lookAt(camera_look);

         std::string camera_clip_distance_text = GameManager::textFromChildNode(camera_node, "clip_distance");
         GameManager::parseFloats(camera_clip_distance_text, values);
         camera->setNearClipDistance(values[0]);
         camera->setFarClipDistance(values[1]);

         TiXmlNode* light_node = scene_node->FirstChild("light");

         std::string light_name_text = GameManager::textFromChildNode(light_node, "name");
         Light* light = scene_manager->createLight(light_name_text);

         std::string light_type_text = GameManager::textFromChildNode(light_node, "type");
         if (light_type_text == "directional")
         {
            light->setType(Light::LT_DIRECTIONAL);
         }
         else
         {
            light->setType(Light::LT_POINT);
         }

         std::string light_position_text = GameManager::textFromChildNode(light_node, "position");
         GameManager::parseFloats(light_position_text, values);
         Vector3 light_pos(values[0], values[1], values[2]);
         if (light_type_text == "directional")
         {
            light->setDirection(light_pos);
         }
         else
         {
            light->setPosition(light_pos);
         }

         std::string light_color_text = GameManager::textFromChildNode(light_node, "color");
         GameManager::parseFloats(light_color_text, values);
         light->setDiffuseColour(values[0], values[1], values[2]);

         TiXmlNode* scene_graph_node = scene_node->FirstChild("scene_graph");
         TiXmlNode* scene_graph_root_node = scene_graph_node->FirstChild("root");
         TiXmlNode* scene_graph_children = scene_graph_root_node->FirstChild("children");
         
         //children will be attached to the root scene node
         addSceneNodeChildren(scene_graph_children, scene_manager->getRootSceneNode(), values);
      }
   }
   else
   {
      game_manager->logComment("Invalid XML file: " + file_name);
      ASSERT(false);
   }
}

void RenderManager::addSceneNodeChildren(TiXmlNode* xml_node, SceneNode* parent_node, float* values)
{

   for(TiXmlNode* child_xml_node = xml_node->FirstChild("child"); child_xml_node; child_xml_node = child_xml_node->NextSibling())
   {
      //check for a child animation node
      TiXmlNode* curr_child_animation_xml = child_xml_node->FirstChild("animation");
      if (curr_child_animation_xml)
      {
         std::string animation_name_text = GameManager::textFromChildNode(curr_child_animation_xml, "name");

         SceneNode* animation_node = scene_manager->createSceneNode(animation_name_text);
         parent_node->addChild(animation_node);

         addSceneNodeAnimation(curr_child_animation_xml, animation_node, animation_name_text, values);
         addSceneNodeChildren(child_xml_node->FirstChild("children"), animation_node, values);
      }

      else  //regular child node
      {
      // Create the child node
      std::string child_name_text = GameManager::textFromChildNode(child_xml_node, "name");

      SceneNode* curr_child_node = scene_manager->createSceneNode(child_name_text);
      parent_node->addChild(curr_child_node);
      
      //process the node entity
      TiXmlNode* scene_graph_child_entity = child_xml_node->FirstChild("entity");
      if (scene_graph_child_entity)
      {
         std::string child_entity_name = GameManager::textFromChildNode(scene_graph_child_entity, "name");
         std::string child_entity_mesh = GameManager::textFromChildNode(scene_graph_child_entity, "mesh");
         std::string child_entity_material = GameManager::textFromChildNode(scene_graph_child_entity, "material");
         Ogre::Entity* child_entity;
         try
         {
            child_entity = scene_manager->createEntity(child_entity_name, child_entity_mesh);
            child_entity->setMaterialName(child_entity_material);
            game_manager->logComment("Entity " + child_entity_name + " successfully placed in scene graph.");
         }
         catch (Ogre::Exception& e)
         {
            game_manager->logComment(e.what());
            ASSERT(false);
         }
         
         curr_child_node->attachObject(child_entity);
      }

      // translate
      std::string child_translation_text = GameManager::textFromChildNode(child_xml_node, "translation");
      GameManager::parseFloats(child_translation_text, values);
      Vector3 child_translation(values[0], values[1], values[2]);
      curr_child_node->translate(child_translation, Node::TS_LOCAL);

      // rotate
      std::string child_rotation_text = GameManager::textFromChildNode(child_xml_node, "rotation");
      GameManager::parseFloats(child_rotation_text, values);
      Vector3 vr(values[1], values[2], values[3]);
      Quaternion q(Degree(values[0]), vr);
      curr_child_node->rotate(q);

      // scale
      std::string child_scale_text = GameManager::textFromChildNode(child_xml_node, "scale");
      GameManager::parseFloats(child_scale_text, values);
      Vector3 child_scale(values[0], values[1], values[2]);
      curr_child_node->scale(child_scale);

      addSceneNodeChildren(child_xml_node->FirstChild("children"), curr_child_node, values);
      }
   }
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
*/
/*
//Ogre has its own internal resource manager, but we will use a custom xml file to specify the ogre path information
void RenderManager::loadOgreResourcesFromXML(std::string file_name, std::string group_name)
{
   //use tiny xml to parse an xml file with the ogre paths in it
   TiXmlDocument doc(file_name.c_str());
   if (doc.LoadFile())
   {
      Ogre::ResourceGroupManager& rgm = Ogre::ResourceGroupManager::getSingleton();
      TiXmlNode* ogre_groups_tree = doc.FirstChild("ogre_groups");
      if (ogre_groups_tree)
      {
         //Enumerate group objects (eventually, child will be false and loop will terminate)
         for(TiXmlNode* group_node = ogre_groups_tree->FirstChild(); group_node; group_node = group_node->NextSibling())
         {
            std::string name_text = GameManager::textFromChildNode(group_node, "name");

            //continue with this section if it matches the requested section
            if (name_text == group_name)
            {
               TiXmlNode* paths_tree = group_node->FirstChild("paths");
               if (paths_tree)
               {
                  //Enumerate path objects
                  for(TiXmlNode* path_node = paths_tree->FirstChild(); path_node; path_node = path_node->NextSibling())
                  {
                     TiXmlElement* path_element = (TiXmlElement*) path_node->ToElement();
                     std::string path_text = path_element->GetText();

                     //FileSystem or Zip
                     //Ogre will look for scripts in these directories
                     rgm.addResourceLocation(path_text, "FileSystem", group_name);
                  }
               }

               TiXmlNode* meshes_tree = group_node->FirstChild("meshes");
               if (meshes_tree)
               {
                  //Enumerate path objects
                  for(TiXmlNode* mesh_node = meshes_tree->FirstChild(); mesh_node; mesh_node = mesh_node->NextSibling())
                  {
                     TiXmlElement* mesh_element = (TiXmlElement*) mesh_node->ToElement();
                     std::string mesh_text = mesh_element->GetText();

                     //Ogre will look for meshes in the paths defined above
                     rgm.declareResource(mesh_text, "Mesh", group_name);  //so that the mesh is loaded when its resource group is loaded
                  }
               }

               //scripts loaded and resources are created, but not loaded
               //use load/unload resource group to manage resource memory footprint
               rgm.initialiseResourceGroup(group_name);  //pre-load the resources located in the specific paths (parse scripts)
               rgm.loadResourceGroup(group_name, true, true);  //load the resources in the specific paths
 
               loaded_group = group_name;

            }  //end if name matches
         }
      }
   }
   else  
   {
      //log the fact that the resource metadata file was not found
      ASSERT(false);
   }

   game_manager->logProgress("Ogre resources loaded for group name: " + group_name);
}

void RenderManager::unloadOgreResources()
{
   if (loaded_group == "") return;

   Ogre::ResourceGroupManager& rgm = Ogre::ResourceGroupManager::getSingleton();
   //rgm.unloadResourceGroup(loaded_group);  //unload the resources in the specific paths
   rgm.destroyResourceGroup(loaded_group);   //completely remove resource information (could use clear instead of destroy)

   loaded_group = "";
}
*/

/*
void RenderManager::buildSceneManually()
{
    camera->setPosition(Vector3(0, 0, 20));
    camera->lookAt(Vector3(0, 0, 0));
    camera->setNearClipDistance(2);
    camera->setFarClipDistance(50);
    
    scene_manager->setAmbientLight(ColourValue(.1,.1,.1));
    Light* light = scene_manager->createLight("Light");
    light->setType(Light::LT_DIRECTIONAL);
    light->setDirection(Vector3(0, 1, -2));
    light->setDiffuseColour(1.0, 1.0, 1.0);

    if (loaded_group == "0")
    {
       buildSubmarineScene();
    }
    else if (loaded_group == "1")
    {
       buildXWingAnimationScene();
    }
    else if (loaded_group == "2")
    {
       buildCannonSceneGraph();
    }
}

void RenderManager::buildSubmarineScene()
{
    camera->setPosition(Ogre::Vector3(0, 0, 10));
    camera->lookAt(Ogre::Vector3(0, 0, 0));
    camera->setNearClipDistance(2);
    camera->setFarClipDistance(50);
    
    scene_manager->setAmbientLight(Ogre::ColourValue(.05,.05,.05));
    Ogre::Light* light = scene_manager->createLight("Light");
    light->setType(Ogre::Light::LT_DIRECTIONAL);
    //light->setType(Ogre::Light::LT_POINT);

    light->setDiffuseColour(1.0, 1.0, 1.0);
    light->setDirection(Ogre::Vector3(0.0,0.0,-1.0));
    //light->setPosition(Ogre::Vector3(1.0,1.0,3.0));

    Ogre::SceneNode* rsn = scene_manager->getRootSceneNode();

    //Entire Submarine with all subcomponents

    //the entire sub transform node is a child to the entire sub animation node
    Ogre::SceneNode* entire_submarine_animation_node = scene_manager->createSceneNode("SubmarineAnimationNode");
    rsn->addChild(entire_submarine_animation_node);

    //process the top level submarine transform node (attached directly to the root node)
    Ogre::SceneNode* entire_sub_transform_node = scene_manager->createSceneNode("EntireSubTransformNode");
    entire_submarine_animation_node->addChild(entire_sub_transform_node);

    Vector3 entire_sub_translation(0,0,0);
    entire_sub_transform_node->translate(entire_sub_translation);

    Vector3 entire_submarine_axis(0,0,1);
    Quaternion entire_submarine_quat(Degree(45), entire_submarine_axis);
    entire_sub_transform_node->rotate(entire_submarine_quat);

    //Submarine

    Ogre::SceneNode* submarine_node = scene_manager->createSceneNode("SubmarineNode");
    entire_sub_transform_node->addChild(submarine_node);

    Ogre::Entity* submarine_entity = scene_manager->createEntity("Submarine", "Submarine.mesh");
    submarine_entity->setMaterialName("Submarine");
    
    //link the entity to the scene node
    submarine_node->attachObject(submarine_entity);

    Vector3 submarine_scale(1,1,1);
    submarine_node->scale(submarine_scale);

    Vector3 submarine_translation(0,0,0);
    submarine_node->translate(submarine_translation);

//
    submarine_node->roll(Degree(90));
    //yaw, pitch, roll version
    submarine_node->yaw(Degree(90));
    //rotate to adjust to Ogre coordinates (from Blender coordinates)
    //this must be applied first, so it is the last rotation added
    submarine_node->pitch(Degree(90));

    //quaternion version
    Vector3 submarine_axis2(0,1,0);
    Quaternion submarine_quat2(Degree(90), submarine_axis2);
    submarine_node->rotate(submarine_quat2);

    //rotate to adjust to Ogre coordinates (from Blender coordinates)
    //this must be applied first, so it is the last rotation added
    Vector3 submarine_axis(1,0,0);
    Quaternion submarine_quat(Degree(90), submarine_axis);
    submarine_node->rotate(submarine_quat);
//

    //quaternion
    Vector3 submarine_axis(.57735, .57735, -.57735);
    Quaternion submarine_quat(Degree(120), submarine_axis);
    submarine_node->rotate(submarine_quat);

    //build the animation that is applied to the submarine animation node
    buildSubmarineAnimation(entire_submarine_animation_node);

    //Periscope

    Ogre::SceneNode* periscope_transform_node = scene_manager->createSceneNode("PeriscopeTransformNode");
    Vector3 periscope_transform_translation(0,.7,0);
    periscope_transform_node->translate(periscope_transform_translation);
    entire_sub_transform_node->addChild(periscope_transform_node);

    //note that this animation node is below the periscope transform node
    Ogre::SceneNode* periscope_animation_node = scene_manager->createSceneNode("PeriscopeAnimationNode");
    periscope_transform_node->addChild(periscope_animation_node);

    Ogre::SceneNode* periscope_node = scene_manager->createSceneNode("PeriscopeNode");
    
    Ogre::Entity* periscope_entity = scene_manager->createEntity("Periscope", "Periscope.mesh");
    periscope_entity->setMaterialName("Periscope");
    
    //link the entity to the scene node
    periscope_node->attachObject(periscope_entity);
    //link the scene node to the root node
    periscope_animation_node->addChild(periscope_node);

    Vector3 periscope_translation(0,0,0);
    periscope_node->translate(periscope_translation);

    //assign transformations to the scene node
    Vector3 periscope_axis(0,1,0);
    Quaternion periscope_quat(Degree(90), periscope_axis);
    periscope_node->rotate(periscope_quat);

    Vector3 periscope_scale(.1,.1,.1);
    periscope_node->scale(periscope_scale);

    buildPeriscopeAnimation(periscope_animation_node);

    //Rudder

    Ogre::SceneNode* rudder_transform_node = scene_manager->createSceneNode("RudderTransformNode");
    Vector3 rudder_transform_translation(-2.8,0.32,0);
    rudder_transform_node->translate(rudder_transform_translation);
    entire_sub_transform_node->addChild(rudder_transform_node);

    Ogre::SceneNode* rudder_animation_node = scene_manager->createSceneNode("RudderAnimationNode");
    rudder_transform_node->addChild(rudder_animation_node);

    Ogre::SceneNode* rudder_node = scene_manager->createSceneNode("RudderNode");
    
    Ogre::Entity* rudder_entity = scene_manager->createEntity("Rudder", "Rudder.mesh");
    rudder_entity->setMaterialName("Rudder");
    
    //link the entity to the scene node
    rudder_node->attachObject(rudder_entity);
    //link the scene node to the root node
    rudder_animation_node->addChild(rudder_node);

    Vector3 rudder_translation(-0.3,0,0);
    rudder_node->translate(rudder_translation);

    //assign transformations to the scene node
    Vector3 rudder_axis(.57735, .57735, .57735);
    Quaternion rudder_quat(Degree(120), rudder_axis);
    rudder_node->rotate(rudder_quat);

    Vector3 rudder_scale(.18,.18,.18);
    rudder_node->scale(rudder_scale);

    buildRudderAnimation(rudder_animation_node);

    //Propeller

    Ogre::SceneNode* propeller_transform_node = scene_manager->createSceneNode("PropellerTransformNode");
    Vector3 propeller_transform_translation(-1.4,-0.8,0);
    propeller_transform_node->translate(propeller_transform_translation);
    entire_sub_transform_node->addChild(propeller_transform_node);

    Ogre::SceneNode* propeller_animation_node = scene_manager->createSceneNode("PropellerAnimationNode");
    propeller_transform_node->addChild(propeller_animation_node);

    //scene nodes can perform transformations and contain entities
    Ogre::SceneNode* propeller_node = scene_manager->createSceneNode("PropellerNode");
    
    //entities are directly associated with meshes and materials
    Ogre::Entity* propeller_entity = scene_manager->createEntity("Propeller", "Propeller.mesh");
    propeller_entity->setMaterialName("Propeller");
    
    //link the entity to the scene node
    propeller_node->attachObject(propeller_entity);
    //link the scene node to the root node
    propeller_animation_node->addChild(propeller_node);

    Vector3 propeller_translation(0,0,0);
    propeller_node->translate(propeller_translation);

    //assign transformations to the scene node
    Vector3 propeller_axis(.57735, .57735, -.57735);
    Quaternion propeller_quat(Degree(120), propeller_axis);
    propeller_node->rotate(propeller_quat);

    Vector3 propeller_scale(.18,.18,.18);
    propeller_node->scale(propeller_scale);

    buildPropellerAnimation(propeller_animation_node);
}

void RenderManager::buildSubmarineAnimation(SceneNode* submarine_animation_node)
{
   Vector3 v(-.7071,.7071,0);
   Ogre::Animation* submarine_animation = scene_manager->createAnimation("Submarine Animation", 10);
   submarine_animation->setInterpolationMode(Ogre::Animation::IM_SPLINE);
   
   //indicate that we want a scene node to be animated (affecting everything lower than it in the scene graph)
   Ogre::NodeAnimationTrack* submarine_track = submarine_animation->createNodeTrack(1, submarine_animation_node);
   
   //specify the rotation at t = 0
   Ogre::TransformKeyFrame* submarine_key = submarine_track->createNodeKeyFrame(0);
   submarine_key->setTranslate(Vector3(-4,-4,0));

   //specify the rotation at t = 1
   submarine_key = submarine_track->createNodeKeyFrame(1);
   submarine_key->setTranslate(Vector3(-2,-2,0));

   //specify the rotation at t = 2
   submarine_key = submarine_track->createNodeKeyFrame(2);
   submarine_key->setTranslate(Vector3(0,0,0));

   //specify the rotation at t = 3
   submarine_key = submarine_track->createNodeKeyFrame(3);
   submarine_key->setTranslate(Vector3(2,2,0));

   //specify the rotation at t = 4
   submarine_key = submarine_track->createNodeKeyFrame(4);
   submarine_key->setTranslate(Vector3(4,4,0));
   Ogre::Quaternion q1(Degree(90), v);
   submarine_key->setRotation(q1);

   //specify the rotation at t = 5
   submarine_key = submarine_track->createNodeKeyFrame(5);
   submarine_key->setTranslate(Vector3(4,4,0));
   Ogre::Quaternion q2(Degree(180), v);
   submarine_key->setRotation(q2);

   //specify the rotation at t = 6
   submarine_key = submarine_track->createNodeKeyFrame(6);
   submarine_key->setTranslate(Vector3(2,2,0));
   submarine_key->setRotation(q2);

   //specify the rotation at t = 7
   submarine_key = submarine_track->createNodeKeyFrame(7);
   submarine_key->setTranslate(Vector3(0,0,0));
   submarine_key->setRotation(q2);

   //specify the rotation at t = 8
   submarine_key = submarine_track->createNodeKeyFrame(8);
   submarine_key->setTranslate(Vector3(-2,-2,0));
   submarine_key->setRotation(q2);

   //specify the rotation at t = 9
   submarine_key = submarine_track->createNodeKeyFrame(9);
   submarine_key->setTranslate(Vector3(-4,-4,0));
   Ogre::Quaternion q3(Degree(270), v);
   submarine_key->setRotation(q3);

   //specify the rotation at t = 10
   submarine_key = submarine_track->createNodeKeyFrame(10);
   submarine_key->setTranslate(Vector3(-4,-4,0));
   Ogre::Quaternion q4(Degree(360), v);
   submarine_key->setRotation(q4);

   Ogre::AnimationState* submarine_animation_state = scene_manager->createAnimationState("Submarine Animation");
   submarine_animation_state->setEnabled(true);
   submarine_animation_state->setLoop(true);

   animation_states->add(submarine_animation_state);
}

void RenderManager::buildRudderAnimation(SceneNode* rudder_animation_node)
{
   Vector3 v(0,1,0);
   Ogre::Animation* rudder_animation = scene_manager->createAnimation("Rudder Animation", 2);
   rudder_animation->setInterpolationMode(Ogre::Animation::IM_SPLINE);
   
   //indicate that we want a scene node to be animated (affecting everything lower than it in the scene graph)
   Ogre::NodeAnimationTrack* rudder_track = rudder_animation->createNodeTrack(1, rudder_animation_node);
   
   //specify the rotation at t = 0.0
   Ogre::TransformKeyFrame* rudder_key = rudder_track->createNodeKeyFrame(0);
   Ogre::Quaternion q1(Degree(0), v);
   rudder_key->setRotation(q1);

   //specify the rotation at t = 0.25
   rudder_key = rudder_track->createNodeKeyFrame(0.25);
   Ogre::Quaternion q2(Degree(22.5), v);
   rudder_key->setRotation(q2);

   //specify the rotation at t = 0.5
   rudder_key = rudder_track->createNodeKeyFrame(0.5);
   Ogre::Quaternion q3(Degree(45), v);
   rudder_key->setRotation(q3);

   //specify the rotation at t = 0.75
   rudder_key = rudder_track->createNodeKeyFrame(0.75);
   rudder_key->setRotation(q2);

   //specify the rotation at t = 1.0
   rudder_key = rudder_track->createNodeKeyFrame(1.0);
   rudder_key->setRotation(q1);

   //specify the rotation at t = 1.25
   rudder_key = rudder_track->createNodeKeyFrame(1.25);
   Ogre::Quaternion q4(Degree(-22.5), v);
   rudder_key->setRotation(q4);

   //specify the rotation at t = 1.5
   rudder_key = rudder_track->createNodeKeyFrame(1.5);
   Ogre::Quaternion q5(Degree(-45), v);
   rudder_key->setRotation(q5);

   //specify the rotation at t = 1.75
   rudder_key = rudder_track->createNodeKeyFrame(1.75);
   rudder_key->setRotation(q4);

   //specify the rotation at t = 2.0
   rudder_key = rudder_track->createNodeKeyFrame(2.0);
   rudder_key->setRotation(q1);

   Ogre::AnimationState* rudder_animation_state = scene_manager->createAnimationState("Rudder Animation");
   rudder_animation_state->setEnabled(true);
   rudder_animation_state->setLoop(true);

   animation_states->add(rudder_animation_state);
}

void RenderManager::buildPeriscopeAnimation(SceneNode* periscope_animation_node)
{
   Vector3 v(0,1,0);
   Ogre::Animation* periscope_animation = scene_manager->createAnimation("Periscope Animation", 2);
   periscope_animation->setInterpolationMode(Ogre::Animation::IM_SPLINE);
   
   //indicate that we want a scene node to be animated (affecting everything lower than it in the scene graph)
   Ogre::NodeAnimationTrack* periscope_track = periscope_animation->createNodeTrack(1, periscope_animation_node);
   
   //specify the rotation at t = 0.0
   Ogre::TransformKeyFrame* periscope_key = periscope_track->createNodeKeyFrame(0);
   Ogre::Quaternion q1(Degree(0), v);
   periscope_key->setRotation(q1);

   //specify the rotation at t = 0.5
   periscope_key = periscope_track->createNodeKeyFrame(0.5);
   Ogre::Quaternion q2(Degree(90), v);
   periscope_key->setRotation(q2);

   //specify the rotation at t = 1
   periscope_key = periscope_track->createNodeKeyFrame(1);
   Ogre::Quaternion q3(Degree(180), v);
   periscope_key->setRotation(q3);

   //specify the rotation at t = 1.5
   periscope_key = periscope_track->createNodeKeyFrame(1.5);
   Ogre::Quaternion q4(Degree(270), v);
   periscope_key->setRotation(q4);

   //specify the rotation at t = 2
   periscope_key = periscope_track->createNodeKeyFrame(2);
   Ogre::Quaternion q5(Degree(360), v);
   periscope_key->setRotation(q5);

   Ogre::AnimationState* periscope_animation_state = scene_manager->createAnimationState("Periscope Animation");
   periscope_animation_state->setEnabled(true);
   periscope_animation_state->setLoop(true);

   animation_states->add(periscope_animation_state);
}

void RenderManager::buildPropellerAnimation(SceneNode* propeller_animation_node)
{
   Vector3 v(1,0,0);
   Ogre::Animation* propeller_animation = scene_manager->createAnimation("Propeller Animation", 1);
   propeller_animation->setInterpolationMode(Ogre::Animation::IM_SPLINE);
   
   //indicate that we want a scene node to be animated (affecting everything lower than it in the scene graph)
   Ogre::NodeAnimationTrack* propeller_track = propeller_animation->createNodeTrack(1, propeller_animation_node);
   
   //specify the rotation at t = 0.0
   Ogre::TransformKeyFrame* propeller_key = propeller_track->createNodeKeyFrame(0);
   Ogre::Quaternion q1(Degree(0), v);
   propeller_key->setRotation(q1);

   //specify the rotation at t = .25
   propeller_key = propeller_track->createNodeKeyFrame(0.25);
   Ogre::Quaternion q2(Degree(90), v);
   propeller_key->setRotation(q2);

   //specify the rotation at t = .5
   propeller_key = propeller_track->createNodeKeyFrame(0.5);
   Ogre::Quaternion q3(Degree(180), v);
   propeller_key->setRotation(q3);

   //specify the rotation at t = .75
   propeller_key = propeller_track->createNodeKeyFrame(0.75);
   Ogre::Quaternion q4(Degree(270), v);
   propeller_key->setRotation(q4);

   //specify the rotation at t = 1
   propeller_key = propeller_track->createNodeKeyFrame(1);
   Ogre::Quaternion q5(Degree(360), v);
   propeller_key->setRotation(q5);

   Ogre::AnimationState* propeller_animation_state = scene_manager->createAnimationState("Propeller Animation");
   propeller_animation_state->setEnabled(true);
   propeller_animation_state->setLoop(true);

   animation_states->add(propeller_animation_state);
}

void RenderManager::buildXWingAnimationScene()
{
    SceneNode* rsn = scene_manager->getRootSceneNode();
    SceneNode* starShip = scene_manager->createSceneNode("StarShip");
    
    Ogre::Entity* bodyEnt = scene_manager->createEntity("Body", "Body.mesh");
    bodyEnt->setMaterialName("Body");
    Ogre::Entity* robotEnt = scene_manager->createEntity("Robot", "Robot.mesh");
    robotEnt->setMaterialName("Robot");
    
    SceneNode* body = scene_manager->createSceneNode("BodyNode");
    body->attachObject(bodyEnt);
    
    body->pitch(Ogre::Degree(90), Node::TS_LOCAL);
    
    SceneNode* robot = scene_manager->createSceneNode("RobotNode");
    robot->attachObject(robotEnt);
    
    robot->scale(.5, .5, .5);
    
    SceneNode* robotTr = scene_manager->createSceneNode("RobotTranNode");
    
    robotTr->translate(0, .85, -3.75, Node::TS_LOCAL);
    
    SceneNode* robotAn = scene_manager->createSceneNode("RobotAnimNode");
    
    buildRobotAnimation(robotAn);
    robotAn->addChild(robot);
    robotTr->addChild(robotAn);
    
    SceneNode* bodyWR = scene_manager->createSceneNode("bodyWithRobot");
    bodyWR->addChild(body);
    bodyWR->addChild(robotTr);
    
    bodyWR->scale(.9, .9, .9);
    
    starShip->addChild(bodyWR);
    
    SceneNode* URWing = scene_manager->createSceneNode("URWingNode");
    SceneNode* wing1 = scene_manager->createSceneNode("WingNode1");
    URWing->addChild(wing1);
    Ogre::Entity* wingEnt1 = scene_manager->createEntity("Wing1", "Wing.mesh");
    wingEnt1->setMaterialName("Wing");
    wing1->attachObject(wingEnt1);
    
    wing1->roll(Ogre::Degree(-90), Node:: TS_LOCAL);
    
    SceneNode* Eng1 = scene_manager->createSceneNode("EngineNode1");
    
    Ogre::Entity* EngEnt1 = scene_manager->createEntity("Engine1", "Engine.mesh");
    EngEnt1->setMaterialName("Engine");
    Eng1->attachObject(EngEnt1);
    
    Eng1->pitch(Ogre::Degree(90), Node::TS_LOCAL);
    Eng1->scale(.45, .45, .45);
    
    SceneNode* EngTr1 = scene_manager->createSceneNode("EngineTranNode1");
    EngTr1->translate(-.35, .50, -.45, Node::TS_LOCAL);
    URWing->addChild(EngTr1);
    EngTr1->addChild(Eng1);
    
    SceneNode* Gun1 = scene_manager->createSceneNode("GunNode1");
    
    Ogre::Entity* GunEnt1 = scene_manager->createEntity("Gun1", "Gun.mesh");
    GunEnt1->setMaterialName("Gun");
    Gun1->attachObject(GunEnt1);
    
    Gun1->pitch(Ogre::Degree(90), Node::TS_LOCAL);
    Gun1->scale(.225, .225, .225);
    
    SceneNode* GunTr1 = scene_manager->createSceneNode("GunTranNode1");
    GunTr1->translate(2.55, .2, 1.85, Node::TS_LOCAL);
    URWing->addChild(GunTr1);
    GunTr1->addChild(Gun1);
    
    URWing->translate(.85, 0, 0, Node::TS_LOCAL);
    
    SceneNode* URWingAn = scene_manager->createSceneNode("URWingAnim");
    
    SceneNode* URWingTr = scene_manager->createSceneNode("URWingTran");
    
    buildURWingAnimation(URWingAn);
    URWingAn->addChild(URWing);
    URWingTr->addChild(URWingAn);
    
    URWingTr->translate(.8, 0, -4, Node::TS_LOCAL);
    
    starShip->addChild(URWingTr);
    
    SceneNode* LRWing = scene_manager->createSceneNode("LRWingNode");
    SceneNode* wing2 = scene_manager->createSceneNode("WingNode2");
    LRWing->addChild(wing2);
    
    Ogre::Entity* wingEnt2 = scene_manager->createEntity("Wing2", "Wing.mesh");
    wingEnt2->setMaterialName("Wing");
    wing2->attachObject(wingEnt2);
    
    wing2->roll(Ogre::Degree(-90), Node:: TS_LOCAL);
    
    SceneNode* Eng2 = scene_manager->createSceneNode("EngineNode2");
    
    Ogre::Entity* EngEnt2 = scene_manager->createEntity("Engine2", "Engine.mesh");
    EngEnt2->setMaterialName("Engine");
    Eng2->attachObject(EngEnt2);
    
    Eng2->pitch(Ogre::Degree(90), Node::TS_LOCAL);
    Eng2->scale(.45, .45, .45);
    
    SceneNode* EngTr2 = scene_manager->createSceneNode("EngineTranNode2");
    EngTr2->translate(-.35, -.50, -.45, Node::TS_LOCAL);
    LRWing->addChild(EngTr2);
    EngTr2->addChild(Eng2);
    
    SceneNode* Gun2 = scene_manager->createSceneNode("GunNode2");
    
    Ogre::Entity* GunEnt2 = scene_manager->createEntity("Gun2", "Gun.mesh");
    GunEnt2->setMaterialName("Gun");
    Gun2->attachObject(GunEnt2);
    
    Gun2->pitch(Ogre::Degree(90), Node::TS_LOCAL);
    Gun2->scale(.225, .225, .225);
    
    SceneNode* GunTr2 = scene_manager->createSceneNode("GunTranNode2");
    GunTr2->translate(2.55, -.2, 1.85, Node::TS_LOCAL);
    LRWing->addChild(GunTr2);
    GunTr2->addChild(Gun2);
    
    LRWing->translate(.85, 0, 0, Node::TS_LOCAL);
    
    SceneNode* LRWingAn = scene_manager->createSceneNode("LRWingAnim");
    
    SceneNode* LRWingTr = scene_manager->createSceneNode("LRWingTran");
    
    buildLRWingAnimation(LRWingAn);
    LRWingAn->addChild(LRWing);
    LRWingTr->addChild(LRWingAn);
    
    LRWing->translate(.8, -.2, -4, Node::TS_LOCAL);
    
    starShip->addChild(LRWingTr);
    
    SceneNode * ULWing = scene_manager->createSceneNode("ULWingNode");
    SceneNode* wing3 = scene_manager->createSceneNode("WingNode3");
    ULWing->addChild(wing3);
    
    Ogre::Entity* wingEnt3 = scene_manager->createEntity("Wing3", "Wing.mesh");
    wingEnt3->setMaterialName("Wing");
    wing3->attachObject(wingEnt3);
    
    wing3->roll(Ogre::Degree(90), Node:: TS_LOCAL);
    
    SceneNode* Eng3 = scene_manager->createSceneNode("EngineNode3");
    
    Ogre::Entity* EngEnt3 = scene_manager->createEntity("Engine3", "Engine.mesh");
    EngEnt3->setMaterialName("Engine");
    Eng3->attachObject(EngEnt3);
    
    Eng3->pitch(Ogre::Degree(90), Node::TS_LOCAL);
    Eng3->scale(.45, .45, .45);
    
    SceneNode* EngTr3 = scene_manager->createSceneNode("EngineTranNode3");
    EngTr3->translate(.35, .50, -.45, Node::TS_LOCAL);
    ULWing->addChild(EngTr3);
    EngTr3->addChild(Eng3);
    
    SceneNode* Gun3 = scene_manager->createSceneNode("GunNode3");
    
    Ogre::Entity* GunEnt3 = scene_manager->createEntity("Gun3", "Gun.mesh");
    GunEnt3->setMaterialName("Gun");
    Gun3->attachObject(GunEnt3);
    
    Gun3->pitch(Ogre::Degree(90), Node::TS_LOCAL);
    Gun3->scale(.225, .225, .225);
    
    SceneNode* GunTr3 = scene_manager->createSceneNode("GunTranNode3");
    GunTr3->translate(-2.55, .2, 1.85, Node::TS_LOCAL);
    ULWing->addChild(GunTr3);
    GunTr3->addChild(Gun3);
    
    ULWing->translate(-.85, 0, 0, Node::TS_LOCAL);
    
    SceneNode* ULWingAn = scene_manager->createSceneNode("ULWingAnim");
    
    SceneNode* ULWingTr = scene_manager->createSceneNode("ULWingTran");
    
    buildULWingAnimation(ULWingAn);
    ULWingAn->addChild(ULWing);
    ULWingTr->addChild(ULWingAn);
    
    ULWing->translate(-.8, 0, -4, Node::TS_LOCAL);
    
    starShip->addChild(ULWingTr);
    
    SceneNode * LLWing = scene_manager->createSceneNode("LLWingNode");
    SceneNode* wing4 = scene_manager->createSceneNode("WingNode4");
    LLWing->addChild(wing4);
    
    Ogre::Entity* wingEnt4 = scene_manager->createEntity("Wing4", "Wing.mesh");
    wingEnt4->setMaterialName("Wing");
    wing4->attachObject(wingEnt4);
    
    wing4->roll(Ogre::Degree(90), Node:: TS_LOCAL);
    
    SceneNode* Eng4 = scene_manager->createSceneNode("EngineNode4");
    
    Ogre::Entity* EngEnt4 = scene_manager->createEntity("Engine4", "Engine.mesh");
    EngEnt4->setMaterialName("Engine");
    Eng4->attachObject(EngEnt4);
    
    Eng4->pitch(Ogre::Degree(90), Node::TS_LOCAL);
    Eng4->scale(.45, .45, .45);
    
    SceneNode* EngTr4 = scene_manager->createSceneNode("EngineTranNode4");
    EngTr4->translate(.35, -.50, -.45, Node::TS_LOCAL);
    LLWing->addChild(EngTr4);
    EngTr4->addChild(Eng4);
    
    SceneNode* Gun4 = scene_manager->createSceneNode("GunNode4");
    
    Ogre::Entity* GunEnt4 = scene_manager->createEntity("Gun4", "Gun.mesh");
    GunEnt4->setMaterialName("Gun");
    Gun4->attachObject(GunEnt4);
    
    Gun4->pitch(Ogre::Degree(90), Node::TS_LOCAL);
    Gun4->scale(.225, .225, .225);
    
    SceneNode* GunTr4 = scene_manager->createSceneNode("GunTranNode4");
    GunTr4->translate(-2.55, -.2, 1.85, Node::TS_LOCAL);
    LLWing->addChild(GunTr4);
    GunTr4->addChild(Gun4);
    
    LLWing->translate(-.85, 0, 0, Node::TS_LOCAL);
    
    SceneNode* LLWingAn = scene_manager->createSceneNode("LLWingAnim");
    
    SceneNode* LLWingTr = scene_manager->createSceneNode("LLWingTran");
    
    buildLLWingAnimation(LLWingAn);
    LLWingAn->addChild(LLWing);
    LLWingTr->addChild(LLWingAn);
    
    LLWing->translate(-.8, -.2, -4, Node::TS_LOCAL);
    
    starShip->addChild(LLWingTr);
    
    Ogre::Quaternion q1(Degree(45), Vector3(-1,1,0));
    starShip->rotate(q1, Node::TS_LOCAL);
    
    
    SceneNode* SSAn = scene_manager->createSceneNode("StarShipAnimNode");
    buildStarShipAnimation(SSAn);
    
    SSAn->addChild(starShip);
    rsn->addChild(SSAn);
}

void RenderManager::buildRobotAnimation(Ogre::SceneNode* robot_node)
{
   Ogre::Animation* robot_animation = scene_manager->createAnimation("Robot Animation", 4);
   robot_animation->setInterpolationMode(Ogre::Animation::IM_SPLINE);
   Ogre::NodeAnimationTrack* robot_track = robot_animation->createNodeTrack(1, robot_node);
   
   Ogre::TransformKeyFrame* robot_key = robot_track->createNodeKeyFrame(0);
   Ogre::Quaternion q1(Degree(0), Vector3(0,1,0));
   robot_key->setRotation(q1);
   
   robot_key = robot_track->createNodeKeyFrame(1);
   Ogre::Quaternion q2(Degree(45), Vector3(0,1,0));
   robot_key->setRotation(q2);
   
   robot_key = robot_track->createNodeKeyFrame(2);
   Ogre::Quaternion q3(Degree(0), Vector3(0,1,0));
   robot_key->setRotation(q3);
   
   robot_key = robot_track->createNodeKeyFrame(3);
   Ogre::Quaternion q4(Degree(-45), Vector3(0,1,0));
   robot_key->setRotation(q4);
   
   robot_key = robot_track->createNodeKeyFrame(4);
   Ogre::Quaternion q5(Degree(0), Vector3(0,1,0));
   robot_key->setRotation(q5);
   
   Ogre::AnimationState* robot_animation_state = scene_manager->createAnimationState("Robot Animation");
   robot_animation_state->setEnabled(true);
   robot_animation_state->setLoop(true);

   animation_states->add(robot_animation_state);
}

void RenderManager::buildURWingAnimation(Ogre::SceneNode* URWing)
{
   Ogre::Animation* URWing_animation = scene_manager->createAnimation("URWing Animation", 20);
   URWing_animation->setInterpolationMode(Ogre::Animation::IM_SPLINE);
   Ogre::NodeAnimationTrack* URWing_track = URWing_animation->createNodeTrack(1, URWing);
   
   Ogre::TransformKeyFrame* URWing_key = URWing_track->createNodeKeyFrame(0);
   Ogre::Quaternion q1(Degree(0), Vector3(0,0,1));
   URWing_key->setRotation(q1);
   
   URWing_key = URWing_track->createNodeKeyFrame(2);
   Ogre::Quaternion q2(Degree(15), Vector3(0,0,1));
   URWing_key->setRotation(q2);
   
   URWing_key = URWing_track->createNodeKeyFrame(10);
   Ogre::Quaternion q3(Degree(15), Vector3(0,0,1));
   URWing_key->setRotation(q3);
   
   URWing_key = URWing_track->createNodeKeyFrame(12);
   Ogre::Quaternion q4(Degree(0), Vector3(0,0,1));
   URWing_key->setRotation(q4);
   
   URWing_key = URWing_track->createNodeKeyFrame(20);
   Ogre::Quaternion q5(Degree(0), Vector3(0,0,1));
   URWing_key->setRotation(q5);
   
   Ogre::AnimationState* URWing_animation_state = scene_manager->createAnimationState("URWing Animation");
   URWing_animation_state->setEnabled(true);
   URWing_animation_state->setLoop(true);

   animation_states->add(URWing_animation_state);
}

void RenderManager::buildLRWingAnimation(Ogre::SceneNode* LRWing)
{
   Ogre::Animation* LRWing_animation = scene_manager->createAnimation("LRWing Animation", 20);
   LRWing_animation->setInterpolationMode(Ogre::Animation::IM_SPLINE);
   Ogre::NodeAnimationTrack* LRWing_track = LRWing_animation->createNodeTrack(1, LRWing);
   
   Ogre::TransformKeyFrame* LRWing_key = LRWing_track->createNodeKeyFrame(0);
   Ogre::Quaternion q1(Degree(0), Vector3(0,0,1));
   LRWing_key->setRotation(q1);
   
   LRWing_key = LRWing_track->createNodeKeyFrame(2);
   Ogre::Quaternion q2(Degree(-15), Vector3(0,0,1));
   LRWing_key->setRotation(q2);
   
   LRWing_key = LRWing_track->createNodeKeyFrame(10);
   Ogre::Quaternion q3(Degree(-15), Vector3(0,0,1));
   LRWing_key->setRotation(q3);
   
   LRWing_key = LRWing_track->createNodeKeyFrame(12);
   Ogre::Quaternion q4(Degree(0), Vector3(0,0,1));
   LRWing_key->setRotation(q4);
   
   LRWing_key = LRWing_track->createNodeKeyFrame(20);
   Ogre::Quaternion q5(Degree(0), Vector3(0,0,1));
   LRWing_key->setRotation(q5);
   
   Ogre::AnimationState* LRWing_animation_state = scene_manager->createAnimationState("LRWing Animation");
   LRWing_animation_state->setEnabled(true);
   LRWing_animation_state->setLoop(true);

   animation_states->add(LRWing_animation_state);
}

void RenderManager::buildULWingAnimation(Ogre::SceneNode* ULWing)
{
   Ogre::Animation* ULWing_animation = scene_manager->createAnimation("ULWing Animation", 20);
   ULWing_animation->setInterpolationMode(Ogre::Animation::IM_SPLINE);
   Ogre::NodeAnimationTrack* ULWing_track = ULWing_animation->createNodeTrack(1, ULWing);
   
   Ogre::TransformKeyFrame* ULWing_key = ULWing_track->createNodeKeyFrame(0);
   Ogre::Quaternion q1(Degree(0), Vector3(0,0,1));
   ULWing_key->setRotation(q1);
   
   ULWing_key = ULWing_track->createNodeKeyFrame(2);
   Ogre::Quaternion q2(Degree(-15), Vector3(0,0,1));
   ULWing_key->setRotation(q2);
   
   ULWing_key = ULWing_track->createNodeKeyFrame(10);
   Ogre::Quaternion q3(Degree(-15), Vector3(0,0,1));
   ULWing_key->setRotation(q3);
   
   ULWing_key = ULWing_track->createNodeKeyFrame(12);
   Ogre::Quaternion q4(Degree(0), Vector3(0,0,1));
   ULWing_key->setRotation(q4);
   
   ULWing_key = ULWing_track->createNodeKeyFrame(20);
   Ogre::Quaternion q5(Degree(0), Vector3(0,0,1));
   ULWing_key->setRotation(q5);
   
   Ogre::AnimationState* ULWing_animation_state = scene_manager->createAnimationState("ULWing Animation");
   ULWing_animation_state->setEnabled(true);
   ULWing_animation_state->setLoop(true);

   animation_states->add(ULWing_animation_state);
}

void RenderManager::buildLLWingAnimation(Ogre::SceneNode* LLWing)
{
   Ogre::Animation* LLWing_animation = scene_manager->createAnimation("LLWing Animation", 20);
   LLWing_animation->setInterpolationMode(Ogre::Animation::IM_SPLINE);
   Ogre::NodeAnimationTrack* LLWing_track = LLWing_animation->createNodeTrack(1, LLWing);
   
   Ogre::TransformKeyFrame* LLWing_key = LLWing_track->createNodeKeyFrame(0);
   Ogre::Quaternion q1(Degree(0), Vector3(0,0,1));
   LLWing_key->setRotation(q1);
   
   LLWing_key = LLWing_track->createNodeKeyFrame(2);
   Ogre::Quaternion q2(Degree(15), Vector3(0,0,1));
   LLWing_key->setRotation(q2);
   
   LLWing_key = LLWing_track->createNodeKeyFrame(10);
   Ogre::Quaternion q3(Degree(15), Vector3(0,0,1));
   LLWing_key->setRotation(q3);
   
   LLWing_key = LLWing_track->createNodeKeyFrame(12);
   Ogre::Quaternion q4(Degree(0), Vector3(0,0,1));
   LLWing_key->setRotation(q4);
   
   LLWing_key = LLWing_track->createNodeKeyFrame(20);
   Ogre::Quaternion q5(Degree(0), Vector3(0,0,1));
   LLWing_key->setRotation(q5);
   
   Ogre::AnimationState* LLWing_animation_state = scene_manager->createAnimationState("LLWing Animation");
   LLWing_animation_state->setEnabled(true);
   LLWing_animation_state->setLoop(true);

   animation_states->add(LLWing_animation_state);
}

void RenderManager::buildStarShipAnimation(Ogre::SceneNode* starShip)
{
   Ogre::Animation* starship_animation = scene_manager->createAnimation("Starship Animation", 10);
   starship_animation->setInterpolationMode(Ogre::Animation::IM_SPLINE);
   Ogre::NodeAnimationTrack* starship_track = starship_animation->createNodeTrack(1, starShip);
   
   Ogre::TransformKeyFrame* starship_key = starship_track->createNodeKeyFrame(0);
   Ogre::Quaternion q1(Degree(0), Vector3(1,1,1));
   starship_key->setRotation(q1);
   
   starship_key = starship_track->createNodeKeyFrame(1);
   Ogre::Quaternion q2(Degree(-36), Vector3(1,1,1));
   starship_key->setRotation(q2);
   
   starship_key = starship_track->createNodeKeyFrame(2);
   Ogre::Quaternion q3(Degree(-72), Vector3(1,1,1));
   starship_key->setRotation(q3);
   
   starship_key = starship_track->createNodeKeyFrame(3);
   Ogre::Quaternion q4(Degree(-108), Vector3(1,1,1));
   starship_key->setRotation(q4);
   
   starship_key = starship_track->createNodeKeyFrame(4);
   Ogre::Quaternion q5(Degree(-144), Vector3(1,1,1));
   starship_key->setRotation(q5);
   
   starship_key = starship_track->createNodeKeyFrame(5);
   Ogre::Quaternion q6(Degree(-180), Vector3(1,1,1));
   starship_key->setRotation(q6);
   
   starship_key = starship_track->createNodeKeyFrame(6);
   Ogre::Quaternion q7(Degree(-216), Vector3(1,1,1));
   starship_key->setRotation(q7);
   
   starship_key = starship_track->createNodeKeyFrame(7);
   Ogre::Quaternion q8(Degree(-252), Vector3(1,1,1));
   starship_key->setRotation(q8);
   
   starship_key = starship_track->createNodeKeyFrame(8);
   Ogre::Quaternion q9(Degree(-288), Vector3(1,1,1));
   starship_key->setRotation(q9);
   
   starship_key = starship_track->createNodeKeyFrame(9);
   Ogre::Quaternion q10(Degree(-324), Vector3(1,1,1));
   starship_key->setRotation(q10);
   
   starship_key = starship_track->createNodeKeyFrame(10);
   Ogre::Quaternion q11(Degree(-360), Vector3(1,1,1));
   starship_key->setRotation(q11);
   
   Ogre::AnimationState* starship_animation_state = scene_manager->createAnimationState("Starship Animation");
   starship_animation_state->setEnabled(true);
   starship_animation_state->setLoop(true);

   animation_states->add(starship_animation_state);
}

*/