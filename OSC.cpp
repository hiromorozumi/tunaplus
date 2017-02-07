/// OSC class - Implementation /////////////////////////

#include <iostream>
#include <cmath>
#include "OSC.h"

using namespace std;

const int OSC::OSC_TABLE_SIZE = 4096;
const int OSC::ENV_TABLE_SIZE = 1024;
const double OSC::OSC_SAMPLE_RATE = 44100.0;
const float OSC::TWO_PI = 6.283185307;

OSC::OSC()
{
	table.resize(OSC_TABLE_SIZE); // wave table (vector)
	
	tableType = 0; // FORCE setTable() to rewrite wavetable
	setTable(1); // default - set up a square table
	tableType = 1;
	yFlip = 1.0f;
	phase = 0.0;
	increment = 0.0;
	freq = 10.0; // not to set to zero to safeguard
	adjustedFreq = 0;
	detune = 0;
	gain = 0.5f; // default gain
	
	resting = false;
	forceSilenceAtBeginning = false;
	
	nAttackFrames = 0;
	nPeakFrames = 3000;
	nDecayFrames = 0;
	nEnvFrames = nAttackFrames + nPeakFrames + nDecayFrames;
	decayStartPos = nAttackFrames + nPeakFrames;
	
	peakLevel = 0.9f;
	sustainLevel = 0.5f;
	decayAmount = peakLevel - sustainLevel;
	envPos = 0;
	envADfinished = false;
	
	nReleaseFrames = 0;
	releasePos = 0;
	envRfinished = true;
	
	beefUp = false;
	beefUpFactor = 1.0f;
	compRatio = 4.0f;
	compThreshold = 0.90f;
	
	popGuardCount = 0;
	lastAmp = 0.0f;
	
	// initialize history table
	clearHistory();
}

OSC::~OSC()
{}

void OSC::setTable(int type)
{
	// DEBUG
	//if(tableType==type)
	//	cout << "NO CHANGE TO WAVEFORM! YEY!!\n";
	
	// if requested type is the currently set type
	// don't have to make any change... skip
	if(tableType==type)
		return;
	
	tableType = type;
	
	switch(type)
	{
		float maxAmp;
		int oneCycleFrames;
		
		// sine table
		case 0:
			maxAmp = 0.99f;
			for(int i=0; i<OSC_TABLE_SIZE; i++)
			{
				table[i] = sin( TWO_PI * (static_cast<float>(i) / static_cast<float>(OSC_TABLE_SIZE) ) ) * maxAmp;
			}
			break;
			
		// square table
		case 1:
		
			for(int i=0; i<OSC_TABLE_SIZE/2; i++)
			{
				table[i] = 0.80f;
			}
			for(int i=OSC_TABLE_SIZE/2; i<OSC_TABLE_SIZE; i++)
			{
				table[i] = -0.80f;
			}
			break;
		
		// sawthooth wave
		case 2:
			for(int i=0; i<OSC_TABLE_SIZE; i++)
				table[i] = -0.99f  + (static_cast<float>(i) / static_cast<float>(OSC_TABLE_SIZE)) * 1.98f;
			break;

		// triangle wave
		case 3:
			for(int i=0; i<OSC_TABLE_SIZE/2; i++)
			{
				int index = OSC_TABLE_SIZE/4 + i;
				table[index] = -0.99f  + (static_cast<float>(i) / static_cast<float>(OSC_TABLE_SIZE/2)) * 1.98f;
			}				
			for(int i=OSC_TABLE_SIZE/2; i<OSC_TABLE_SIZE; i++)
			{
				int index = OSC_TABLE_SIZE/4 + i;
				if(index>=OSC_TABLE_SIZE) index -= OSC_TABLE_SIZE;
				table[index] = 0.99f  - (static_cast<float>(i-OSC_TABLE_SIZE/2) / static_cast<float>(OSC_TABLE_SIZE/2)) * 1.98f;
			}
			break;

		// default is SQUARE wave...
		default:
			for(int i=0; i<OSC_TABLE_SIZE/2; i++)
			{
				table[i] = -0.80f;
			}
			for(int i=OSC_TABLE_SIZE/2; i<OSC_TABLE_SIZE; i++)
			{
				table[i] = 0.80f;
			}
			break;
			
	}
	
	// limit...
	for(int i=0; i<OSC_TABLE_SIZE; i++)
	{
		if(table[i]>0.99f) table[i] = 0.99f;
		else if(table[i] < -0.99f) table[i] = -0.99f;
	}	
	
}

void OSC::setGain(float g)
	{ gain = g; }

float OSC::getGain()
	{ return gain; }

void OSC::advanceEnvelope()
{
	if(!resting) // currently playing a note
	{
		if(!envADfinished)
		{
			envPos++;
			if(envPos >= nEnvFrames)
				envADfinished = true;
		}
	}
	else // you are on a rest now
	{
		if(!envRfinished)
		{
			releasePos++;
			if(releasePos >= nReleaseFrames)
			{
				envRfinished = true;
			}
		}
	}
}

// go back to the beginning of envelope
void OSC::refreshEnvelope()
{
	envPos = 0;
	envADfinished = false;
	releasePos = 0;
	envRfinished = false;
}

float OSC::getEnvelopeOutput()
{
	float output;
	
	if(!resting) // when you're not on a rest - playing a note now
	{
		if(envPos < nAttackFrames) // in attack stage
		{
			output = peakLevel * 
			(static_cast<float>(envPos) / static_cast<float>(nAttackFrames));
		}
		else if(envPos < (nAttackFrames + nPeakFrames)) // in peak stage
		{
			output = peakLevel;
		}
		else if(envPos < nEnvFrames) // in decay stage
		{
			output = peakLevel - decayAmount * ( static_cast<float>(envPos - decayStartPos) / static_cast<float> (nDecayFrames) );
		}
		else if(envADfinished) // in sustain stage
		{
			output = sustainLevel;
		}
	}
	
	// if resting flag is on, means you're in release stage
	if(resting)
	{
		if(!envRfinished && !forceSilenceAtBeginning)
			output = sustainLevel * ( static_cast<float>(nReleaseFrames - releasePos) / static_cast<float>(nReleaseFrames) );
		else
		{
			output = 0.0f;
			phase = 0; // reset phase for next note!
		}
	}
	
	return output;
}

void OSC::setToRest()
{
	resting = true;
}

void OSC::confirmFirstNoteIsRest()
{
	forceSilenceAtBeginning = true;
}

void OSC::advance()
{
	// advance on the sample table
	phase += increment;
	
	adjustedFreq = freq;
	
	while(phase >= OSC_TABLE_SIZE)
	{
		phase -= OSC_TABLE_SIZE;
	}
	
	// advance envelope also
	advanceEnvelope();
}

//
//	use THIS ONE! - it's all inclusive of all the below methods
//
void OSC::setNewNote(double newFreq)
{
	forceSilenceAtBeginning = false;
	setFrequency(newFreq);
	initializePhase();
	refreshEnvelope();
	resting = false;
	
	// enable pop-guarding...
	popGuardCount = 60;
}

// set the frequency and phase increment at once
void OSC::setFrequency(double noteFreq)
{
	freq = noteFreq;
	adjustedFreq = freq;
	setIncrement(freq);
}

// set the phase increment to travel across wave table every frame
void OSC::setIncrement(double noteFreq)
{
	// calculate with detune
	adjustedFreq = noteFreq + detune;

	// finally, set the phase increment
	increment = ( static_cast<double>(OSC_TABLE_SIZE) / ( OSC_SAMPLE_RATE / adjustedFreq ) );
	
	if(increment < 0)
		increment = 0;
}

void OSC::setAttackTime(int attackTimeMS)
{
	nAttackFrames = static_cast<int> (OSC_SAMPLE_RATE * attackTimeMS / 1000.0);
	readjustEnvParams();
}

void OSC::setPeakTime(int peakTimeMS)
{
	nPeakFrames = static_cast<int> (OSC_SAMPLE_RATE * peakTimeMS / 1000.0);
	readjustEnvParams();
}

void OSC::setDecayTime(int decayTimeMS)
{
	nDecayFrames = static_cast<int> (OSC_SAMPLE_RATE * decayTimeMS / 1000.0);
	readjustEnvParams();
}

void OSC::setReleaseTime(int releaseTimeMS)
{ 
	nReleaseFrames = static_cast<int> (OSC_SAMPLE_RATE * releaseTimeMS / 1000.0);
	readjustEnvParams();
}
	
void OSC::setPeakLevel(float peakLV)
{
	peakLevel = peakLV;
	readjustEnvParams();
}

void OSC::setSustainLevel(float sustainLV)
{
	sustainLevel = sustainLV;
	readjustEnvParams();
}	

void OSC::setEnvelope(int attackTimeMS, int peakTimeMS, int decayTimeMS, int releaseTimeMS,
						float peakLV, float sustainLV)
{
	setAttackTime(attackTimeMS);
	setPeakTime(peakTimeMS);
	setDecayTime(decayTimeMS);
	setReleaseTime(releaseTimeMS);
	setPeakLevel(peakLV);
	setSustainLevel(sustainLV);
	readjustEnvParams();
}

void OSC::readjustEnvParams()
{
	nEnvFrames = nAttackFrames + nPeakFrames + nDecayFrames;
	decayStartPos = nAttackFrames + nPeakFrames;
	decayAmount = peakLevel - sustainLevel;	
}
	
void OSC::initializePhase()
{
	phase = 0;
}

void OSC::refreshForSongBeginning()
{
	initializePhase();
	lastAmp = 0.0f;
	refreshEnvelope();
}

float OSC::getOutput()
{
	/*
	// trying out linear interpolation...
	deviation = modf(phase, &dblPhaseIntPart);
	intPhase = static_cast<int>(dblPhaseIntPart);
	intPhaseNext = intPhase++;
	if(intPhaseNext >= OSC_TABLE_SIZE)
		intPhaseNext = 0;
	
	float out = (table[intPhase] + (table[intPhaseNext] - table[intPhase]) * deviation) 
		* getEnvelopeOutput() * gain;
	*/
	
	int ph = static_cast<int> (phase);
	// cout << "phase=" << phase << "..";
	
	float out;
	
	if(yFlip > 0)
		out = table[ph]; // * getEnvelopeOutput();
	else
		out = -table[ph]; // * getEnvelopeOutput();
	
	// if BeefUp is enabled... beef up and compress!
	if(beefUp)
		out = compress(out * beefUpFactor);
	
	out *= gain;
	
	// popguard - just for the first 2 frames...
	if(popGuardCount>0)
		out = popGuard(out);
	else
		lastAmp = out;
	
	historyWriteWait++;
	if(historyWriteWait >= 8)
	{
		pushHistory(out);
		historyWriteWait = 0;
	}
	
	return out;
}

float OSC::compress(float in)
{
	float out = in;
	if(in >= 0.0f && in > compThreshold) // positive value
	{
		float delta = in - compThreshold;
		delta = delta / compRatio;
		out = compThreshold + delta;
		if(out>=0.99f) out = 0.99f;
	}
	else if(in <= 0.0f && in < -compThreshold) // negative value
	{
		float delta = in + compThreshold;
		delta = delta / compRatio;
		out = -compThreshold + delta;
		if(out<=-0.99f) out = -0.99f;
	}
	return out;
}

float OSC::popGuard(float in)
{
	float inPositive = in + 1.0f;
	if(inPositive<0.0f) inPositive = 0.0f;
	float lastAmpPositive = lastAmp + 1.0f;
	if(lastAmpPositive<0.0f) lastAmpPositive = 0.0f;
	// float travelAmount = lastAmpPositive - inPositive;
	inPositive += (lastAmpPositive - inPositive) * (static_cast<float>(popGuardCount) / 60.0f);
	popGuardCount--;
	
	return (inPositive-1.0f);
}

void OSC::enableBeefUp()
{ beefUp = true; }

void OSC::disableBeefUp()
{ beefUp = false; }

void OSC::setBeefUpFactor(float factor)
{ beefUpFactor = factor; }

// push current data to table that keeps an array of historical data
// (used for meter visualization)
void OSC::pushHistory(float g)
{
	if(g < 0) // flip sign when negative
		g = g * -1.0;
	history[historyWriteIndex] = g; // store value
	historyWriteIndex++;
	if(historyWriteIndex >= OSC_HISTORY_SIZE)
		historyWriteIndex = 0;
}

// get the current averaged output
// (average derived from (HISTORY_SIZE * historyWriteWait) frames)
float OSC::getHistoricalAverage()
{
	float sum = 0;
	for(int i=0;i<OSC_HISTORY_SIZE; i++)
		sum += history[i];
	float avg = static_cast<float>(sum / OSC_HISTORY_SIZE);
	return avg;
}

void OSC::clearHistory()
{
	for(int i=0;i<OSC_HISTORY_SIZE; i++)
		history[i] = 0.0f;
	historyWriteWait = 0;
	historyWriteIndex = 0;
}

// set yFlip between 1.0 and -1.0
// determines if wavetable is read as is or vertically inverted
void OSC::flipYAxis()
{
	yFlip = -1.0f;
}

// reset yFlip to default normal 1.0 (table reading won't get vertically inverted)
void OSC::resetYFlip()
{
	yFlip = 1.0f;
}