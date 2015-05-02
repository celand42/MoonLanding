#include "Quaternion.h"
#include <cmath>
#include <iostream>
using namespace std;

#define PI 3.1415927

//theta is assumed to be in degrees
Quaternion::Quaternion(float theta, float axis_x, float axis_y, float axis_z)
{
   theta = theta * PI/180.0;  //convert degrees to radians
   w = cos(theta/2.0);
   x = axis_x * sin(theta/2.0);
   y = axis_y * sin(theta/2.0);
   z = axis_z * sin(theta/2.0);

   normalize();
}

Quaternion::Quaternion(float* q)
{
   w = q[0];
   x = q[1];
   y = q[2];
   z = q[3];

   normalize();
}

float Quaternion::getAngle()
{
   float angle = 2*acos(w);
   return angle * 180.0/PI;  //convert from radians to degrees
}

float* Quaternion::getAxis()
{
   float* axis = new float[3];

   float val = getAngle() * PI/180.0;
   val = sin(val/2.0);

   axis[0] = x/val;
   axis[1] = y/val;
   axis[2] = z/val;
   
   return axis;
}

Quaternion::~Quaternion()
{
}

float Quaternion::length()
{
   float temp = w*w + x*x + y*y + z*z;
   return sqrt(temp);
}

void Quaternion::normalize()
{
   float len = length();
   w = w/len;
   x = x/len;
   y = y/len;
   z = z/len;
}

Quaternion* Quaternion::conjugate()
{
   float q[4];
   int len = length();

   q[0] = (this->w)/len;
   q[1] = -1*(this->x)/len;
   q[2] = -1*(this->y)/len;
   q[3] = -1*(this->z)/len;

   return new Quaternion(q);
}

Quaternion* Quaternion::multiply(Quaternion* that)
{
   return multiply(this, that);
}

Quaternion* Quaternion::multiply(Quaternion* q1, Quaternion* q2)
{
   float q[4];
   q[0] = (q1->w)*(q2->w) - (q1->x)*(q2->x) - (q1->y)*(q2->y) - (q1->z)*(q2->z);
   q[1] = (q1->w)*(q2->x) + (q1->x)*(q2->w) + (q1->y)*(q2->z) - (q1->z)*(q2->y);
   q[2] = (q1->w)*(q2->y) - (q1->x)*(q2->z) + (q1->y)*(q2->w) + (q1->z)*(q2->x);
   q[3] = (q1->w)*(q2->z) + (q1->x)*(q2->y) - (q1->y)*(q2->x) + (q1->z)*(q2->w);

   return new Quaternion(q);
}

void Quaternion::displayQuaternion()
{
   cout << "angle (degrees): " << getAngle() << endl;

   float* axis = getAxis();
   cout << "axis x-component: " << axis[0] << endl;
   cout << "axis y-component: " << axis[1] << endl;
   cout << "axis z-component: " << axis[2] << endl;
   cout << endl;

   delete[] axis;
}

int main()
{
   Quaternion* q1 = new Quaternion(20, 0, 0, 1);
   Quaternion* q2 = new Quaternion(180, 0, 1, 0);

   Quaternion* q3 = q1->multiply(q2);
   q3->displayQuaternion();
}

