
#include "aabbgeometry.h"

using namespace r1h;

const int AABBGeometry::kTypeID = 'AABB';


AABBGeometry::AABBGeometry(const Vector3 min, const Vector3 max):
	Geometry(), aabb(min, max)
{}

AABB AABBGeometry::getAABB() const { return aabb; }

bool AABBGeometry::isIntersection(const Ray &ray, Intersection *intersect) const {
	double t;
	int axis;
	
	Hitpoint *hitpoint = &intersect->hitpoint;
	
	if(aabb.isIntersect(ray, &t, &axis)) {
		hitpoint->distance = t;
		hitpoint->position = ray.origin + ray.direction * t;
		hitpoint->materialId = 0;
		
		double d = (ray.direction.v[axis] > 0.0)? -1.0 : 1.0;
		switch(axis) {
			case 0:
				hitpoint->normal = Vector3(d, 0.0, 0.0);
				break;
			case 1:
				hitpoint->normal = Vector3(0.0, d, 0.0);
				break;
			case 2:
				hitpoint->normal = Vector3(0.0, 0.0, d);
				break;
		}
		
		return true;
	}
	return false;
}