#include "ResourceManager.h"
#include "GameManager.h"

#include "MeshResource.h"
#include "PathResource.h"
#include "AudioResource.h"

#include "TinyXML.h"

std::string ResourceManager::getCurrentGroupName()
{
   return current_group;
}

ResourceManager::ResourceManager(GameManager* gm)
{
   game_manager = gm;
   current_group = "";
   all_resources = new TableAVL<KeyedListArray<GameResource>, string >(&KeyedListArray<GameResource>::compare_items, &KeyedListArray<GameResource>::compare_keys);
}

ResourceManager::~ResourceManager()
{
   AVLTreeIterator<KeyedListArray<GameResource> >* all_lists_iter = all_resources->tableIterator();

   while(all_lists_iter->hasNext())
   {
      KeyedListArray<GameResource>* list = all_lists_iter->next();
      ListArrayIterator<GameResource>* list_iter = list->iterator();

      while(list_iter->hasNext())
      {
         GameResource* resource = list_iter->next();
         delete resource;
      }

      delete list_iter;
      delete list;
   }

   delete all_lists_iter;
   delete all_resources;
}

void ResourceManager::loadResources(std::string group_name)
{
   if (current_group == group_name) return;
   if (current_group != "") unloadResources();

   KeyedListArray<GameResource>* list = all_resources->tableRetrieve(&group_name);

   //make sure that the PATHs are all loaded first (these directories are checked for the meshes)
   ListArrayIterator<GameResource>* iter = list->iterator();
   while(iter->hasNext())
   {
      GameResource* resource = iter->next();
      if (resource->getResourceType() == PATH)
      {
         resource->load();
      }
   }
   delete iter;

   //load the rest
   iter = list->iterator();
   while(iter->hasNext())
   {
      GameResource* resource = iter->next();
      GameResourceType grt = resource->getResourceType();

      if (grt != PATH)
      {
         resource->load();
      }
   }
   delete iter;

   //initialize and load the Ogre group
   game_manager->initialiseRenderResourceGroup(group_name);
   game_manager->loadRenderResourceGroup(group_name);


cout << "meshes should all be loaded" << endl;
   current_group = group_name;
}

GameResource* ResourceManager::findResourceByID(uint32 resource_id)
{
   if (current_group == "") return NULL;

   KeyedListArray<GameResource>* current_list = all_resources->tableRetrieve(&current_group);
   ListArrayIterator<GameResource>* current_list_iter = current_list->iterator();

   while(current_list_iter->hasNext())
   {
      GameResource* game_resource = current_list_iter->next();
      uint32 test_resource_id = game_resource->getResourceID();

      if (test_resource_id == resource_id)
      {
         delete current_list_iter;
         return game_resource;
/*
         if (game_resource->isLoaded())
         {
            return game_resource;
         }
         else
         {
            return NULL;
         }
*/
      }
   }
   delete current_list_iter;

   return NULL;  //the requested resource_id was not found
}

void ResourceManager::unloadResources()
{
   std::string group_name = current_group;

   KeyedListArray<GameResource>* list = all_resources->tableRetrieve(&group_name);  //old scope

   ListArrayIterator<GameResource>* iter = list->iterator();
   while(iter->hasNext())
   {
      GameResource* resource = iter->next();       
      resource->unload();
   }

   delete iter;

   game_manager->unloadRenderResourceGroup(group_name);  //path and mesh resources are actually unloaded here
   current_group = "";
}

void ResourceManager::loadFromXMLFile(std::string file_name)
{
   int resource_count = 1;

   TiXmlDocument doc(file_name.c_str());
   if (doc.LoadFile())
   {
      TiXmlNode* resource_tree = doc.FirstChild("resources");
      if (resource_tree)
      {
         //Enumerate resource objects (eventually, child will be false and loop will terminate)
         for(TiXmlNode* child = resource_tree->FirstChild(); child; child = child->NextSibling())
         {
            TiXmlElement* resource_element = child->ToElement();
            if(resource_element)
            {
               GameResource* game_resource = NULL;

               TiXmlElement* uid_element = (TiXmlElement*) resource_element->FirstChild("uid");
               if (!uid_element) ASSERT(false);
               uint32 uid = atoi(uid_element->GetText());
               TiXmlElement* type_element = (TiXmlElement*) resource_element->FirstChild("resource_type");
               if (!type_element) ASSERT(false);
               std::string resource_type = type_element->GetText();
               TiXmlElement* filename_element = (TiXmlElement*) resource_element->FirstChild("file_name");
               if (!filename_element) ASSERT(false);
               std::string file_name = filename_element->GetText();
               TiXmlElement* scope_element = (TiXmlElement*) resource_element->FirstChild("scope");
               if (!scope_element) ASSERT(false);
               std::string scope = scope_element->GetText();

               if (resource_type == "audio")  //audio elements have an additional field, audio_type
               {
                  TiXmlElement* audio_element = (TiXmlElement*) resource_element->FirstChild("audio_type");
                  std::string audio_type = audio_element->GetText();

                  if (audio_type == "stream")
                  {
                     game_resource = new AudioResource(uid, scope, file_name, AUDIO, STREAM, game_manager);
                  }
                  else
                  {
                     game_resource = new AudioResource(uid, scope, file_name, AUDIO, SAMPLE, game_manager);
                  }
               }

               else if (resource_type == "path")
               {
                  game_resource = new PathResource(uid, scope, file_name, PATH, game_manager);
               }
               else if (resource_type == "mesh")
               {
                  game_resource = new MeshResource(uid, scope, file_name, MESH, game_manager);
               }

               if (game_resource)
               {
                  KeyedListArray<GameResource>* list = all_resources->tableRetrieve(&scope);  //place in the appropriate list
                  if (list)
                  {
                     list->add(game_resource);
                  }
                  else
                  {
                     list = new KeyedListArray<GameResource>(scope);
                     list->add(game_resource);
                     all_resources->tableInsert(list);
                  }
                  resource_count++;
               }
            }  //individual resource element

         }  //resource elements for loop

         game_manager->logComment("Resource metadata loaded.");

      }  //resource tree

   }  //document
   else 
   {
      //log the fact that the resource metadata file was not found or the file not a valid xml file
      game_manager->logComment("Resource metadata file not found.");
      ASSERT(false);
   }

}
