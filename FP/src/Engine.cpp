/**
 * @file Engine.cpp
 * @author Vincent Marias [@qtf0x]
 * @date 04/02/2023
 *
 * @brief CSCI 544 ~ Advanced Computer Graphics [Spring 2023]
 *        A2 ~ Noisy Teapotahedron
 */

#include <cstdio>
#include <cstdlib>
#include <cstring>  // for memcpy
#include <iomanip>  // for fixed, precision
#include <iostream> // for cout
#include <sstream>  // for stringstream

#include <glm/gtc/matrix_transform.hpp> // for scale, translate
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp> // for value_ptr

#include "TeapotData.hpp"

#include "Engine.hpp"

#define st (size_t)
static constexpr GLfloat PI = glm::pi<GLfloat>();

/* https://stackoverflow.com/a/18067245/10323091 */
void ETB_GL_ERROR_CALLBACK(GLenum source, GLenum type, GLuint id,
                           GLenum severity, GLsizei length,
                           const GLchar* message, GLvoid* userParam);

// *****************************************************************************
// Engine Interface

Engine::~Engine() { shutdown(); }

void Engine::initialize() {
    if (_isInitialized)
        return;

    std::cout << "Initializing OpenGL engine ...\n";

    _setupGLFW();
    _setupGLAD();
    _setupOpenGL();

    _printSetupInfo();

    _setupBuffers();
    _setupShaders();
    _setupScene();
    _setupTextures();

    _isInitialized = GL_TRUE;
}

void Engine::run() {
    std::cout << "Rendering scene ...\n";

    // this is the main draw loop
    while (!glfwWindowShouldClose(_window)) {
        // first pass: render shadow textures to cubemap
        if (_which_shadows == TEXTURES)
            _renderShadowTextures();
        if (_which_shadows == MAPS)
            _renderShadowMaps();

        glDrawBuffer(GL_BACK); // work with our back frame buffer
        // clear the current color contents and depth buffer in the window
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
                GL_STENCIL_BUFFER_BIT);

        /* Get the size of our framebuffer. Ideally this should be the same
        dimensions as our window, but when using a Retina display the actual
        window can be larger than the requested window. Therefore, query what
        the actual size of the window we are rendering to is. */
        GLint framebufferWidth, framebufferHeight;
        glfwGetFramebufferSize(_window, &framebufferWidth, &framebufferHeight);

        // define Z range
        GLfloat minZ{0.001f}, maxZ{1000.f};

        // update viewport - tell OpenGL we want to render to the whole window
        glViewport(0, 0, framebufferWidth, framebufferHeight);

        /*https://www3.ntu.edu.sg/home/ehchua/programming/opengl/CG_BasicsTheory.html*/
        // manually define viewport transform
        GLfloat w2 = (GLfloat)framebufferWidth / 2.f;
        GLfloat h2 = (GLfloat)framebufferHeight / 2.f;

        mat4 viewportMatrix{{w2, 0.f, 0.f, 0.f},
                            {0.f, -h2, 0.f, 0.f},
                            {0.f, 0.f, maxZ - minZ, 0.f},
                            {w2, h2, minZ, 1.f}};

        // set up our look at matrix to position our camera
        mat4 viewMatrix{_arcballCam->getViewMatrix()};

        /* set the projection matrix based on the window size
        use a perspective projection that ranges
        with a FOV of 45 degrees, for our current aspect ratio, and Z ranges
        from [0.001, 1000]. */
        mat4 projectionMatrix{glm::perspective(
            45.f, (GLfloat)framebufferWidth / (GLfloat)framebufferHeight, minZ,
            maxZ)};

        // second pass: draw everything to the window
        _renderScene(viewMatrix, projectionMatrix, viewportMatrix);

        _updateScene();

        // flush the OpenGL commands and make sure they get rendered!
        glfwSwapBuffers(_window);
        glfwPollEvents(); // check for any events and signal to redraw screen
    }
}

void Engine::shutdown() {
    if (_isShutDown)
        return;

    std::cout << "Shutting down ...\n";

    _cleanupBuffers();
    _cleanupShaders();
    _cleanupScene();

    _cleanupGLFW();

    _isShutDown = GL_TRUE;
}

// *****************************************************************************
// Event Handlers

void Engine::handleKeyEvent(GLint key, GLint action) {
    // add to active key cache
    if (key != GLFW_KEY_UNKNOWN)
        _keys[key] = ((action == GLFW_PRESS) || (action == GLFW_REPEAT));

    if (action == GLFW_PRESS) {
        switch (key) {
        // quit!
        case GLFW_KEY_Q:
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(_window, GLFW_TRUE);
            break;

        // toggle wireframe renderer
        case GLFW_KEY_W:
            _wireframe = _wireframe ? 0 : 1;
            break;

        // toggle teapot control points
        // case GLFW_KEY_C:
        //     _controlPoints = _controlPoints ? 0 : 1;
        //     break;

        // update tessellation levels
        case GLFW_KEY_UP:
            ++_tessLevel;
            break;
        case GLFW_KEY_DOWN:
            if (_tessLevel > 0.f)
                --_tessLevel;
            break;

        // adjust spinning objects
        case GLFW_KEY_LEFT:
            if (!_spinObjects) {
                _angle_offset += 0.1f;
                if (_angle_offset >= 2.f * PI)
                    _angle_offset = 0.f;
            }
            break;
        case GLFW_KEY_RIGHT:
            if (!_spinObjects) {
                _angle_offset -= 0.1f;
                if (_angle_offset <= 0.f)
                    _angle_offset = 2.f * PI;
            }
            break;

        // adjust light position
        case GLFW_KEY_B:
            if (!_moveLight && light_position.y > 1.25f) {
                if (_light_offset > 0)
                    light_position.y -= 20.f * _light_offset;
                else
                    light_position.y += 20.f * _light_offset;
            }
            break;
        case GLFW_KEY_N:
            if (!_moveLight && light_position.y < 8.75f) {
                if (_light_offset > 0)
                    light_position.y += 20.f * _light_offset;
                else
                    light_position.y -= 20.f * _light_offset;
            }
            break;

        // toggle outer ring of spheres
        case GLFW_KEY_O:
            _outerRing = !_outerRing;
            break;

        // toggle spinning of scene objects
        case GLFW_KEY_S:
            _spinObjects = !_spinObjects;
            break;

        // toggle moving the light up and down
        case GLFW_KEY_L:
            _moveLight = !_moveLight;
            break;

        // adjust shadow texture resolution
        case GLFW_KEY_Z:
            if (SHADOW_TEXTURE_RESOLUTION > 2)
                SHADOW_TEXTURE_RESOLUTION /= 2;
            break;
        case GLFW_KEY_X:
            if (SHADOW_TEXTURE_RESOLUTION < 8192)
                SHADOW_TEXTURE_RESOLUTION *= 2;
            break;

        // adjust shadow bias
        case GLFW_KEY_C:
            if (_shadowBias >= 0.01f)
                _shadowBias -= 0.01f;
            if (_shadowBias <= 0.f)
                _shadowBias = 0.f;
            break;
        case GLFW_KEY_V:
            _shadowBias += 0.01f;
            break;

        // adjust shadow map samples
        case GLFW_KEY_G:
            if (_shadowMapSamples > 2.f)
                _shadowMapSamples /= 2.f;
            if (_shadowMapSamples <= 2.f)
                _shadowMapSamples = 2.f;
            break;
        case GLFW_KEY_H:
            _shadowMapSamples *= 2.f;
            break;

        // shadow options
        case GLFW_KEY_0:
            _which_shadows = NONE;
            break;
        case GLFW_KEY_1:
            _which_shadows = PLANAR;
            if (_options(PLANAR_DEPTH_TEST))
                _turn_off(PLANAR_DEPTH_TEST);
            else
                _turn_on(PLANAR_DEPTH_TEST);
            break;
        case GLFW_KEY_2:
            _which_shadows = PLANAR;
            if (_options(PLANAR_BLEND))
                _turn_off(PLANAR_BLEND);
            else
                _turn_on(PLANAR_BLEND);
            break;
        case GLFW_KEY_3:
            _which_shadows = PLANAR;
            if (_options(PLANAR_STENCIL_TEST))
                _turn_off(PLANAR_STENCIL_TEST);
            else
                _turn_on(PLANAR_STENCIL_TEST);
            break;
        case GLFW_KEY_4:
            _which_shadows = TEXTURES;
            break;
        case GLFW_KEY_5:
            if (_options(LINEAR_TEXTURE_FILTER))
                _turn_off(LINEAR_TEXTURE_FILTER);
            else
                _turn_on(LINEAR_TEXTURE_FILTER);
            break;
        case GLFW_KEY_6:
            _which_shadows = MAPS;
            break;
        case GLFW_KEY_7:
            if (_doMultisampling == 0)
                _doMultisampling = 1;
            else
                _doMultisampling = 0;
        }
    }
}

void Engine::handleMouseButtonEvent(GLint button, GLint action) {
    // if the event is for the left mouse button, update button state
    if (button == GLFW_MOUSE_BUTTON_LEFT)
        _leftMouseButtonState = action;
}

void Engine::handleCursorPositionEvent(vec2 currMousePosition) {
    // rotate the camera by the distance the mouse moved
    if (_leftMouseButtonState == GLFW_PRESS)
        _arcballCam->rotate((currMousePosition.x - _mousePosition.x) * 0.005f,
                            (_mousePosition.y - currMousePosition.y) * 0.005f);

    // update mouse position
    _mousePosition = currMousePosition;
}

void Engine::handleScrollEvent(vec2 offset) {
    // zoom the camera in and out on scroll
    _arcballCam->zoom(offset.y * 0.2f);
}

// *****************************************************************************
// Engine Setup

void Engine::_setupGLFW() {
    std::cout << "Starting GLFW ...\n";

    // set GLFW error callback function
    glfwSetErrorCallback(error_callback);

    glfwInit(); // initialize GLFW

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, MAJOR_VERSION);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, MINOR_VERSION);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE); // request double buffering
    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

    // request window creation from OS
    _window =
        glfwCreateWindow(_windowWidth, _windowHeight, _windowTitle.c_str(),
                         glfwGetPrimaryMonitor(), nullptr);

    glfwMakeContextCurrent(_window); // make new window current one
    glfwSwapInterval(1);             // update screen after at least 1 refresh

    glfwSetWindowUserPointer(_window, (void*)this); // connect engine to window

    // set callback functions
    glfwSetKeyCallback(_window, keyboard_callback);
    glfwSetMouseButtonCallback(_window, mouse_button_callback);
    glfwSetCursorPosCallback(_window, cursor_callback);
    glfwSetScrollCallback(_window, scroll_callback);

    // read current time
    _lastTime = glfwGetTime();
}

void Engine::_setupGLAD() {
    std::cout << "Initializing GLAD ...\n";

    gladLoadGL(); // initialize GLAD
}

void Engine::_setupOpenGL() {
    std::cout << "Setting up OpenGL ...\n";

    // SUPER VERBOSE DEBUGGING!
    /* https://stackoverflow.com/a/18067245/10323091 */
    if (glDebugMessageControlARB != NULL) {
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
        glDebugMessageControlARB(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0,
                                 NULL, GL_TRUE);
        glDebugMessageCallbackARB((GLDEBUGPROCARB)ETB_GL_ERROR_CALLBACK, NULL);
        std::cout << "DEBUG ON\n";
    } else
        std::cout << "DEBUG OFF\n";

    // 16 control points per Bezier patch
    glPatchParameteri(GL_PATCH_VERTICES, PATCH_DIMENSION * PATCH_DIMENSION);

    glEnable(GL_DEPTH_TEST); // enable depth testing
    glDepthFunc(GL_LESS);    // use less than depth test

    glEnable(GL_BLEND); // enable blending
    glBlendFunc(GL_SRC_ALPHA,
                GL_ONE_MINUS_SRC_ALPHA); // use one minus blending equation

    glClearColor(0.f, 0.f, 0.f, 1.f); // clear the frame buffer to black

    glFrontFace(GL_CCW); // the front faces are CCW
    // glCullFace(GL_BACK); // cull the back faces
    glDisable(GL_CULL_FACE);
}

void Engine::_setupShaders() {
    // setup wireframe shader program
    _wireShader = new ShaderProgram;

    std::cout << "Compiling wireframe (no tessellation) shader program ...\n";

    _wireShader->compileShader("shaders/gouraud.vert", GL_VERTEX_SHADER);
    _wireShader->compileShader("shaders/gouraud.geom", GL_GEOMETRY_SHADER);
    _wireShader->compileShader("shaders/gouraud.frag", GL_FRAGMENT_SHADER);

    std::cout << "Linking shader program and detaching shader objects ...\n";

    _wireShader->linkProgram();

    // setup wireframe tessellation shader program
    _wireTesShader = new ShaderProgram;

    std::cout << "Compiling wireframe (tessellation) shader program ...\n";

    _wireTesShader->compileShader("shaders/teapot.vert", GL_VERTEX_SHADER);
    _wireTesShader->compileShader("shaders/teapot.tesc",
                                  GL_TESS_CONTROL_SHADER);
    _wireTesShader->compileShader("shaders/teapot.tese",
                                  GL_TESS_EVALUATION_SHADER);
    _wireTesShader->compileShader("shaders/gouraud.geom", GL_GEOMETRY_SHADER);
    _wireTesShader->compileShader("shaders/perlin.frag", GL_FRAGMENT_SHADER);

    std::cout << "Linking shader program and detaching shader objects ...\n";

    _wireTesShader->linkProgram();

    // setup flat shader program
    _flatShader = new ShaderProgram;

    std::cout << "Compiling flat shader program ...\n";

    _flatShader->compileShader("shaders/flat.vert", GL_VERTEX_SHADER);
    _flatShader->compileShader("shaders/flat.geom", GL_GEOMETRY_SHADER);
    _flatShader->compileShader("shaders/flat.frag", GL_FRAGMENT_SHADER);

    std::cout << "Linking shader program and detaching shader objects ...\n";

    _flatShader->linkProgram();

    // setup flat shader program (for the light)
    _flatLightShader = new ShaderProgram;

    std::cout << "Compiling flat light shader program ...\n";

    _flatLightShader->compileShader("shaders/flat.vert", GL_VERTEX_SHADER);
    _flatLightShader->compileShader("shaders/flat.geom", GL_GEOMETRY_SHADER);
    _flatLightShader->compileShader("shaders/flat_light.frag",
                                    GL_FRAGMENT_SHADER);

    std::cout << "Linking shader program and detaching shader objects ...\n";

    _flatLightShader->linkProgram();

    // setup planar shadows shader (teapots)
    _teapotPlanarShadowShader = new ShaderProgram;

    std::cout << "Compiling planar shadows (teapot) shader program ...\n";

    _teapotPlanarShadowShader->compileShader("shaders/teapot.vert",
                                             GL_VERTEX_SHADER);
    _teapotPlanarShadowShader->compileShader("shaders/teapot.tesc",
                                             GL_TESS_CONTROL_SHADER);
    _teapotPlanarShadowShader->compileShader("shaders/planar_shadow.tese",
                                             GL_TESS_EVALUATION_SHADER);
    _teapotPlanarShadowShader->compileShader("shaders/planar_shadow.frag",
                                             GL_FRAGMENT_SHADER);

    std::cout << "Linking shader program and detaching shader objects ...\n";

    _teapotPlanarShadowShader->linkProgram();

    // setup planar shadows shader (spheres)
    _spherePlanarShadowShader = new ShaderProgram;

    std::cout << "Compiling planar shadows (sphere) shader program ...\n";

    _spherePlanarShadowShader->compileShader("shaders/planar_shadow.vert",
                                             GL_VERTEX_SHADER);
    _spherePlanarShadowShader->compileShader("shaders/planar_shadow.frag",
                                             GL_FRAGMENT_SHADER);

    std::cout << "Linking shader program and detaching shader objects ...\n";

    _spherePlanarShadowShader->linkProgram();

    // setup shadow textures cubemap shader (spheres)
    _shadowTextureCubemapShader = new ShaderProgram;

    std::cout << "Compiling shadow texture cubemap shader program ...\n";

    _shadowTextureCubemapShader->compileShader(
        "shaders/shadow_texture_cubemap.vert", GL_VERTEX_SHADER);
    _shadowTextureCubemapShader->compileShader(
        "shaders/shadow_texture_cubemap.frag", GL_FRAGMENT_SHADER);

    std::cout << "Linking shader program and detaching shader objects ...\n";

    _shadowTextureCubemapShader->linkProgram();

    // setup shadow textures cubemap shader (teapots)
    _shadowTextureCubemapTesShader = new ShaderProgram;

    std::cout
        << "Compiling shadow texture cubemap tessellation shader program ...\n";

    _shadowTextureCubemapTesShader->compileShader("shaders/teapot.vert",
                                                  GL_VERTEX_SHADER);
    _shadowTextureCubemapTesShader->compileShader("shaders/teapot.tesc",
                                                  GL_TESS_CONTROL_SHADER);
    _shadowTextureCubemapTesShader->compileShader(
        "shaders/shadow_texture_cubemap.tese", GL_TESS_EVALUATION_SHADER);
    _shadowTextureCubemapTesShader->compileShader(
        "shaders/shadow_texture_cubemap.frag", GL_FRAGMENT_SHADER);

    std::cout << "Linking shader program and detaching shader objects ...\n";

    _shadowTextureCubemapTesShader->linkProgram();

    // setup shadow textures shader
    _shadowTextureShader = new ShaderProgram;

    std::cout << "Compiling shadow texture shader program ...\n";

    _shadowTextureShader->compileShader("shaders/gouraud.vert",
                                        GL_VERTEX_SHADER);
    _shadowTextureShader->compileShader("shaders/gouraud.geom",
                                        GL_GEOMETRY_SHADER);
    _shadowTextureShader->compileShader("shaders/shadow_texture.frag",
                                        GL_FRAGMENT_SHADER);

    std::cout << "Linking shader program and detcaching shader objects ...\n";

    _shadowTextureShader->linkProgram();

    // setup shadow map shader
    _shadowMapShader = new ShaderProgram;

    std::cout << "Compiling shadow map shader program ...\n";

    _shadowMapShader->compileShader("shaders/gouraud.vert", GL_VERTEX_SHADER);
    _shadowMapShader->compileShader("shaders/gouraud.geom", GL_GEOMETRY_SHADER);
    _shadowMapShader->compileShader("shaders/shadow_map.frag",
                                    GL_FRAGMENT_SHADER);

    std::cout << "Linking shader program and detaching shader objects ...\n";

    _shadowMapShader->linkProgram();

    // setup shadow map shader (w/ tessellation)
    _shadowMapTesShader = new ShaderProgram;

    std::cout << "Compiling shadow map shader program (w/ tesssellation) ...\n";

    _shadowMapTesShader->compileShader("shaders/teapot.vert", GL_VERTEX_SHADER);
    _shadowMapTesShader->compileShader("shaders/teapot.tesc",
                                       GL_TESS_CONTROL_SHADER);
    _shadowMapTesShader->compileShader("shaders/teapot.tese",
                                       GL_TESS_EVALUATION_SHADER);
    _shadowMapTesShader->compileShader("shaders/gouraud.geom",
                                       GL_GEOMETRY_SHADER);
    _shadowMapTesShader->compileShader("shaders/shadow_map.frag",
                                       GL_FRAGMENT_SHADER);

    std::cout << "Linking shader program and detaching shader objects ...\n";

    _shadowMapTesShader->linkProgram();

    // setup shadow map cubemap shader
    _depthCubemapShader = new ShaderProgram;

    std::cout << "Compiling depth cubemap shader program ...\n";

    _depthCubemapShader->compileShader("shaders/shadow_map_cubemap.vert",
                                       GL_VERTEX_SHADER);
    _depthCubemapShader->compileShader("shaders/shadow_map_cubemap.frag",
                                       GL_FRAGMENT_SHADER);

    std::cout << "Linking shader program and detaching shader objects ...\n";

    _depthCubemapShader->linkProgram();

    // setup shadow map cubemap shader (w/ tessellation)
    _depthCubemapTesShader = new ShaderProgram;

    std::cout
        << "Compiling depth cubemap shader program (w/ tessellation) ...\n";

    _depthCubemapTesShader->compileShader("shaders/teapot.vert",
                                          GL_VERTEX_SHADER);
    _depthCubemapTesShader->compileShader("shaders/teapot.tesc",
                                          GL_TESS_CONTROL_SHADER);
    _depthCubemapTesShader->compileShader("shaders/shadow_map_cubemap.tese",
                                          GL_TESS_EVALUATION_SHADER);
    _depthCubemapTesShader->compileShader("shaders/shadow_map_cubemap.frag",
                                          GL_FRAGMENT_SHADER);

    std::cout << "Linking shader program and detaching shader objects ...\n";

    _depthCubemapTesShader->linkProgram();
}

void Engine::_setupBuffers() {
    std::cout << "Generating buffers ...\n";

    // generate VAO/VBO/IBO/UBO descriptors
    glGenVertexArrays(NUM_VAOS, _vaos);

    glGenBuffers(NUM_VAOS, _vbos);
    glGenBuffers(NUM_VAOS, _ibos);

    glGenBuffers(NUM_UBOS, _ubos);
}

void Engine::_setupTextures() {
    // create cubemap textures
    glGenTextures(1, &_shadowCubeMap);
    glGenTextures(1, &_depthCubeMap);

    // create framebuffer objects to render to
    glGenFramebuffers(1, &_shadowCubeMapFBO);
    glGenFramebuffers(1, &_depthCubeMapFBO);
}

void Engine::_setupScene() {
    std::cout << "Constructing geometry ...\n";

    // create scene objects
    _createPlatform(_vaos[VAO_ID::PLATFORM], _vbos[VAO_ID::PLATFORM],
                    _ibos[VAO_ID::PLATFORM], _numVAOPoints[VAO_ID::PLATFORM]);
    _createSphere(_vaos[VAO_ID::SPHERE], _vbos[VAO_ID::SPHERE],
                  _ibos[VAO_ID::SPHERE], _numVAOPoints[VAO_ID::SPHERE]);
    _createTeapot(_vaos[VAO_ID::TEAPOT], _vbos[VAO_ID::TEAPOT],
                  _ibos[VAO_ID::TEAPOT], _numVAOPoints[VAO_ID::TEAPOT]);

    std::cout << "Filling GPU uniform buffers ...\n";

    /* Scene Uniforms */

    // buffer used for staging data to send to GPU
    GLubyte* blockBuffer{nullptr};

    // just send identity matrices initially, will get updated in render loop
    mat4 model{1.f}, viewProjection{1.f}, modelViewProjection{1.f},
        viewportMatrix{1.f};
    mat4 shadowViewProjection{1.f};
    vec3 eyePos{0.f};

    // ask the GPU how the block is stored in memory, cache responses
    _wireShader->queryUniformBlock(
        "Scene",
        {"model", "viewProjection", "modelViewProjection", "viewportMatrix",
         "shadowViewProjection", "tessLevel", "eyePos", "wireframe",
         "controlPoints"},
        _blockSizes[UBO_ID::SCENE], _uniformOffsets[UBO_ID::SCENE]);

    // set up CPU-side buffer mirroring memory layout on GPU
    blockBuffer = (GLubyte*)malloc(_blockSizes[UBO_ID::SCENE]);

    memcpy(blockBuffer + _uniformOffsets[UBO_ID::SCENE].at(st 0u),
           &model[st 0u][st 0u], sizeof(mat4));
    memcpy(blockBuffer + _uniformOffsets[UBO_ID::SCENE].at(st 1u),
           &viewProjection[st 0u][st 0u], sizeof(mat4));
    memcpy(blockBuffer + _uniformOffsets[UBO_ID::SCENE].at(st 2u),
           &modelViewProjection[st 0u][st 0u], sizeof(mat4));
    memcpy(blockBuffer + _uniformOffsets[UBO_ID::SCENE].at(st 3u),
           &viewportMatrix[st 0u][st 0u], sizeof(mat4));
    memcpy(blockBuffer + _uniformOffsets[UBO_ID::SCENE].at(st 4u),
           &shadowViewProjection[0][0], sizeof(mat4));
    memcpy(blockBuffer + _uniformOffsets[UBO_ID::SCENE].at(st 5u), &_tessLevel,
           sizeof(GLfloat));
    memcpy(blockBuffer + _uniformOffsets[UBO_ID::SCENE].at(st 6u),
           &eyePos[st 0u], sizeof(vec3));
    memcpy(blockBuffer + _uniformOffsets[UBO_ID::SCENE].at(st 7u), &_wireframe,
           sizeof(GLint));
    memcpy(blockBuffer + _uniformOffsets[UBO_ID::SCENE].at(st 8u),
           &_controlPoints, sizeof(GLint));

    // send buffer to GPU
    glBindBuffer(GL_UNIFORM_BUFFER, _ubos[UBO_ID::SCENE]);
    glBufferData(GL_UNIFORM_BUFFER, _blockSizes[UBO_ID::SCENE], blockBuffer,
                 GL_DYNAMIC_DRAW);

    free(blockBuffer);
    blockBuffer = nullptr;

    // bind the buffer object to the uniform buffer-binding point at the index
    // specified by the binding-layout qualifier in the shader
    glBindBufferBase(GL_UNIFORM_BUFFER, 0u, _ubos[UBO_ID::SCENE]);

    /* Light Uniforms */

    vec3 lightAmb{1.f}, lightDiff{1.f}, lightSpec{1.f}; // white light
    // attenuation values for distance of 3250
    GLfloat attenConst{1.f}, attenLin{0.0014f}, attenQuad{0.000007f},
        shadowBias{0.f}, shadowMapSamples{0.f};
    GLint doMultisampling{0};

    // ask the GPU how the block is stored in memory, cache responses
    _wireShader->queryUniformBlock(
        "Light",
        {"lightPos", "lightAmb", "lightDiff", "lightSpec", "attenConst",
         "attenLin", "attenQuad", "shadowBias", "doMultisampling",
         "shadowMapSamples"},
        _blockSizes[UBO_ID::LIGHT], _uniformOffsets[UBO_ID::LIGHT]);

    // set up CPU-side buffer mirroring memory layout on GPU
    blockBuffer = (GLubyte*)malloc(_blockSizes[UBO_ID::LIGHT]);

    memcpy(blockBuffer + _uniformOffsets[UBO_ID::LIGHT].at(st 0u),
           &light_position[st 0u], sizeof(vec4));
    memcpy(blockBuffer + _uniformOffsets[UBO_ID::LIGHT].at(st 1u),
           &lightAmb[st 0u], sizeof(vec3));
    memcpy(blockBuffer + _uniformOffsets[UBO_ID::LIGHT].at(st 2u),
           &lightDiff[st 0u], sizeof(vec3));
    memcpy(blockBuffer + _uniformOffsets[UBO_ID::LIGHT].at(st 3u),
           &lightSpec[st 0u], sizeof(vec3));
    memcpy(blockBuffer + _uniformOffsets[UBO_ID::LIGHT].at(st 4u), &attenConst,
           sizeof(GLfloat));
    memcpy(blockBuffer + _uniformOffsets[UBO_ID::LIGHT].at(st 5u), &attenLin,
           sizeof(GLfloat));
    memcpy(blockBuffer + _uniformOffsets[UBO_ID::LIGHT].at(st 6u), &attenQuad,
           sizeof(GLfloat));
    memcpy(blockBuffer + _uniformOffsets[UBO_ID::LIGHT].at(7), &shadowBias,
           sizeof shadowBias);
    memcpy(blockBuffer + _uniformOffsets[UBO_ID::LIGHT].at(8), &doMultisampling,
           sizeof doMultisampling);
    memcpy(blockBuffer + _uniformOffsets[UBO_ID::LIGHT].at(9),
           &shadowMapSamples, sizeof shadowMapSamples);

    // send buffer to GPU
    glBindBuffer(GL_UNIFORM_BUFFER, _ubos[UBO_ID::LIGHT]);
    glBufferData(GL_UNIFORM_BUFFER, _blockSizes[UBO_ID::LIGHT], blockBuffer,
                 GL_DYNAMIC_DRAW);

    free(blockBuffer);
    blockBuffer = nullptr;

    // bind the buffer object to the uniform buffer-binding point at the index
    // specified by the binding-layout qualifier in the shader
    glBindBufferBase(GL_UNIFORM_BUFFER, 1u, _ubos[UBO_ID::LIGHT]);

    /* Material Uniforms */

    // just send white material initially, will get updated in render loop
    vec3 materialAmb{1.f}, materialDiff{1.f}, materialSpec{1.f};
    GLfloat shininess{64.f}; // that's pretty shiny ig

    // ask the GPU how the block is stored in memory, cache responses
    _wireShader->queryUniformBlock(
        "Material",
        {"materialAmb", "materialDiff", "materialSpec", "shininess"},
        _blockSizes[UBO_ID::MATERIAL], _uniformOffsets[UBO_ID::MATERIAL]);

    // set up CPU-side buffer mirroring memory layout on GPU
    blockBuffer = (GLubyte*)malloc(_blockSizes[UBO_ID::MATERIAL]);

    memcpy(blockBuffer + _uniformOffsets[UBO_ID::MATERIAL].at(st 0u),
           &materialAmb[st 0u], sizeof(vec3));
    memcpy(blockBuffer + _uniformOffsets[UBO_ID::MATERIAL].at(st 1u),
           &materialDiff[st 0u], sizeof(vec3));
    memcpy(blockBuffer + _uniformOffsets[UBO_ID::MATERIAL].at(st 2u),
           &materialSpec[st 0u], sizeof(vec3));
    memcpy(blockBuffer + _uniformOffsets[UBO_ID::MATERIAL].at(st 3u),
           &shininess, sizeof(GLfloat));

    // send buffer to GPU
    glBindBuffer(GL_UNIFORM_BUFFER, _ubos[UBO_ID::MATERIAL]);
    glBufferData(GL_UNIFORM_BUFFER, _blockSizes[UBO_ID::MATERIAL], blockBuffer,
                 GL_DYNAMIC_DRAW);

    free(blockBuffer);
    blockBuffer = nullptr;

    // bind the buffer object to the uniform buffer-binding point at the index
    // specified by the binding-layout qualifier in the shader
    glBindBufferBase(GL_UNIFORM_BUFFER, 2u, _ubos[UBO_ID::MATERIAL]);

    glBindBuffer(GL_UNIFORM_BUFFER, 0u); // unbind uniform buffers from staging

    // set up camera
    std::cout << "Setting up arcball camera ...\n";

    _arcballCam = new ArcballCam(); // use default values

    // shouldn't need to do this manually, but there's a black frame at start of
    // rendering if I don't :(
    _arcballCam->setLookAtPoint(EYE_CENTER);
    _arcballCam->setUpVector(vec3(0.f, 1.f, 0.f));
}

void Engine::_printSetupInfo() {
    // request OpenGL driver info
    const GLubyte* renderer{glGetString(GL_RENDERER)};
    const GLubyte* vendor{glGetString(GL_VENDOR)};
    const GLubyte* version{glGetString(GL_VERSION)};
    const GLubyte* glslVersion{glGetString(GL_SHADING_LANGUAGE_VERSION)};

    // request GL version
    GLint major, minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);

    std::cout << "GL Vendor            : " << vendor << '\n';
    std::cout << "GL Renderer          : " << renderer << '\n';
    std::cout << "GL Version (string)  : " << version << '\n';
    std::cout << "GL Version (integer) : " << major << '.' << minor << '\n';
    std::cout << "GLSL Version         : " << glslVersion << '\n';
}

// *****************************************************************************
// Engine Cleanup

void Engine::_cleanupGLFW() {
    std::cout << "Shutting down GLFW ...\n";

    glfwDestroyWindow(_window);
    _window = nullptr;

    glfwTerminate();
}

void Engine::_cleanupShaders() {
    std::cout << "Deleting shader programs ...\n";

    delete _wireShader;
    _wireShader = nullptr;

    delete _wireTesShader;
    _wireTesShader = nullptr;

    delete _flatShader;
    _flatShader = nullptr;

    delete _flatLightShader;
    _flatLightShader = nullptr;

    delete _teapotPlanarShadowShader;
    _teapotPlanarShadowShader = nullptr;

    delete _spherePlanarShadowShader;
    _spherePlanarShadowShader = nullptr;

    delete _shadowTextureCubemapShader;
    _shadowTextureCubemapShader = nullptr;

    delete _shadowTextureCubemapTesShader;
    _shadowTextureCubemapTesShader = nullptr;

    delete _shadowTextureShader;
    _shadowTextureShader = nullptr;

    delete _shadowMapShader;
    _shadowMapShader = nullptr;

    delete _shadowMapTesShader;
    _shadowMapTesShader = nullptr;

    delete _depthCubemapShader;
    _depthCubemapShader = nullptr;

    delete _depthCubemapTesShader;
    _depthCubemapTesShader = nullptr;
}

void Engine::_cleanupBuffers() {
    std::cout << "Deleting GPU buffers ...\n";

    glDeleteVertexArrays(NUM_VAOS, _vaos);

    glDeleteBuffers(NUM_VAOS, _vbos);
    glDeleteBuffers(NUM_VAOS, _ibos);

    glDeleteBuffers(NUM_UBOS, _ubos);
}

void Engine::_cleanupScene() {
    std::cout << "Deleting camera ...\n";

    delete _arcballCam;
}

// *****************************************************************************
// Engine Rendering & Updating

void Engine::_renderScene(const mat4& viewMatrix, const mat4& projectionMatrix,
                          const mat4& viewportMatrix) {
    // if shader program is null, do not continue to prevent run time errors
    if (!_wireShader || !_wireTesShader || !_flatShader)
        return;

    // send light properties
    vec3 lightAmb{1.f}, lightDiff{1.f}, lightSpec{1.f}; // white light
    /* https://learnopengl.com/Lighting/Light-casters */
    // attenuation values for distance of 160
    GLfloat attenConst{1.f}, attenLin{0.027f}, attenQuad{0.0028f};

    _sendLightBlock(light_position, lightAmb, lightDiff, lightSpec, attenConst,
                    attenLin, attenQuad);

    // matrices to use for setting object transformations
    mat4 model{1.f}, modelView{1.f}, modelViewProjection{1.f};
    mat4 viewProjection{projectionMatrix * viewMatrix};
    mat4 shadowViewProjections{1.f};
    vec3 eyePos{_arcballCam->getPosition()};

    // vectors to use for setting object materials
    vec3 materialAmb{0.f}, materialDiff{0.f}, materialSpec{0.f};
    GLfloat shininess{0.f};

    // positions of objects in the scene
    std::vector<vec3> teapot_positions{
        circlePos(9.f, _angle_offset + 0.f, 0.5f),
        circlePos(9.f, _angle_offset + PI / 2.f, 1.5f),
        circlePos(9.f, _angle_offset + PI, 0.5f),
        circlePos(9.f, _angle_offset + 3.f * PI / 2.f, 1.5f)};

    std::vector<vec3> sphere_positions{
        circlePos(9.f, _angle_offset + PI / 4.f, 1.1f),
        circlePos(9.f, _angle_offset + 3.f * PI / 4.f, 1.1f),
        circlePos(9.f, _angle_offset + 5.f * PI / 4.f, 1.1f),
        circlePos(9.f, _angle_offset + 7.f * PI / 4.f, 1.1f)};

    std::vector<vec3> outer_sphere_positions{
        circlePos(20.f, 0.f, 1.6f),
        circlePos(20.f, PI / 4.f, 1.6f),
        circlePos(20.f, PI / 2.f, 1.6f),
        circlePos(20.f, 3.f * PI / 4.f, 1.6f),
        circlePos(20.f, PI, 1.6f),
        circlePos(20.f, 5.f * PI / 4.f, 1.6f),
        circlePos(20.f, 3.f * PI / 2.f, 1.6f),
        circlePos(20.f, 7.f * PI / 4.f, 1.6f)};

    /* Drawing the Platform */

    if (_which_shadows == TEXTURES) {
        _shadowTextureShader->useProgram();

        glBindTexture(GL_TEXTURE_CUBE_MAP, _shadowCubeMap);
    } else if (_which_shadows == MAPS) {
        _shadowMapShader->useProgram();

        glBindTexture(GL_TEXTURE_CUBE_MAP, _depthCubeMap);
    } else
        _wireShader->useProgram();

    if (_which_shadows == PLANAR && _options(PLANAR_STENCIL_TEST)) {
        glEnable(GL_STENCIL_TEST);
        glStencilFunc(GL_ALWAYS, 0, 0xffff);
        glStencilOp(GL_KEEP, GL_INCR, GL_INCR);
    }

    // compute and send transformation matrices
    model = glm::scale(mat4(1.f), vec3(100.f));

    modelView = viewMatrix * model;
    modelViewProjection = projectionMatrix * modelView;

    _sendSceneBlock(model, viewProjection, modelViewProjection, viewportMatrix,
                    shadowViewProjections, eyePos);

    // send material properties
    materialAmb = materialDiff = vec3{0.2f, 0.1f, 0.7f}; // bluish
    materialSpec = vec3{1.f};                            // white
    shininess = 64.f;                                    // pwetty shiny uwu

    _sendMaterialBlock(materialAmb, materialDiff, materialSpec, shininess);

    _drawPlatform(); // draw the platform

    /* Drawing planar projection of teapot onto the platform */

    if (_which_shadows == PLANAR) {
        glDisable(GL_CULL_FACE);

        if (_options(PLANAR_DEPTH_TEST))
            glDisable(GL_DEPTH_TEST);

        if (_options(PLANAR_STENCIL_TEST)) {
            // glDisable(GL_DEPTH_TEST);
            glStencilFunc(GL_EQUAL, 1, 0xffff);
            glStencilOp(GL_KEEP, GL_INCR, GL_INCR);
        }

        // send material properties
        if (_options(PLANAR_BLEND))
            shininess = 0.5f; // using this as alpha value
        else
            shininess = 1.f;

        _sendMaterialBlock(materialAmb, materialDiff, materialSpec, shininess);

        _teapotPlanarShadowShader->useProgram();
        for (std::size_t i{0}; i < teapot_positions.size(); ++i) {
            model = glm::translate(mat4(1.f), teapot_positions.at(i));
            model = glm::rotate(model, PI / -2.f, {1.f, 0.f, 0.f});
            model = glm::rotate(model, ((GLfloat)i + 1.f) * (PI / 2.f),
                                {0.f, 0.f, 1.f});

            modelView = viewMatrix * model;
            modelViewProjection = projectionMatrix * modelView;

            _sendSceneBlock(model, viewProjection, modelViewProjection,
                            viewportMatrix, shadowViewProjections, eyePos);

            _drawTeapot();
        }

        _spherePlanarShadowShader->useProgram();
        for (std::size_t i{0}; i < sphere_positions.size(); ++i) {
            model = glm::translate(mat4(1.f), sphere_positions.at(i));
            model = glm::scale(model, vec3(1.f));

            modelView = viewMatrix * model;
            modelViewProjection = projectionMatrix * modelView;

            _sendSceneBlock(model, viewProjection, modelViewProjection,
                            viewportMatrix, shadowViewProjections, eyePos);

            _drawSphere();
        }

        if (_outerRing) {
            for (const auto& pos : outer_sphere_positions) {
                model = glm::translate(mat4(1.f), pos);
                model = glm::scale(model, vec3(1.5f));

                modelView = viewMatrix * model;
                modelViewProjection = projectionMatrix * modelView;

                _sendSceneBlock(model, viewProjection, modelViewProjection,
                                viewportMatrix, shadowViewProjections, eyePos);

                _drawSphere();
            }
        }

        glEnable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);
    }

    /* Drawing the teapots */

    if (_which_shadows == MAPS) {
        _shadowMapTesShader->useProgram();

        glBindTexture(GL_TEXTURE_CUBE_MAP, _depthCubeMap);
    } else
        _wireTesShader->useProgram();

    // send material properties
    materialAmb = materialDiff = {0.4f, 0.f, 0.6f}; // purplish
    materialSpec = vec3{1.f};                       // white
    shininess = 64.f;                               // weeeee big specular

    _sendMaterialBlock(materialAmb, materialDiff, materialSpec, shininess);

    for (std::size_t i{0}; i < teapot_positions.size(); ++i) {
        model = glm::translate(mat4(1.f), teapot_positions.at(i));
        model = glm::rotate(model, PI / -2.f, {1.f, 0.f, 0.f});
        model = glm::rotate(model, ((GLfloat)i + 1.f) * (PI / 2.f),
                            {0.f, 0.f, 1.f});

        modelView = viewMatrix * model;
        modelViewProjection = projectionMatrix * modelView;

        _sendSceneBlock(model, viewProjection, modelViewProjection,
                        viewportMatrix, shadowViewProjections, eyePos);

        _drawTeapot();
    }

    /* Drawing the spheres */

    if (_which_shadows == MAPS) {
        _shadowMapShader->useProgram();

        glBindTexture(GL_TEXTURE_CUBE_MAP, _depthCubeMap);
    } else
        _wireShader->useProgram();

    // send material properties
    materialAmb = materialDiff = {1.f, 0.078f, 0.576f}; // pink!
    materialSpec = vec3{1.f};                           // white
    shininess = 64.f;                                   // :o shinnne

    _sendMaterialBlock(materialAmb, materialDiff, materialSpec, shininess);

    for (std::size_t i{0}; i < sphere_positions.size(); ++i) {
        model = glm::translate(mat4(1.f), sphere_positions.at(i));
        model = glm::scale(model, vec3(1.f));

        modelView = viewMatrix * model;
        modelViewProjection = projectionMatrix * modelView;

        _sendSceneBlock(model, viewProjection, modelViewProjection,
                        viewportMatrix, shadowViewProjections, eyePos);

        _drawSphere();
    }

    // outer ring of unmoving circles
    if (_outerRing) {
        if (_which_shadows == TEXTURES) {
            _shadowTextureShader->useProgram();

            glBindTexture(GL_TEXTURE_CUBE_MAP, _shadowCubeMap);

        } else if (_which_shadows == MAPS) {
            _shadowMapShader->useProgram();

            glBindTexture(GL_TEXTURE_CUBE_MAP, _depthCubeMap);
        } else
            _wireShader->useProgram();

        materialAmb = materialDiff = {0.f, 0.9804f, 0.6039f}; // pale green?

        _sendMaterialBlock(materialAmb, materialDiff, materialSpec, shininess);

        for (const auto& pos : outer_sphere_positions) {
            model = glm::translate(mat4(1.f), pos);
            model = glm::scale(model, vec3(1.5f));

            modelView = viewMatrix * model;
            modelViewProjection = projectionMatrix * modelView;

            _sendSceneBlock(model, viewProjection, modelViewProjection,
                            viewportMatrix, shadowViewProjections, eyePos);

            _drawSphere();
        }
    }

    if (_which_shadows == TEXTURES)
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    if (_which_shadows == MAPS)
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    /* Drawing the light */

    _flatLightShader->useProgram();
    // _wireShader->useProgram();

    // send material properties
    materialAmb = materialDiff = {1.f, 1.f, 1.f}; // white privilege
    materialSpec = vec3{1.f};                     // white light buh duh buh duh
    shininess = 128.f;                            // even shinier ooOOoo

    _sendMaterialBlock(materialAmb, materialDiff, materialSpec, shininess);

    // compute and send transformation matrices
    model = glm::translate(
        mat4(1.f), vec3(light_position.x, light_position.y, light_position.z));
    model = glm::scale(model, vec3(0.1f));

    modelView = viewMatrix * model;
    modelViewProjection = projectionMatrix * modelView;
    // normalMtx = glm::transpose(glm::inverse(modelView));

    _sendSceneBlock(model, viewProjection, modelViewProjection, viewportMatrix,
                    shadowViewProjections, eyePos);

    _drawSphere();

    /* Drawing the teapot control points */

    // _flatShader->useProgram();
    // // _wireShader->useProgram();

    // // send material properties
    // materialAmb = materialDiff = {0.f, 1.f, 0.f}; // oooo so green
    // materialSpec = vec3{1.f};                     // white
    // shininess = 128.f;                            // even shinier ooOOoo

    // _sendMaterialBlock(materialAmb, materialDiff, materialSpec, shininess);

    // // TODO: use instanced rendering for this (maybe that would even be fast
    // // enough to go back to actually shading the spheres instead of using
    // flat
    // // shader)
    // for (size_t i{st 0u}; i < TEAPOT_NUM_VERTICES; ++i) {
    //     // compute and send transformation matrices
    //     model = glm::translate(mat4(1.f), {0.f, 1.f, 0.f});
    //     model = glm::rotate(model, glm::pi<GLfloat>() / -2.f, {1.f, 0.f,
    //     0.f}); model = glm::translate(model, teapot_cp_vertices[i]); model =
    //     glm::scale(model, vec3(0.03f));

    //     modelView = viewMatrix * model;
    //     modelViewProjection = projectionMatrix * modelView;
    //     // normalMtx = glm::transpose(glm::inverse(modelView));

    //     _sendSceneBlock(model, viewProjection, modelViewProjection,
    //                     viewportMatrix, eyePos);

    //     _drawSphere();
    // }
}

void Engine::_renderShadowTextures() {
    // assign a texture image to each face of the cubemap
    glBindTexture(GL_TEXTURE_CUBE_MAP, _shadowCubeMap);

    for (GLuint i = 0; i < 6u; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA,
                     SHADOW_TEXTURE_RESOLUTION, SHADOW_TEXTURE_RESOLUTION, 0,
                     GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }

    // texture settings
    if (_options(LINEAR_TEXTURE_FILTER)) {
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    } else {
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // each face uses the same projection matrix
    mat4 shadowProjection =
        glm::perspective(glm::radians(90.f), 1.f, 0.001f, 1'000.f);

    // faces use different view matrices (right, left, top, bottom, near, far)
    vec3 lightPos = vec3(light_position);

    std::vector<mat4> shadowViewProjections{
        glm::lookAt(lightPos, lightPos + vec3(1.f, 0.f, 0.f),
                    vec3(0.f, -1.f, 0.f)),
        glm::lookAt(lightPos, lightPos + vec3(-1.f, 0.f, 0.f),
                    vec3(0.f, -1.f, 0.f)),
        glm::lookAt(lightPos, lightPos + vec3(0.f, 1.f, 0.f),
                    vec3(0.f, 0.f, 1.f)),
        glm::lookAt(lightPos, lightPos + vec3(0.f, -1.f, 0.f),
                    vec3(0.f, 0.f, -1.f)),
        glm::lookAt(lightPos, lightPos + vec3(0.f, 0.f, 1.f),
                    vec3(0.f, -1.f, 0.f)),
        glm::lookAt(lightPos, lightPos + vec3(0.f, 0.f, -1.f),
                    vec3(0.f, -1.f, 0.f))};

    for (auto& m : shadowViewProjections)
        m = shadowProjection * m;

    // rendering code below

    // matrices to use for setting object transformations
    mat4 model{1.f}, modelView{1.f}, modelViewProjection{1.f};
    mat4 viewProjection{1.f}, viewportMatrix{1.f};
    vec3 eyePos{lightPos};

    // positions of objects in the scene
    std::vector<vec3> teapot_positions{
        circlePos(9.f, _angle_offset + 0.f, 0.5f),
        circlePos(9.f, _angle_offset + PI / 2.f, 1.5f),
        circlePos(9.f, _angle_offset + PI, 0.5f),
        circlePos(9.f, _angle_offset + 3.f * PI / 2.f, 1.5f)};

    std::vector<vec3> sphere_positions{
        circlePos(9.f, _angle_offset + PI / 4.f, 1.1f),
        circlePos(9.f, _angle_offset + 3.f * PI / 4.f, 1.1f),
        circlePos(9.f, _angle_offset + 5.f * PI / 4.f, 1.1f),
        circlePos(9.f, _angle_offset + 7.f * PI / 4.f, 1.1f)};

    for (std::size_t i{0}; i < 6u; ++i) {
        // attach the texture to the framebuffer object
        glBindFramebuffer(GL_FRAMEBUFFER, _shadowCubeMapFBO);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);

        glViewport(0, 0, SHADOW_TEXTURE_RESOLUTION, SHADOW_TEXTURE_RESOLUTION);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                               _shadowCubeMap, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cerr << "\nSHADOW CUBEMAP FRAMEBUFFER IS BROKEN!!"
                      << std::endl;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
                GL_STENCIL_BUFFER_BIT);

        /* Drawing the spheres */
        _shadowTextureCubemapShader->useProgram();

        for (std::size_t j{0}; j < sphere_positions.size(); ++j) {
            model = glm::translate(mat4(1.f), sphere_positions.at(j));
            model = glm::scale(model, vec3(1.f));

            _sendSceneBlock(model, viewProjection, modelViewProjection,
                            viewportMatrix, shadowViewProjections.at(i),
                            eyePos);

            _drawSphere();
        }

        /* Drawing the teapots */
        _shadowTextureCubemapTesShader->useProgram();

        for (std::size_t j{0}; j < teapot_positions.size(); ++j) {
            model = glm::translate(mat4(1.f), teapot_positions.at(j));
            model = glm::rotate(model, PI / -2.f, {1.f, 0.f, 0.f});
            model = glm::rotate(model, ((GLfloat)j + 1.f) * (PI / 2.f),
                                {0.f, 0.f, 1.f});

            _sendSceneBlock(model, viewProjection, modelViewProjection,
                            viewportMatrix, shadowViewProjections.at(i),
                            eyePos);

            _drawTeapot();
        }
    }

    // unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Engine::_renderShadowMaps() {
    /* https://learnopengl.com/Advanced-Lighting/Shadows/Point-Shadows */

    // cull front faces to fix peter-panning
    if (_options(MAPS_CULL_FRONT_FACE))
        glCullFace(GL_FRONT);

    // assign a texture image to each face of the cubemap
    glBindTexture(GL_TEXTURE_CUBE_MAP, _depthCubeMap);

    for (GLuint i = 0; i < 6u; ++i) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
                     SHADOW_TEXTURE_RESOLUTION, SHADOW_TEXTURE_RESOLUTION, 0,
                     GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }

    // texture settings
    if (_options(LINEAR_TEXTURE_FILTER)) {
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    } else {
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // each face uses the same projection matrix
    mat4 shadowProjection =
        glm::perspective(glm::radians(90.f), 1.f, 0.001f, 1'000.f);

    // faces use different view matrices (right, left, top, bottom, near, far)
    vec3 lightPos = vec3(light_position);

    std::vector<mat4> shadowViewProjections{
        glm::lookAt(lightPos, lightPos + vec3(1.f, 0.f, 0.f),
                    vec3(0.f, -1.f, 0.f)),
        glm::lookAt(lightPos, lightPos + vec3(-1.f, 0.f, 0.f),
                    vec3(0.f, -1.f, 0.f)),
        glm::lookAt(lightPos, lightPos + vec3(0.f, 1.f, 0.f),
                    vec3(0.f, 0.f, 1.f)),
        glm::lookAt(lightPos, lightPos + vec3(0.f, -1.f, 0.f),
                    vec3(0.f, 0.f, -1.f)),
        glm::lookAt(lightPos, lightPos + vec3(0.f, 0.f, 1.f),
                    vec3(0.f, -1.f, 0.f)),
        glm::lookAt(lightPos, lightPos + vec3(0.f, 0.f, -1.f),
                    vec3(0.f, -1.f, 0.f))};

    for (auto& m : shadowViewProjections)
        m = shadowProjection * m;

    // rendering code below

    // matrices to use for setting object transformations
    mat4 model{1.f}, modelView{1.f}, modelViewProjection{1.f};
    mat4 viewProjection{1.f}, viewportMatrix{1.f};
    vec3 eyePos{lightPos};

    // positions of objects in the scene
    std::vector<vec3> teapot_positions{
        circlePos(9.f, _angle_offset + 0.f, 0.5f),
        circlePos(9.f, _angle_offset + PI / 2.f, 1.5f),
        circlePos(9.f, _angle_offset + PI, 0.5f),
        circlePos(9.f, _angle_offset + 3.f * PI / 2.f, 1.5f)};

    std::vector<vec3> sphere_positions{
        circlePos(9.f, _angle_offset + PI / 4.f, 1.1f),
        circlePos(9.f, _angle_offset + 3.f * PI / 4.f, 1.1f),
        circlePos(9.f, _angle_offset + 5.f * PI / 4.f, 1.1f),
        circlePos(9.f, _angle_offset + 7.f * PI / 4.f, 1.1f)};

    std::vector<vec3> outer_sphere_positions{
        circlePos(20.f, 0.f, 1.6f),
        circlePos(20.f, PI / 4.f, 1.6f),
        circlePos(20.f, PI / 2.f, 1.6f),
        circlePos(20.f, 3.f * PI / 4.f, 1.6f),
        circlePos(20.f, PI, 1.6f),
        circlePos(20.f, 5.f * PI / 4.f, 1.6f),
        circlePos(20.f, 3.f * PI / 2.f, 1.6f),
        circlePos(20.f, 7.f * PI / 4.f, 1.6f)};

    for (std::size_t i{0}; i < 6u; ++i) {
        // attach the texture to the framebuffer object
        glBindFramebuffer(GL_FRAMEBUFFER, _depthCubeMapFBO);
        glDrawBuffer(GL_NONE);

        glViewport(0, 0, SHADOW_TEXTURE_RESOLUTION, SHADOW_TEXTURE_RESOLUTION);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                               GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                               _depthCubeMap, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cerr << "\nDEPTH CUBEMAP FRAMEBUFFER IS BROKEN!!" << std::endl;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT |
                GL_STENCIL_BUFFER_BIT);

        /* Drawing the spheres */
        _depthCubemapShader->useProgram();

        for (const auto& pos : sphere_positions) {
            model = glm::translate(mat4(1.f), pos);
            model = glm::scale(model, vec3(1.f));

            _sendSceneBlock(model, viewProjection, modelViewProjection,
                            viewportMatrix, shadowViewProjections.at(i),
                            eyePos);

            _drawSphere();
        }

        if (_outerRing) {
            for (const auto& pos : outer_sphere_positions) {
                model = glm::translate(mat4(1.f), pos);
                model = glm::scale(model, vec3(1.5f));

                _sendSceneBlock(model, viewProjection, modelViewProjection,
                                viewportMatrix, shadowViewProjections.at(i),
                                eyePos);

                _drawSphere();
            }
        }

        /* Drawing the teapots */
        _depthCubemapTesShader->useProgram();

        for (std::size_t j{0}; j < teapot_positions.size(); ++j) {
            model = glm::translate(mat4(1.f), teapot_positions.at(j));
            model = glm::rotate(model, PI / -2.f, {1.f, 0.f, 0.f});
            model = glm::rotate(model, ((GLfloat)j + 1.f) * (PI / 2.f),
                                {0.f, 0.f, 1.f});

            _sendSceneBlock(model, viewProjection, modelViewProjection,
                            viewportMatrix, shadowViewProjections.at(i),
                            eyePos);

            _drawTeapot();
        }
    }

    // unbind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // cull back faces again
    glCullFace(GL_BACK);
}

void Engine::_updateScene() {
    // set the window title with current rendering info
    _windowTitle = "FP - Shadows [ ";

    // show tessellation level
    _windowTitle += "Tessellation Level ";

    // calculate FPS
    GLdouble currentTime = glfwGetTime();
    GLdouble delta = currentTime - _lastTime;
    ++_nFrames;

    if (delta >= 1.0) { // if last update was more than 1 sec ago
        _fps = (GLdouble)_nFrames / delta;
        _nFrames = 0;
        _lastTime = currentTime;
    }

    // update window title
    std::stringstream ss;
    ss << _windowTitle << glm::floor(_tessLevel) << " | " << std::fixed
       << std::setprecision(3) << _fps << " FPS ]";
    _windowTitle = ss.str();

    // display new window title
    glfwSetWindowTitle(_window, _windowTitle.c_str());

    // animating the objects
    if (_spinObjects) {
        _angle_offset += 0.01f;
        if (_angle_offset >= 2.f * PI)
            _angle_offset = 0.f;
    }

    // animating the light
    if (_moveLight) {
        light_position.y += _light_offset;
        if (light_position.y <= 1.f || light_position.y >= 9.f)
            _light_offset *= -1;
    }
}

void Engine::_drawPlatform() {
    glBindVertexArray(_vaos[VAO_ID::PLATFORM]); // bind platform VAO

    glDrawElements(GL_TRIANGLE_STRIP, _numVAOPoints[VAO_ID::PLATFORM],
                   GL_UNSIGNED_SHORT, GL_NONE);

    glBindVertexArray(GL_NONE); // unbind platform VAO
}

void Engine::_drawTeapot() {
    glBindVertexArray(_vaos[VAO_ID::TEAPOT]); // bind teapot VAO

    glDrawElements(GL_PATCHES,
                   TEAPOT_NUM_PATCHES * PATCH_DIMENSION * PATCH_DIMENSION,
                   GL_UNSIGNED_SHORT, GL_NONE);

    glBindVertexArray(GL_NONE); // unbind teapot VAO
}

void Engine::_drawSphere() {
    glBindVertexArray(_vaos[VAO_ID::SPHERE]); // bind sphere VAO

    glDrawElements(GL_TRIANGLES, _numVAOPoints[VAO_ID::SPHERE],
                   GL_UNSIGNED_SHORT, GL_NONE);

    glBindVertexArray(GL_NONE); // unbind sphere VAO
}

// *****************************************************************************
// VAO & Object Information

void Engine::_createPlatform(const GLuint& vao, const GLuint& vbo,
                             const GLuint& ibo, GLsizei& numVAOPoints) {
    // container for our vertex data
    struct VertexAttributes {
        GLfloat x, y, z;    // x. y, z position
        GLfloat nx, ny, nz; // x, y, z normal vector
    };

    // create our platform
    VertexAttributes platformVertices[4] = {
        {-1.f, 0.f, -1.f, 0.f, 1.f, 0.f}, // 0 - BL
        {1.f, 0.0f, -1.f, 0.f, 1.f, 0.f}, // 1 - BR
        {-1.f, 0.f, 1.f, 0.f, 1.f, 0.f},  // 2 - TL
        {1.f, 0.f, 1.f, 0.f, 1.f, 0.f}    // 3 - TR
    };

    // order to connect the vertices in
    GLushort platformIndices[4] = {(GLushort)0u, (GLushort)2u, (GLushort)1u,
                                   (GLushort)3u};
    numVAOPoints = 4;

    glBindVertexArray(vao); // bind platform VAO

    // bind and fill vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(platformVertices), platformVertices,
                 GL_STATIC_DRAW);

    // doesn't matter which program we query (attribute layouts should match)
    GLuint vPosAttribLocation{_wireShader->getAttributeLocation("vPos")};
    GLuint vNormAttribLocation{_wireShader->getAttributeLocation("vNorm")};

    // enable generic attribute arrays and define access patterns
    glEnableVertexAttribArray(vPosAttribLocation);
    glVertexAttribPointer(vPosAttribLocation, 3, GL_FLOAT, GL_FALSE,
                          sizeof(VertexAttributes), GL_NONE);

    glEnableVertexAttribArray(vNormAttribLocation);
    glVertexAttribPointer(vNormAttribLocation, 3, GL_FLOAT, GL_FALSE,
                          sizeof(VertexAttributes),
                          (void*)(sizeof(GLfloat) * 3));

    // bind and fill index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(platformIndices),
                 platformIndices, GL_STATIC_DRAW);

    glBindVertexArray(GL_NONE); // unbind platform VAO

    std::cout << "Platform read into GPU memory with VAO/VBO/IBO " << vao << '/'
              << vbo << '/' << ibo << " & " << numVAOPoints << " points\n";
}

void Engine::_createTeapot(const GLuint& vao, const GLuint& vbo,
                           const GLuint& ibo, GLsizei& numVAOPoints) {
    // retrieve control point vertices (normals will be calculated on GPU)
    vec3 teapotVertices[TEAPOT_NUM_VERTICES];

    for (size_t i{(size_t)0u}; i < TEAPOT_NUM_VERTICES; ++i) {
        teapotVertices[i].x = teapot_cp_vertices[i].x;
        teapotVertices[i].y = teapot_cp_vertices[i].y;
        teapotVertices[i].z = teapot_cp_vertices[i].z;
    }

    // order to connect the vertices in
    numVAOPoints = TEAPOT_NUM_PATCHES * PATCH_DIMENSION * PATCH_DIMENSION;

    GLushort teapotIndices[numVAOPoints];

    for (size_t i{(size_t)0u}; i < numVAOPoints; ++i)
        teapotIndices[i] = teapot_patches[i] - (GLushort)1u;

    glBindVertexArray(vao); // bind teapot VAO

    // bind and fill vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(teapotVertices), teapotVertices,
                 GL_STATIC_DRAW);

    // doesn't matter which program we query (attribute layouts should match)
    GLuint vPosAttribLocation{_wireTesShader->getAttributeLocation("vPos")};

    // enable generic attribute arrays and define access patterns
    glEnableVertexAttribArray(vPosAttribLocation);
    glVertexAttribPointer(vPosAttribLocation, 3, GL_FLOAT, GL_FALSE, GL_NONE,
                          GL_NONE);

    // bind and fill index buffer
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(teapotIndices), teapotIndices,
                 GL_STATIC_DRAW);

    glBindVertexArray(GL_NONE); // unbind platform VAO

    std::cout << "Teapot read into GPU memory with VAO/VBO/IBO " << vao << '/'
              << vbo << '/' << ibo << " & " << numVAOPoints << " points\n";
}

void Engine::_createSphere(const GLuint& vao, const GLuint& vbo,
                           const GLuint& ibo, GLsizei& numVAOPoints) {
    std::vector<GLfloat> vertices;
    std::vector<GLushort> indices;

    _buildIcosphere(vertices, indices); // generate vertices

    // TODO: optimize the index buffer

    // container for our vertex data
    struct VertexAttributes {
        GLfloat x, y, z;    // x. y, z position
        GLfloat nx, ny, nz; // x, y, z normal vector
    };

    // create our sphere, generate normals (sphere starts at origin)
    VertexAttributes sphereVertices[indices.size()];

    for (size_t i{0ul}; i < indices.size(); ++i) {
        sphereVertices[i] = {
            vertices.at(i * 3),     vertices.at(i * 3 + 1),
            vertices.at(i * 3 + 2), // position
            vertices.at(i * 3),     vertices.at(i * 3 + 1),
            vertices.at(i * 3 + 2) // normal
        };
    }

    // order to connect the vertices in
    numVAOPoints = (GLsizei)indices.size();

    glBindVertexArray(vao); // vind sphere VAO

    // bind and fill vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(sphereVertices), sphereVertices,
                 GL_STATIC_DRAW);

    // doesn't matter which program we query (attribute layouts should
    // match)
    GLuint vPosAttribLocation{_wireShader->getAttributeLocation("vPos")};
    GLuint vNormAttribLocation{_wireShader->getAttributeLocation("vNorm")};

    // enable generic attribute arrays and define access patterns
    glEnableVertexAttribArray(vPosAttribLocation);
    glVertexAttribPointer(vPosAttribLocation, 3, GL_FLOAT, GL_FALSE,
                          sizeof(VertexAttributes), GL_NONE);

    glEnableVertexAttribArray(vNormAttribLocation);
    glVertexAttribPointer(vNormAttribLocation, 3, GL_FLOAT, GL_FALSE,
                          sizeof(VertexAttributes),
                          (void*)(sizeof(GLfloat) * 3));

    // bind and fill index buffer
    GLushort sphereIndices[indices.size()];
    for (int i = 0; i < indices.size(); ++i)
        sphereIndices[i] = indices.at(i);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(sphereIndices), sphereIndices,
                 GL_STATIC_DRAW);

    glBindVertexArray(GL_NONE); // unbind sphere VAO

    std::cout << "Sphere read into GPU memory with VAO/VBO/IBO " << vao << '/'
              << vbo << '/' << ibo << " & " << numVAOPoints << " points\n";
}

void Engine::_buildIcosphere(std::vector<GLfloat>& vertices,
                             std::vector<GLushort>& indices) {
    /* https://www.songho.ca/opengl/gl_sphere.html */

    // compute 12 shared vertices of icosahedron
    std::vector<GLfloat> tmpVertices{_generateIcosahedron()};

    // clear memory of prev arrays
    std::vector<GLfloat>().swap(vertices);
    std::vector<GLushort>().swap(indices);

    const GLfloat *v0, *v1, *v2, *v3, *v4, *v11; // vertex positions
    GLushort index{0u};

    // compute and add 20 triangles of icosahedron first
    v0 = &tmpVertices.at(0);       // 1st vertex
    v11 = &tmpVertices.at(11 * 3); // 12th vertex

    for (int i = 1; i <= 5; ++i) {
        // 4 vertices in the 2nd row
        v1 = &tmpVertices.at(i * 3);
        if (i < 5)
            v2 = &tmpVertices.at((i + 1) * 3);
        else
            v2 = &tmpVertices.at(3);

        v3 = &tmpVertices.at((i + 5) * 3);
        if ((i + 5) < 10)
            v4 = &tmpVertices.at((i + 6) * 3);
        else
            v4 = &tmpVertices.at(6 * 3);

        // add a triangle in 1st row
        _addVertices(v0, v1, v2, vertices);
        _addIndices(index, index + 1, index + 2, indices);

        // add 2 triangles in 2nd row
        _addVertices(v1, v3, v2, vertices);
        _addIndices(index + 3, index + 4, index + 5, indices);

        _addVertices(v2, v3, v4, vertices);
        _addIndices(index + 6, index + 7, index + 8, indices);

        // add a triangle in 3rd row
        _addVertices(v3, v11, v4, vertices);
        _addIndices(index + 9, index + 10, index + 11, indices);

        // next index
        index += 12;
    }

    // subdivide the icosahedron to get a more sphere-like object
    _subdivideIcosahedron(vertices, indices);
}

std::vector<GLfloat> Engine::_generateIcosahedron() {
    /* https://www.songho.ca/opengl/gl_sphere.html */

    // constants
    constexpr auto PI{pi<GLfloat>()};
    constexpr GLfloat H_ANGLE{glm::radians(72.f)}; // 72 degree = 360 / 5
    const GLfloat V_ANGLE{glm::atan(0.5f)};        // elevation = 26.565 degree
    constexpr GLfloat radius{1.f};                 // radius = 1

    std::vector<GLfloat> vertices(12ul * 3ul); // array of 12 vertices (x,y,z)
    size_t i1{0ul}, i2{0ul};                   // indices
    GLfloat z{0.f}, xy{0.f};                   // coords
    GLfloat hAngle1 =
        -PI / 2.f - H_ANGLE / 2.f; // start from -126 deg at 1st row
    GLfloat hAngle2 = -PI / 2.f;   // start from -90 deg at 2nd row

    // the first top vertex at (0, 0, r)
    vertices.at(0ul) = 0.f;
    vertices.at(1ul) = 0.f;
    vertices.at(2ul) = radius;

    // compute 10 vertices at 1st and 2nd rows
    for (size_t i{1ul}; i <= 5ul; ++i) {
        i1 = i * 3ul;         // index for 1st row
        i2 = (i + 5ul) * 3ul; // index for 2nd row

        z = radius * glm::sin(V_ANGLE);  // elevation
        xy = radius * glm::cos(V_ANGLE); // length on XY plane

        vertices.at(i1) = xy * glm::cos(hAngle1); // x
        vertices.at(i2) = xy * glm::cos(hAngle2);
        vertices.at(i1 + 1ul) = xy * glm::sin(hAngle1); // y
        vertices.at(i2 + 1ul) = xy * glm::sin(hAngle2);
        vertices.at(i1 + 2ul) = z; // z
        vertices.at(i2 + 2ul) = -z;

        // next horizontal angles
        hAngle1 += H_ANGLE;
        hAngle2 += H_ANGLE;
    }

    // the last bottom vertex at (0, 0, -r)
    i1 = 11ul * 3ul;
    vertices.at(i1) = 0.f;
    vertices.at(i1 + 1ul) = 0.f;
    vertices.at(i1 + 2ul) = -radius;

    return vertices;
}

void Engine::_subdivideIcosahedron(std::vector<GLfloat>& vertices,
                                   std::vector<GLushort>& indices) {
    /* https://www.songho.ca/opengl/gl_sphere.html */

    std::vector<GLfloat> tmpVertices;
    std::vector<GLushort> tmpIndices;
    const GLfloat *v1, *v2, *v3; // ptr to original vertices of a triangle
    GLfloat newV1[3ul], newV2[3ul], newV3[3ul]; // new vertex positions
    GLushort index{(GLushort)0u};

    // iterate all subdivision levels
    for (GLuint i{1u}; i <= SPHERE_SUBDIVISIONS; ++i) {
        // copy prev vertex/index arrays and clear
        tmpVertices = vertices;
        tmpIndices = indices;
        vertices.clear();
        indices.clear();
        index = (GLushort)0u;

        // perform subdivision for each triangle
        for (size_t j{0ul}; j < tmpIndices.size(); j += 3ul) {
            // get 3 vertices of a triangle
            v1 = &tmpVertices.at(tmpIndices.at(j) * 3ul);
            v2 = &tmpVertices.at(tmpIndices.at(j + 1ul) * 3ul);
            v3 = &tmpVertices.at(tmpIndices.at(j + 2ul) * 3ul);

            /* compute 3 new vertices by splitting half on each edge
                    v1
                   / \
            newV1 *---* newV3
                 / \ / \
               v2---*---v3
                  newV2
            */
            _computeHalfVertex(v1, v2, newV1);
            _computeHalfVertex(v2, v3, newV2);
            _computeHalfVertex(v1, v3, newV3);

            // add 4 new triangles to vertex array
            _addVertices(v1, newV1, newV3, vertices);
            _addVertices(newV1, v2, newV2, vertices);
            _addVertices(newV1, newV2, newV3, vertices);
            _addVertices(newV3, newV2, v3, vertices);

            // add indices of 4 new triangles
            _addIndices(index, index + (GLushort)1u, index + (GLushort)2u,
                        indices);
            _addIndices(index + (GLushort)3u, index + (GLushort)4u,
                        index + (GLushort)5u, indices);
            _addIndices(index + (GLushort)6u, index + (GLushort)7u,
                        index + (GLushort)8u, indices);
            _addIndices(index + (GLushort)9u, index + (GLushort)10u,
                        index + (GLushort)11u, indices);

            index += (GLushort)12u; // next index
        }
    }
}

void Engine::_computeHalfVertex(const GLfloat v1[3ul], const GLfloat v2[3ul],
                                GLfloat newV[3ul]) {
    /* https://www.songho.ca/opengl/gl_sphere.html */

    // add vectors
    newV[0ul] = v1[0ul] + v2[0ul]; // x
    newV[1ul] = v1[1ul] + v2[1ul]; // y
    newV[2ul] = v1[2ul] + v2[2ul]; // z

    // assume radius of 1
    GLfloat scale =
        1.f / glm::sqrt(newV[0ul] * newV[0ul] + newV[1ul] * newV[1ul] +
                        newV[2ul] * newV[2ul]);

    // scale/normalize them
    newV[0ul] *= scale;
    newV[1ul] *= scale;
    newV[2ul] *= scale;
}

void Engine::_addVertices(const GLfloat v1[3], const GLfloat v2[3],
                          const GLfloat v3[3], std::vector<GLfloat>& vertices) {
    vertices.push_back(v1[0ul]);
    vertices.push_back(v1[1ul]);
    vertices.push_back(v1[2ul]);

    vertices.push_back(v2[0ul]);
    vertices.push_back(v2[1ul]);
    vertices.push_back(v2[2ul]);

    vertices.push_back(v3[0ul]);
    vertices.push_back(v3[1ul]);
    vertices.push_back(v3[2ul]);
}

void Engine::_addIndices(const GLushort& i1, const GLushort& i2,
                         const GLushort& i3, std::vector<GLushort>& indices) {
    indices.push_back(i1);
    indices.push_back(i2);
    indices.push_back(i3);
}

void addToUniformBlock(GLvoid* buffer_ptr, const mat4& data) {
    memcpy(buffer_ptr, glm::value_ptr(data), sizeof(data));
}

void Engine::_sendSceneBlock(const mat4& model, const mat4& viewProjection,
                             const mat4& modelViewProjection,
                             const mat4& viewportMatrix,
                             const mat4& shadowViewProjection,
                             const vec3& eyePos) {
    // set up CPU-side buffer mirroring memory layout on GPU
    GLvoid* blockBuffer{malloc(_blockSizes[UBO_ID::SCENE])};

    memcpy(blockBuffer + _uniformOffsets[UBO_ID::SCENE].at(st 0u),
           glm::value_ptr(model), sizeof(model));
    memcpy(blockBuffer + _uniformOffsets[UBO_ID::SCENE].at(st 1u),
           glm::value_ptr(viewProjection), sizeof(viewProjection));
    memcpy(blockBuffer + _uniformOffsets[UBO_ID::SCENE].at(st 2u),
           glm::value_ptr(modelViewProjection), sizeof(modelViewProjection));
    memcpy(blockBuffer + _uniformOffsets[UBO_ID::SCENE].at(st 3u),
           glm::value_ptr(viewportMatrix), sizeof(viewportMatrix));
    memcpy(blockBuffer + _uniformOffsets[UBO_ID::SCENE].at(st 4u),
           glm::value_ptr(shadowViewProjection), sizeof(shadowViewProjection));
    memcpy(blockBuffer + _uniformOffsets[UBO_ID::SCENE].at(st 5u),
           glm::value_ptr(glm::vec2(_tessLevel)), sizeof(_tessLevel));
    memcpy(blockBuffer + _uniformOffsets[UBO_ID::SCENE].at(st 6u),
           glm::value_ptr(eyePos), sizeof(eyePos));
    memcpy(blockBuffer + _uniformOffsets[UBO_ID::SCENE].at(st 7u), &_wireframe,
           sizeof(_wireframe));
    memcpy(blockBuffer + _uniformOffsets[UBO_ID::SCENE].at(st 8u),
           &_controlPoints, sizeof(_controlPoints));

    // send buffer to GPU
    glBindBuffer(GL_UNIFORM_BUFFER, _ubos[UBO_ID::SCENE]);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, _blockSizes[UBO_ID::SCENE],
                    blockBuffer);

    free(blockBuffer);
    blockBuffer = nullptr;

    glBindBuffer(GL_UNIFORM_BUFFER, GL_NONE); // unbind
}

void Engine::_sendLightBlock(const vec4& lightPos, const vec3& lightAmb,
                             const vec3& lightDiff, const vec3& lightSpec,
                             const GLfloat& attenConst, const GLfloat& attenLin,
                             const GLfloat& attenQuad) {
    GLvoid* blockBuffer{malloc(_blockSizes[UBO_ID::LIGHT])};

    memcpy(blockBuffer + _uniformOffsets[UBO_ID::LIGHT].at(st 0u),
           &lightPos[st 0u], sizeof(vec4));
    memcpy(blockBuffer + _uniformOffsets[UBO_ID::LIGHT].at(st 1u),
           &lightAmb[st 0u], sizeof(vec3));
    memcpy(blockBuffer + _uniformOffsets[UBO_ID::LIGHT].at(st 2u),
           &lightDiff[st 0u], sizeof(vec3));
    memcpy(blockBuffer + _uniformOffsets[UBO_ID::LIGHT].at(st 3u),
           &lightSpec[st 0u], sizeof(vec3));
    memcpy(blockBuffer + _uniformOffsets[UBO_ID::LIGHT].at(st 4u), &attenConst,
           sizeof(GLfloat));
    memcpy(blockBuffer + _uniformOffsets[UBO_ID::LIGHT].at(st 5u), &attenLin,
           sizeof(GLfloat));
    memcpy(blockBuffer + _uniformOffsets[UBO_ID::LIGHT].at(st 6u), &attenQuad,
           sizeof(GLfloat));
    memcpy(blockBuffer + _uniformOffsets[UBO_ID::LIGHT].at(7), &_shadowBias,
           sizeof _shadowBias);
    memcpy(blockBuffer + _uniformOffsets[UBO_ID::LIGHT].at(8),
           &_doMultisampling, sizeof _doMultisampling);
    memcpy(blockBuffer + _uniformOffsets[UBO_ID::LIGHT].at(9),
           &_shadowMapSamples, sizeof _shadowMapSamples);

    // send buffer to GPU
    glBindBuffer(GL_UNIFORM_BUFFER, _ubos[UBO_ID::LIGHT]);
    glBufferData(GL_UNIFORM_BUFFER, _blockSizes[UBO_ID::LIGHT], blockBuffer,
                 GL_DYNAMIC_DRAW);

    free(blockBuffer);
    blockBuffer = nullptr;

    glBindBuffer(GL_UNIFORM_BUFFER, GL_NONE); // unbind
}

void Engine::_sendMaterialBlock(const vec3& materialAmb,
                                const vec3& materialDiff,
                                const vec3& materialSpec,
                                const GLfloat& shininess) {
    GLvoid* blockBuffer{malloc(_blockSizes[UBO_ID::MATERIAL])};

    memcpy(blockBuffer + _uniformOffsets[UBO_ID::MATERIAL].at(st 0u),
           &materialAmb[st 0u], sizeof(vec3));
    memcpy(blockBuffer + _uniformOffsets[UBO_ID::MATERIAL].at(st 1u),
           &materialDiff[st 0u], sizeof(vec3));
    memcpy(blockBuffer + _uniformOffsets[UBO_ID::MATERIAL].at(st 2u),
           &materialSpec[st 0u], sizeof(vec3));
    memcpy(blockBuffer + _uniformOffsets[UBO_ID::MATERIAL].at(st 3u),
           &shininess, sizeof(GLfloat));

    // send buffer to GPU
    glBindBuffer(GL_UNIFORM_BUFFER, _ubos[UBO_ID::MATERIAL]);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, _blockSizes[UBO_ID::MATERIAL],
                    blockBuffer);

    free(blockBuffer);
    blockBuffer = nullptr;

    glBindBuffer(GL_UNIFORM_BUFFER, GL_NONE); // unbind
}

// *****************************************************************************
// Debug stuff
/* https://stackoverflow.com/a/18067245/10323091 */

const char* ETB_GL_DEBUG_SOURCE_STR(GLenum source) {
    static const char* sources[] = {
        "API",         "Window System", "Shader Compiler", "Third Party",
        "Application", "Other",         "Unknown"};

    int str_idx = glm::min((GLuint64)(source - GL_DEBUG_SOURCE_API),
                           sizeof(sources) / sizeof(const char*));

    return sources[str_idx];
}

const char* ETB_GL_DEBUG_TYPE_STR(GLenum type) {
    static const char* types[] = {
        "Error",       "Deprecated Behavior", "Undefined Behavior",
        "Portability", "Performance",         "Other",
        "Unknown"};

    int str_idx = glm::min((GLuint64)(type - GL_DEBUG_TYPE_ERROR),
                           sizeof(types) / sizeof(const char*));

    return types[str_idx];
}

const char* ETB_GL_DEBUG_SEVERITY_STR(GLenum severity) {
    static const char* severities[] = {"High", "Medium", "Low", "Unknown"};

    int str_idx = glm::min((GLuint64)(severity - GL_DEBUG_SEVERITY_HIGH),
                           sizeof(severities) / sizeof(const char*));

    return severities[str_idx];
}

GLuint ETB_GL_DEBUG_SEVERITY_COLOR(GLenum severity) {
    static GLuint severities[] = {
        0xff0000ff, // High (Red)
        0xff00ffff, // Med  (Yellow)
        0xff00ff00, // Low  (Green)
        0xffffffff  // ???  (White)
    };

    int col_idx = glm::min((GLuint64)(severity - GL_DEBUG_SEVERITY_HIGH),
                           sizeof(severities) / sizeof(GLuint));

    return severities[col_idx];
}

void ETB_GL_ERROR_CALLBACK(GLenum source, GLenum type, GLuint id,
                           GLenum severity, GLsizei length,
                           const GLchar* message, GLvoid* userParam) {
    fprintf(stderr, "OpenGL Error:\n");
    fprintf(stderr, "=============\n");

    fprintf(stderr, " Object ID: ");
    fprintf(stderr, "%d\n", id);

    fprintf(stderr, " Severity:  ");
    fprintf(stderr, "%s\n", ETB_GL_DEBUG_SEVERITY_STR(severity));

    fprintf(stderr, " Type:      ");
    fprintf(stderr, "%s\n", ETB_GL_DEBUG_TYPE_STR(type));

    fprintf(stderr, " Source:    ");
    fprintf(stderr, "%s\n", ETB_GL_DEBUG_SOURCE_STR(source));

    fprintf(stderr, " Message:   ");
    fprintf(stderr, "%s\n\n", message);

    // Trigger a breakpoint in gDEBugger...
    glFinish();
}

// *****************************************************************************
// GLFW callback function overloads

void keyboard_callback(GLFWwindow* window, GLint key, GLint scancode,
                       GLint action, GLint mods) {
    auto engine{(Engine*)glfwGetWindowUserPointer(window)};

    // pass the key and action through to the engine
    if (engine)
        engine->handleKeyEvent(key, action);
}

void cursor_callback(GLFWwindow* window, GLdouble x, GLdouble y) {
    auto engine{(Engine*)glfwGetWindowUserPointer(window)};

    // pass the cursor position through to the engine
    if (engine)
        engine->handleCursorPositionEvent({x, y});
}

void mouse_button_callback(GLFWwindow* window, GLint button, GLint action,
                           GLint mods) {
    auto engine{(Engine*)glfwGetWindowUserPointer(window)};

    // pass the mouse button and action through to the engine
    if (engine)
        engine->handleMouseButtonEvent(button, action);
}

void scroll_callback(GLFWwindow* window, GLdouble xOffset, GLdouble yOffset) {
    auto engine{(Engine*)glfwGetWindowUserPointer(window)};

    // pass the scroll offset through to the engine
    if (engine)
        engine->handleScrollEvent({xOffset, yOffset});
}

void error_callback(GLint error, const GLchar* description) {
    std::cerr << "Error [ " << error << " ] : " << description << '\n';
}
