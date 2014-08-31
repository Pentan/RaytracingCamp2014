#ifndef R1H_GEOMETRY_H
#define R1H_GEOMETRY_H

/*
#include <vector>
#include "rco.h"
#include "material.h"
#include "ray.h"
#include "intersection.h"
#include "aabb.h"
*/

#include <memory>
#include "r1htypes.h"
#include "ray.h"
#include "aabb.h"
#include "intersection.h"

namespace r1h {
	
class Geometry {
public:
    Geometry() {}
    virtual ~Geometry() {};
    
    virtual AABB getAABB() const = 0;
    virtual bool isIntersect(const Ray &ray, Intersection *intersect) const = 0;
	
	// optional. ex.:build VBH for mesh
	virtual void prepareRendering() {}
};

typedef std::shared_ptr<Geometry> GeometryRef;

}
#endif
