
#include "sceneobject.h"
#include "intersection.h"

using namespace r1h;

SceneObject::SceneObject():
	geometry(nullptr)
{}

SceneObject::~SceneObject() {}

void SceneObject::setGeometry(GeometryRef geom) {
	geometry = geom;
}
Geometry* SceneObject::getGeometry() {
	return geometry.get();
}

int SceneObject::addMaterial(MaterialRef matref) {
	materials.push_back(matref);
	return (int)materials.size() - 1;
}

Material* SceneObject::getMaterialById(int matid) const {
	return materials[matid].get();
}

void SceneObject::prepareRendering() {
	geometry->prepareRendering();
}

AABB SceneObject::getAABB() const {
	// when use transform. calc transformed.
	return geometry->getAABB();
}

bool SceneObject::isIntersect(const Ray &ray, Intersection *intersect) {
#if 1
	return geometry->isIntersect(ray, intersect);
#else
	AABB aabb = getAABB();
	if(aabb.isIntersect(ray)) {
		return geometry->isIntersect(ray, intersect);
	}
	return false;
#endif
}
