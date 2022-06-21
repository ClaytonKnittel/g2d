
#include <cassert>
#include <iostream>

#include <g2d/g2d.h>

#ifdef USE_AUDIO_TOOLBOX
#include <g2d/audio_toolbox/audio_toolbox.h>
#else
#include <g2d/openal/al.h>
#endif

#include <check.h>

static uint32_t f2i(float f) {
	union cvt_f2i {
		int int_val;
		float float_val;
	} tmp;
	tmp.float_val = f;
	return tmp.int_val;
}

int
main()
{
	//return call_test_ray();
	//return al_test();
	//return audio_toolbox_test();

	gl_context_t gl_ctx;
	gl_init(&gl_ctx, 640, 480);

	gl_set_bg_color(gen_color(240, 230, 220, 255));

	drawable d;
	uint32_t vertices[] = {
		f2i(-0.5), f2i(-0.5), gen_color(140, 80, 90, 255),
		f2i(-0.5), f2i(0.5), gen_color(0, 0, 100, 255),
		f2i(0.5), f2i(0.5), gen_color(0, 100, 0, 255),
		f2i(0.5), f2i(0.5), gen_color(0, 100, 0, 255),
		f2i(0.5), f2i(-0.5), gen_color(10, 10, 10, 255),
		f2i(-0.5), f2i(-0.5), gen_color(140, 80, 90, 255),
	};
	gl_load_static_monochrome_drawable(&d, vertices, 6);

	program p;
	gl_load_program(&p,
			"/home/claytonknittel/Documents/g2d/src/opengl/res/two.vs",
			"/home/claytonknittel/Documents/g2d/src/opengl/res/two.fs");

	while (!gl_should_exit(&gl_ctx)) {
		gl_clear(&gl_ctx);

		gl_use_program(&p);
		gl_draw(&d);

		gl_render(&gl_ctx);
	}

	gl_unload_program(&p);
	gl_unload_static_monochrome_drawable(&d);

	gl_exit(&gl_ctx);
}

