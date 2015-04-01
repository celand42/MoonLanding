#if !defined QUATERNION
#define QUATERNION

class Quaternion
{

   private:
      Quaternion(float* q);
      float w;
      float x;
      float y;
      float z;

   public:
      Quaternion(float angle, float axis_x, float axis_y, float axis_z);
      virtual ~Quaternion();
      void displayQuaternion();
 
      float getAngle();
      float* getAxis();

      float length();
      void normalize();
      Quaternion* conjugate();
      Quaternion* multiply(Quaternion* other);
      static Quaternion* multiply(Quaternion* q1, Quaternion* q2);
};

#endif

