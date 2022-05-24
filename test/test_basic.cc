
#include <cassert>
#include <iostream>

#include <g2d/g2d.h>

#ifdef USE_AUDIO_TOOLBOX
#include <g2d/audio_toolbox/audio_toolbox.h>
#else
#include <g2d/openal/al.h>
#endif

#include <check.h>

int
main()
{
	//return call_test_ray();
	//return al_test();
	return audio_toolbox_test();
}

