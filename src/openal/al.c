
#include <g2d/openal/al.h>

#include <math.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

//#include <AudioToolbox/AudioToolbox.h>

// Include relative paths for OpenAL headers for portability.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"

#include "al.h"
#include "alc.h"

#define FREQUENCY 48000lu

static int16_t*
gen_buf()
{
	int16_t* buf = (int16_t*) malloc(FREQUENCY * 2 * sizeof(uint16_t));

	for (uint64_t i = 0; i < FREQUENCY; i++) {
		float env = (i < FREQUENCY / 10) ? (float) i / (FREQUENCY / 10) :
			(i > 9 * FREQUENCY / 10) ? (float) (FREQUENCY - i) / (FREQUENCY / 10) :
			1.f;

		double t;
		if (i < FREQUENCY / 3) {
			t = (double) i / FREQUENCY;
		}
		else if (i < 2 * FREQUENCY / 3) {
			uint64_t newi = i - FREQUENCY / 3;
			t = (double) i / FREQUENCY + ((double) newi * newi * 3 / (4 * FREQUENCY * FREQUENCY));
		}
		else {
			double offset = -1 / 4.0;
			t = (double) i * 3 / (2 * FREQUENCY) + offset;
		}

		int16_t val = 0 + 8192 * env * sin(t * 2 * 3.14159 * 440);
		int16_t val2 = 0 + 8192 * env * sin(t * 2 * 3.14159 * 733.333);
		int16_t val3 = 0 + 8192 * env * sin(t * 2 * 3.14159 * 968);
		int16_t val4 = 0 + 8192 * env * sin(t * 2 * 3.14159 * 550);

		buf[2 * i + 0] = val + val3;
		buf[2 * i + 1] = val2 + val4;
	}

	return buf;
}

void
check_err(ALCdevice* device)
{
	ALCenum error = alcGetError(device);
	switch (error) {
		case ALC_INVALID_VALUE:
			printf("ALC_INVALID_VALUE: an invalid value was passed to an OpenAL function\n");
			break;
		case ALC_INVALID_DEVICE:
			printf("ALC_INVALID_DEVICE: a bad device was passed to an OpenAL function\n");
			break;
		case ALC_INVALID_CONTEXT:
			printf("ALC_INVALID_CONTEXT: a bad context was passed to an OpenAL function\n");
			break;
		case ALC_INVALID_ENUM:
			printf("ALC_INVALID_ENUM: an unknown enum value was passed to an OpenAL function\n");
			break;
		case ALC_OUT_OF_MEMORY:
			printf("ALC_OUT_OF_MEMORY: an unknown enum value was passed to an OpenAL function\n");
			break;
		default:
			break;
	}
}

int
al_test()
{
	const ALCchar* devices;
	devices = alcGetString(NULL, ALC_DEVICE_SPECIFIER);

	while (*devices != '\0') {
		const ALCchar* device = devices;
		ALCsizei len = strlen(device);
		devices = devices + len + 1;

		printf("Device: %s\n", device);
	}

	ALCdevice* device = alcOpenDevice(NULL);
	check_err(device);

	ALCcontext* context = alcCreateContext(device, NULL);

	if (!alcMakeContextCurrent(context)) {
		printf("Failed to make context current\n");
		return -1;
	}

	ALuint buffer;
	alGenBuffers(1, &buffer);

	int16_t* buf = gen_buf();
	alBufferData(buffer, AL_FORMAT_MONO16, buf, 2 * FREQUENCY * sizeof(int16_t), FREQUENCY);

	ALuint source;
	alGenSources(1, &source);
	alSourcef(source, AL_PITCH, 1);
	alSourcef(source, AL_GAIN, 1);
	alSource3f(source, AL_POSITION, 1, 0, .4);
	alSource3f(source, AL_VELOCITY, 0, 0, 0);
	alSourcei(source, AL_LOOPING, false);
	alSourcei(source, AL_BUFFER, buffer);

	alSourcePlay(source);

	float x = 1;
	for (;;) {
		ALint state;
		alGetSourcei(source, AL_SOURCE_STATE, &state);
		x -= .0001;
		alSource3f(source, AL_POSITION, x, 0, .4);

		if (state == AL_STOPPED) {
			break;
		}

		usleep(100);
	}

	free(buf);
	alDeleteSources(1, &source);
	alDeleteBuffers(1, &buffer);

	alcMakeContextCurrent(NULL);
	alcDestroyContext(context);
	alcCloseDevice(device);

	return 0;
}

#pragma GCC diagnostic pop

