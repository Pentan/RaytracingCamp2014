#ifndef R1H_BACKGROUND_H
#define R1H_BACKGROUND_H

#include "r1htypes.h"
#include "sceneobject.h"

namespace r1h {

class BackgroundMaterial : public Material {
public:
    BackgroundMaterial() {};
    ~BackgroundMaterial() {};
    
	Color skyColor(const Ray &ray) const { return Color(0.5); };
	
    Color albedo(const SceneObject *obj, const Hitpoint &hp) const { return Vector3(0.0); };
    Color emission(const SceneObject *obj, const Hitpoint &hp)  const { return Vector3(1.0); };
    void makeNextRays(const Ray &ray, const Hitpoint &hp, const int depth, Random *rnd, std::vector<Ray> *outvecs) const {};
    
    virtual Color backgroundColor(const Ray &ray) { return Color(0.8); }
};

}
#endif
