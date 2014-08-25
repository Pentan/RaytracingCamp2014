
#include <algorithm>
#include "mesh.h"

using namespace r1h;

const int Mesh::kTypeID = 'MESH';

Mesh::Face::Face() : matid(-1) {}
Mesh::Face::Face(const int a, const int b, const int c, const int m) {
	AttrCoord aco = {0, a, b, c};
	v0 = n0 = a;
	v1 = n1 = b;
	v2 = n2 = c;
	attrs.push_back(aco);
	matid = m;
}

inline void Mesh::Face::setV(const int a, const int b, const int c) {
	v0 = a;
	v1 = b;
	v2 = c;
}
inline void Mesh::Face::setN(const int a, const int b, const int c) {
	n0 = a;
	n1 = b;
	n2 = c;
}
inline void Mesh::Face::addAttr(const int attrid, const int a, const int b, const int c) {
	AttrCoord aco = {attrid, a, b, c};
	attrs.push_back(aco);
}

/////
Mesh::Mesh(const int vreserve, const int freserv): Geometry(), bvhRoot(0) {
	if(vreserve > 0) {
		vertices.reserve(vreserve);
		normals.reserve(vreserve);
	}
	if(freserv > 0) {
		faces.reserve(freserv);
	}
	vertex_reserved = vreserve;
	face_reserved = freserv;
}

Mesh::~Mesh() {
	//printf("Sphere %p destructed\n", this);
	if(bvhRoot) {
		delete bvhRoot;
	}
}

size_t Mesh::addVertexWithAttrs(const Vector3 &p, const Vector3 &n, const Vector3 &uv, const int uvid) {
	size_t ret = addVertex(p);
	addNormal(n);
	if(uvid >= 0) {
		addAttribute(uvid, uv);
	}
	return ret;
}

size_t Mesh::addVertex(const Vector3 &v) {
	vertices.push_back(v);
	return vertices.size() - 1;
}
size_t Mesh::addNormal(const Vector3 &v) {
	normals.push_back(v);
	return normals.size() - 1;
}
size_t Mesh::newAttributeContainer() {
	std::vector<Vector3> attrv;
	attrv.reserve(vertex_reserved);
	attributes.push_back(attrv);
	return attributes.size() - 1;
}
size_t Mesh::addAttribute(const int attrid, const Vector3 &v) {
	if(attrid < 0 || attrid > (int)attributes.size()) {
		throw "vertex attributes out of range";
	}
	attributes[attrid].push_back(v);
	return attributes[attrid].size() - 1;
}

size_t Mesh::addFace(const Mesh::Face &f) {
	faces.push_back(f);
	return faces.size() - 1;
}
size_t Mesh::addFace(const int a, const int b, const int c, const int matid) {
	faces.push_back(Face(a, b, c, matid));
	return faces.size() - 1;
}

/////+++++
Vector3 Mesh::getVaryingAttr(const int faceid, const int attrid, const Vector3 weights) {
    const Face &face = faces[faceid];
    const AttrCoord &attrco = face.attrs[attrid];
    const std::vector<Vector3> &attrvec = attributes[attrco.attrid];
    const Vector3 a1 = attrvec[attrco.co1];
    const Vector3 a2 = attrvec[attrco.co2];
    const Vector3 a3 = attrvec[attrco.co3];
    
    return a1 * weights.x_ + a2 * weights.y_ + a3 * weights.z_;
}

void Mesh::calcSmoothNormals() {
    // clear normals
    for(size_t i = 0; i < normals.size(); i++) {
        normals[i] = Vector3(0.0, 0.0, 0.0);
    }
    // calc face normal
    for(size_t i = 0; i < faces.size(); i++) {
        const Face &f = faces[i];
        const Vector3 v01 = vertices[f.v1] - vertices[f.v0];
        const Vector3 v02 = vertices[f.v2] - vertices[f.v0];
        const Vector3 nv = Vector3::cross(v01, v02);
        normals[f.n0] = normals[f.n0] + nv;
        normals[f.n1] = normals[f.n1] + nv;
        normals[f.n2] = normals[f.n2] + nv;
    }
    // normalize
    for(size_t i = 0; i < normals.size(); i++) {
        normals[i] = Vector3::normalized(normals[i]);
    }
}

void Mesh::buildBVH() {
    if(bvhRoot) {
        // rebuild?
        //std::cout << "rebuild BVH ?" << std::endl;
        delete bvhRoot;
        bvhRoot = 0;
    }
    
    size_t facenum = faces.size();
    //std::cout  << "faces:" << facenum << std::endl;
    
    AABB *faceAABBs = new AABB[facenum];
    for(int i = 0; i < facenum; i++) {
        Face &fc = faces[i];
        AABB &ab = faceAABBs[i];
        ab.expand(vertices[fc.v0]);
        ab.expand(vertices[fc.v1]);
        ab.expand(vertices[fc.v2]);
        ab.dataId = i;
    }
    bvhRoot = new BVHNode();
    //int maxdepth = recurseBuildBVH(*bvhRoot, faceAABBs, facenum);
    size_t maxdepth = bvhRoot->buildAABBTree(faceAABBs, (int)facenum);
    
    //std::cout << "max BVH depth:" << maxdepth << std::endl;
    
    delete [] faceAABBs;
}

AABB Mesh::getAABB() const {
	return bvhRoot->aabb;
}

bool Mesh::triangleIntersect(const int faceid, const Ray &ray, TriangleHitInfo *hitinfo) const {
    const Face &face = faces[faceid];
    const Vector3 &v0 = vertices[face.v0];
    const Vector3 &v1 = vertices[face.v1];
    const Vector3 &v2 = vertices[face.v2];
    
    const Vector3 v01 = v1 - v0;
    const Vector3 v02 = v2 - v0;
    const Vector3 r = ray.origin - v0;
    const Vector3 u = Vector3::cross(ray.direction, v02);
    const Vector3 v = Vector3::cross(r, v01);
    
    const double div = 1.0 / Vector3::dot(u, v01);
    const double t = Vector3::dot(v, v02) * div;
    const double b = Vector3::dot(u, r) * div;
    const double c = Vector3::dot(v, ray.direction) * div;
    
    /*
    printf("face(%d,%d,%d)¥n", face.v0, face.v1, face.v2);
    v0.print("v0");
    v1.print("v1");
    v2.print("v2");
    v01.print("v01");
    v02.print("v02");
    ray.dir_.print("ray.dir_");
    ray.org_.print("ray.org_");
    r.print("r");
    u.print("u");
    v.print("v");
    printf("(t,b,c)=(%lf,%lf,%lf)¥n", t, b, c);//+++++
    */
    if((b + c < 1.0 && b > 0.0 && c > 0.0) && t > 0.0) {
        // hit
        const double a = 1.0 - b - c;
        hitinfo->distance = t;
        hitinfo->position = v0 * a + v1 * b + v2 * c;
        hitinfo->w0 = a;
        hitinfo->w1 = b;
        hitinfo->w2 = c;
        hitinfo->faceid = faceid;
        
        return true;
    }
    return false;
}

bool Mesh::intersectBVHNode(const BVHNode &node, const Ray &ray, TriangleHitInfo *hitinfo) const {
    if(node.isLeaf()) {
        // 葉ノード
        TriangleHitInfo tmp_info;
        if(triangleIntersect(node.dataId, ray, &tmp_info)) {
            if(tmp_info.distance < hitinfo->distance) {
                *hitinfo = tmp_info;
                return true;
            }
        }
        //return false; // at last
    } else {
        double d;
        if(node.aabb.isIntersect(ray, &d)) { // レイがAABBにヒットしている
            if(d < hitinfo->distance) { // より近くで
                TriangleHitInfo nearest_info, tmp_info;
                for(int i = 0; i < node.childNum; i++) {
                    // 子ノードをチェック
                    if(intersectBVHNode(node.children[i], ray, &tmp_info)) {
                        if(tmp_info.distance < nearest_info.distance) {
                            nearest_info = tmp_info;
                        }
                    }
                }
                // 今のヒット位置よりも近かったら採用
                if(nearest_info.distance < hitinfo->distance) {
                    *hitinfo = nearest_info;
                    return true;
                }
            }
        }
    }
    
    return false;
}

bool Mesh::isIntersection(const Ray &ray, Intersection *intersect) const {
    
	Hitpoint *hitpoint = &intersect->hitpoint;
	
#if 0
    /////+++++
    // brute force now!
    TriangleHitInfo nearest_info, tmp_info;
    for(int iface = 0; iface < faces.size(); iface++) {
        //printf("face[%d]¥n", iface);//+++++
        if(triangleIntersect(iface, ray, &tmp_info)) {
            if(tmp_info.distance < nearest_info.distance) {
                nearest_info = tmp_info;
                nearest_info.faceid = iface;
            }
        }
    }
    if(nearest_info.faceid < 0) {
        return false;
    }
    /////+++++
#else
    /////+++++
    // traverse BVH
    TriangleHitInfo nearest_info;
    if(!intersectBVHNode(*bvhRoot, ray, &nearest_info)) {
        return false;
    }
    /////+++++
#endif
    
    // calc face infomation
    const Face &hitface = faces[nearest_info.faceid];
    hitpoint->distance = nearest_info.distance;
    hitpoint->position = nearest_info.position;
    hitpoint->normal =
        normals[hitface.v0] * nearest_info.w0 +
        normals[hitface.v1] * nearest_info.w1 +
        normals[hitface.v2] * nearest_info.w2;
    // TODO: attributes
    //hitpoint->material = (hitface.matid < 0)? 0 : materials[hitface.matid];
    hitpoint->materialId = hitface.matid;
    hitpoint->faceId = nearest_info.faceid;
    hitpoint->varyingWeight = Vector3(nearest_info.w0, nearest_info.w1, nearest_info.w2);
    
    return true;
}
