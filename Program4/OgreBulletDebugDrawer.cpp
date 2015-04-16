#include "OgreBulletDebugDrawer.h"
#include "RenderManager.h"

OgreBulletDebugDrawer::OgreBulletDebugDrawer(RenderManager* rm/*Ogre::SceneManager* scene_manager*/)
{
   render_manager = rm;
   db = rm->createOgreBulletDebugDrawer();
   // this->scene_manager = scene_manager;
	
   // manual_object =  scene_manager->createManualObject("PhysicsLines");
   // manual_object->setDynamic(true);
   // static const char* matName = "OgreBulletCollisionsDebugDefault";
   // Ogre::MaterialPtr manual_object_material = Ogre::MaterialManager::getSingleton().getDefaultSettings()->clone(matName);
   // manual_object_material->setReceiveShadows(false); 
   // manual_object_material->getTechnique(0)->setLightingEnabled(false); 
   // manual_object_node = scene_manager->getRootSceneNode()->createChildSceneNode();
   // manual_object_node->attachObject(manual_object);
	
   count = (int*)malloc(sizeof(int));
   count[0] = 0;
}

OgreBulletDebugDrawer::~OgreBulletDebugDrawer()
{
   //delete scene_manager;
   //delete manual_object;
   render_manager->destroyDebugDrawer(db);

   free(count);
}

void OgreBulletDebugDrawer::drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
{
   // make these three arrays with the x,y,z

   int from_value[3] = {(int)from.x(), (int)from.y(),(int)from.z()};
   int to_value[3] = {(int)from.x(), (int)from.y(),(int)from.z() };
   int color_value[3] = {(int)from.x(), (int)from.y(),(int)from.z()};
   render_manager->drawLine(db, from_value, to_value, color_value, count);
   count[0]++;

}

void OgreBulletDebugDrawer::drawContactPoint(const btVector3 &PointOnB, const btVector3 &normalOnB, btScalar distance, int lifeTime, const btVector3 &color)
{

}

void OgreBulletDebugDrawer::reportErrorWarning(const char *warningString)
{
   std::cout << warningString << "\n";
}

void OgreBulletDebugDrawer::draw3dText(const btVector3 &location, const char *textString)
{

}

void OgreBulletDebugDrawer::setDebugMode(int debug_mode)
{
   this->debug_mode = debug_mode;
}

int OgreBulletDebugDrawer::getDebugMode() const
{
   return (DebugDrawModes) debug_mode;
}

void OgreBulletDebugDrawer::clearLines()
{
   render_manager->clearLines(db);
}