#include <utils/shader.hpp>
#include <fstream>
#include <iostream>
#include <sstream>

void ShaderProgram::print_shader_info_log(Shader shader)
{
    static char log_buffer[1024];
    glGetShaderInfoLog(shader.id, sizeof(log_buffer), NULL, log_buffer);
    printf("Shader (%s) compilation failed:\n", shader.filepath.c_str());
    printf("%s", log_buffer);
}

void ShaderProgram::print_program_info_log()
{
    static char log_buffer[1024];
    glGetProgramInfoLog(id, sizeof(log_buffer), NULL, log_buffer);
    printf("Program linking failed:\n");
    printf("%s", log_buffer);
}

void ShaderProgram::compile_shader(Shader shader)
{
    GLint success;
    const char *src = shader.src.c_str();
    glShaderSource(shader.id, 1, &src, NULL);
    glCompileShader(shader.id);
    glGetShaderiv(shader.id, GL_COMPILE_STATUS, &success);
    if (!success) {
        print_shader_info_log(shader);
    }
}

ShaderProgram::ShaderProgram(const char *vshader_path, const char *fshader_path)
{
    // ugly hack to get the full path to the src/shaders folder
    // SHADER_DIR is exported by CMake as a preprocessor env variable
    #ifdef SHADER_DIR 
    char path_buf[1024];
    snprintf(path_buf, 1024, "%s/%s", SHADER_DIR, vshader_path);
    std::ifstream vshader_file(path_buf);
    snprintf(path_buf, 1024, "%s/%s", SHADER_DIR, fshader_path);
    std::ifstream fshader_file(path_buf);
    #else
    std::ifstream vshader_file(vshader_path);
    std::ifstream fshader_file(fshader_path);
    #endif

    if (vshader_file.fail()) {
        printf("Failed to open shader: %s\n", vshader_path);
        return;
    } else if (fshader_file.fail()) {
        printf("Failed to open shader: %s\n", fshader_path);
        return;
    }

    std::stringstream vshader_buf, fshader_buf;
    vshader_buf << vshader_file.rdbuf();
    fshader_buf << fshader_file.rdbuf();

    vshader.id = glCreateShader(GL_VERTEX_SHADER);
    vshader.filepath = std::string(vshader_path);
    vshader.src = vshader_buf.str();

    fshader.id = glCreateShader(GL_FRAGMENT_SHADER);
    fshader.filepath = std::string(fshader_path);
    fshader.src = fshader_buf.str();

    compile_shader(vshader);
    compile_shader(fshader);
    
    GLint success;

    id = glCreateProgram();
    glAttachShader(id, vshader.id);
    glAttachShader(id, fshader.id);
    glLinkProgram(id);
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
        print_program_info_log();
    }

    glDeleteShader(vshader.id);
    glDeleteShader(fshader.id);
}

void ShaderProgram::use()
{
    glUseProgram(id);
}

void ShaderProgram::setFloat(const char *name, float value)
{
    GLint location = glGetUniformLocation(id, name);
    if (location == -1) {
        // printf("Error setting value for \"%s\": uniform not found\n", name);
        return;
    }
    glUniform1f(location, value);
}