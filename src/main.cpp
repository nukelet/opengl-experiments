#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cstdio>
#include <cstdlib>

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void process_input(GLFWwindow *window)
{
    static bool polygon_mode = false;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    } else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    } else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
}

void print_shader_info_log(GLuint shader)
{
    static char log_buffer[512];
    glGetShaderInfoLog(shader, sizeof(log_buffer), NULL, log_buffer);
    printf("Shader (%u) compilation failed:\n", shader);
    printf("%s", log_buffer);
}

void print_program_info_log(GLuint program)
{
    static char log_buffer[512];
    glGetProgramInfoLog(program, sizeof(log_buffer), NULL, log_buffer);
    printf("Program (%u) linking failed:\n", program);
    printf("%s", log_buffer);
}

GLuint gen_shader_program(const char *vshader_source, const char *fshader_source)
{
    int compilation_success;
    bool error = false;
    char log[512];

    GLuint vertex_shader;
    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vshader_source, NULL);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compilation_success); // query the shader compilation status
    if (!compilation_success) {
        print_shader_info_log(vertex_shader);
        error = true;
    }

    unsigned int fragment_shader;
    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fshader_source, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compilation_success);
    if (!compilation_success) {
        print_shader_info_log(fragment_shader);
        error = true;
    }

    unsigned int shader_program;
    shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    glGetProgramiv(shader_program, GL_LINK_STATUS, &compilation_success);
    if (!compilation_success) {
        print_program_info_log(shader_program);
        error = true;
    }

    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    if (error) {
        return 0;
    } else {
        return shader_program;
    }
}


const char *vertex_shader_source =
"#version 330 core\n"
"layout (location = 0) in vec3 aPos;\n"
"void main()\n"
"{\n"
"gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
"}\0";

const char *fragment_shader_source_orange =
"#version 330 core\n"
"out vec4 FragColor;\n"
"void main() {\n"
"FragColor = vec4(1.0, 0.5, 0.2, 1.0);"
"}\0";

const char *fragment_shader_source_yellow =
"#version 330 core\n"
"out vec4 FragColor;\n"
"void main() {\n"
"FragColor = vec4(1.0f, 1.0f, 0.0f, 1.0f);"
"}\0";

void check_error(GLFWwindow *window)
{
    GLuint err = glGetError();
    if (err != GL_NO_ERROR) {
        printf("Error: 0x%x\n", err);
        glfwTerminate();
        exit(1);
    }
}

int main(int argc, char **argv)
{
    glfwInit();

    // Tell GLFW we want to use OpenGL 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    // Core Profile: smaller subset of OpenGL, without
    // backwards-compatibility stuff
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(800, 600, "OpenGL", NULL, NULL);
    if (!window) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    } else {
        printf("Successfully created window\n");
    }
    glfwMakeContextCurrent(window);

    // we pass the (os-specific) function to load the address
    // of the OpenGL function pointers; glfwGetProcAddress is 
    // provided by GLFW
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return -1;
    }

    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    GLuint shader_program_orange =
        gen_shader_program(vertex_shader_source, fragment_shader_source_orange);

    GLuint shader_program_yellow =
        gen_shader_program(vertex_shader_source, fragment_shader_source_yellow);

    if (shader_program_orange == 0 || shader_program_yellow == 0) {
        return -1;
    }

    unsigned int vao;
    glGenVertexArrays(1, &vao);

    /**
     *  Let's go over what we did so far:
     *  1. We created a VBO (vertex buffer object), which is just a buffer containing
     *     vertex info (in this case, the vertex positions) 
     *  2. We bound that VBO using glBindBuffer, and then copied our data (from the vertices
     *     buffer to the GPU memory)
     *  3. We compiled vertex and fragment shaders and linked them into a shader
     *     program
     *  4. We created a VAO to store our attribute pointers and shit (not sure how this works)
     */


    unsigned int vbo;
    glGenBuffers(1, &vbo);
    unsigned int ebo; // element buffer object
    glGenBuffers(1, &ebo);

    glBindVertexArray(vao);

    GLfloat vertices_a[] = {
        0.0f, 0.0f, 0.0f,
        0.5f, 0.0f, 0.0f,
        0.25f, 0.5f, 0.0f,
        -0.5f, 0.0f, 0.0f,
        -0.25f, 0.5f, 0.0f,
    };

    GLuint indices[] = {
        0, 1, 2,
        0, 3, 4, 
    };

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_a), vertices_a, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    GLenum gl_error;

    while (!glfwWindowShouldClose(window))  {
        process_input(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_program_yellow);
        glBindVertexArray(vao);
        // glDrawArrays(GL_TRIANGLES, 0, 6);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        // glBindVertexArray(0);


        // gl_error = glGetError();
        // if (gl_error != GL_NO_ERROR) {
        //     printf("Render error: 0x%x\n", gl_error);
        //     break;
        // }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}
