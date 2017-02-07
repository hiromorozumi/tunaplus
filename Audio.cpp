// Audio class implementation /////////////////////////////////////////////////////////////

#include "Audio.h"

using namespace std;

Audio::Audio()
{
	initialize();
}

// real callback function
int Audio::audioCallback(
								const void *inputBuffer,
								void *outputBuffer,
								unsigned long framesPerBuffer,
								const PaStreamCallbackTimeInfo* timeInfo,
								PaStreamCallbackFlags statusFlags
							)
{
	int *in = (int*) inputBuffer;
	float *out = (float*)(outputBuffer);
	int nBufferFrames = (int)framesPerBuffer;
	
	(void) timeInfo;
	(void) statusFlags;
	
	int writePos = 0;
	capturedLen = 0;
	
	if(currentMode == RECORDING_MODE)
	{
		for(int i=0; i<nBufferFrames; i++)
		{
			//capturedData[writePos] = *in;
			captured.push_back(*in);
			in++; // advance pointer
			writePos++;
			capturedLen++;
		}
	}
	else if(currentMode == METRONOME_MODE)
	{
		for(int i=0; i<nBufferFrames; i++)
		{
			*out = metronome->getOneOutputSample();
			out++;
		}
	}
	
	// flag to inform interface main loop on data waiting to be processed
	callbackJustExecuted = true;
	
	return paContinue;
}

void Audio::initialize()
{	
	// clear recorder data buffer with all zeros
	captured.clear();
	int val = 0;
	
	for(int i=0; i<RECORDER_BUFFER_SIZE; i++)
		captured.push_back(val);

	errorRaised = false;
	err = Pa_Initialize();
	if( err != paNoError ) raiseError(err);

	inputParameters.device = Pa_GetDefaultInputDevice();
	if(inputParameters.device == paNoDevice)
	{
		cout << "Error: No default input device.\n";
	}
	
	// set up input parameters
	inputParameters.channelCount = 1; // mono input
	inputParameters.sampleFormat = paInt32; // 32-bit integer
	inputParameters.suggestedLatency = Pa_GetDeviceInfo( inputParameters.device )->defaultLowInputLatency;
	inputParameters.hostApiSpecificStreamInfo = NULL;
	
	// set up output parameters
    outputParameters.device = Pa_GetDefaultOutputDevice(); // use default device
    if (outputParameters.device == paNoDevice)
	{
      cout << "Error: No default output device." << endl;
    }
    outputParameters.channelCount = 1; // mono
    outputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;
	
	// open audio stream for recording using default device
	err = Pa_OpenStream(
		&stream,
		&inputParameters,
		&outputParameters, // no output params
		SAMPLE_RATE,
		FRAMES_PER_BUFFER,
		paClipOff,
		audioCallbackReroute,
		this
	);
	
	if( err != paNoError ) raiseError(err);
	
	// initialize...
	bool captureLoopDone = false;
	int maxRawAudioSize = SAMPLE_RATE * 10;
	callbackJustExecuted = false;
	
	cout << "Audio iniialized!\n";
}

void Audio::bindMetronome(Metronome* m)
{
	metronome = m;
}

void Audio::start()
{
	// start the audio stream!
	err = Pa_StartStream(stream);
	if(err != paNoError) raiseError(err);
}

// check if there is incoming data waiting to be processed
// if so - see if the captured data vector has exceeded 32768-sample length
// if exceeded, remove the oldest data and make it 32768-sample size again
void Audio::update()
{
	if(callbackJustExecuted)
	{
		if(captured.size()>RECORDER_BUFFER_SIZE)
		{
			int nRemove = captured.size() - RECORDER_BUFFER_SIZE;
			captured.erase(captured.begin(), captured.begin()+nRemove);
			// cout << "captured data size = " << captured.size() << endl;
		}
	
		callbackJustExecuted = false;
	}
}

// in recording mode, callback will only record
void Audio::setRecordingMode()
{
	currentMode = RECORDING_MODE;
}

// in recording mode, callback will only play
void Audio::setMetronomeMode()
{
	currentMode = METRONOME_MODE;
}

// clears record buffer etc.
void Audio::refresh()
{
	captured.clear();
}

void Audio::stop()
{
	// stop the audio stream
	err = Pa_StopStream(stream);
	if(err != paNoError) raiseError(err);	
}

void Audio::terminate()
{
	err = Pa_CloseStream( stream );
	if(err != paNoError) raiseError(err);	
	err = Pa_Terminate( );
	if(err != paNoError) raiseError(err);	
}

void Audio::raiseError(PaError e)
{
	cout << "Port audio error:\n";
	string errText = Pa_GetErrorText(e);
	cout << errText << endl;
	errorRaised = true;
}