#ifndef R1H_SCENE_H
#define R1H_SCENE_H

#include <vector>
#include "r1htypes.h"
#include "ray.h"
#include "renderer.h"
#include "sceneobject.h"
#include "material.h"
#include "camera.h"
#include "intersection.h"

namespace r1h {

class Camera;
class BVHNode;

class Scene {
public:
    Scene();
    virtual ~Scene();
    
    // setup
    virtual bool load();
	
    int addObject(SceneObjectRef objref);
    
    Camera* getCamera();
	void setCamera(CameraRef camref);
    
	Material* getBackgroundMaterial();
	void setBackgroundMaterial(MaterialRef matref);
	
	// render
    Color radiance(Renderer::Context *cntx, const Ray &ray);
    
private:
    CameraRef camera;
    std::vector<SceneObjectRef> sceneObjects;
    BVHNode *objectBVH;
    
    MaterialRef bgMaterial;
	
	bool intersectSceneObjects(const Ray &ray, Intersection *intersect);
};

}

#endif