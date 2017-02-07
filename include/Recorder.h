// Audio class /////////////////////////////////////////////////////////////////////

#ifndef AUDIO_H
#define AUDIO_H

#include <vector>
#include <portaudio/portaudio.h>

using namespace std;

class Audio
{	
	
public:

	enum{ RECORDING_MODE, PLAYING_MODE };
	int currentMode; // switch recording/playing depending on GUI function

	static const int SAMPLE_RATE = 44100;
	static const int FRAMES_PER_BUFFER = 256;
	static const int RECORDER_BUFFER_SIZE = 65535;

	PaStreamParameters	inputParameters;
	PaStream 			*stream;
	PaError 			err;
	bool				errorRaised;
	// int captured[CAPTURE_SIZE];
	int capturedLen;
	bool callbackJustExecuted;
	vector<int> captured;

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
void start();
void update();
void setRecordingMode();
void setPlayingMode();
void refresh();
void stop();
void terminate();
void raiseError(PaError e);

};

#endif