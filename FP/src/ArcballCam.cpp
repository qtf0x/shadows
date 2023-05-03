/**
 * @file ArcballCam.cpp
 * @author Vincent Marias [@qtf0x]
 * @date 04/02/2023
 *
 * @brief CSCI 544 ~ Advanced Computer Graphics [Spring 2023]
 *        A2 ~ Noisy Teapotahedron
 */

#include <glm/common.hpp>               // for clamp
#include <glm/gtc/matrix_transform.hpp> // for lookat
#include <glm/trigonometric.hpp>        // for sin, cos, etc.

#include "ArcballCam.hpp"

// *****************************************************************************
// Public

void ArcballCam::rotate(GLfloat dTheta, GLfloat dPhi) {
    _theta += dTheta; // update theta
    _phi += dPhi;     // update phi

    _clampPhi();            // bounds check phi
    recomputeOrientation(); // convert to cartesian
}

void ArcballCam::zoom(GLfloat zoomFactor) {
    _radius += zoomFactor; // update radius

    _clampRadius();         // bounds check radius
    recomputeOrientation(); // convert to cartesian
}

mat4 ArcballCam::getViewMatrix() { return _viewMatrix; }

vec3 ArcballCam::getPosition() { return _position; }

void ArcballCam::setLookAtPoint(const vec3& lookAtPoint) {
    _lookAtPoint = lookAtPoint;

    recomputeOrientation();
}

void ArcballCam::setUpVector(const vec3& upVector) {
    _upVector = upVector;

    recomputeOrientation();
}

// *****************************************************************************
// Private

void ArcballCam::_clampPhi() {
    _phi = glm::clamp(_phi, 0.001f, pi<GLfloat>() - 0.001f);
}

void ArcballCam::_clampRadius() {
    _radius = glm::clamp(_radius, MIN_ZOOM, MAX_ZOOM);
}

void ArcballCam::recomputeOrientation() {
    _position = vec3(_radius * glm::sin(_theta) * glm::sin(_phi),
                     -_radius * glm::cos(_phi),
                     -_radius * glm::cos(_theta) * glm::sin(_phi)) +
                _lookAtPoint;

    _viewMatrix = glm::lookAt(_position, _lookAtPoint, _upVector);
}
