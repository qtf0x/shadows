/**
 * @file ShaderProgram.cpp
 * @author Vincent Marias [@qtf0x]
 * @date 04/02/2023
 *
 * @brief CSCI 544 ~ Advanced Computer Graphics [Spring 2023]
 *        A2 ~ Noisy Teapotahedron
 */

#include <fstream>  // for ifstream
#include <iostream> // for cerr
#include <iterator> // for istreambuf_iterator
#include <vector>

#include "ShaderProgram.hpp"

// *****************************************************************************
// Public

ShaderProgram::~ShaderProgram() {
    if (_handle == 0u)
        return;

    detachAndDeleteShaderObjects();

    glDeleteProgram(_handle);
}

void ShaderProgram::compileShader(const std::string& filename, GLenum type) {
    if (_handle == 0u)
        _handle = glCreateProgram();

    std::ifstream fin{filename};

    /* https://stackoverflow.com/a/116177 */
    // "slurp" the shader source code into a string
    std::string shaderSrc{std::istreambuf_iterator<GLchar>{fin}, {}};

    fin.close();

    // create shader object
    GLuint shaderHandle{glCreateShader(type)};

    // send shader source code to GPU
    const GLchar* code = shaderSrc.c_str();
    glShaderSource(shaderHandle, 1, &code, nullptr);

    // compile the shader
    glCompileShader(shaderHandle);

    // check for errors
    GLint result{0};
    glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        // compile failed, get log
        GLint length{0};
        glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &length);

        std::string errLog(length, ' ');
        glGetShaderInfoLog(shaderHandle, length, nullptr, &errLog[(size_t)0u]);

        std::cerr << filename << ": shader compilation failed\n"
                  << errLog << '\n';
    } else {
        GLint length{0};
        glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &length);

        std::string errLog(length, ' ');
        glGetShaderInfoLog(shaderHandle, length, nullptr, &errLog[(size_t)0u]);

        std::cout << filename << ": shader compilation succeeded\n"
                  << errLog << '\n';

        glAttachShader(_handle, shaderHandle); // compile succeeded, attach
    }
}

void ShaderProgram::linkProgram() {
    if (_linked)
        return;

    glLinkProgram(_handle);

    detachAndDeleteShaderObjects();

    _linked = GL_TRUE;
}

void ShaderProgram::useProgram() { glUseProgram(_handle); }

void ShaderProgram::queryUniformBlock(
    const std::string& blockName,
    const std::vector<const GLchar*>& uniformNames, GLint& blockSize,
    std::vector<GLint>& offset) {
    GLsizei numUniforms{(GLsizei)uniformNames.size()};

    // get the index of the uniform block
    auto blockIndex{getUniformBlockLocation(blockName)};

    // get the size of the block
    glGetActiveUniformBlockiv(_handle, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE,
                              &blockSize);

    // find the index of each variable within the block
    GLuint indices[numUniforms];
    glGetUniformIndices(_handle, numUniforms, uniformNames.data(), indices);

    // query for the offset of each variable within the block
    offset.resize(uniformNames.size()); // allocate space for offsets
    glGetActiveUniformsiv(_handle, numUniforms, indices, GL_UNIFORM_OFFSET,
                          offset.data());
}

GLuint ShaderProgram::getAttributeLocation(const std::string& name) {
    if (_attributeLocations.find(name) == _attributeLocations.end())
        _attributeLocations.insert(
            {name, glGetAttribLocation(_handle, name.c_str())});

    return _attributeLocations.at(name);
}

GLuint ShaderProgram::getUniformBlockLocation(const std::string& name) {
    if (_uniformBlockLocations.find(name) == _uniformBlockLocations.end())
        _uniformBlockLocations.insert(
            {name, glGetUniformBlockIndex(_handle, name.c_str())});

    return _uniformBlockLocations.at(name);
}

void ShaderProgram::setSubroutineActive(const std::string& name, GLenum type) {
    GLuint index{glGetSubroutineIndex(_handle, type, name.c_str())};

    glUniformSubroutinesuiv(type, 1, &index);
}

// *****************************************************************************
// Private

void ShaderProgram::detachAndDeleteShaderObjects() {
    // query GPU for number of attached shaders
    GLint numShaders = 0;
    glGetProgramiv(_handle, GL_ATTACHED_SHADERS, &numShaders);

    // query GPU for list of attached shaders
    std::vector<GLuint> shaderHandles(numShaders);
    glGetAttachedShaders(_handle, numShaders, NULL, shaderHandles.data());

    // detach and delete each attached shader object
    for (const auto& shader : shaderHandles) {
        glDetachShader(_handle, shader);
        glDeleteShader(shader);
    }
}
