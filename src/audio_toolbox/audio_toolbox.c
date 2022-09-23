
#include <AudioToolbox/AudioToolbox.h>

#include <g2d/audio_toolbox/audio_toolbox.h>

static void CheckError(OSStatus error, const char *operation) {
  if (error == noErr) {
    return;
  }

  char errorString[20];
  *(UInt32 *) (errorString + 1) =
      CFSwapInt32HostToBig(error);  // we have 4 bytes and we put them in
                                    // Big-endian ordering. 1st byte the biggest
  if (isprint(errorString[1]) && isprint(errorString[2]) &&
      isprint(errorString[3]) && isprint(errorString[4])) {
    errorString[0] = errorString[5] = '\'';
    errorString[6] = '\0';
  } else {
    sprintf(errorString, "%d", (int) error);
  }
  fprintf(stderr, "Error: %s (%s)\n", operation, errorString);
  exit(1);
}

#define SINE_FREQUENCY   440.0
#define SAMPLE_FREQUENCY 44100.0

typedef struct CallBackData {
  double startingFrameCount;
} CallBackData;

static OSStatus SineWaveRenderProc(void *inRefCon,
                                   AudioUnitRenderActionFlags *ioActionFlags,
                                   const AudioTimeStamp *inTimeStamp,
                                   UInt32 inBusNumber, UInt32 inNumberFrames,
                                   AudioBufferList *ioData) {
  CallBackData *pCallBackData = (CallBackData *) inRefCon;
  double frameInPeriod = pCallBackData->startingFrameCount;

  // This is the number of samples/frames we need to cover a period/cycle
  // of the SINE waveform, if we sample/frame SAMPLE_FREQUENCY samples/frames
  // per second.
  double sinePeriodLengthInFrames = SAMPLE_FREQUENCY / SINE_FREQUENCY;

  int frame = 0;
  // we have to fill in all the frames that the callback caller specifies.
  for (frame = 0; frame < inNumberFrames; frame++) {
    // We populate the current frame audio data value/sample with the result of
    // `sin` function
    Float32 sampleValue =
        (Float32) sin(2 * M_PI * (frameInPeriod / sinePeriodLengthInFrames));
    sampleValue *= 0.5;

    // I go to the first buffer and the value of `mData` and save it into a
    // `Float32 *` var. Hence +data+ is a pointer to the beginning of the audio
    // data buffer.

    // first for the +left+ channel
    Float32 *data = (Float32 *) ioData->mBuffers[0].mData;
    (data)[frame] = sampleValue;

    // then for the right channel too
    data = (Float32 *) ioData->mBuffers[1].mData;
    (data)[frame] = sampleValue;

    frameInPeriod += 1.0;

    if (frameInPeriod >= sinePeriodLengthInFrames) {
      frameInPeriod = 0;  // returns it back to 0?
    }
  }  // for ()

  pCallBackData->startingFrameCount = frameInPeriod;

  return noErr;
}

static void BuildAudioComponentDescription(
    AudioComponentDescription *oAudioComponentDescription) {
  oAudioComponentDescription->componentType = kAudioUnitType_Output;
  oAudioComponentDescription->componentSubType =
      kAudioUnitSubType_DefaultOutput;
  oAudioComponentDescription->componentManufacturer =
      kAudioUnitManufacturer_Apple;
}

static AudioComponent FindAudioComponent(
    AudioComponentDescription iAudioComponentDescription) {
  AudioComponent outputAudioUnit =
      AudioComponentFindNext(NULL, &iAudioComponentDescription);
  if (!outputAudioUnit) {
    fprintf(stderr, "Cannot get the audio unit matching given description\n");
    exit(1);
  }
  return outputAudioUnit;
}

static void StopAudioOutputUnit(AudioUnit inAudioUnit) {
  CheckError(AudioOutputUnitStop(inAudioUnit),
             "Stopping the Output Audio Unit");
  CheckError(AudioUnitUninitialize(inAudioUnit),
             "Uninitializing the Output Audio Unit");
  CheckError(AudioComponentInstanceDispose(inAudioUnit),
             "Disposing the Output Audio Unit");
}

int audio_toolbox_test() {
  AudioComponentDescription outputAudioComponentDescription = { 0 };

  BuildAudioComponentDescription(&outputAudioComponentDescription);

  AudioComponent outputAudioComponent =
      FindAudioComponent(outputAudioComponentDescription);

  AudioUnit outputAudioUnit;
  CheckError(AudioComponentInstanceNew(outputAudioComponent, &outputAudioUnit),
             "Instantiating the Output Audio Unit");

  CallBackData callBackData = { 0 };

  AURenderCallbackStruct input;
  input.inputProc = SineWaveRenderProc;
  input.inputProcRefCon = &callBackData;
  CheckError(AudioUnitSetProperty(
                 outputAudioUnit, kAudioUnitProperty_SetRenderCallback,
                 kAudioUnitScope_Input, 0, &input, sizeof(input)),
             "Setting the Output Audio Unit property for rendering callback");

  CheckError(AudioUnitInitialize(outputAudioUnit), "Audio Unit Initialize");

  CheckError(AudioOutputUnitStart(outputAudioUnit),
             "Starting the Output Audio Unit");

  sleep(5);

  StopAudioOutputUnit(outputAudioUnit);

  return 0;
}
