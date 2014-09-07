#ifndef R1H_SCENEOBJECT_H
#define R1H_SCENEOBJECT_H

#include <string>
#include <vector>
#include <memory>
#include "material.h"
#include "geometry.h"
#include "intersection.h"
#include "aabb.h"

namespace r1h {

class SceneObject {

public:
	SceneObject();
	virtual ~SceneObject();
	
	void setGeometry(GeometryRef geom);
	Geometry* getGeometry();
	
	int addMaterial(MaterialRef matref);
	Material* getMaterialById(int matid) const;
	size_t getMaterialCount() const;
	void replaceMaterial(int matid, MaterialRef matref);
	
	AABB getAABB() const;
	
	void prepareRendering();
	
	bool isIntersect(const Ray &ray, Intersection *intersect);
	
	//+++++
	//int objectId;
	
	void setName(const std::string &newname);
	std::string getName() const;

private:
	GeometryRef geometry;
	std::vector<MaterialRef> materials;
	std::string name;

	//Matrix4 transform;
};

typedef std::shared_ptr<SceneObject> SceneObjectRef;

}
#endif
