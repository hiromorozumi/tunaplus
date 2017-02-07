// Tap class implementation /////////////////////////////////////////////////////////////

#include "Tap.h"

using namespace std;

Tap::Tap()
{
	restartTimer();
	elapsedTime = 0;
	currentAverageLength = 0;
	currentBPM = -1;
	setBPM(120.0);
}

// returns time in milliseconds x 100 (100000 = 1 sec)
unsigned long long Tap::getTimeMs64()
{
	/* Windows */
	FILETIME ft;
	LARGE_INTEGER li;

	/* Get the amount of 100 nano seconds intervals elapsed since January 1, 1601 (UTC) and copy it
	* to a LARGE_INTEGER structure. */
	GetSystemTimeAsFileTime(&ft);
	li.LowPart = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;
	
	unsigned long long ret = li.QuadPart;
	ret -= 116444736000000000LL; /* Convert from file time to UNIX epoch time. */
	ret /= 100; /* From 100 nano seconds (10^-7) to 1 millisecond (10^-3) intervals */
	
	return ret;
}

void Tap::restartTimer()
{
	startedTime = getTimeMs64();
}

long Tap::getElapsedTime()
{
	long elapsedTimeToReturn = (long)(getTimeMs64() - startedTime);
	if(elapsedTimeToReturn<0) elapsedTimeToReturn = 0;
	return elapsedTimeToReturn;
}

void Tap::pushHistory(double value)
{
	noteLength.push_back(value);
	if(noteLength.size()>5)
		noteLength.pop_front();
}

int Tap::getHistorySize()
{ return (int)(noteLength.size()); }

void Tap::tap()
{
	// get elapsed time since last time 'z' was pressed
	elapsedTime = getElapsedTime();
	
	if(elapsedTime > 170000) // more than 1.7 seconds, will restart
								// this time don't have to push into history
	{
		clearHistory();
		cout << "Listening now...\n";
		currentBPM = -1;
	}
	else
	{
		pushHistory((double)elapsedTime);
		currentAverageLength = getAverage();
		currentBPM = 6000000.0 / currentAverageLength;
		
		// report...
		// cout << "nNotes: " << noteLength.size();
		// cout << "\tavg: " << currentAverageLength;
		// cout << "\tBPM: " << currentBPM << endl;
	}
		
	// reset timer
	restartTimer();
}

bool Tap::isBPMReady()
{
	if (currentBPM < 0)
		return false;
	else
		return true;
}

double Tap::getBPM()
{	return currentBPM; }

double Tap::setBPM(double bpm)
{	currentBPM = bpm; }

void Tap::clearHistory()
{
	noteLength.clear();
}

double Tap::getAverage()
{
	double accum = 0;
	int nNotes = noteLength.size();
	for(int i=0; i<nNotes; i++)
		accum += noteLength[i];
	return ( accum / (double)nNotes );
}

