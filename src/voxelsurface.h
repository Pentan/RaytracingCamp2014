#ifndef R1H_VOXELSURFACE_H
#define R1H_VOXELSURFACE_H

#include <vector>
#include "r1htypes.h"

namespace r1h {

class VoxelEdge;
class VoxelCell;

class Voxel {
public:
    Vector3 p;
    R1hFPType value;
    VoxelEdge *edges[3];
    
    void init(R1hFPType x, R1hFPType y, R1hFPType z, R1hFPType val);
};

class VoxelEdge {
public:
    Voxel *v0;
    Voxel *v1;
    VoxelCell *cells[4];
    struct {
        int flag;
        R1hFPType t;
        Vector3 p;
    } intersect;
    
    void init(Voxel *vxl0, Voxel *vxl1);
    int checkIntersect(R1hFPType thre);
};

class VoxelCell {
public:
    Voxel *vertices[8];
    Vector3 ip;
    std::vector<VoxelEdge*> edges;
    
    void init(Voxel *v000, Voxel *v100, Voxel *v010, Voxel *v110, Voxel *v001, Voxel *v101, Voxel *v011, Voxel *v111);
    void updateInnerPoint();
    Vector3 calcSurfacePoint(R1hFPType thre);
};

class VoxelGrid {
public:
    struct {
        int x, y, z, xy;
        int cellx, celly, cellz, cellxy;
    } size;
    
    std::vector<Voxel> voxels;
    std::vector<VoxelCell> cells;
    std::vector<VoxelEdge> edges;
    
	R1hFPType unit;
    R1hFPType threshould;
    
	VoxelGrid(int sx, int sy, int sz, R1hFPType uni);
    
    Voxel* voxelAt(int x, int y, int z); //+++++
    int getVoxelIndex(int x, int y, int z);
    Voxel* getVoxel(int x, int y, int z);
    
    int getCellIndex(int x, int y, int z);
    VoxelCell* getCell(int x, int y, int z);
};

class SphereBlob {
public:
    Vector3 position;
    R1hFPType halfRadius;
    R1hFPType scale;
    
    SphereBlob(R1hFPType cx, R1hFPType cy, R1hFPType cz, R1hFPType hr);
    R1hFPType force(R1hFPType cx, R1hFPType cy, R1hFPType cz);
};


}

#endif
