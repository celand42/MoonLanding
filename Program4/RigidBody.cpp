#include "RigidBody.h"
#include <iostream>

RigidBody::RigidBody(std::string id, btRigidBody* rb)
{
   rigid_body_id = id;
   rigid_body = rb;
}

RigidBody::~RigidBody()
{
   delete rigid_body;
}

std::string RigidBody::getKey()
{
   return rigid_body_id;
}

btRigidBody* RigidBody::getRigidBody()
{
   return rigid_body;
}

int RigidBody::compare_items(RigidBody* rgb1, RigidBody* rgb2)
{
   std::string key_1 = rgb1->getKey();
   std::string key_2 = rgb2->getKey();
   int key_compare = key_1.compare(key_2);
   return key_compare;
}

int RigidBody::compare_keys(std::string* key, RigidBody* rgb)
{
   std::string key_1 = *key;
   std::string key_2 = rgb->getKey();
   int key_compare = key_1.compare(key_2);
   return key_compare;
}
