#ifndef R1H_EDUPTSCENE_H
#define R1H_EDUPTSCENE_H

#include "material.h"
#include "bsdf.h"
#include "bsdfdiffuse.h"
#include "bsdfspecular.h"
#include "bsdfrefraction.h"
#include "scene.h"

namespace r1h {

/// material
class EduptMaterial : public Material {
public:
	enum ReflectionType {
		DIFFUSE,
		SPECULAR,
		REFRACTION,
		BACKGROUND
	};
	
    EduptMaterial(const Color &col, const Color &emit, const ReflectionType reft);
    ~EduptMaterial();
    
	Color skyColor(const Ray &ray) const;
    Color albedo(const SceneObject *obj, const Hitpoint &hp) const;
    Color emission(const SceneObject *obj, const Hitpoint &hp) const;
    void makeNextRays(const Ray &ray, const Hitpoint &hp, const int depth, Random *rnd, std::vector<Ray> *outvecs) const;
	
private:
    Color color_;
    Color emission_;
    ReflectionType reflection_type_;
    
    BSDF *bsdf;
};

/// scene
class EduptScene {
public:
	static bool load(Scene *scene, double aspect);
};

}

#endif
