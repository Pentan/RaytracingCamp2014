
#include "bsdfspecular.h"

using namespace r1h;

SpecularBSDF::~SpecularBSDF()
{
}

void SpecularBSDF::makeNextRays(const Ray &ray, const Hitpoint &hp, const int depth, Random *rnd, std::vector<Ray> *outvecs) {
	Ray nxtray;
	nxtray = Ray(hp.position, ray.direction - hp.normal * 2.0 * Vector3::dot(hp.normal, ray.direction));
	nxtray.weight = Color(1.0);
	outvecs->push_back(nxtray);
}
