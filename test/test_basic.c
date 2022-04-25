
#include <stdio.h>

#include <g2d/gl/gl.h>

#include <check.h>

int
main()
{
	gl_context_t ctx;
	gl_init(&ctx, 800, 600);

	gl_set_bg_color(0xffd3b6ff);

	while (!gl_should_exit(&ctx)) {
		gl_clear(&ctx);

		gl_render(&ctx);
	}

	gl_exit(&ctx);

	return 0;
}

