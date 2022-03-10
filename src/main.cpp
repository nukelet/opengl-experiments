#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <utils/shader.hpp>

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>

#include <signal.h>
#include <stdlib.h>

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

void check_error()
{
    GLuint err = glGetError();
    if (err != GL_NO_ERROR) {
        printf("Error: 0x%x\n", err);
        glfwTerminate();
        exit(1);
    }
}

struct __attribute__((packed)) vertex_data {
    float pos[3];
    float rgb[3];
};

void vertex_data_set(struct vertex_data* p, 
                        float x, float y, float z,
                        float r, float g, float b)
{
    float data[6] = {x, y, z, r, g, b};
    memcpy(p, data, sizeof(*p));
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

    const char *vshader = "basics/shaders/vshader.glsl";
    const char *fshader = "basics/shaders/fshader.glsl";
    ShaderProgram shader_prog(vshader, fshader);

    GLuint indices[] = {
        0, 1, 2,
        0, 3, 4, 
    };

    vertex_data vertices[3];
    vertex_data_set(&vertices[0], -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f);
    vertex_data_set(&vertices[1], 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f);
    vertex_data_set(&vertices[2], 0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f);
    printf("vertex_data size: %lu\n", sizeof(struct vertex_data));
    struct vertex_data *ptr = vertices;
    printf("(%f %f %f %f %f %f)\n", ptr->pos[0], ptr->pos[1], ptr->pos[2],
                                    ptr->rgb[0], ptr->rgb[1], ptr->rgb[2]);

    /**
     *  Let's go over what we're doing:
     *  1. We create a VBO (vertex buffer object), which is just a buffer containing
     *     vertex info (in this case, the vertex positions) 
     *  2. We bind that VBO using glBindBuffer, and then copy our data (from the vertices
     *     buffer to the GPU memory)
     *  3. We compile vertex and fragment shaders and link them into a shader
     *     program
     *  4. We create a VAO to store our attribute pointers and shit (not sure how this works)
     */

    unsigned int vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    unsigned int vbo;
    glGenBuffers(1, &vbo);
    unsigned int ebo; // element buffer object
    glGenBuffers(1, &ebo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);

    // position attribute
    // this (void *) typecast needs to be there for legacy reasons... go figure
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) 0);
    glEnableVertexAttribArray(0);
    
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*) (3 * sizeof(float)));
    glEnableVertexAttribArray(1); 

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    GLenum gl_error;

    while (!glfwWindowShouldClose(window))  {
        process_input(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // const char *green_uniform_name = "input_color";
        // GLint green_uniform = glGetUniformLocation(shader_prog.id, green_uniform_name);

        // if (green_uniform == -1) {
        //     printf("Couldn't find uniform: %s\n", green_uniform_name);
        // }

        shader_prog.use();
        // glUniform4f(green_uniform, 0.0f, green_value, 0.0f, 1.0f);

        // float time = glfwGetTime();
        // float position = (sin(time)/2.0f);
        // shader_prog.setFloat("offset", position);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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
