
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

size_t SceneObject::getMaterialCount() const {
	return materials.size();
}

void SceneObject::replaceMaterial(int matid, MaterialRef matref) {
	materials[matid] = matref;
}

void SceneObject::prepareRendering() {
	geometry->prepareRendering();
}

AABB SceneObject::getAABB() const {
	// when use transform. calc transformed.
	return geometry->getAABB();
}

bool SceneObject::isIntersect(const Ray &ray, Intersection *intersect) {
	return geometry->isIntersect(ray, intersect);
}

void SceneObject::setName(const std::string &newname) {
	name = newname;
}

std::string SceneObject::getName() const {
	return name;
}
