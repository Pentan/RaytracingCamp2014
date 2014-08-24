
#include "camera.h"
#include <cmath>

using namespace r1h;

Camera::Camera():
    position(0.0, 0.0, 0.0),
    direction(0.0, 0.0, -1.0),
    up(0.0, 1.0, 0.0),
    side(1.0, 0.0, 0.0),
    aspect(1.778),
    screenLeft(1.0)
{}

Camera::~Camera() {}

void Camera::setLookat(const Vector3 &eye, const Vector3 &look, const Vector3 &nup) {
    position = eye;
    direction = Vector3::normalized(look - eye);
    up = nup;
    side = Vector3::normalized(Vector3::cross(direction, up));
    up = Vector3::normalized(Vector3::cross(direction, side));
}

void Camera::setAspectRatio(const R1hFPType asp) {
    aspect = asp;
}
void Camera::setFocal(const R1hFPType focalmm, const R1hFPType sensorwidth) {
    screenLeft = sensorwidth / focalmm;
}
void Camera::setFieldOfView(const R1hFPType vdegree) {
    screenLeft = tan(vdegree * M_PI / 180.0 * 0.5) * 2.0;
}

Ray Camera::getRay(const double tx, const double ty) const {
    Vector3 left = side * (screenLeft * tx * 0.5);
    Vector3 top = up * (screenLeft / aspect * ty * 0.5);
    return Ray(position, Vector3::normalized(direction + left + top));
}
