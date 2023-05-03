/**
 * @file Engine.hpp
 * @author Vincent Marias [@qtf0x]
 * @date 04/02/2023
 *
 * @brief CSCI 544 ~ Advanced Computer Graphics [Spring 2023]
 *        A2 ~ Noisy Teapotahedron
 */

#ifndef TEAPOTAHEDRON_ENGINE_HPP
#define TEAPOTAHEDRON_ENGINE_HPP

#include <string>

#include <glad/glad.h>
// include glad before glfw
#include <GLFW/glfw3.h>

#include <glm/gtc/constants.hpp> // for pi
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/trigonometric.hpp> // for sin, cos, etc.
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

using glm::mat3;
using glm::mat4;
using glm::vec2;
using glm::vec3;
using glm::vec4;

#include "ArcballCam.hpp"
#include "ShaderProgram.hpp"

class Engine {
  public:
    // *************************************************************************
    // Engine Interface

    Engine(const std::string& windowTitle = "FP - Shadows [ 0.000 FPS ]",
           const GLuint& windowWidth = 1920, const GLuint& windowHeight = 1080)
        : _isInitialized{GL_FALSE}, _isShutDown{GL_FALSE},
          _windowTitle{windowTitle}, _windowWidth{windowWidth},
          _windowHeight{windowHeight}, _mousePosition{-1.f, -1.f},
          _leftMouseButtonState{GLFW_RELEASE}, _keys{GLFW_FALSE}, _nFrames{0},
          _fps{0.f} {}

    ~Engine();

    /**
     * @brief set up everything necessary to start rendering
     */
    void initialize();

    /**
     * @brief render the scene to the open GLFW window until it closes
     */
    void run();

    /**
     * @brief clean up memory and shut down all external processes
     */
    void shutdown();

    // *************************************************************************
    // Event Handlers

    /**
     * @brief handle any key events inside the engine
     *
     * @param key key as represented by GLFW_KEY_ macros
     * @param action key event action as represented by GLFW_ macros
     */
    void handleKeyEvent(GLint key, GLint action);

    /**
     * @brief handle any mouse button events inside the engine
     *
     * @param button mouse button as represented by GLFW_MOUSE_BUTTON_ macros
     * @param action mouse event as reprsented by GLFW_ macros
     */
    void handleMouseButtonEvent(GLint button, GLint action);

    /**
     * @brief handle any cursor movement events inside the engine
     *
     * @param currMousePosition current cursor position
     */
    void handleCursorPositionEvent(vec2 currMousePosition);

    /**
     * @brief handle any scroll events inside the engine
     *
     * @param offset current scroll offset
     */
    void handleScrollEvent(vec2 offset);

  private:
    // *************************************************************************
    // Shadow Properties

    enum SHADOW_TYPE { NONE, PLANAR, TEXTURES, MAPS, VOLUMES, PCSS };
    enum SHADOW_OPTIONS {
        PLANAR_DEPTH_TEST = 2,
        PLANAR_BLEND = 4,
        PLANAR_STENCIL_TEST = 8,
        LINEAR_TEXTURE_FILTER = 16,
        MAPS_CULL_FRONT_FACE = 32
    };

    int _shadow_options = PLANAR_DEPTH_TEST;
    SHADOW_TYPE _which_shadows = NONE;

    GLuint SHADOW_TEXTURE_RESOLUTION{512};
    GLfloat _shadowBias{0.f}, _shadowMapSamples{2.f};
    GLint _doMultisampling{0};

    // handles for cubemap texture and framebuffer object
    GLuint _shadowCubeMap, _shadowCubeMapFBO;
    GLuint _depthCubeMap, _depthCubeMapFBO;

    bool _options(int bits) { return (_shadow_options & bits) == bits; }

    void _turn_on(int bits) { _shadow_options |= bits; }

    void _turn_off(int bits) { _shadow_options &= ~bits; }

    void _renderShadowTextures();

    void _renderShadowMaps();

    GLboolean _isInitialized, _isShutDown; // engine tracks it's own status

    GLboolean _spinObjects{GL_TRUE}; // are the objects in the scene spinning?
    GLboolean _moveLight{GL_TRUE};   // is the light moving up and down?
    GLboolean _outerRing{GL_FALSE};  // draw outer ring of spheres?
    GLint _wireframe{0};             // is the wireframe renderer on?
    GLint _controlPoints{0};         // are the control points visible?
    GLfloat _tessLevel{64.f};        // inner/outer tessellation level

    // *************************************************************************
    // Window Info (GLFW)

    // OpenGL context version (core profile)
    static constexpr GLuint MAJOR_VERSION{4u}, MINOR_VERSION{6u};

    GLFWwindow* _window;      // the GLFW window itself
    std::string _windowTitle; // the current title being displayed on window
    GLuint _windowWidth, _windowHeight; // GLFW window dimensions

    // *************************************************************************
    // Engine Setup

    void _setupGLFW();

    void _setupGLAD();

    void _setupOpenGL();

    void _setupShaders();

    void _setupBuffers();

    void _setupTextures();

    void _setupScene();

    void _printSetupInfo();

    // *************************************************************************
    // Engine Cleanup

    void _cleanupGLFW();

    void _cleanupShaders();

    void _cleanupBuffers();

    void _cleanupScene();

    // *************************************************************************
    // Engine Rendering & Updating

    GLint _nFrames;     // frame counter for FPS
    GLdouble _lastTime; // timer for FPS
    GLdouble _fps;      // current fps

    void _renderScene(const mat4& viewMatrix, const mat4& projectionMatrix,
                      const mat4& viewportMatrix);

    void _updateScene();

    void _drawPlatform();

    void _drawTeapot();

    void _drawSphere();

    // *************************************************************************
    // Input Tracking (Keyboard & Mouse)

    // tracks number of different keys that can be present as determined by GLFW
    static constexpr GLuint NUM_KEYS{GLFW_KEY_LAST};

    // boolean array tracking each key state. if true, then the key is in a
    // pressed or held down state. if false, then the key is in a released state
    // and not being interacted with
    GLboolean _keys[NUM_KEYS];

    vec2 _mousePosition; // last location of the mouse in window coordinates
    GLint _leftMouseButtonState; // current state of the left mouse button

    // *************************************************************************
    // Camera Information

    ArcballCam* _arcballCam{nullptr}; // the arcball camera in our world

    static constexpr vec3 EYE_CENTER{0.f, 2.5f, 0.f};

    // just above and to the side of origin
    vec4 light_position{0.f, 5.f, 0.f, 1.f};
    GLfloat _light_offset{-0.01f};

    // *************************************************************************
    // VAO & Object Information

    GLfloat _angle_offset{0.f};

    // total number of VAOs in our scene
    static constexpr GLsizei NUM_VAOS{3};

    // used to index through our VAO/VBO/IBO array to give named access
    enum VAO_ID {
        PLATFORM, // the platform/ground for everything to appear on
        SPHERE,   // the icoshpere at the center of our scene
        TEAPOT    // the tessllated Utah Teapot
    };

    GLuint _vaos[NUM_VAOS];          // VAO handles
    GLuint _vbos[NUM_VAOS];          // VBO handles
    GLuint _ibos[NUM_VAOS];          // IBO handles
    GLsizei _numVAOPoints[NUM_VAOS]; // number of points that make up our VAO

    /**
     * @brief creates the platform object
     *
     * @param [in] vao VAO handle to bind
     * @param [in] vbo VBO handle to bind
     * @param [in] ibo IBO handle to bind
     * @param [out] numVAOPoints number of vertices that make up index buffer
     */
    void _createPlatform(const GLuint& vao, const GLuint& vbo,
                         const GLuint& ibo, GLsizei& numVAOPoints);

    void _createTeapot(const GLuint& vao, const GLuint& vbo, const GLuint& ibo,
                       GLsizei& numVAOPoints);

    // number of subdivisions to use when generating the icosphere (how smooth)
    static constexpr GLuint SPHERE_SUBDIVISIONS{5u};

    /**
     * @brief creates the icoshpere object
     *
     * @param [in] vao VAO handle to bind
     * @param [in] vbo VBO handle to bind
     * @param [in] ibo IBO handle to bind
     * @param [out] numVAOPoints sets number of vertices that make up IBO buffer
     */
    void _createSphere(const GLuint& vao, const GLuint& vbo, const GLuint& ibo,
                       GLsizei& numVAOPoints);

    void _buildIcosphere(std::vector<GLfloat>& vertices,
                         std::vector<GLushort>& indices);

    /**
     * @brief generate the 12 vertices for an icosahedron; each face can then be
     * subdivided many times to approximate a sphere
     *
     * @param [out] vertices 12 vertices of an icosahedron of radius 1
     * @param [out] indices naive index list, no optimization
     */
    std::vector<GLfloat> _generateIcosahedron();

    void _subdivideIcosahedron(std::vector<GLfloat>& vertices,
                               std::vector<GLushort>& indices);

    void _computeHalfVertex(const GLfloat v1[3ul], const GLfloat v2[3ul],
                            GLfloat newV[3ul]);

    void _addVertices(const GLfloat v1[3], const GLfloat v2[3],
                      const GLfloat v3[3], std::vector<GLfloat>& vertices);

    void _addIndices(const GLushort& i1, const GLushort& i2, const GLushort& i3,
                     std::vector<GLushort>& indices);

    //**************************************************************************
    // Shader Program Information

    ShaderProgram *_wireTesShader{nullptr}, *_wireShader{nullptr},
        *_flatShader{nullptr}, *_flatLightShader{nullptr},
        *_teapotPlanarShadowShader{nullptr},
        *_spherePlanarShadowShader{nullptr},
        *_shadowTextureCubemapShader{nullptr},
        *_shadowTextureCubemapTesShader{nullptr},
        *_shadowTextureShader{nullptr}, *_shadowMapShader{nullptr},
        *_shadowMapTesShader{nullptr}, *_depthCubemapShader{nullptr},
        *_depthCubemapTesShader{nullptr};

    // total number of UBOs in our scene
    static constexpr GLsizei NUM_UBOS{3};

    // used to index through our UBO array to give named access
    enum UBO_ID {
        SCENE,   // uniform matrix info
        LIGHT,   // uniform light info
        MATERIAL // uniform material properties
    };

    GLuint _ubos[NUM_UBOS];                       // UBO handles
    GLint _blockSizes[NUM_UBOS];                  // UBO block sizes
    std::vector<GLint> _uniformOffsets[NUM_UBOS]; // UBO uniform offsets

    void _sendSceneBlock(const mat4& model, const mat4& viewProjection,
                         const mat4& modelViewProjection,
                         const mat4& viewportMatrix,
                         const mat4& shadowViewProjection, const vec3& eyePos);

    void _sendLightBlock(const vec4& lightPos, const vec3& lightAmb,
                         const vec3& lightDiff, const vec3& lightSpec,
                         const GLfloat& attenConst, const GLfloat& attenLin,
                         const GLfloat& attenQuad);

    void _sendMaterialBlock(const vec3& materialAmb, const vec3& materialDiff,
                            const vec3& materialSpec, const GLfloat& shininess);
};

static vec3 circlePos(GLfloat radius, GLfloat angle, GLfloat height) {
    return vec3(radius * glm::cos(angle), height, radius * glm::sin(angle));
}

// *****************************************************************************
// GLFW callback function overloads

void keyboard_callback(GLFWwindow* window, GLint key, GLint scancode,
                       GLint action, GLint mods);

void cursor_callback(GLFWwindow* window, GLdouble x, GLdouble y);

void mouse_button_callback(GLFWwindow* window, GLint button, GLint action,
                           GLint mods);

void scroll_callback(GLFWwindow* window, GLdouble xOffset, GLdouble yOffset);

void error_callback(GLint error, const GLchar* description);

#endif // TEAPOTAHEDRON_ENGINE_HPP
