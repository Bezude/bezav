#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "linmath.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ObjParser.hpp>
#include "lodepng.h"
#include <stdio.h>


static void error_callback(int error, const char* description)
{
    fprintf(stderr, "Error: %s\n", description);
}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}
static void shaderErrorCheck(GLuint component, bool isProgram) {
    GLint success = 0;
    if(isProgram) {
        glGetProgramiv(component, GL_LINK_STATUS, &success);
    }
    else {
        glGetShaderiv(component, GL_COMPILE_STATUS, &success);
    }
    if(!success) {
        GLint logSize = 0;
        if(isProgram) {
            glGetProgramiv(component, GL_INFO_LOG_LENGTH, &logSize);
        }
        else {
            glGetShaderiv(component, GL_INFO_LOG_LENGTH, &logSize);
        }
        std::vector<GLchar> errorLog(logSize);
        if(isProgram) {
            glGetProgramInfoLog(component, logSize, NULL, &errorLog[0]);
        }
        else {
            glGetShaderInfoLog(component, logSize, NULL, &errorLog[0]);
        }
        for (std::vector<char>::const_iterator i = errorLog.begin(); i != errorLog.end(); ++i) {
            std::cout << *i;
        }
        std::cout << '\n';
    }
}
static void errorCheck(const std::string id) {
    GLenum err = GL_NO_ERROR;
    while((err = glGetError()) != GL_NO_ERROR)
    {
        std::cout << id << ' ' << err << '\n';
      //printf("%s 0x%x", id.c_str(), err);
    }
}
int main(int argc, char* argv[])
{
    ObjParser op("../obj/test.obj");
    int vertCount = op.getVertCount();
    std::cout << vertCount << '\n';
    int elemCount = op.getElemCount();
    std::cout << elemCount << '\n';
    float* vertData = new float[vertCount*5];
    unsigned short* elemData = new unsigned short[elemCount*3];
    op.fillVertArray(vertData, true, false);
    op.fillElementArray(elemData);

    unsigned char* texData;
    unsigned texWidth;
    unsigned texHeight;
    const char* texFilename = "../img/catSing.png";
    lodepng_decode32_file(&texData, &texWidth, &texHeight, texFilename);

    std::ifstream vfs("../src/shaders/vert.vert");
    std::ifstream ffs("../src/shaders/frag.frag");
    const char* vertex_shader_text;
    const char* fragment_shader_text;
    if(!vfs.is_open() || !ffs.is_open()) {
        exit(EXIT_FAILURE);
    }
    std::string vertstring((std::istreambuf_iterator<char>(vfs)), std::istreambuf_iterator<char>());
    vertex_shader_text = vertstring.c_str();
    std::string fragstring((std::istreambuf_iterator<char>(ffs)), std::istreambuf_iterator<char>());
    fragment_shader_text = fragstring.c_str();



    GLFWwindow* window;
    GLuint vertex_buffer, vertex_array, element_buffer, textureID, vertex_shader, fragment_shader, program;
    GLint mvp_location, vpos_location, uv_location, sampler_location;
    glfwSetErrorCallback(error_callback);
    if (!glfwInit())
        exit(EXIT_FAILURE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    window = glfwCreateWindow(640, 480, "bezav", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }
    glfwSetKeyCallback(window, key_callback);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval(1);
    // NOTE: OpenGL error checks have been omitted for brevity
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    errorCheck("Depth");

    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertCount * 5, vertData, GL_STATIC_DRAW);
    errorCheck("vertex buffer");

    glGenBuffers(1, &element_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned short) * elemCount * 3, elemData, GL_STATIC_DRAW);
    errorCheck("element buffer");

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texWidth, texHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
    errorCheck("texture");

    // When MAGnifying the image (no bigger mipmap available), use LINEAR filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // When MINifying the image, use a LINEAR blend of two mipmaps, each filtered LINEARLY too
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    // Generate mipmaps, by the way.
    glGenerateMipmap(GL_TEXTURE_2D);
    errorCheck("texture params");

    vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);
    shaderErrorCheck(vertex_shader, false);
    errorCheck("vert shader");

    fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);
    shaderErrorCheck(fragment_shader, false);
    errorCheck("frag shader");

    program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);
    shaderErrorCheck(program, true);
    errorCheck("shader program link");

    mvp_location = glGetUniformLocation(program, "MVP");
    sampler_location = glGetUniformLocation(program, "myTextureSampler");
    vpos_location = glGetAttribLocation(program, "vPos");
    uv_location = glGetAttribLocation(program, "vertexUV");
    errorCheck("shader variable gets");

    glEnableVertexAttribArray(vpos_location);
    errorCheck("vpos attribute enable");
    glVertexAttribPointer(vpos_location, 3, GL_FLOAT, GL_FALSE,
                          sizeof(float) * 5, (void*) 0);
    errorCheck("vpos attribute pointer");
    glEnableVertexAttribArray(uv_location);
    errorCheck("uv attribute enable");
    glVertexAttribPointer(uv_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(float) * 5, (void*) (sizeof(float) * 3));
    errorCheck("uv attribute pointer");

    

    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;
        mat4x4 m, p, mvp;
        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        mat4x4_identity(m);
        //mat4x4_rotate_Z(m, m, (float) glfwGetTime());
        mat4x4_translate(m, -1, 0, 0);
        mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        mat4x4_mul(mvp, p, m);
        glUseProgram(program);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
        glDrawElements(GL_TRIANGLES, elemCount, GL_UNSIGNED_SHORT, (void*)0 );
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
    exit(EXIT_SUCCESS);
}
