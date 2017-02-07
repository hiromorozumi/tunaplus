// Pitch class /////////////////////////////////////////////////////////////////////

#ifndef PITCH_H
#define PITCH_H

#include <Windows.h>
#include <vector>
#include <deque>
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include <fftw/fftw3.h>
#include "Audio.h"

class Pitch
{
public:

	static constexpr double TWOPI = 6.283185307179586476925286766559;
	static constexpr int N = 65535;
	static constexpr double sampleRate = 44100;
	static constexpr int HISTORY_SIZE = 2;

	fftw_complex *in, *out;
	fftw_plan p;
	Audio* audio;
	double magnitude[65535];
	
	double frequencyTable[108];
	double semitoneRatio;
	double middleC;
	double cZero;
	
	double currentFrequency;
	int currentNoteNumber;
	double currentNoteNumberDouble;
	std::string currentNoteName;
	double currentNoteCenterFrequency;
	double currentFreqencyDelta;
	double currentNoteDeltaPercent;
	double currentPeakAmplitude;
	double thresholdAmplitude;

	std::deque<double> frequencyHistory;
	std::deque<int> deltaPercentHistory;
	
	double fundamentalFreqAtHalf;
	double fundamentalFreqAtOneThird;
	bool fundamentalFoundAtHalf;
	bool fundamentalFoundAtOneThird;
	
	Pitch();
	~Pitch(){}
	void initialize();
	void refresh();
	void bindAudio(Audio* r);
	double detect();
	double tau(double x);
	void buildFreqencyTable();
	double noteToFrequency(int noteNum);
	double frequencyToNote(double freq);
	double getCurrentNoteDeltaPercent();
	std::string noteToString(int noteNum);
	bool loudEnough();
	std::string getCurrentNoteName();
	void pushHistory();
	double getFrequencyAvg();
	double getDeltaPercentAvg();
};

#endif