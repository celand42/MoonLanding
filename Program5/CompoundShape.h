#if !defined (COMPOUND_SHAPE)
#define COMPOUND_SHAPE

#include "btBulletDynamicsCommon.h"
#include <string>

class CompoundShape
{
    private:
        std::string id;
        btCompoundShape* shape;
        float mass;
        
    public:
        CompoundShape(std::string i, btCompoundShape* s);
        virtual ~CompoundShape();
        
        void addChildShape(btTransform* tr, btCollisionShape* cs, float m);
        
        float getMass();
        btCompoundShape* getCompoundShape();
        std::string getKey();
        static int compare_items(CompoundShape* c1, CompoundShape* c2);
        static int compare_keys(std::string* key, CompoundShape* c);
};
#endif
