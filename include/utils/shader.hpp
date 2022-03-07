#ifndef _SHADER_HPP
#define _SHADER_HPP

#include <glad/glad.h>

class Shader {
    private:
    GLuint id;

    public:
    
    /**
     * @brief Construct a new Shader program
     * 
     * @param vshader_path  path to the vertex shader
     * @param fshader_path  path to the fragment shader 
     */
    Shader(const char *vshader_path, const char *fshader_path);

    /**
     * @brief Use the current shader
     */
    void use();
};

#endif