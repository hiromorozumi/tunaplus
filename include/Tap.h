#ifndef TAP_H
#define TAP_H

#include <windows.h>
#include <iostream>
#include <deque>

unsigned long long getTimeMs64();
void restartTimer();
long getElapsedTime();

class Tap
{
public:

	std::deque<double> noteLength;
	unsigned long long startedTime;
	long elapsedTime;
	double currentAverageLength;
	double currentBPM;

	Tap();
	~Tap(){}
	
	void tap();
	bool isBPMReady();
	double getBPM();
	double setBPM(double bpm);
	
	unsigned long long getTimeMs64();
	void restartTimer();
	long getElapsedTime();
	void pushHistory(double value);
	int getHistorySize();
	void clearHistory();
	double getAverage();
	
protected:

};

#endif