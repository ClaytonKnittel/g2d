
#include <stdio.h>

#include <g2d/gl/gl.h>

#include <check.h>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

int
main()
{
	gl_context_t ctx;
	gl_init(&ctx, 800, 600);

	gl_set_bg_color(0xffd3b6ff);

	int counter = 0;

	while (!gl_should_exit(&ctx)) {
		gl_clear(&ctx);

		gl_render(&ctx);
		glfwPollEvents();
	}

	gl_exit(&ctx);

	return 0;
}

