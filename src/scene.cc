
#include "scene.h"
#include "camera.h"
#include "material.h"
#include "bvhnode.h"
#include "sceneobject.h"
#include "intersection.h"

using namespace r1h;

Scene::Scene():
    camera(0), objectBVH(0), bgMaterial(0)
{}

Scene::~Scene() {
    if(objectBVH) {
        objectBVH->releaseAll();
        delete objectBVH;
        objectBVH = NULL;
    }
}
    
bool Scene::load() {
    camera = CameraRef(new Camera());
	bgMaterial = MaterialRef(nullptr);
	// FIXME
	
	return true;
}

int Scene::addObject(SceneObjectRef objref) {
	sceneObjects.push_back(objref);
    return (int)sceneObjects.size() - 1;
};

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
			const Hitpoint &hitpoint = intersect.hitpoint;
			
			// from hit face info
			const Material *hitmat = hitobject->getMaterialById(hitpoint.materialId);
			const Color albedocol = hitmat->albedo(hitobject, hitpoint);
            const Color emitcol = hitmat->emission(hitobject, hitpoint);
            
            radiancecol += Color::mul(emitcol, traceray.weight);
            
			//+++++
			// for debugging
            // normal
            //radiancecol += hitpoint.normal * 0.5 + Vector3(0.5);
            //continue;
			// color
			//radiancecol += albedocol;
			//continue;
            //+++++
			
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
            hitmat->makeNextRays(traceray, hitpoint, depth, crnd, tmprays);
            for(int j = 0; j < (int)tmprays->size(); j++) {
                Ray &nwray = tmprays->at(j);
                nwray.smallOffset(hitpoint.normal);
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

bool Scene::intersectSceneObjects(const Ray &ray, Intersection *intersect) {
	
    // brute force
    intersect->clear();
    for(int i = 0; i < (int)sceneObjects.size(); ++i) {
		Intersection tmpinter;
        if(sceneObjects[i]->isIntersection(ray, &tmpinter)) {
			if(tmpinter.hitpoint.distance < intersect->hitpoint.distance) {
				intersect->hitpoint = tmpinter.hitpoint;
				intersect->objectId = i;
			}
        }
    }
    
	return intersect->objectId != Intersection::kNoIntersected;
}


