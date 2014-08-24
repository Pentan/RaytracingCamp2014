
#include "eduptscene.h"
#include "sphere.h"
#include "sceneobject.h"

using namespace r1h;

/// Material
EduptMaterial::EduptMaterial(const Color &col, const Color &emit, const ReflectionType reft):
	color_(col), emission_(emit), reflection_type_(reft)
{
	switch(reflection_type_) {
		case DIFFUSE:
			bsdf = new DiffuseBSDF();
			break;
		case SPECULAR:
			bsdf = new SpecularBSDF();
			break;
		case REFRACTION:
			bsdf = new RefractionBSDF();
			break;
		case BACKGROUND:
			break;
	}
}

EduptMaterial::~EduptMaterial() {
	delete bsdf;
}

Color EduptMaterial::skyColor(const Ray &ray) const {
	return color_;
}

Color EduptMaterial::albedo(const SceneObject *obj, const Hitpoint &hp) const {
	return color_;
}
Color EduptMaterial::emission(const SceneObject *obj, const Hitpoint &hp) const {
	return emission_;
}
void EduptMaterial::makeNextRays(const Ray &ray, const Hitpoint &hp, const int depth, Random *rnd, std::vector<Ray> *outvecs) const {
	bsdf->makeNextRays(ray, hp, depth, rnd, outvecs);
}

/// scene
bool EduptScene::load(Scene *scene, double aspect) {
	
	struct SphereDef {
		R1hFPType r;
		Vector3 pos;
		Color emittion;
		Color diffuse;
		EduptMaterial::ReflectionType type;
	} spheres[] = {
		{1e5, Vector3( 1e5 + 1.0, 40.8, 81.6),    Color(), Color(0.75, 0.25, 0.25), EduptMaterial::DIFFUSE},
		{1e5, Vector3(-1e5 + 99.0, 40.8, 81.6),   Color(), Color(0.25, 0.25, 0.75), EduptMaterial::DIFFUSE},
		{1e5, Vector3(50.0, 40.8, 1e5),           Color(), Color(0.75, 0.75, 0.75), EduptMaterial::DIFFUSE},
		{1e5, Vector3(50.0, 40.8, 1e5 + 250.0),   Color(), Color(0.0),                 EduptMaterial::DIFFUSE},
		{1e5, Vector3(50.0, 1e5, 81.6),           Color(), Color(0.75, 0.75, 0.75), EduptMaterial::DIFFUSE},
		{1e5, Vector3(50.0, 1e5 + 81.6, 81.6),    Color(), Color(0.75, 0.75, 0.75), EduptMaterial::DIFFUSE},
		{20.0, Vector3(65.0, 20.0, 20.0),         Color(), Color(0.25, 0.75, 0.25), EduptMaterial::DIFFUSE},
		{16.5, Vector3(27.0, 16.5, 47.0),         Color(), Color(0.99, 0.99, 0.99), EduptMaterial::SPECULAR},
		{16.5, Vector3(77.0, 16.5, 78.0),         Color(), Color(0.99, 0.99, 0.99), EduptMaterial::REFRACTION},
		{15.0, Vector3(50.0, 90.0, 81.6),         Color(36.0, 36.0, 36.0), Color(), EduptMaterial::DIFFUSE}
	};
	int numspheres = sizeof(spheres) /sizeof(SphereDef);
	
	for(int i = 0; i < numspheres; i++) {
		SphereDef &sphrdef = spheres[i];
		
		SceneObject *obj = new SceneObject();
		
		Sphere *sphere = new Sphere(sphrdef.r, sphrdef.pos);
		obj->setGeometry(GeometryRef(sphere));
		
		EduptMaterial *mat = new EduptMaterial(sphrdef.diffuse, sphrdef.emittion, sphrdef.type);
		//EduptMaterial *mat = new EduptMaterial(Vector3(0.0), Vector3(1.0), sphrdef.type);
		//EduptMaterial *mat = new EduptMaterial(sphrdef.diffuse, sphrdef.diffuse, sphrdef.type);
		obj->addMaterial(MaterialRef(mat));
		
		scene->addObject(SceneObjectRef(obj));
	}
	
	EduptMaterial *bgmat = new EduptMaterial(Color(0.5), Color(0.5), EduptMaterial::BACKGROUND);
	scene->setBackgroundMaterial(MaterialRef(bgmat));
	
	Camera *camera = new Camera();
	camera->setLookat(
					  Vector3(50.0, 52.0, 220.0),
					  Vector3(50.0, 52.0, 220.0 - 1.0),
					  Vector3(0.0, 1.0, 0.0)
	);
	camera->setFocal(40.0, 30.0 * aspect);
	camera->setAspectRatio(aspect);
	scene->setCamera(CameraRef(camera));
	
	return true;
}