/**
 * @file ShaderProgram.hpp
 * @author Vincent Marias [@qtf0x]
 * @date 04/02/2023
 *
 * @brief CSCI 544 ~ Advanced Computer Graphics [Spring 2023]
 *        A2 ~ Noisy Teapotahedron
 */

#ifndef TEAPOTAHEDRON_SHADER_PROGRAM_HPP
#define TEAPOTAHEDRON_SHADER_PROGRAM_HPP

#include <string>
#include <unordered_map>
#include <vector>

#include <glad/glad.h> // for GL types

class ShaderProgram {
  public:
    ShaderProgram() : _handle{0u}, _linked{GL_FALSE} {}
    ~ShaderProgram();

    // make it non-copyable
    ShaderProgram(const ShaderProgram&) = delete;
    ShaderProgram& operator=(const ShaderProgram&) = delete;

    /**
     * @brief Read GLSL source code from shader file into GPU memory, compile
     * it, attach it to this shader object (creates a shader object if one
     * doesn't exist yet). Prints compile error log from GPU driver if
     * compilation fails.
     *
     * @param filename GLSL shader source file
     * @param type GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
     */
    void compileShader(const std::string& filename, GLenum type);

    /**
     * @brief link together the currently attached shader objects, then detach
     * and delete them
     *
     */
    void linkProgram();

    /**
     * @brief set this as the currently enabled shader program for draw calls
     */
    void useProgram();

    /**
     * @brief retrieves the size of a given uniform block, as well as the
     * offsets within the block for a list of uniforms
     *
     * @param [in] blockName public name of the block as defined in the shader
     * @param [in] uniformNames names of the uniforms to search for
     * @param [out] blockSize size in bytes of the block in GPU memory
     * @param [out] offset list of offsets, indices correspond to uniformNames
     */
    void queryUniformBlock(const std::string& blockName,
                           const std::vector<const GLchar*>& uniformNames,
                           GLint& blockSize, std::vector<GLint>& offset);

    GLuint getAttributeLocation(const std::string& name);

    GLuint getUniformBlockLocation(const std::string& name);

    /**
     * @brief set which subroutine to use, by it's name w/in the shader
     *
     * @param name name of the subroutine to activate
     * @param type location of subroutine (GL_VERTEX_SHADER or
     * GL_FRAGMENT_SHADER)
     */
    void setSubroutineActive(const std::string& name, GLenum type);

  private:
    GLuint _handle;    // shader program GPU handle
    GLboolean _linked; // has this program already been linked?

    // hash maps to cache GPU handles for shader attributes + uniform blocks
    std::unordered_map<std::string, GLuint> _attributeLocations;
    std::unordered_map<std::string, GLuint> _uniformBlockLocations;

    /**
     * @brief find all currently attached shader objects, detach then delete
     * them
     */
    void detachAndDeleteShaderObjects();
};

#endif // TEAPOTAHEDRON_SHADER_PROGRAM_HPP
