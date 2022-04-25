#ifndef _MGL_H
#define _MGL_H

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <g2d/opengl/color.h>

typedef union wh {
    // width and height of the window
    struct {
        GLint w, h;
    };
    // width and height variable in one 64-bit integer to make
    // modifying their values atomic
    uint64_t wh;
} width_height;

typedef struct gl_context {
    GLFWwindow* window;
    width_height wh;

    // key callback function, forwarded in OpenGL key listener callback
    void (*key_callback)(struct gl_context*, int, int, int, int);

    // for user to specify, will never be modified, can be accessed by callbacks
    void* user_data;
} gl_context_t;


int gl_init(gl_context_t* context, GLint width, GLint height);

void gl_exit(gl_context_t* context);


void gl_set_bg_color(color_t color);

void gl_register_key_callback(gl_context_t* c,
        void (*callback)(gl_context_t*, int key, int action, int scancode,
            int mods));

void gl_clear(gl_context_t* c);

void gl_render(gl_context_t* c);

int gl_should_exit(gl_context_t* c);

#endif /* _MGL_H */
