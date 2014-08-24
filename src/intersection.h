#ifndef R1H_INTERSECTION_H
#define R1H_INTERSECTION_H

#include <vector>
#include "r1htypes.h"
#include "ray.h"
//#include "material.h"

namespace r1h {

class Hitpoint {
public:
    R1hFPType distance;
    Vector3 normal;
    Vector3 position;
    
    int materialId;
    int faceId;
    Vector3 varyingWeight;
    
	/*
    struct {
        Vector3 position;
        Vector3 normal;
        Vector3 tangent;
    } world, local;
    */
	
    std::vector<Vector3> attributes;
    
    //Material *material;
    
    Hitpoint() : distance(kINF), normal(), position() {}
    Vector3 orientingNormal(const Ray &ray) const;
};

class Intersection {
public:
    enum {
        kNoIntersected = -1
    };
    
    Hitpoint hitpoint;
    int objectId;
    
    Intersection() : objectId(kNoIntersected) {}
    void clear();
};

}

#endif
