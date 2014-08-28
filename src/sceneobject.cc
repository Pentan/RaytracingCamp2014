
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

int SceneObject::addMaterial(MaterialRef matref) {
	materials.push_back(matref);
	return (int)materials.size() - 1;
}

Material* SceneObject::getMaterialById(int matid) const {
	return materials[matid].get();
}

bool SceneObject::isIntersection(const Ray &ray, Intersection *intersect) {
	return geometry->isIntersection(ray, intersect);
}
