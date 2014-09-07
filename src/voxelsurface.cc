#include "voxelsurface.h"

using namespace r1h;

/// Voxel
void Voxel::init(R1hFPType x, R1hFPType y, R1hFPType z, R1hFPType val) {
    p.set(x, y, z);
    value = val;
    edges[0] = nullptr;
    edges[1] = nullptr;
    edges[2] = nullptr;
}

/// VoxelEdge
void VoxelEdge::init(Voxel *vxl0, Voxel *vxl1) {
    v0 = vxl0;
    v1 = vxl1;
    cells[0] = nullptr;
    cells[1] = nullptr;
    cells[2] = nullptr;
    cells[3] = nullptr;
    intersect.flag = 0;
    intersect.t = 0.0;
}

int VoxelEdge::checkIntersect(R1hFPType thre) {
    if(v0->value < thre && v1->value >= thre) {
        intersect.flag = -1;
    }
    else if(v0->value >= thre && v1->value < thre) {
        intersect.flag = 1;
    }
    else {
        intersect.flag = 0;
    }
    
    if(intersect.flag != 0) {
        double t1 = (thre - v0->value) / (v1->value - v0->value);
        double t0 = 1.0 - t1;
        intersect.t = t0;
        intersect.p = (v0->p * t0) + (v1->p * t1);
    }
    
    return intersect.flag;
};

/// VoxelCell
void VoxelCell::init(Voxel *v000, Voxel *v100, Voxel *v010, Voxel *v110, Voxel *v001, Voxel *v101, Voxel *v011, Voxel *v111) {
    vertices[0] = v000;
    vertices[1] = v100;
    vertices[2] = v010;
    vertices[3] = v110;
    vertices[4] = v001;
    vertices[5] = v101;
    vertices[6] = v011;
    vertices[7] = v111;
    edges.reserve(4);
    updateInnerPoint();
}

void VoxelCell::updateInnerPoint() {
    ip.set(0.0, 0.0, 0.0);
    for(int i = 0; i < 8; i++) {
        ip += vertices[i]->p;
    }
    ip = ip / 8.0;
}

Vector3 VoxelCell::calcSurfacePoint(R1hFPType thre) {
    ip.set(0.0, 0.0, 0.0);
    size_t edgesnum = edges.size();
    
    int c = 0;
    Vector3 ep;
    for(size_t i = 0; i < edgesnum; i++) {
        VoxelEdge *tmpe = edges[i];
        if(tmpe->checkIntersect(thre) != 0) {
            ip += tmpe->intersect.p;
            ++c;
        }
    }
    if(c > 0) {
        ip = ip / c;
    }
    
    return ip;
}

/// VoxelGrid
VoxelGrid::VoxelGrid(int sx, int sy, int sz, R1hFPType uni) {
    unit = uni;
    
    size.x = sx;
    size.y = sy;
    size.z = sz;
    size.xy = sx * sy;
    size.cellx = sx - 1;
    size.celly = sy - 1;
    size.cellz = sz - 1;
    size.cellxy = (sx - 1) * (sy - 1);
    
    // create vertices
    voxels.resize(sx * sy * sz);
    for(int iz = 0; iz < sz; iz++) {
        for(int iy = 0; iy < sy; iy++) {
            for(int ix = 0; ix < sx; ix++) {
                int i = getVoxelIndex(ix, iy, iz);
                voxels[i].init(ix * unit, iy * unit, iz * unit, 0.0);
            }
        }
    }
    
    // create cells
    cells.resize((sx - 1) * (sy - 1) * (sz - 1));
    for(int iz = 0; iz < sz; iz++) {
        for(int iy = 0; iy < sy; iy++) {
            for(int ix = 0; ix < sx; ix++) {
                int cellid = getCellIndex(ix, iy, iz);
                cells[cellid].init(
                    getVoxel(ix    , iy    , iz    ),
                    getVoxel(ix + 1, iy    , iz    ),
                    getVoxel(ix    , iy + 1, iz    ),
                    getVoxel(ix + 1, iy + 1, iz    ),
                    getVoxel(ix    , iy    , iz + 1),
                    getVoxel(ix + 1, iy    , iz + 1),
                    getVoxel(ix    , iy + 1, iz + 1),
                    getVoxel(ix + 1, iy + 1, iz + 1)
                );
            }
        }
    }
    
    // create edges
    edges.resize(sx * sy * sz * 3 - (sx * sy + sy * sz + sz * sx));
    int iex = 0;
    int iey = (sx - 1) * sy * sz;
    int iez = iey + sx * (sy - 1) * sz;
    for(int iz = 0; iz < sz; iz++) {
        for(int iy = 0; iy < sy; iy++) {
            for(int ix = 0; ix < sx; ix++) {
                Voxel *v000 = getVoxel(ix, iy, iz);
                Voxel *v100 = getVoxel(ix + 1, iy, iz);
                Voxel *v010 = getVoxel(ix, iy + 1, iz);
                Voxel *v001 = getVoxel(ix, iy, iz + 1);
                // cells are CCW
                if(v100) {
                    VoxelEdge *tmpedge = &edges[iex];
                    iex++;
                    tmpedge->init(v000, v100);
                    tmpedge->cells[0] = getCell(ix, iy    , iz    );
                    tmpedge->cells[1] = getCell(ix, iy - 1, iz    );
                    tmpedge->cells[2] = getCell(ix, iy - 1, iz - 1);
                    tmpedge->cells[3] = getCell(ix, iy    , iz - 1);
                    for(int icell = 0; icell < 4; icell++) {
                        VoxelCell *c = tmpedge->cells[icell];
                        if(c != nullptr) {
                            c->edges.push_back(tmpedge);
                        }
                    }
                    v000->edges[0] = tmpedge;
                }
                if(v010) {
                    VoxelEdge *tmpedge = &edges[iey];
                    iey++;
                    tmpedge->init(v000, v010);
                    tmpedge->cells[0] = getCell(ix    , iy, iz    );
                    tmpedge->cells[1] = getCell(ix    , iy, iz - 1);
                    tmpedge->cells[2] = getCell(ix - 1, iy, iz    );
                    tmpedge->cells[3] = getCell(ix - 1, iy, iz - 1);
                    for(int icell = 0; icell < 4; icell++) {
                        VoxelCell *c = tmpedge->cells[icell];
                        if(c != nullptr) {
                            c->edges.push_back(tmpedge);
                        }
                    }
                    v000->edges[1] = tmpedge;
                }
                if(v001) {
                    VoxelEdge *tmpedge = &edges[iez];
                    iez++;
                    tmpedge->init(v000, v001);
                    tmpedge->cells[0] = getCell(ix    , iy    , iz);
                    tmpedge->cells[1] = getCell(ix - 1, iy    , iz);
                    tmpedge->cells[2] = getCell(ix - 1, iy - 1, iz);
                    tmpedge->cells[3] = getCell(ix    , iy - 1, iz);
                    for(int icell = 0; icell < 4; icell++) {
                        VoxelCell *c = tmpedge->cells[icell];
                        if(c != nullptr) {
                            c->edges.push_back(tmpedge);
                        }
                    }
                    v000->edges[2] = tmpedge;
                }
            }
        }
    }
    threshould = 0.5;
}

int VoxelGrid::getVoxelIndex(int x, int y, int z) {
    if(x < 0 || y < 0 || z < 0 || x >= size.x || y >= size.y || z >= size.z) {
        return -1;
    }
    return x + size.x * y + size.xy * z;
}
Voxel* VoxelGrid::getVoxel(int x, int y, int z) {
    int i = getVoxelIndex(x, y, z);
    return (i < 0)? nullptr : &voxels[i];
}

int VoxelGrid::getCellIndex(int x, int y, int z) {
    if(x < 0 || y < 0 || z < 0 || x >= size.cellx || y >= size.celly || z >= size.cellz) {
        return -1;
    }
    return x + size.cellx * y + size.cellxy * z;
}
VoxelCell* VoxelGrid::getCell(int x, int y, int z) {
    int i = getCellIndex(x, y, z);
    return (i < 0)? nullptr : &cells[i];
};

SphereBlob::SphereBlob(R1hFPType cx, R1hFPType cy, R1hFPType cz, R1hFPType hr):
    position(cx, cy, cz), halfRadius(hr), scale(1.0)
{
}

R1hFPType SphereBlob::force(R1hFPType cx, R1hFPType cy, R1hFPType cz) {
    Vector3 d = (Vector3(cx, cy, cz) - position) * scale;
    R1hFPType r2 = Vector3::dot(d, d);
    return 1.0 / (1.0 + r2);
}







