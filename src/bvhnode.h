#ifndef R1H_BVHNODE_H
#define R1H_BVHNODE_H

#include "r1htypes.h"
#include <vector>
#include "aabb.h"

namespace r1h {

class BVHNode {
public:
    BVHNode *children;
    int childNum;
    AABB aabb;
    union {
        int axis;
        int dataId;
    };
    
    BVHNode();
    ~BVHNode();
    
    void allocChildren(const int chnum=2);
    bool isLeaf() const;
    int buildAABBTree(AABB *aabbArray, const int aabbnum);
    
private:
    struct AABBAxisComparator {
        int axisId;
        AABBAxisComparator(const int a);
        bool operator() (const AABB &aabb1, const AABB &aabb2);
    };
	
    int recurseBuildTree(BVHNode &node, AABB *aabbArray, const int aabbnum, const int depth);
};


}
#endif
