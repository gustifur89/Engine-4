#include "Physics.h"

// ========================== Physics ==========================
/*
Physics::Physics()
{
    gravity = glm::vec3(0.0);
    boundary = glm::vec3(10);
}

collisionManifold Physics::collideCapsuleCapsule(std::shared_ptr<GameObject> objectA, std::shared_ptr<GameObject> objectB)
{
    collisionManifold colInfo;
    std::shared_ptr<Capsule> capA = std::dynamic_pointer_cast<Capsule>(objectA->collider);
    std::shared_ptr<Capsule> capB = std::dynamic_pointer_cast<Capsule>(objectB->collider);

    //std::shared_ptr<B> bp = std::dynamic_pointer_cast<B>(ap);
    glm::vec3 ptA, ptB;

    CollisionFunctions::getClosestPoints(objectA->transform.getTransformMatrix(), capA, objectB->transform.getTransformMatrix(), capB, &ptA, &ptB);

    //glm::vec3 ptA = capA->getClosestPointTo(objectA->transform.getTransformMatrix(), objectB->transform.getTransformMatrix(), capB);
    //glm::vec3 ptB = capB->getClosestPointTo(objectB->transform.getTransformMatrix(), objectA->transform.getTransformMatrix(), capA);

    glm::vec3 dif = ptB - ptA;
    float dist = glm::length(dif);
    bool doesCollide = dist <= capA->radius + capB->radius;

    colInfo.doesCollide = doesCollide;

    if (doesCollide)
    {
        colInfo.normal = glm::normalize(dif);
        colInfo.contactPoint = ptA + (capA->radius / (capA->radius + capB->radius)) * dif;
        colInfo.sepDepth = capA->radius + capB->radius - dist;
    }

    return colInfo;
}

collisionManifold Physics::collideAABBAABB(std::shared_ptr<GameObject> objectA, std::shared_ptr<GameObject> objectB)
{
    collisionManifold colInfo;
    std::shared_ptr<AABB> cubeA = std::dynamic_pointer_cast<AABB>(objectA->collider);
    std::shared_ptr<AABB> cubeB = std::dynamic_pointer_cast<AABB>(objectB->collider);

    glm::vec3 scaleA = cubeA->size;// objectA->transform.getScale();// objectA->transform.getScaleMatrix()* glm::vec4(cubeA->size, 0.0);
    glm::vec3 scaleB = cubeB->size;//objectB->transform.getScale();// ->transform.getScaleMatrix()* glm::vec4(cubeB->size, 0.0);

    glm::vec3 posA = objectA->transform.getPosition();
    glm::vec3 posB = objectB->transform.getPosition();

    colInfo.doesCollide = false;

    glm::vec3 maxA = posA + scaleA;
    glm::vec3 minA = posA - scaleA;
    glm::vec3 maxB = posB + scaleB;
    glm::vec3 minB = posB - scaleB;

    // Vector from A to B
    glm::vec3 n = posB - posA;

    // Calculate half extents along x axis for each object
    glm::vec3 a_extent = 0.5f * (maxA - minA);
    glm::vec3 b_extent = 0.5f * (maxB - minB);

    // Calculate overlap on x axis
    glm::vec3 overlap = a_extent + b_extent - glm::abs(n);

    // SAT test for bounding boxes
    if (overlap.x > 0 && overlap.y > 0 && overlap.z > 0)
    {
        colInfo.doesCollide = true;
        if (overlap.x < overlap.y && overlap.x < overlap.z)
        {
            // Point towards B knowing that n points from A to B
            if (n.x < 0)
                colInfo.normal = glm::vec3(-1, 0, 0);
            else
                colInfo.normal = glm::vec3(1, 0, 0);

            colInfo.sepDepth = overlap.x;
        }
        else if (overlap.y < overlap.z)
        {
            // Point towards B knowing that n points from A to B
            if (n.y < 0)
                colInfo.normal = glm::vec3(0, -1, 0);
            else
                colInfo.normal = glm::vec3(0, 1, 0);

            colInfo.sepDepth = overlap.y;
        }
        else
        {
            if (n.z < 0)
                colInfo.normal = glm::vec3(0, 0, -1);
            else
                colInfo.normal = glm::vec3(0, 0, 1);

            colInfo.sepDepth = overlap.z;
        }

    }

    return colInfo;
}

collisionManifold Physics::collideSphereSphere(std::shared_ptr<GameObject> objectA, std::shared_ptr<GameObject> objectB)
{
    collisionManifold colInfo;
    std::shared_ptr<Sphere> sphA = std::dynamic_pointer_cast<Sphere>(objectA->collider);
    std::shared_ptr<Sphere> sphB = std::dynamic_pointer_cast<Sphere>(objectB->collider);

    glm::vec3 dif = objectB->transform.getPosition() - objectA->transform.getPosition();
    float dist = glm::length(dif);

    bool doesCollide = (dist <= sphA->radius + sphB->radius) && (dist > 0.0);

    colInfo.doesCollide = doesCollide;

    if (doesCollide)
    {
        colInfo.normal = glm::normalize(dif);
        colInfo.contactPoint = objectA->transform.getPosition() + (sphA->radius / (sphA->radius + sphB->radius)) * dif;
        colInfo.sepDepth = sphA->radius + sphB->radius - dist;
    }

    return colInfo;
}

collisionManifold Physics::collideEllipsoidEllipsoid(std::shared_ptr<GameObject> objectA, std::shared_ptr<GameObject> objectB)
{
    collisionManifold colInfo;
    std::shared_ptr<Ellipsoid> elpA = std::dynamic_pointer_cast<Ellipsoid>(objectA->collider);
    std::shared_ptr<Ellipsoid> elpB = std::dynamic_pointer_cast<Ellipsoid>(objectB->collider);

    glm::vec3 dif = objectB->transform.getPosition() - objectA->transform.getPosition();
    float dist = glm::length(dif);
    glm::vec3 norm = glm::normalize(dif);

    float radiusA = glm::length(norm * elpA->axis);
    float radiusB = glm::length(norm * elpB->axis);

    bool doesCollide = (dist <= radiusA + radiusB) && (dist > 0.0);

    colInfo.doesCollide = doesCollide;

    if (doesCollide)
    {
        colInfo.normal = norm;
        colInfo.contactPoint = objectA->transform.getPosition() + (radiusA / (radiusA + radiusB)) * dif;
        colInfo.sepDepth = radiusA + radiusB - dist;
    }

    return colInfo;
}

collisionManifold Physics::collideEllipsoidSphere(std::shared_ptr<GameObject> objectA, std::shared_ptr<GameObject> objectB)
{
    collisionManifold colInfo;
    std::shared_ptr<Ellipsoid> elpA = std::dynamic_pointer_cast<Ellipsoid>(objectA->collider);
    std::shared_ptr<Sphere> sphB = std::dynamic_pointer_cast<Sphere>(objectB->collider);

    glm::vec3 dif = objectB->transform.getPosition() - objectA->transform.getPosition();
    float dist = glm::length(dif);
    glm::vec3 norm = glm::normalize(dif);

    float radiusA = glm::length(norm * elpA->axis);

    bool doesCollide = (dist <= radiusA + sphB->radius) && (dist > 0.0);

    colInfo.doesCollide = doesCollide;

    if (doesCollide)
    {
        colInfo.normal = norm;
        colInfo.contactPoint = objectA->transform.getPosition() + (radiusA / (radiusA + sphB->radius)) * dif;
        colInfo.sepDepth = radiusA + sphB->radius - dist;
    }

    return colInfo;
}

collisionManifold Physics::collideSphereEllipsoid(std::shared_ptr<GameObject> objectA, std::shared_ptr<GameObject> objectB)
{
    collisionManifold colInfo = collideEllipsoidSphere(objectB, objectA);
    colInfo.normal *= -1.0f;
    return colInfo;
}

collisionManifold Physics::collideEllipsoidCapsule(std::shared_ptr<GameObject> objectA, std::shared_ptr<GameObject> objectB)
{
    collisionManifold colInfo = collideCapsuleEllipsoid(objectB, objectA);
    colInfo.normal *= -1.0f;
    return colInfo;
}

collisionManifold Physics::collideCapsuleEllipsoid(std::shared_ptr<GameObject> objectA, std::shared_ptr<GameObject> objectB)
{
    collisionManifold colInfo;
    colInfo.doesCollide = false;
    return colInfo;
}

collisionManifold Physics::collideSphereCapsule(std::shared_ptr<GameObject> objectA, std::shared_ptr<GameObject> objectB)
{
    collisionManifold colInfo = collideCapsuleSphere(objectB, objectA);
    colInfo.normal *= -1.0f;
    return colInfo;
}

collisionManifold Physics::collideCapsuleSphere(std::shared_ptr<GameObject> objectA, std::shared_ptr<GameObject> objectB)
{
    collisionManifold colInfo;
    std::shared_ptr<Capsule> capA = std::dynamic_pointer_cast<Capsule>(objectA->collider);
    std::shared_ptr<Sphere> sphB = std::dynamic_pointer_cast<Sphere>(objectB->collider);

    //std::shared_ptr<B> bp = std::dynamic_pointer_cast<B>(ap);
    glm::vec3 ptA;
    glm::vec3 ptB = objectB->transform.getPosition();

    CollisionFunctions::getClosestPointToSphere(objectA->transform.getTransformMatrix(), capA, objectB->transform.getTransformMatrix(), sphB, &ptA);

    //From A to B

    glm::vec3 dif = objectB->transform.getPosition() - ptA;
    float dist = glm::length(dif);
    bool doesCollide = dist <= capA->radius + sphB->radius;

    colInfo.doesCollide = doesCollide;

    if (doesCollide)
    {
        colInfo.normal = glm::normalize(dif);
        colInfo.contactPoint = ptA + (capA->radius / (capA->radius + sphB->radius)) * dif;
        colInfo.sepDepth = capA->radius + sphB->radius - dist;
    }

    return colInfo;
}

collisionManifold Physics::collideAABBCapsule(std::shared_ptr<GameObject> objectA, std::shared_ptr<GameObject> objectB)
{
    collisionManifold colInfo = collideCapsuleAABB(objectB, objectA);
    colInfo.normal *= -1.0f;
    return colInfo;
}

collisionManifold Physics::collideCapsuleAABB(std::shared_ptr<GameObject> objectA, std::shared_ptr<GameObject> objectB)
{
    //collide from capsule to AABB
    collisionManifold colInfo;

    colInfo.doesCollide = false;

    std::shared_ptr<Capsule> capA = std::dynamic_pointer_cast<Capsule>(objectA->collider);
    std::shared_ptr<AABB> cubeB = std::dynamic_pointer_cast<AABB>(objectB->collider);

    glm::vec3 scaleA(capA->radius, 0.5f * capA->height + capA->radius, capA->radius);
    glm::vec3 scaleB = cubeB->size;//objectB->transform.getScale();

    glm::vec3 posA = objectA->transform.getPosition();
    glm::vec3 posB = objectB->transform.getPosition();

    colInfo.doesCollide = false;

    glm::vec3 maxA = posA + scaleA;
    glm::vec3 minA = posA - scaleA;
    glm::vec3 maxB = posB + scaleB;
    glm::vec3 minB = posB - scaleB;

    // Vector from A to B
    glm::vec3 n = posB - posA;

    // Calculate half extents along x axis for each object
    glm::vec3 a_extent = 0.5f * (maxA - minA);
    glm::vec3 b_extent = 0.5f * (maxB - minB);

    // Calculate overlap on x axis
    glm::vec3 overlap = a_extent + b_extent - glm::abs(n);

    // SAT test for bounding boxes
    if (overlap.x > 0 && overlap.y > 0 && overlap.z > 0)
    {
        /*
        colInfo.doesCollide = true;
        if (overlap.x < overlap.y && overlap.x < overlap.z)
        {
            // Point towards B knowing that n points from A to B
            if (n.x < 0)
                colInfo.normal = glm::vec3(-1, 0, 0);
            else
                colInfo.normal = glm::vec3(1, 0, 0);
            colInfo.sepDepth = overlap.x;
        }
        else if (overlap.y < overlap.z)
        {
            // Point towards B knowing that n points from A to B
            if (n.y < 0)
                colInfo.normal = glm::vec3(0, -1, 0);
            else
                colInfo.normal = glm::vec3(0, 1, 0);
            colInfo.sepDepth = overlap.y;
        }
        else
        {
            if (n.z < 0)
                colInfo.normal = glm::vec3(0, 0, -1);
            else
                colInfo.normal = glm::vec3(0, 0, 1);
            colInfo.sepDepth = overlap.z;
        }
       // * /

       //need to see if colliding from the top or sides...
       //bool onTop = posA.x <= maxB.x && posA.x >= minB.x && posA.z <= maxB.z && posA.z >= maxB.z;
       //If on top, we collide the closest point 

       //we just need to collide the AABB with a sphere made from the closest point on the capsule to the center of the AABB.
      // /*
        glm::vec3 ptA;

        CollisionFunctions::getClosestPointToPoint(objectA->transform.getTransformMatrix(), capA, posB, &ptA);

        // ptA = objectA->transform.getPosition();

         //Now it is just like a sphere vs AABB collision:
          //Center of AABB + projection of sphere onto surface
        glm::vec3 projection = glm::clamp(ptA - posB, -scaleB, scaleB);

        glm::vec3 mag = glm::abs(projection / scaleB);

        //This should renormalize so that it goes back to the boundary.
        float maxVal = fmax(mag.x, fmax(mag.y, mag.z));

        bool insideTag = maxVal < 1.0f;
        //std::cout << projection.x << " : " << projection.y << " : " << projection.z << "\n";
        glm::vec3 boundaryPt = posB + projection;

        if (insideTag)
        {
            //Need to project the sphere up onto the surface...
            if (mag.x > mag.y&& mag.x > mag.z)
            {
                boundaryPt = ptA;
                //collide on x...
                boundaryPt.x = posB.x + glm::sign(projection.x) * scaleB.x;
            }
            else if (mag.y > mag.z)
            {
                boundaryPt = ptA;
                //collide on x...
                boundaryPt.y = posB.y + glm::sign(projection.y) * scaleB.y;
            }
            else
            {
                boundaryPt = ptA;
                //collide on x...
                boundaryPt.z = posB.z + glm::sign(projection.z) * scaleB.z;
            }
        }

        //Now we collide like a sphere...
        glm::vec3 dif = boundaryPt - ptA;
        float dist = glm::length(dif);
        bool doesCollide = dist <= capA->radius;
        //colInfo.doesCollide = doesCollide;

        //std::cout << dif.x << " : " << dif.y << " : " << dif.z << "\n";

        if (doesCollide)
        {
            colInfo.doesCollide = true;
            colInfo.normal = glm::normalize(dif);
            if (insideTag)
                colInfo.normal *= -1.0f;
            colInfo.contactPoint = boundaryPt;
            colInfo.sepDepth = capA->radius - dist;
        }
        //* /

    }

    return colInfo;
}

collisionManifold Physics::collideAABBSphere(std::shared_ptr<GameObject> objectA, std::shared_ptr<GameObject> objectB)
{
    collisionManifold colInfo = collideSphereAABB(objectB, objectA);
    colInfo.normal *= -1.0f;
    return colInfo;
}

collisionManifold Physics::collideSphereAABB(std::shared_ptr<GameObject> objectA, std::shared_ptr<GameObject> objectB)
{
    //collide from Sphere to AABB
    collisionManifold colInfo;

    colInfo.doesCollide = false;

    std::shared_ptr<Sphere> sphA = std::dynamic_pointer_cast<Sphere>(objectA->collider);
    std::shared_ptr<AABB> cubeB = std::dynamic_pointer_cast<AABB>(objectB->collider);

    glm::vec3 scaleA(sphA->radius, sphA->radius, sphA->radius);
    glm::vec3 scaleB = objectB->transform.getScale();

    glm::vec3 posA = objectA->transform.getPosition();
    glm::vec3 posB = objectB->transform.getPosition();

    glm::vec3 maxA = posA + scaleA;
    glm::vec3 minA = posA - scaleA;
    glm::vec3 maxB = posB + scaleB;
    glm::vec3 minB = posB - scaleB;

    // Vector from A to B
    glm::vec3 n = posB - posA;

    // Calculate half extents along x axis for each object
    glm::vec3 a_extent = 0.5f * (maxA - minA);
    glm::vec3 b_extent = 0.5f * (maxB - minB);

    // Calculate overlap on x axis
    glm::vec3 overlap = a_extent + b_extent - glm::abs(n);

    // SAT test for bounding boxes
    if (overlap.x > 0 && overlap.y > 0 && overlap.z > 0)
    {

        //Now we get the closest point in box to circle..

        //Center of AABB + projection of sphere onto surface
        glm::vec3 projection = glm::clamp(posA - posB, -scaleB, scaleB);

        glm::vec3 mag = glm::abs(projection / scaleB);

        //This should renormalize so that it goes back to the boundary.
        float maxVal = fmax(mag.x, fmax(mag.y, mag.z));
        // if (maxVal < 1.0f) std::cout << "yeet\n";
        bool insideTag = maxVal < 1.0f;
        //std::cout << projection.x << " : " << projection.y << " : " << projection.z << "\n";
        glm::vec3 boundaryPt = posB + projection;

        if (insideTag)
        {
            //Need to project the sphere up onto the surface...
            if (mag.x > mag.y&& mag.x > mag.z)
            {
                boundaryPt = posA;
                //collide on x...
                boundaryPt.x = posB.x + glm::sign(projection.x) * scaleB.x;
            }
            else if (mag.y > mag.z)
            {
                boundaryPt = posA;
                //collide on x...
                boundaryPt.y = posB.y + glm::sign(projection.y) * scaleB.y;
            }
            else
            {
                boundaryPt = posA;
                //collide on x...
                boundaryPt.z = posB.z + glm::sign(projection.z) * scaleB.z;
            }
        }


        //Now we collide like a sphere...
        glm::vec3 dif = boundaryPt - posA;
        float dist = glm::length(dif);
        bool doesCollide = dist <= sphA->radius;
        //colInfo.doesCollide = doesCollide;

        //std::cout << dif.x << " : " << dif.y << " : " << dif.z << "\n";

        if (doesCollide)
        {
            colInfo.doesCollide = true;
            colInfo.normal = glm::normalize(dif);
            if (insideTag)
                colInfo.normal *= -1.0f;
            colInfo.contactPoint = boundaryPt;
            colInfo.sepDepth = sphA->radius - dist;
        }
    }

    return colInfo;
}

collisionManifold Physics::collide(std::shared_ptr<GameObject> objectA, std::shared_ptr<GameObject> objectB)
{
    if (objectA->collider->type == Collider::CAPSULE && objectB->collider->type == Collider::CAPSULE)
    {
        return collideCapsuleCapsule(objectA, objectB);
    }
    else if (objectA->collider->type == Collider::AABB && objectB->collider->type == Collider::AABB)
    {
        return collideAABBAABB(objectA, objectB);
    }
    else if (objectA->collider->type == Collider::SPHERE && objectB->collider->type == Collider::SPHERE)
    {
        return collideSphereSphere(objectA, objectB);
    }
    else if (objectA->collider->type == Collider::ELLIPSOID && objectB->collider->type == Collider::ELLIPSOID)
    {
        return collideEllipsoidEllipsoid(objectA, objectB);
    }
    else if (objectA->collider->type == Collider::AABB && objectB->collider->type == Collider::CAPSULE)
    {
        return collideAABBCapsule(objectA, objectB);
    }
    else if (objectA->collider->type == Collider::CAPSULE && objectB->collider->type == Collider::AABB)
    {
        return collideCapsuleAABB(objectA, objectB);
    }
    else if (objectA->collider->type == Collider::SPHERE && objectB->collider->type == Collider::CAPSULE)
    {
        return collideSphereCapsule(objectA, objectB);
    }
    else if (objectA->collider->type == Collider::CAPSULE && objectB->collider->type == Collider::SPHERE)
    {
        return collideCapsuleSphere(objectA, objectB);
    }
    else if (objectA->collider->type == Collider::ELLIPSOID && objectB->collider->type == Collider::CAPSULE)
    {
        return collideEllipsoidCapsule(objectA, objectB);
    }
    else if (objectA->collider->type == Collider::CAPSULE && objectB->collider->type == Collider::ELLIPSOID)
    {
        return collideCapsuleEllipsoid(objectA, objectB);
    }
    else if (objectA->collider->type == Collider::SPHERE && objectB->collider->type == Collider::ELLIPSOID)
    {
        return collideSphereEllipsoid(objectA, objectB);
    }
    else if (objectA->collider->type == Collider::ELLIPSOID && objectB->collider->type == Collider::SPHERE)
    {
        return collideEllipsoidSphere(objectA, objectB);
    }
    else if (objectA->collider->type == Collider::AABB && objectB->collider->type == Collider::SPHERE)
    {
        return collideAABBSphere(objectA, objectB);
    }
    else if (objectA->collider->type == Collider::SPHERE && objectB->collider->type == Collider::AABB)
    {
        return collideSphereAABB(objectA, objectB);
    }

    return collisionManifold();
}

void Physics::collision(std::shared_ptr<GameObject> objectA, std::shared_ptr<GameObject> objectB)
{
    if (!objectA->collider || !objectB->collider) return;
    collisionManifold colInfo = collide(objectA, objectB);

    if (colInfo.doesCollide)
    {
        objectA->isColliding = true;
        objectB->isColliding = true;

        glm::vec3 v1 = objectA->velocity;
        glm::vec3 v2 = objectB->velocity;

        glm::vec3 dir = colInfo.normal;

        glm::vec3 rv = (objectB->velocity) - (objectA->velocity);

        float velAlongDir = glm::dot(rv, dir);

        if (velAlongDir >= 0)
            return;

        float e = fmin(objectA->elasticity, objectB->elasticity);

        float j = -(1 + e) * velAlongDir;

        float denominator = objectA->invMass + objectB->invMass;

        j /= denominator;

        glm::vec3 impulse = j * dir;

        //friction resolution
        glm::vec3 t = rv - velAlongDir * colInfo.normal;

        if (glm::length(t) > 0)
            t = glm::normalize(rv - velAlongDir * colInfo.normal);

        float jt = -(1 + e) * glm::dot(rv, t);

        jt /= denominator;

        float mu = sqrt(objectA->staticFriction * objectA->staticFriction + objectB->staticFriction * objectB->staticFriction);

        glm::vec3 frictionImpules(0);
        if (fabs(jt) < j * mu)
        {
            frictionImpules = jt * t;
        }
        else
        {
            float dynam = sqrt(objectA->dynamicFriction * objectA->dynamicFriction + objectB->dynamicFriction * objectB->dynamicFriction);
            frictionImpules = -j * t * dynam;
        }

        objectA->applyImpulse(-impulse, colInfo.contactPoint);
        objectB->applyImpulse(impulse, colInfo.contactPoint);
        //friction
        objectA->applyImpulse(-frictionImpules, colInfo.contactPoint);
        objectB->applyImpulse(frictionImpules, colInfo.contactPoint);

        penetrationCorrection(objectA, objectB, colInfo);

    }
}

void Physics::penetrationCorrection(std::shared_ptr<GameObject> objectA, std::shared_ptr<GameObject> objectB, collisionManifold& colInfo)
{

    float percent = 0.2; // usually 20% to 80% 
    float k_slop = 0.01; // usually 0.01 to 0.1
    float denominator = objectA->invMass + objectB->invMass;
    glm::vec3 correction = glm::max(colInfo.sepDepth - k_slop, 0.0f) / denominator * percent * colInfo.normal;
    objectA->transform.translate(-objectA->invMass * correction);
    objectB->transform.translate(objectB->invMass * correction);

}

void Physics::addObject(std::shared_ptr<GameObject> object)
{
    objectList.push_back(object);
}

void Physics::setGravity(glm::vec3 gravity)
{
    this->gravity = gravity;
}

void Physics::setBounceBoundary(glm::vec3 boundary)
{
    this->boundary = boundary;
}

void Physics::bounceBoundary(std::shared_ptr<GameObject> object)
{
    glm::vec3 pos = object->transform.getPosition();
    glm::vec3 vel = object->velocity;

    if ((pos.x < -boundary.x && vel.x < 0.0) || (pos.x > boundary.x&& vel.x > 0.0))
        object->applyImpulse(object->mass * glm::vec3(-2 * vel.x, 0, 0), pos);

    if ((pos.y < -boundary.y && vel.y < 0.0) || (pos.y > boundary.y&& vel.y > 0.0))
        object->applyImpulse(object->mass * glm::vec3(0, -2 * vel.y, 0), pos);

    if ((pos.z < -boundary.z && vel.z < 0.0) || (pos.z > boundary.z&& vel.z > 0.0))
        object->applyImpulse(object->mass * glm::vec3(0, 0, -2 * vel.z), pos);

}

bool Physics::killBoundary(std::shared_ptr<GameObject> object)
{
    glm::vec3 pos = object->transform.getPosition();
    glm::vec3 vel = object->velocity;

    /*
    if (pos.x < -boundary.x || pos.x > boundary.x || pos.y < -boundary.y || pos.y > boundary.y || pos.z < -boundary.z || pos.z > boundary.z)
    {
        object->toRemove = true;
        return true;
    }
    * /


    if (pos.y < -boundary.y)
    {
        object->toRemove = true;
        return true;
    }

    return false;
}

void Physics::physicsStep(float dt, int iterations)
{
    dt = dt / iterations;

    for (int iters = 0; iters < iterations; iters++)
    {

        for (std::shared_ptr<GameObject> object : objectList)
        {
            object->isColliding = false;
        }

        for (int i = 0; i < objectList.size(); i++)
        {
            for (int j = i + 1; j < objectList.size(); j++)
            {
                collision(objectList[i], objectList[j]);
            }
        }

        //update movements
        for (int i = 0; i < objectList.size(); i++)
        {
            //v += p/m + (f/m) * t
            //p += v*t
            //if (Settings::useDrag)
            //    applyDrag(body);

            std::shared_ptr<GameObject> object = objectList[i];

            bounceBoundary(object);

            /*
            if (killBoundary(object))
            {
                objectList.erase(objectList.begin() + i);
                i--;
                continue;
            }
            //* /

            if (object->mass > 0.0)
            {
                //linear
                object->force += object->mass * gravity;
                object->velocity += object->invMass * (object->impulse + dt * object->force);
                object->transform.translate(dt * object->velocity);
                object->impulse = glm::vec3(0.0);
                object->force = glm::vec3(0.0);
            }
        }
    }
}
*/
