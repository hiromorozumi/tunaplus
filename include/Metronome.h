#ifndef METRONOME_H
#define METRONOME_H

#include <vector>
#include <cmath>
#include <iostream>
#include "OSC.h"

struct Note
{
	bool isRest;
	double freq;
	float gain;
	int length;
};

// GUI class /////////////////////////////////////////////////////////////////////


class Metronome
{

public:

	static constexpr double SAMPLE_RATE = 44100;
	static constexpr float VOLUME_SCALING_FACTOR = 0.60f;
	
	// std::vector<Note> sequence;
	Note sequence[20];
	OSC osc;
	double highNoteFreq;
	double lowNoteFreq;
	double highNoteVolume;
	double lowNoteVolume;
	double highNoteLength;
	double lowNoteLength;
	double currentBPM;
	int oneNoteLength;
	int nBeats;
	int nElementsInSequence;
	
	bool metronomeActive;
	int currentElement;
	int counter;
	float metronomeVolume;
	bool flashRequested;
	int blinkerColor;

	Metronome();
	~Metronome(){}

	void setBPM(double newBPM);
	double getBPM();
	void setNBeats(int nbeats);
	int getNBeats();
	void buildSequence();
	void clearSequence();
	float getOneOutputSample();
	void start();
	void stop();
	void setVolume(float v);
	float getVolume();
	bool blinkerFlashRequested();
	int getBlinkerFlashColor();
	
	// DEBUG
	void report();
};

#endif