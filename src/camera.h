#ifndef R1H_CAMERA_H
#define R1H_CAMERA_H

#include <memory>
#include "r1htypes.h"
#include "ray.h"
#include "sceneobject.h"

namespace r1h {

class Camera : public SceneObject {
public:
    Camera();
    ~Camera();
    
    void setLookat(const Vector3 &eye, const Vector3 &look, const Vector3 &nup);
    void setAspectRatio(const R1hFPType asp);
    void setFocal(const R1hFPType focalmm, const R1hFPType sensorwidth=36.0);
    void setFieldOfView(const R1hFPType vdegree);
    
    /// expects(-1,1)
    Ray getRay(const double tx, const double ty) const;
    
private:
    Vector3 position;
    Vector3 direction;
    Vector3 up;
    Vector3 side;
    
    R1hFPType aspect;
    R1hFPType screenLeft;
};

typedef std::shared_ptr<Camera> CameraRef;
	
}
#endif