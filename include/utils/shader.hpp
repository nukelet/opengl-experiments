#ifndef _SHADER_HPP
#define _SHADER_HPP

#include <string>
#include <glad/glad.h>

class ShaderProgram {
    private:

    struct Shader {
        GLuint id;
        std::string filepath;
        std::string src;
    }; 

    public:
    GLuint id;

    void compile_shader(Shader shader);
    void print_shader_info_log(Shader shader);
    void print_program_info_log();

    Shader vshader;
    Shader fshader;

    public:
    
    /**
     * @brief Construct a new Shader program
     * 
     * @param vshader_path  path to the vertex shader
     * @param fshader_path  path to the fragment shader 
     */
    ShaderProgram(const char *vshader_path, const char *fshader_path);

    /**
     * @brief Use the current shader
     */
    void use();
    void setFloat(const char *name, float value);
};

#endif