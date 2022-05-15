#ifndef _G2D_UTIL_H
#define _G2D_UTIL_H

#include <stdint.h>
#include <stdio.h>

#ifdef USE_OPENGL

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

GLenum glCheckError_(const char *file, int line);

#define glCheckError() glCheckError_(__FILE__, __LINE__)

#endif /* USE_OPENGL */

#endif /* _G2D_UTIL_H */
