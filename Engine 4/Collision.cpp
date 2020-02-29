#include "Collision.h"



// ==================== Legacy stuff ===============

Capsule::Capsule(float radius, float height)
{
    this->radius = radius;
    this->height = height;

    A = glm::vec4(0, height / 2, 0, 1);
    B = glm::vec4(0, -height / 2, 0, 1);
    type = TYPE::CAPSULE;
}

void Capsule::temp()
{
}

//========================== Collider ============================

void Collider::temp()
{

}

// ========================== AABB ===================

AABB::AABB(glm::vec3 size)
{
    this->size = size;
    type = TYPE::AABB;
}

void AABB::temp()
{
}

// ===================== Sphere =============

Sphere::Sphere(float radius)
{
    this->radius = radius;
    this->type = TYPE::SPHERE;
}

void Sphere::temp()
{
}

// ===================== Ellipsoid =============

Ellipsoid::Ellipsoid(glm::vec3 axis)
{
    this->axis = axis;
    this->type = TYPE::ELLIPSOID;
}

void Ellipsoid::temp()
{
}


// ======================== CollisionFunctions ===============

void CollisionFunctions::getClosestPoints(glm::mat4 aToWorld, std::shared_ptr<Capsule> capA, glm::mat4 bToWorld, std::shared_ptr<Capsule> capB, glm::vec3* ptA, glm::vec3* ptB)
{
    glm::vec3 thisA = aToWorld * capA->A;
    glm::vec3 thisB = aToWorld * capA->B;
    glm::vec3 otherA = bToWorld * capB->A;
    glm::vec3 otherB = bToWorld * capB->B;

    glm::vec3 u = thisA - thisB;
    glm::vec3 v = otherA - otherB;
    glm::vec3 w = thisB - otherB;

    //http://geomalgorithms.com/a07-_distance.html#dist3D_Segment_to_Segment()

    const float SMALL_NUMBER = 1.0E-10;

    float    a = dot(u, u);         // always >= 0
    float    b = dot(u, v);
    float    c = dot(v, v);         // always >= 0
    float    d = dot(u, w);
    float    e = dot(v, w);
    float    D = a * c - b * b;        // always >= 0
    float    sc, sN, sD = D;       // sc = sN / sD, default sD = D >= 0
    float    tc, tN, tD = D;       // tc = tN / tD, default tD = D >= 0

    // compute the line parameters of the two closest points
    if (D < SMALL_NUMBER) { // the lines are almost parallel
        sN = 0.0;         // force using point P0 on segment S1
        sD = 1.0;         // to prevent possible division by 0.0 later
        tN = e;
        tD = c;
    }
    else {                 // get the closest points on the infinite lines
        sN = (b * e - c * d);
        tN = (a * e - b * d);
        if (sN < 0.0) {        // sc < 0 => the s=0 edge is visible
            sN = 0.0;
            tN = e;
            tD = c;
        }
        else if (sN > sD) {  // sc > 1  => the s=1 edge is visible
            sN = sD;
            tN = e + b;
            tD = c;
        }
    }

    if (tN < 0.0) {            // tc < 0 => the t=0 edge is visible
        tN = 0.0;
        // recompute sc for this edge
        if (-d < 0.0)
            sN = 0.0;
        else if (-d > a)
            sN = sD;
        else {
            sN = -d;
            sD = a;
        }
    }
    else if (tN > tD) {      // tc > 1  => the t=1 edge is visible
        tN = tD;
        // recompute sc for this edge
        if ((-d + b) < 0.0)
            sN = 0;
        else if ((-d + b) > a)
            sN = sD;
        else {
            sN = (-d + b);
            sD = a;
        }
    }
    // finally do the division to get sc and tc
    sc = (abs(sN) < SMALL_NUMBER ? 0.0 : sN / sD);
    tc = (abs(tN) < SMALL_NUMBER ? 0.0 : tN / tD);

    *ptA = (sc * u) + thisB;
    *ptB = (tc * v) + otherB;

    //  return point;   // return the closest distance


      /*
      float a = glm::dot(u, u);
      float b = glm::dot(u, v);
      float c = glm::dot(v, v);
      float d = glm::dot(u, w);
      float e = glm::dot(v, w);
      float denom = a * c - b * b;
      if (denom == 0)
      {
          //parallel...
          std::cout << "parallel...... Fix this\n";
          return glm::vec3(-1, -1, -1);
      }
      else
      {
          //non parallel
          float s = glm::clamp((b * e - c * d) / denom, 0.0f, 1.0f);
          //float t = glm::clamp((a * e - b * d) / denom, 0.0f, 1.0f);
          return thisB + s * u;
      }
      return glm::vec3();
      */
}

void CollisionFunctions::getClosestPointToSphere(glm::mat4 aToWorld, std::shared_ptr<Capsule> capA, glm::mat4 bToWorld, std::shared_ptr<Sphere> sphB, glm::vec3* pt)
{
    glm::vec3 A2 = aToWorld * capA->A;
    glm::vec3 A1 = aToWorld * capA->B;
    glm::vec3 p = bToWorld * glm::vec4(0, 0, 0, 1);

    glm::vec3 ap = p - A1, ab = A2 - A1;
    float denom = glm::dot(ab, ab);

    float h = (fabs(denom) > 0.0) ? glm::clamp(glm::dot(ap, ab) / denom, 0.0f, 1.0f) : 0.0;

    *pt = A1 + h * ab;
}

void CollisionFunctions::getClosestPointToPoint(glm::mat4 aToWorld, std::shared_ptr<Capsule> capA, glm::vec3 pt, glm::vec3* outPt)
{

    glm::vec3 A2 = aToWorld * capA->A;
    glm::vec3 A1 = aToWorld * capA->B;

    glm::vec3 ap = pt - A1, ab = A2 - A1;
    float denom = glm::dot(ab, ab);

    float h = (fabs(denom) > 0.0) ? glm::clamp(glm::dot(ap, ab) / denom, 0.0f, 1.0f) : 0.0;

    *outPt = A1 + h * ab;
}

