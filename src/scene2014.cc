#include <iostream>
#include <cstdio>
#include <map>

#include "scene.h"
#include "sphere.h"
#include "sceneobject.h"
#include "scenesupport.h"
#include "mesh.h"
#include "camera.h"
#include "wavefrontobj.h"
#include "bsdf.h"
#include "bsdfdiffuse.h"
#include "bsdfspecular.h"
#include "bsdfrefraction.h"
#include "bsdfpaint.h"
#include "aabbgeometry.h"
#include "texture.h"

#include "scene2014.h"

/////
using namespace r1h;

///////
class FXEmitMaterial : public Material {
public:
	BSDF *bsdf;

	FXEmitMaterial() : bsdf(0) {
		bsdf = new DiffuseBSDF();
	}
	~FXEmitMaterial() {
		delete bsdf;
	}

	Color albedo(const SceneObject *obj, const Intersection &hp) const {
		return Color(0.0);
	}
	Color emission(const SceneObject *obj, const Intersection &hp) const {
		return Color(4.0, 5.5, 6.0);
	}
	void makeNextRays(const Ray &ray, const Intersection &hp, const int depth, Random *rnd, std::vector<Ray> *outvecs) const {
		bsdf->makeNextRays(ray, hp, depth, rnd, outvecs);
	}
};

class ObjectMaterial : public Material {
public:
	BSDF *bsdf;
	VoronoiTexture *voronoi;

	ObjectMaterial() : bsdf(0), voronoi(0) {
		bsdf = new PaintBSDF();
		//bsdf = new DiffuseBSDF();
		voronoi = new VoronoiTexture(0.8);
		voronoi->setTransform(Matrix4::makeScale(4.0, 4.0, 4.0));
	}
	~ObjectMaterial() {
		delete bsdf;
		delete voronoi;
	}

	Color albedo(const SceneObject *obj, const Intersection &hp) const {
		return Color(0.8);// *voronoi->sample(hp.position).r;
	}
	Color emission(const SceneObject *obj, const Intersection &hp) const {
		return Color(0.0, 0.0, 0.0);
	}
	void makeNextRays(const Ray &ray, const Intersection &hp, const int depth, Random *rnd, std::vector<Ray> *outvecs) const {
		bsdf->makeNextRays(ray, hp, depth, rnd, outvecs);
	}
};

class DebrisMaterial : public Material {
public:
	BSDF *bsdf;

	DebrisMaterial() : bsdf(0) {
		bsdf = new RefractionBSDF();
	}
	~DebrisMaterial() {
		delete bsdf;
	}

	Color albedo(const SceneObject *obj, const Intersection &hp) const {
		return Color(0.8, 0.9, 0.98);
	}
	Color emission(const SceneObject *obj, const Intersection &hp) const {
		return Color(0.0, 0.03, 0.1);
	}
	void makeNextRays(const Ray &ray, const Intersection &hp, const int depth, Random *rnd, std::vector<Ray> *outvecs) const {
		bsdf->makeNextRays(ray, hp, depth, rnd, outvecs);
	}
};

class BackgroundMaterial : public Material {
public:
	BSDF *bsdf;
	VoronoiTexture *voronoi;

	BackgroundMaterial() : bsdf(0), voronoi(0) {
		bsdf = new PaintBSDF();
		voronoi = new VoronoiTexture(0.8);
		voronoi->setTransform(Matrix4::makeScale(50.0, 50.0, 50.0));
	}
	~BackgroundMaterial() {
		delete bsdf;
		delete voronoi;
	}

	Color skyColor(const Ray &ray) const {
		Color col = voronoi->sample(ray.direction);
		return Color((col.r < 0.1) ? 1.0 : 0.0);// *std::max(0.0, pow(ray.direction.y, 0.25));
		//return Color(0.5) * std::max(0.0, ray.direction.y);
	};

	Color albedo(const SceneObject *obj, const Intersection &hp) const {
		return Color(0.75, 0.8, 0.65) * voronoi->sample(hp.position).r;
	}
	Color emission(const SceneObject *obj, const Intersection &hp) const {
		return Color(0.0, 0.0, 0.0);
	}
	void makeNextRays(const Ray &ray, const Intersection &hp, const int depth, Random *rnd, std::vector<Ray> *outvecs) const {
		bsdf->makeNextRays(ray, hp, depth, rnd, outvecs);
	}
};

bool r1h::setupMainScene2014(int w, int h, Scene *scene) {
    
    // objs
	bool loaded = scene->loadWavefrontObj("extra/fantasy.obj");
	//bool loaded = scene->loadWavefrontObj("extra/bleriot.obj");

	// replace materials
	MaterialRef emitmatref = MaterialRef(new FXEmitMaterial());
	MaterialRef objmatref = MaterialRef(new ObjectMaterial());
	MaterialRef dbrmatref = MaterialRef(new DebrisMaterial());


	size_t objnum = scene->getObjectsCount();
	for (size_t i = 0; i < objnum; i++) {
		SceneObject *obj = scene->getObject(i);
		MaterialRef usematref;
		if (obj->getName().find("FX") != std::string::npos) {
			usematref = emitmatref;
		}
		else if (obj->getName().find("Icosphere") != std::string::npos) {
			usematref = dbrmatref;
		}
		else {
			usematref = objmatref;
		}

		size_t matnum = obj->getMaterialCount();
		for (size_t imat = 0; imat < matnum; imat++) {
			obj->replaceMaterial(imat, usematref);
		}
	}

    //----- background
	//scene->setBackgroundMaterial(objmatref);
	MaterialRef bgmatref = MaterialRef(new BackgroundMaterial());
	scene->setBackgroundMaterial(bgmatref);

    //----- camera
	R1hFPType aspect = R1hFPType(w) / h;

    Camera *camera = scene->getCamera();
	camera->setLookat(
        Vector3(-30.0, 0.0, 0.0),
        Vector3(0.0, 0.0, 0.0),
        Vector3(0.0, 1.0, 0.0)
    );
	camera->setFieldOfView(30.0);
	camera->setAspectRatio(aspect);
	camera->setFocusDistance(30.0);
	camera->setApertureRadius(0.8);

	return loaded;
}
