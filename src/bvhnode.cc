
#include "bvhnode.h"

using namespace r1h;

BVHNode::BVHNode(): children(0), childNum(0) {}
BVHNode::~BVHNode() {
	if(children) {
		delete [] children;
	}
}

void BVHNode::allocChildren(const int chnum) {
	if(chnum > childNum) {
		delete [] children;
	}
	children = new BVHNode[chnum];
	childNum = chnum;
}

bool BVHNode::isLeaf() const {
	return (children)? false : true;
}

int BVHNode::buildAABBTree(AABB *aabbArray, const int aabbnum) {
    int maxdepth = recurseBuildTree(*this, aabbArray, aabbnum, 1);
    //std::cout << "max BVH depth:" << maxdepth << std::endl;
    return maxdepth;
}

int BVHNode::recurseBuildTree(BVHNode &node, AABB *aabbArray, const int aabbnum, const int depth) {
    int ret;
    
    if(aabbnum > 1) {
        // make contains AABB
        AABB centroAABB;
        node.aabb.clear();
        for(int i = 0; i < aabbnum; i++) {
            const AABB &ab = aabbArray[i];
            node.aabb.expand(ab);
            centroAABB.expand(ab);
        }
        
        // detect divide axis
        Vector3 centroSize = centroAABB.getSize();
        int chooseaxis = (centroSize.x_ > centroSize.y_)? 0 : 1;
        if(centroSize.z_ > centroSize.v[chooseaxis]) {
            chooseaxis = 2;
        }
        
        // TODO? direct sorting AABB structure array
        // sort AABB array
        AABBAxisComparator comparator(chooseaxis);
        std::sort(aabbArray, aabbArray + aabbnum, comparator);
        /*
		 //+++++
		 std::cout << "BVHNode(" << aabbnum << " tri) divided[" << divaxis << "]:";
		 //std::cout << "(" << node.aabb.min.x_ <<  "," << node.aabb.min.y_ <<  "," << node.aabb.min.z_ << ")";
		 //std::cout << ",(" << node.aabb.max.x_ <<  "," << node.aabb.max.y_ <<  "," << node.aabb.max.z_ << ")";
		 Vec aabbsize = node.aabb.getSize();
		 std::cout << "size:(" << aabbsize.x_ <<  "," << aabbsize.y_ <<  "," << aabbsize.z_ << ")";
		 std::cout << "v:" << (aabbsize.x_ * aabbsize.y_ * aabbsize.z_);
		 std::cout << std::endl;
		 //+++++
		 */
        node.allocChildren(2);
        int halfnum = aabbnum / 2;
        int d1 = recurseBuildTree(node.children[0], aabbArray, halfnum, depth+1);
        int d2 = recurseBuildTree(node.children[1], aabbArray + halfnum, aabbnum - halfnum, depth+1);
        
        ret = (d1 > d2)? d1:d2;
    } else {
        // contain one triangle.
        const AABB &curaabb = aabbArray[0];
        node.dataId = curaabb.dataId;
        node.aabb = curaabb;
        
        ret = depth;
        /*
		 //+++++
		 std::cout << "tri(d:" << depth << ")[f:" << curaabb.dataId << "]:";
		 //std::cout << "(" << node.aabb.min.x_ <<  "," << node.aabb.min.y_ <<  "," << node.aabb.min.z_ << ")";
		 //std::cout << ",(" << node.aabb.max.x_ <<  "," << node.aabb.max.y_ <<  "," << node.aabb.max.z_ << ")";
		 Vec aabbsize = node.aabb.getSize();
		 std::cout << "size:(" << aabbsize.x_ <<  "," << aabbsize.y_ <<  "," << aabbsize.z_ << ")";
		 std::cout << "v:" << (aabbsize.x_ * aabbsize.y_ * aabbsize.z_);
		 std::cout << std::endl;
		 //+++++
		 */
    }
    return ret;
}

///
/*
BVHNode::TraverseResult BVHNode::isIntersect(Ray &ray, BVHNode &node, TraverseInfo *trvinfo, LeafNodeCallback leafcallback) {
	if(node.isLeaf()) {
		// lead
	} else {
		double d;
        if(node.aabb.isIntersect(ray, &d)) {
			// The Ray intersects AABB
            if(d < trvinfo->distance) {
				// closer!
                TraverseInfo closer_info, tmp_info;
                for(int i = 0; i < node.childNum; i++) {
                    // check children
					TraverseResult trvresult = isIntersect(ray, node.children[i], &tmp_info, leafcallback);
                    if(trvresult.intersected) {
                        if(tmp_info.distance < closer_info.distance) {
                            closer_info = tmp_info;
                        }
                    }
                }
                // if closer than already calc point.
                if(closer_info.distance < trvinfo->distance) {
                    *trvinfo = closer_info;
                    return trvresult;
                }
            }
        }
	}
	return TraverseResult();
}
*/
///
BVHNode::AABBAxisComparator::AABBAxisComparator(const int a): axisId(a)
{}

bool BVHNode::AABBAxisComparator::operator() (const AABB &aabb1, const AABB &aabb2) {
	return aabb1.centroid.v[axisId] < aabb2.centroid.v[axisId];
}
