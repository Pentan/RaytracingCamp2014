
#include "intersection.h"

using namespace r1h;

Vector3 Hitpoint::orientingNormal(const Ray &ray) const {
	return (Vector3::dot(normal, ray.direction) < 0.0)? normal : (normal * -1.0);
}

void Intersection::clear() {
    hitpoint.distance = kINF;
    objectId = -1;
}
