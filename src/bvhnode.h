#ifndef R1H_BVHNODE_H
#define R1H_BVHNODE_H

#include <vector>
//#include "aabb.h"

namespace r1h {

/*
struct BVHNode {
    BVHNode *children;
    int childNum;
    AABB aabb;
    union {
        int axis;
        int dataId;
    };
    
    BVHNode(): children(0), childNum(0) {}
    ~BVHNode() {
        if(children) {
            delete [] children;
        }
    }
    
    void allocChildren(const int chnum=2) {
        if(chnum > childNum) {
            delete [] children;
        }
        children = new BVHNode[chnum];
        childNum = chnum;
    }
    
    bool isLeaf() const {
        return (children)? false : true;
    }
    
    int buildAABBTree(AABB *aabbArray, const int aabbnum);
    
private:
    //
    struct AABBAxisComparator {
        int axisId;
        AABBAxisComparator(const int a): axisId(a) {}
        bool operator() (const AABB &aabb1, const AABB &aabb2) {
            return aabb1.centroid.v[axisId] < aabb2.centroid.v[axisId];
        }
    };
    int recurseBuildTree(BVHNode &node, AABB *aabbArray, const int aabbnum, const int depth);
};
*/

class BVHNode {
public:
    BVHNode() {}
    ~BVHNode() {}
    
    void releaseAll() {}
};

}
#endif
