
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

#define DURATION 5
#define FREQUENCY 48000lu
#define ENV_PCT 20

static double
waveform(double t, double f)
{
	/*double res = 0;
	for (uint64_t i = 1; i < 24; i++) {
		res += 1. / i * sin(2 * M_PI * t * f * i);
		res += -1. / (i + 1) * sin(2 * M_PI * (t * f + .25) * i);
	}
	return .38 + res / 2;*/
	double res = 0;
	for (uint64_t i = 1; i < 60; i++) {
		uint64_t newi = 2 * i - 1;
		res += (i % 2 == 1 ? -1. : 1) / (newi * newi) * sin(2 * M_PI * t * f * newi);
		//res += -1. / (i + 1) * sin(2 * M_PI * (t * f + .25) * i);
	}
	return res * 3 / 4;
}

static int16_t*
gen_buf()
{
	int16_t* buf = (int16_t*) malloc(DURATION * FREQUENCY * 2 * sizeof(uint16_t));

	for (uint64_t i = 0; i < DURATION * FREQUENCY; i++) {
		float env = (i < DURATION * FREQUENCY / ENV_PCT) ? (float) i / (DURATION * FREQUENCY / ENV_PCT) :
			(i > (ENV_PCT - 1) * DURATION * FREQUENCY / ENV_PCT) ? (float) (DURATION * FREQUENCY - i) / (DURATION * FREQUENCY / ENV_PCT) :
			1.f;

		double t;
		if (i < DURATION * FREQUENCY / 3) {
			t = (double) i / FREQUENCY;
		}
		else if (i < 2 * DURATION * FREQUENCY / 3) {
			uint64_t newi = i - DURATION * FREQUENCY / 3;
			t = (double) i / FREQUENCY + ((double) newi * newi * 3 / (4 * DURATION * FREQUENCY * FREQUENCY));

			if (i == 2 * DURATION * FREQUENCY / 3 - 1) {
				printf("%g\n", t);
			}
		}
		else {
			double offset = -DURATION / 4.0;
			t = (double) i * 3 / (2 * FREQUENCY) + offset;

			if (i == 2 * DURATION * FREQUENCY / 3) {
				printf("%g\n", t);
			}
		}

#define SCALE 16000
#define BASE_FREQ 30.
		int16_t val = 0 + SCALE * env * waveform(t, BASE_FREQ);
		int16_t val2 = val;//0 + SCALE * env * waveform(t, 5 * BASE_FREQ / 3);
		int16_t val3 = 0;//0 + SCALE * env * waveform(t, 11 * BASE_FREQ / 5);
		int16_t val4 = 0;//0 + SCALE * env * waveform(t, 5 * BASE_FREQ / 4);

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
	alBufferData(buffer, AL_FORMAT_MONO16, buf, 2 * DURATION * FREQUENCY * sizeof(int16_t), FREQUENCY);

	ALuint source;
	alGenSources(1, &source);
	alSourcef(source, AL_PITCH, 1);
	alSourcef(source, AL_GAIN, 1);
	alSource3f(source, AL_POSITION, 0, 0, 0);
	alSource3f(source, AL_VELOCITY, 0, 0, 0);
	alSourcei(source, AL_LOOPING, false);
	alSourcei(source, AL_BUFFER, buffer);

	alSourcePlay(source);

	for (;;) {
		ALint state;
		alGetSourcei(source, AL_SOURCE_STATE, &state);

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

