﻿
#include <iostream>
#include <string>

#include "scene.h"
#include "camera.h"
#include "geometry.h"
#include "mesh.h"
#include "bsdfdiffuse.h"
#include "material.h"
#include "bvhnode.h"
#include "sceneobject.h"
#include "intersection.h"
#include "wavefrontobj.h"
#include "aabb.h"
#include "scenesupport.h"

#include "aabbgeometry.h"

///
using namespace r1h;

Scene::Scene():
    camera(0), objectBVH(0), bgMaterial(0)
{
    camera = CameraRef(new Camera());
	bgMaterial = MaterialRef(nullptr);
}

Scene::~Scene() {
    if(objectBVH) {
        //objectBVH->releaseAll();
        delete objectBVH;
        objectBVH = nullptr;
    }
}

/*
bool Scene::load() {
	// FIXME
	printf("FIXME : Scene::load\n");
	return true;
}
*/

bool Scene::loadWavefrontObj(std::string filename) {
	SimpleObjLoader loader(filename.c_str(), this);
	loader.load();
	
	/*
	//+++++
	for(size_t i = 0; i < sceneObjects.size(); i++) {
		SceneObject *obj = sceneObjects[i].get();
		Mesh *mesh = dynamic_cast<Mesh*>(obj->getGeometry());
		printf("mesh[%ld] %p\n", i, mesh);
	}
	//+++++
	*/
	return true;
}

int Scene::addObject(SceneObjectRef objref) {
	sceneObjects.push_back(objref);
    return (int)sceneObjects.size() - 1;
};

SceneObject* Scene::getObject(int objid) {
	if(objid < 0 || objid >= sceneObjects.size()) {
		return nullptr;
	}
	return sceneObjects[objid].get();
}

int Scene::getObjectsCount() const {
	return (int)sceneObjects.size();
}

// render
Camera* Scene::getCamera() {
	return camera.get();
}

void Scene::setCamera(CameraRef camref) {
	camera = camref;
}

Material* Scene::getBackgroundMaterial() {
	return bgMaterial.get();
}

void Scene::setBackgroundMaterial(MaterialRef matref) {
	bgMaterial = matref;
}


void Scene::prepareRendering() {
	std::vector<AABB> aabbvec(sceneObjects.size());
	
	// prepare and get AABB
	for(int i = 0; i < (int)sceneObjects.size(); i++) {
		sceneObjects[i]->prepareRendering();
		AABB objaabb = sceneObjects[i]->getAABB();
		objaabb.dataId = i;
		aabbvec[i] = objaabb;
	}
	
	// build BVH tree
	if(objectBVH) {
		printf("object BVH is already build. rebuild it.\n");
		delete objectBVH;
		objectBVH = nullptr;
	}
	objectBVH = new BVHNode();
	int maxdepth = objectBVH->buildAABBTree(aabbvec.data(), (int)aabbvec.size());
	//printf("%ld objects. BVH max depth: %d\n", sceneObjects.size(), maxdepth);
}

Color Scene::radiance(Renderer::Context *cntx, const Ray &ray) {
	Random *crnd = &cntx->random;
	
    std::vector<Ray> *currays = &cntx->rayVector1;
	std::vector<Ray> *nextrays = &cntx->rayVector2;
	std::vector<Ray> *tmprays = &cntx->workVector;
    
	currays->clear();
	nextrays->clear();
	tmprays->clear();
	
	// init radiance
	Color radiancecol(0.0);
	// first ray
	currays->push_back(ray);
	
	// trace!
	int depth = 0;
	while(currays->size() > 0) {
		// clear next ray container
		nextrays->clear();
		
		// trace
        double minDepth = cntx->config->minDepth;
        double depthLimit = cntx->config->maxDepth;
        
		int raynum = (int)currays->size();
		for(int i = 0; i < raynum; i++) {
			const Ray &traceray = currays->at(i);
			Intersection intersect;
			
			// not intersected. pick background
			if(!intersectSceneObjects(traceray, &intersect)) {
				Color bgcol(0.0);
				if(bgMaterial.get() != nullptr) {
					bgcol = bgMaterial->skyColor(traceray);
				};
				radiancecol += Color::mul(bgcol, traceray.weight);
				continue;
			}
			
			// hit!
			const SceneObject *hitobject = sceneObjects[intersect.objectId].get();
			
			// from hit face info
			const Material *hitmat = hitobject->getMaterialById(intersect.materialId);
			const Color albedocol = hitmat->albedo(hitobject, intersect);
            const Color emitcol = hitmat->emission(hitobject, intersect);
            
#if 1
            radiancecol += Color::mul(emitcol, traceray.weight);
#else
			//+++++
			// for debugging
            // normal
            radiancecol += intersect.normal * 0.5 + Vector3(0.5);
            continue;
			// color
			//radiancecol += albedocol;
			//continue;
            //+++++
#endif
            double russianprob = std::max(albedocol.r, std::max(albedocol.g, albedocol.b));
            if(depth > depthLimit) {
                russianprob *= pow(0.5, depth - depthLimit);
            }
            if(depth < minDepth) {
                russianprob = 1.0;
            } else if(crnd->next01() >= russianprob) {
                // russian roulette kill
                continue;
            }
            
            const Color nextweight = albedocol * russianprob;
            
            tmprays->clear();
            hitmat->makeNextRays(traceray, intersect, depth, crnd, tmprays);
            for(int j = 0; j < (int)tmprays->size(); j++) {
                Ray &nwray = tmprays->at(j);
                nwray.smallOffset(intersect.normal);
                nwray.weight = Color::mul(nwray.weight, Color::mul(traceray.weight, nextweight));
                nextrays->push_back(nwray);
            }
		}
        
        std::swap(currays, nextrays);
        ++depth;
	}
	
	return radiancecol;
    //return Color(fabs(ray.direction.x), fabs(ray.direction.y), fabs(ray.direction.z));
}

bool Scene::isIntersectLeaf(int dataid, const Ray &ray, Intersection *intersect) const {
	SceneObject *obj = sceneObjects[dataid].get();
	Intersection tmp_isect;
	
#if 0 // AABB display for debug
	AABBGeometry aabbgeom(obj->getAABB());
	if(aabbgeom.isIntersect(ray, &tmp_isect)) {
		*intersect = tmp_isect;
		intersect->objectId = dataid;
		return true;
	}
#else
	if(obj->isIntersect(ray, &tmp_isect)) {
		*intersect = tmp_isect;
		intersect->objectId = dataid;
		return true;
	}
#endif
	
	return false;
}

bool Scene::intersectSceneObjects(const Ray &ray, Intersection *intersect) {
	
    intersect->clear();
	
#if 0
    // brute force
    for(int i = 0; i < (int)sceneObjects.size(); ++i) {
		Intersection tmpinter;
        if(sceneObjects[i]->isIntersect(ray, &tmpinter)) {
			if(tmpinter.distance < intersect->distance) {
				*intersect = tmpinter;
				intersect->objectId = i;
			}
        }
    }
	return intersect->objectId != Intersection::kNoIntersected;
#else
	// BVH
	return BVHNode::isIntersectBVHTree(this, *objectBVH, ray, intersect);
#endif
	
}
