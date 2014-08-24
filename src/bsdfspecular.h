#ifndef R1H_BSDFSPECULAR_H
#define R1H_BSDFSPECULAR_H

#include "bsdf.h"

namespace r1h {

class SpecularBSDF : public BSDF {
public:
    ~SpecularBSDF();
    void makeNextRays(const Ray &ray, const Hitpoint &hp, const int depth, Random *rnd, std::vector<Ray> *outvecs);
};

}
#endif
