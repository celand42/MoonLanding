#include "CompoundShape.h"

CompoundShape::CompoundShape(std::string i, btCompoundShape* s)
{
   id = i;
   shape = s;
   mass = 0;
}

CompoundShape::~CompoundShape()
{
   shape = 0;
}

float CompoundShape::getMass()
{
   return mass;
}

void CompoundShape::addChildShape(btTransform* tr, btCollisionShape* cs, float m)
{
   shape->addChildShape(*tr, cs);
   mass += m;
}

btCompoundShape* CompoundShape::getCompoundShape()
{
   return shape;
}

std::string CompoundShape::getKey()
{
   return id;
}

int CompoundShape::compare_items(CompoundShape* c1, CompoundShape* c2)
{
   std::string key_1 = c1->getKey();
   std::string key_2 = c2->getKey();
   int key_compare = key_1.compare(key_2);
   return key_compare;
}

int CompoundShape::compare_keys(std::string* key, CompoundShape* c)
{
   std::string key_1 = *key;
   std::string key_2 = c->getKey();
   int key_compare = key_1.compare(key_2);
   return key_compare;
}