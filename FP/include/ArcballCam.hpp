/**
 * @file ArcballCam.hpp
 * @author Vincent Marias [@qtf0x]
 * @date 04/02/2023
 *
 * @brief CSCI 544 ~ Advanced Computer Graphics [Spring 2023]
 *        A2 ~ Noisy Teapotahedron
 */

#ifndef TEAPOTAHEDRON_ARCBALL_CAM_HPP
#define TEAPOTAHEDRON_ARCBALL_CAM_HPP

#include <glad/glad.h> // for GL types

#include <glm/gtc/constants.hpp> // for pi
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

using glm::mat4;
using glm::pi;
using glm::vec3;

class ArcballCam {
  public:
    ArcballCam(const GLfloat& theta = 0.f,
               const GLfloat& phi = pi<GLfloat>() * 0.75f,
               const GLfloat& radius = 5.f, const vec3& lookAtPoint = vec3(0.f))
        : _theta{theta}, _phi{phi}, _radius{radius}, _position{0.f},
          _lookAtPoint{lookAtPoint}, _upVector{0.f, 1.f, 0.f}, _viewMatrix{
                                                                   1.f} {}

    /**
     * @brief rotates the camera's POV by adding to theta & phi then ensuring
     * phi stays within [0, pi] range and finally calls through to
     * recomputeOrientation() to update the view parameters after the rotation
     *
     * @param dTheta change in rotation of theta represented in radians
     * @param dPhi change in rotation of phi represented in radians
     */
    void rotate(GLfloat dTheta, GLfloat dPhi);

    /**
     * @brief adjust the distance of the camera from the lookat point
     *
     * @param zoomFactor amount to change camera radius by (positive or
     * negative)
     */
    void zoom(GLfloat zoomFactor);

    // *************************************************************************
    // Getters + Setters

    mat4 getViewMatrix();

    vec3 getPosition();

    void setLookAtPoint(const vec3& lookAtPoint);

    void setUpVector(const vec3& upVector);

  private:
    // TODO: make min/max dynamic, engine can set them as necessary
    // sphere is radius 1, so keep us outside of it
    static constexpr GLfloat MIN_ZOOM{0.01f}, MAX_ZOOM{50.f};

    GLfloat _theta;  // spherical angle for yaw direction in radians
    GLfloat _phi;    // spherical angle for pitch direction in radians
    GLfloat _radius; // spherical magnitude for direction in world space

    vec3 _position;    // cartesian position in world space of the camera
    vec3 _lookAtPoint; // world space point in front of the camera
    vec3 _upVector;    // up vector of the camera specified in world space

    mat4 _viewMatrix; // view matrix (inverse of camera matrix)

    /**
     * @brief keeps phi within the range (0, pi) to prevent the camera from
     * flipping over
     */
    void _clampPhi();

    /**
     * @brief don't let camera get too close or too far away
     */
    void _clampRadius();

    /**
     * @brief uses theta, phi, & radius to update camera's view matrix
     * parameters (camera orientation controlled via spherical coordinates, this
     * method orients/positions cameria in cartesian coordinates)
     */
    void recomputeOrientation();
};

#endif // TEAPOTAHEDRON_ARCBALL_CAM_HPP
