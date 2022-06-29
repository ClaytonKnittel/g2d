#ifndef _G2D_H
#define _G2D_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#ifdef USE_OPENGL

#include <g2d/opengl/gl.h>

#elif defined(USE_METAL)

#include <g2d/metal/metal.h>

#endif

#ifdef __cplusplus
}
#endif /* __cplusplus */

typedef struct g2d_context {
	uint32_t width;
	uint32_t height;

	uint32_t target_framerate;

	//color_t background_color;

	const char* title;
} g2d_context_t;

/*
 */
int g2d_start();

int call_test_ray();

#endif /* _G2D_H */
