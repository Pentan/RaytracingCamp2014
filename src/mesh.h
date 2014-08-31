/*
### Mesh data format ###
vertices[] = {Vector3, ...}
normals[] = {Vector3, ...}
attributes[] = {{Vector3, ...}, ...}
faces[] = {Face, ...}

MapCoord { id mapid, co1, co2, co3; }

Face {
    id v1, v2, v3;
    id n1, n2, n3;
    attributes[] = {MapCoord, ...}
    Material *m
}
*/

#ifndef R1H_MESH_H
#define R1H_MESH_H

#include <vector>

#include "r1htypes.h"
#include "ray.h"
#include "material.h"
#include "intersection.h"
#include "geometry.h"
#include "aabb.h"
#include "bvhnode.h"

namespace r1h {

class Mesh : public Geometry {
public:
	
    static const int kTypeID;
    
    struct AttrCoord {
        int attrid;         // index of Geometry's attributes
        int co0, co1, co2;
    };
    
    struct Face {
        int v0, v1, v2;
        int n0, n1, n2;
        std::vector<AttrCoord> attrs;
        int matid;
        
        Face();
        Face(const int a, const int b, const int c, const int m);
        
        void setV(const int a, const int b, const int c);
        void setN(const int a, const int b, const int c);
        void addAttr(const int attrid, const int a, const int b, const int c);
    };
    
public:
    // empty mesh
    Mesh(const int vreserve=0, const int freserv=0);
    ~Mesh();
    
    size_t addVertexWithAttrs(const Vector3 &p, const Vector3 &n, const Vector3 &uv=0, const int uvid=-1);
    size_t addVertex(const Vector3 &v);
    size_t getVertexCount() const;
	
	size_t addNormal(const Vector3 &v);
    size_t getNormalCount() const;
    
	size_t newAttributeContainer();
    size_t addAttribute(const int attrid, const Vector3 &v);
	size_t getAttributeCount(const int attrid) const;
    
    size_t addFace(const Mesh::Face &f);
    size_t addFace(const int a, const int b, const int c, const int matid=0);
    size_t getFaceCount() const;
	
    Vector3 getVaryingAttr(const int faceid, const int attrid, const Vector3 weights);
    
    /////
    struct TriangleHitInfo {
        double distance;
        Vector3 position;
        double w0, w1, w2;
        int faceid;
        TriangleHitInfo(): distance(kINF), faceid(-1) {}
    };
    
    void calcSmoothNormals();
    void buildBVH();
    
    bool triangleIntersect(const int faceid, const Ray &ray, TriangleHitInfo *hitinfo) const;
	
	// override
	virtual AABB getAABB() const;
    virtual bool isIntersect(const Ray &ray, Intersection *intersect) const;
    virtual void prepareRendering();
	
	/// for debug
	void dumpFaces() const;
	
private:
	std::vector<Vector3> vertices;
    std::vector<Vector3> normals;
    std::vector<std::vector<Vector3> > attributes;
    std::vector<Face> faces;
    
    BVHNode *bvhRoot;
    
    int vertex_reserved;
    int face_reserved;
	
    //int recurseBuildBVH(BVHNode &node, AABB *aabbPtrs, const int aabbnum, const int depth=0);
    bool intersectBVHNode(const BVHNode &node, const Ray &ray, TriangleHitInfo *hitinfo) const;
};

}
#endif
