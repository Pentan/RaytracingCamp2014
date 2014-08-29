#ifndef R1H_BSDFREFRACTION_H
#define R1H_BSDFREFRACTION_H

#include "bsdf.h"

namespace r1h {

class RefractionBSDF : public BSDF {
public:
	const double kIor = 1.5; // ior of REFLECTION_TYPE_REFRACTION

    ~RefractionBSDF();
    void makeNextRays(const Ray &ray, const Intersection &hp, const int depth, Random *rnd, std::vector<Ray> *outvecs);
};

}
#endif
