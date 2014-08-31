#ifndef R1H_SCENEOBJECT_H
#define R1H_SCENEOBJECT_H

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
	
	AABB getAABB() const;
	
	void prepareRendering();
	
	bool isIntersect(const Ray &ray, Intersection *intersect);
	
	//+++++
	//int objectId;
	
private:
	GeometryRef geometry;
	std::vector<MaterialRef> materials;
	
	//Matrix4 transform;
};

typedef std::shared_ptr<SceneObject> SceneObjectRef;

}
#endif
