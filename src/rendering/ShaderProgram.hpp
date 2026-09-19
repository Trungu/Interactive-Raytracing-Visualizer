#pragma once

#define GLFW_INCLUDE_GLCOREARB
#include <GLFW/glfw3.h>

/**
 * A function for compiling a shader given its type and source code
 *
 *
 * @param type an integer type `GLenum` used to identify the type of shader to compile
 * @param source a pointer to the shader's source code, stored in a C-style string
 * @return an unsigned integer type `GLuint` that holds the shader's ID, used to identify the shader
 */
GLuint compileShader(GLenum type, const char *source);

/**
 * A function for linking compiled vertex and fragment shaders into a shader program
 *
 * @param vertexShader an unsigned integer type `GLuint` identifying the compiled vertex shader
 * @param fragmentShader an unsigned integer type `GLuint` identifying the compiled fragment shader
 * @return an unsigned integer type `GLuint` that holds the program's ID, or 0 if linking fails
 */
GLuint linkProgram(GLuint vertexShader, GLuint fragmentShader);
