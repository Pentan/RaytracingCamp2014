#include <iostream>

#include "r1htypes.h"
#include "scenesupport.h"
#include "bsdfdiffuse.h"
#include "camera.h"
#include "sceneobject.h"

using namespace r1h;

ObjMaterial::ObjMaterial():
diffuseColor(0.7), emitColor(0.0)
{
	bsdf = new DiffuseBSDF();
}
ObjMaterial::~ObjMaterial() {
	delete bsdf;
}

Color ObjMaterial::skyColor(const Ray &ray) const {
	return Color(1.0) * (ray.direction.y * 0.5 + 0.5);
}

Color ObjMaterial::albedo(const SceneObject *obj, const Intersection &isect) const {
	return diffuseColor;
}
Color ObjMaterial::emission(const SceneObject *obj, const Intersection &isect) const {
	return emitColor;
}
void ObjMaterial::makeNextRays(const Ray &ray, const Intersection &isect, const int depth, Random *rnd, std::vector<Ray> *outvecs) const {
	bsdf->makeNextRays(ray, isect, depth, rnd, outvecs);
}

void ObjMaterial::setKd(Color kd) {
	diffuseColor = kd;
}
void ObjMaterial::setEmissionColor(Color emit) {
	emitColor = emit;
}

//
SimpleObjLoader::SimpleObjLoader(std::string filepath, Scene *sc):
	WavefrontObj(filepath), scene(sc),
	curNumVs(0), curNumVNs(0), curNumVTs(0),
	vOffset(0), vnOffset(0), vtOffset(0)
{
	// backgroud material
	scene->setBackgroundMaterial(MaterialRef(new ObjMaterial()));
	// default material
	ObjMaterial *mat = new ObjMaterial();
	mat->setEmissionColor(Color(1.0, 0.0, 0.0));
	defaultMat = MaterialRef(mat);
}
SimpleObjLoader::~SimpleObjLoader() {}

// v, vn, vt, Ns, Ka, Kd, Ks, Ni, d
void SimpleObjLoader::foundVector(const ParameterType pt, const double x, const double y, const double z) {
	
	Mesh *mesh;
	Vector3 v(x, y, z);
	
	switch(pt) {
		case OBJ_v:
			mesh = dynamic_cast<Mesh*>(curObj->getGeometry());
			mesh->addVertex(v);
			++curNumVs;
			//mesh->addVertexWithAttrs(v, Vector3(0.0), Vector3(0.0)); //+++++
			sceneAABB.expand(v);
			break;
		case OBJ_vn:
			mesh = dynamic_cast<Mesh*>(curObj->getGeometry());
			mesh->addNormal(v);
			++curNumVNs;
			break;
		case OBJ_vt:
			mesh = dynamic_cast<Mesh*>(curObj->getGeometry());
			mesh->addAttribute(0, v);
			++curNumVTs;
			break;
		case MTL_Ns:	break;
		case MTL_Ka:	break;
		case MTL_Kd:
			curMat->setKd(Color(x, y, z));
			break;
		case MTL_Ks:	break;
		case MTL_Ni:	break;
		case MTL_d:		break;
		default:
			break;
	}
}

// mtllib, o, s, usemtl, newmtl, MTL_map_Kd
void SimpleObjLoader::foundString(const ParameterType pt, const std::string &str) {
	switch(pt) {
		case OBJ_usemtl:
			curMatId = curObj->addMaterial(matMap[str]);
			//std::cout << "select " << str << ": " << curMatId << std::endl;
			break;
		case OBJ_o: // FIXME?
		case OBJ_g:
			//std::cout << "new object : " << str << std::endl;
			vOffset = curNumVs;
			vnOffset = curNumVNs;
			vtOffset = curNumVTs;
			
			curObj = new SceneObject();
			curMatId = curObj->addMaterial(defaultMat); // push default material
			curMesh = new Mesh();
			// atribute container for vt
			curMesh->newAttributeContainer();
			// dummy vertices for 1 start index. (don't increment numVs etc)
			curMesh->addVertex(Vector3(0.0));
			curMesh->addNormal(Vector3(0.0));
			curMesh->addAttribute(0, Vector3(0.0));
			
			curObj->setGeometry(GeometryRef(curMesh));
			scene->addObject(SceneObjectRef(curObj));
			break;
		case OBJ_s:			break;
		case OBJ_mtllib:
			//std::cout << "import : " << str << std::endl;;
			importMTL(str.c_str());
			break;
		case MTL_newmtl:
			//std::cout << "new material : " << str << std::endl;;
			curMat = new ObjMaterial();
			matMap[str] = MaterialRef(curMat);
			break;
		case MTL_map_Kd:	break;
		default:
			break;
	}
}
// illum
void SimpleObjLoader::foundInteger(const ParameterType pt, const int i) {
	switch(pt) {
		case MTL_illum:
			break;
		default:
			printf("unknown integer value %d found in material\n", i);
	}
}
// f
void SimpleObjLoader::foundFace(const ParameterType pt, const std::vector<FaceInfo> &fids) {
	Mesh::Face face;
	
	// first triangle
	face.setV(fids[0].v - vOffset, fids[1].v - vOffset, fids[2].v - vOffset);
	face.setN(fids[0].vn - vnOffset, fids[1].vn - vnOffset, fids[2].vn - vnOffset);
	face.addAttr(0, fids[0].vt - vtOffset, fids[1].vt - vtOffset, fids[2].vt - vtOffset);
	face.matid = curMatId;
	curMesh->addFace(face);
	
	//std::cout << "f " << fids[0].v << "," << fids[1].v << "," << fids[2].v << std::endl;
	
	if(fids.size() > 3) {
		// quad
		face.setV(fids[2].v - vOffset, fids[3].v - vOffset, fids[0].v - vOffset);
		face.setN(fids[2].vn - vnOffset, fids[3].vn - vnOffset, fids[0].vn - vnOffset);
		face.addAttr(0, fids[2].vt - vtOffset, fids[3].vt - vtOffset, fids[0].vt - vtOffset);
		face.matid = curMatId;
		curMesh->addFace(face);
	}
}
// mtl, use default
//void SimpleObjLoader::importMTL(const char *filename);

// endof .mtl or .obj
void SimpleObjLoader::endFile(std::string fullpath) {
	
	std::cout << fullpath << " ended" << std::endl;
	
	if(fullpath.substr(fullpath.length() - 3) == "obj") {
		const R1hFPType fovdeg = 60.0;
		const R1hFPType aspect = 16.0 / 9.0;
		
		Camera *camera = scene->getCamera();
		camera->setFieldOfView(fovdeg);
		camera->setAspectRatio(aspect);
		
		Vector3 aabbsize = sceneAABB.max - sceneAABB.min;
		R1hFPType d = aabbsize.y * 0.5 / tan(fovdeg * 0.5 * M_PI / 180.0);
		
		/*
		/////
		int objcount = scene->getObjectsCount();
		for(int i = 0; i < objcount; i++) {
			SceneObject *obj = scene->getObject(i);
			Mesh *mesh = dynamic_cast<Mesh*>(obj->getGeometry());
			//mesh->calcSmoothNormals();
			mesh->dumpFaces();
		}
		/////
		*/
		
		int objcount = scene->getObjectsCount();
		printf("%d objects loaded\n", objcount);
		
		sceneAABB.updateCentroid();
		Vector3 look = sceneAABB.centroid;
		//Vector3 pos = look + Vector3(0.0, 0.0, d * 4.0);
		Vector3 pos = look + Vector3::normalized(Vector3(0.5, 0.5, 1.0)) * d * 3.0;
		camera->setLookat(pos, look, Vector3(0.0, 1.0, 0.0));
		/*
		 camera->setLookat(
		 Vector3(0.0, 0.0, 10.0),
		 Vector3(0.0, 0.0, 0.0),
		 Vector3(0.0, 1.0, 0.0)
		 );
		 */
	}
}