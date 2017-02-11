// Audio class /////////////////////////////////////////////////////////////////////

#ifndef AUDIO_H
#define AUDIO_H

#include <iostream>
#include <vector>
#include <portaudio/portaudio.h>
#include "Metronome.h"

class Audio
{	
	
public:

	Metronome* metronome;

	enum{ RECORDING_MODE, METRONOME_MODE };
	int currentMode; // switch recording/playing depending on GUI function

	static constexpr int SAMPLE_RATE = 44100;
	static constexpr int FRAMES_PER_BUFFER = 256;
	static constexpr int RECORDER_BUFFER_SIZE = 65535;

	PaStreamParameters	inputParameters;
	PaStreamParameters  outputParameters;
	PaStream 			*stream;
	PaError 			err;
	bool				errorRaised;
	// int captured[CAPTURE_SIZE];
	int capturedLen;
	bool callbackJustExecuted;
	std::vector<int> captured;

// constructor
Audio();
~Audio(){}

// real callback function
int audioCallback(
								const void *inputBuffer,
								void *outputBuffer,
								unsigned long framesPerBuffer,
								const PaStreamCallbackTimeInfo* timeInfo,
								PaStreamCallbackFlags statusFlags
							);

// portaudio will call this function on callback. then reroute to real callback func.
static int audioCallbackReroute(
		const 
		void *input, void *output,
		unsigned long frameCount,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags,
		void *userData )
{
		return ((Audio*)userData)
			->audioCallback(input, output, frameCount, timeInfo, statusFlags);
}

void initialize();
void bindMetronome(Metronome* m);
void start();
void update();
void setRecordingMode();
void setMetronomeMode();
void refresh();
void stop();
void terminate();
void raiseError(PaError e);
bool streamStateOkay();

};

#endif