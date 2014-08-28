#include <iostream>
#include <cstdio>
#include <map>

#include "scene.h"
#include "sphere.h"
#include "sceneobject.h"
#include "mesh.h"
#include "camera.h"
//#include "eduptmaterial.h"
#include "wavefrontobj.h"
#include "bsdf.h"
#include "bsdfdiffuse.h"
#include "bsdfspecular.h"
#include "aabbgeometry.h"
#include "background.h"
#include "texture.h"

#include "scene2013.h"

/////
namespace r1h {

class AccentGrayMaterial : public Material {
public:
    BSDF *bsdf;
    VoronoiTexture *voronoi;
    
    AccentGrayMaterial() : bsdf(0), voronoi(0) {
        bsdf = new DiffuseBSDF();
        voronoi = new VoronoiTexture(0.8);
        voronoi->setTransform(Matrix4::makeScale(10.0, 10.0, 10.0));
    }
    ~AccentGrayMaterial() {
        delete bsdf;
        delete voronoi;
    }
	
    Color albedo(const SceneObject *obj, const Hitpoint &hp) const {
        return Color(0.106, 0.106, 0.106) * 0.8;
    }
    Color emission(const SceneObject *obj, const Hitpoint &hp) const {
        return Color(0.0, 0.0, 0.0);
    }
	void makeNextRays(const Ray &ray, const Hitpoint &hp, const int depth, Random *rnd, std::vector<Ray> *outvecs) const {
        bsdf->makeNextRays(ray, hp, depth, rnd, outvecs);
    }
};

//
class BodyWhiteMaterial : public Material {
public:
    BSDF *bsdf;
    
    BodyWhiteMaterial() : bsdf(0) {
        bsdf = new DiffuseBSDF();
    }
    ~BodyWhiteMaterial() {
        delete bsdf;
    }
    
    Color albedo(const SceneObject *obj, const Hitpoint &hp) const {
        return Color(0.68);
    }
    Color emission(const SceneObject *obj, const Hitpoint &hp) const {
        return Color(0.0, 0.0, 0.0);
    }
    void makeNextRays(const Ray &ray, const Hitpoint &hp, const int depth, Random *rnd, std::vector<Ray> *outvecs) const {
        bsdf->makeNextRays(ray, hp, depth, rnd, outvecs);
    }
};

//
class EyeColorMaterial : public Material {
public:
    BSDF *bsdf;
    
    EyeColorMaterial() : bsdf(0) {
        bsdf = new DiffuseBSDF();
    }
    ~EyeColorMaterial() {
        delete bsdf;
    }
    
    Color albedo(const SceneObject *obj, const Hitpoint &hp) const {
        return Color(0.128783, 0.610820, 0.640000);
    }
    Color emission(const SceneObject *obj, const Hitpoint &hp) const {
        return Color(0.128783, 0.610820, 0.640000) * 2.0;
    }
    void makeNextRays(const Ray &ray, const Hitpoint &hp, const int depth, Random *rnd, std::vector<Ray> *outvecs) const {
        bsdf->makeNextRays(ray, hp, depth, rnd, outvecs);
    }
};

//
class JointGrayMaterial : public Material {
public:
    BSDF *bsdf;
    
    JointGrayMaterial() : bsdf(0) {
        bsdf = new DiffuseBSDF();
    }
    ~JointGrayMaterial() {
        delete bsdf;
    }
    
    Color albedo(const SceneObject *obj, const Hitpoint &hp) const {
        return Color(0.219066, 0.219066, 0.219066) * 0.7;
    }
    Color emission(const SceneObject *obj, const Hitpoint &hp) const {
        return Color(0.0, 0.0, 0.0);
    }
    void makeNextRays(const Ray &ray, const Hitpoint &hp, const int depth, Random *rnd, std::vector<Ray> *outvecs) const {
        bsdf->makeNextRays(ray, hp, depth, rnd, outvecs);
    }
};

/////
class FloorMaterial : public Material {
public:
    BSDF *diffuseBsdf;
    BSDF *specularBsdf;
    VoronoiTexture *voronoi;
    
    FloorMaterial(): diffuseBsdf(0), specularBsdf(0), voronoi(0) {
        diffuseBsdf = new DiffuseBSDF();
        specularBsdf = new SpecularBSDF();
        voronoi = new VoronoiTexture(0.9, (int)time(NULL));
        voronoi->setTransform(Matrix4::makeScale(0.5, 0.5, 0.5));
    }
    ~FloorMaterial() {
        delete diffuseBsdf;
        delete specularBsdf;
        delete voronoi;
    }
    
    Color albedo(const SceneObject *obj, const Hitpoint &hp) const {
        Color voro = voronoi->sample(hp.position);
        return Color::lerp(Color(0.26, 0.3, 0.28), Color(0.68, 0.7, 0.65), pow(voro.r, 4.0));
        //return Color(0.1, 0.3, 0.0);
    }
    Color emission(const SceneObject *obj, const Hitpoint &hp) const {
        //Color voro = voronoi->sample(hp.position);
        return Color(0.0, 0.0, 0.0);
    }
    void makeNextRays(const Ray &ray, const Hitpoint &hp, const int depth, Random *rnd, std::vector<Ray> *outvecs) const {
        Color voro = voronoi->sample(hp.position);
        if(voro.r > 0.5) {
            diffuseBsdf->makeNextRays(ray, hp, depth, rnd, outvecs);
        } else {
            specularBsdf->makeNextRays(ray, hp, depth, rnd, outvecs);
        }
    }
};

class WallMaterial : public Material {
public:
    BSDF *bsdf;
    FractalNoiseTexture *fnoise;
    
    WallMaterial(): bsdf(0), fnoise(0) {
        bsdf = new DiffuseBSDF();
        fnoise = new FractalNoiseTexture(FractalNoiseTexture::CellNoise, 6, 10);
        //fnoise->setTransform(Mat4::makeScale(4.0, 4.0, 4.0));
    }
    ~WallMaterial() {
        delete bsdf;
        delete fnoise;
    }
    
    Color albedo(const SceneObject *obj, const Hitpoint &hp) const {
        Color noise = fnoise->sample(hp.position);
        return noise.r * Vector3(0.55, 0.55, 0.4);
    }
    Color emission(const SceneObject *obj, const Hitpoint &hp) const {
        //Color noise = fnoise->sample(hp.position_);
        return Vector3(0.0);
    }
    void makeNextRays(const Ray &ray, const Hitpoint &hp, const int depth, Random *rnd, std::vector<Ray> *outvecs) const {
        bsdf->makeNextRays(ray, hp, depth, rnd, outvecs);
    }
};


/////
class MainModelLoader : public WavefrontObj {
    Mesh *mesh;
	
public:
	GeometryRef meshref;
    std::map<std::string, int>materialMap;
    int matId;
    
    MainModelLoader(const char *filepath) : WavefrontObj(filepath), mesh(0) {
        mesh = new Mesh(1000, 1000);
        mesh->newAttributeContainer();
        mesh->addVertexWithAttrs(Vector3(0.0), Vector3(0.0), Vector3(0.0), 0);
		meshref = GeometryRef(mesh);
        matId = 0;
    }
    ~MainModelLoader() {
    }
    
	Mesh* getMesh() {
		return dynamic_cast<Mesh*>(meshref.get());
	}
	
    // v, vn, vt, Ns, Ka, Kd, Ks, Ni, d
    void foundVector(const ParameterType pt, const double x, const double y, const double z) {
        switch(pt) {
            case OBJ_v:
                mesh->addVertexWithAttrs(Vector3(x, y, z), Vector3(0.0), Vector3(x, y, z), 0);
                break;
            case OBJ_vn:
            case OBJ_vt:
            case MTL_Ns:
            case MTL_Ka:
            case MTL_Kd:
            case MTL_Ks:
            case MTL_Ni:
            case MTL_d:
            default:
                break;
        }
    }
    
    // mtllib, o, s, usemtl, newmtl, MTL_map_Kd
    void foundString(const ParameterType pt, const std::string &str) {
        switch(pt) {
            case OBJ_usemtl:
                matId = (materialMap.empty())? 0 : materialMap[str];
                //std::cout << "select " << str << ": " << ((materialMap[str])? "found":"not defined") << std::endl;
                break;
            case OBJ_o:
            case OBJ_g:
            case OBJ_s:
            case OBJ_mtllib:
            case MTL_newmtl:
            case MTL_map_Kd:
            default:
                break;
        }
    }
    
    // illum
    void foundInteger(const ParameterType pt, const int i) {
        //case MTL_illum:
        // noop
    }
    // f
    void foundFace(const ParameterType pt, const std::vector<FaceInfo> &fids) {
        
        // first triangle
        mesh->addFace(fids[0].v, fids[1].v, fids[2].v, matId);
        //std::cout << "f " << fids[0].v << "," << fids[1].v << "," << fids[2].v << std::endl;
        
        if(fids.size() > 3) {
            // quad
            mesh->addFace(fids[2].v, fids[3].v, fids[0].v, matId);
            //std::cout << "f " << fids[2].v << "," << fids[3].v << "," << fids[0].v << std::endl;
        }
    }
    
    // mtl
    void importMTL(const char *filename) {
        WavefrontObj::importMTL(filename);
    }
    
    // endof .mtl or .obj
    virtual void endFile() {
        //std::cout << "obj end" << std::endl;
    }
};
}
/////

using namespace r1h;

// bg
class GradientBg : public BackgroundMaterial {
public:
    Color skyColor(const Ray &ray) const {
        Vector3 upv = Vector3::normalized(Vector3(0.0, 2.0, -0.5));
        Color sun = Color(10.0) * pow(fabs(Vector3::dot(ray.direction, upv)), 8.0);
        Color grad = Color::lerp(Color(1.0, 0.5, 0.6), Color(0.6, 2.0, 2.0), fabs(ray.direction.y));
        return (sun + grad * 1.0);
    }
};

//
static void setupRobot(Scene &scene) {
    
    //----- robo
    MainModelLoader sceneloader("mainscene.obj");
    sceneloader.setBasePath("../bin/model");
    
	
    // create object
    SceneObject *obj;
    obj = new SceneObject();
	
    MaterialRef matref;
    // materials
    matref = MaterialRef(new AccentGrayMaterial());
    sceneloader.materialMap[std::string("AccentGray")] = obj->addMaterial(matref);
    
    matref = MaterialRef(new BodyWhiteMaterial());
    sceneloader.materialMap[std::string("BodyWhite")] = obj->addMaterial(matref);
    
    matref = MaterialRef(new EyeColorMaterial());
    sceneloader.materialMap[std::string("EyeColor")] = obj->addMaterial(matref);
    
    matref = MaterialRef(new JointGrayMaterial());
    sceneloader.materialMap[std::string("JointGray")] = obj->addMaterial(matref);
    
    //
    sceneloader.load();
    
    // finish
    sceneloader.getMesh()->calcSmoothNormals();
    sceneloader.getMesh()->buildBVH();
    
	obj->setGeometry(sceneloader.meshref);
	
    scene.addObject(SceneObjectRef(obj));
}

static void setupBGObjs(Scene &scene) {
    SceneObject *obj;
    
    const char *objfiles[] = { "mainfloor.obj", "mainfront.obj", "mainright.obj", "mainleft.obj" };
    
	// create object
	obj = new SceneObject();
    
    for(int i = 0; i < 4; i++) {
        MainModelLoader sceneloader(objfiles[i]);
        sceneloader.setBasePath("./model2013");
        sceneloader.load();
        
        Material *mat;
        if(i == 0) {
            mat = new FloorMaterial();
        } else {
            mat = new WallMaterial();
        }
        obj->addMaterial(MaterialRef(mat));
        
        sceneloader.getMesh()->calcSmoothNormals();
        sceneloader.getMesh()->buildBVH();
        
        // create object
        obj->setGeometry(sceneloader.meshref);
        scene.addObject(SceneObjectRef(obj));
    }
}

//
void r1h::setupMainScene2013(Scene &scene) {
    
    // objs
    setupRobot(scene);
    setupBGObjs(scene);
    
    //----- background
    GradientBg *bgmat = new GradientBg();
    scene.setBackgroundMaterial(MaterialRef(bgmat));
    
    //----- camera
    // z-up(x,y,z)=y-up(x,z,-y)
    scene.setCamera(CameraRef(new Camera()));
    scene.getCamera()->setLookat(
        Vector3(17.98972, 13.00431, -44.434),
        Vector3(0.0, 5.95464, 0.0),
        Vector3(0.0, 1.0, 0.0)
    );
    scene.getCamera()->setFocal(65.0, 32.0);
}
