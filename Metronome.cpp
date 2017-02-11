// Metronome class implementation /////////////////////////////////////////////////////////////

#include "Metronome.h"

using namespace std;

Metronome::Metronome()
{
	setBPM(120.0);
	nBeats = 4;
	highNoteFreq = 660;
	lowNoteFreq = 450;
	highNoteVolume = 0.8f;
	lowNoteVolume = 0.5f;
	highNoteLength = 1200;
	lowNoteLength = 1200;
	metronomeActive = false;
	metronomeVolume = 0.5f;
	flashRequested = false;
	blinkerColor = 0;
}

void Metronome::setBPM(double newBPM)
{
	if(newBPM<20) newBPM = 20;
	else if(newBPM > 600) newBPM = 600;
	currentBPM = newBPM;
	oneNoteLength = 60.0 / currentBPM * SAMPLE_RATE;
	buildSequence();
}

double Metronome::getBPM()
{
	return currentBPM;
}

void Metronome::setNBeats(int nbeats)
{
	if(nbeats<0) nBeats = 0;
	else if(nBeats>7) nBeats = 7;
	nBeats = nbeats;
	buildSequence();
}

int Metronome::getNBeats()
{
	return nBeats;
}

// get only one sample frame to be sent to portaudio output
// meanwhile advances both Metronome and OSC counter to the next position
float Metronome::getOneOutputSample()
{
	float output = 0.0f;
	
	if(metronomeActive)
	{
		// if you're at the first counter position of an element, request update to OSC
		if(counter==0)
		{
			if(sequence[currentElement].isRest)
			{
				osc.setToRest();
				osc.setGain(0);
			}
			else
			{
				osc.setNewNote(sequence[currentElement].freq);
				osc.setGain(sequence[currentElement].gain);
				
				if(currentElement==0) // if at very top, blinker should flash green
					blinkerColor = 0; // 0 for green
				else
					blinkerColor = 1;
				
				flashRequested = true;
			}
		}
		
		// get output from OSC, then advance OSC
		output = osc.getOutput();
		
		// DEBUG
		// cout << output << " ";
		
		output *= metronomeVolume;
		
		osc.advance();
		
		// increment counter position
		// go to next element when you reach end of element
		counter++;
		if(counter >= sequence[currentElement].length)
		{
			counter = 0;
			currentElement++;
		}
		
		// if you go past the very last element's last sample, refresh to beginning
		if(currentElement>=nElementsInSequence)
		{
			counter = 0;
			currentElement = 0;
		}
	}
		
	return output;
}

void Metronome::start()
{
	buildSequence();
	osc.refreshForSongBeginning();
	metronomeActive = true;
	currentElement = 0;
	counter = 0;
	flashRequested = false;
	blinkerColor = 0;
}

void Metronome::stop()
{
	metronomeActive = false;
	flashRequested = false;
	blinkerColor = 0;
}

void Metronome::buildSequence()
{
	clearSequence();
	
	cout << "in buildSeq..\n";
		
	int nHighNotes = 1;
	int nLowNotes = nBeats - 1;
	
	// special case when we have 0/4 time signature
	// we'll just play low notes only
	if(nBeats==0)
	{
		nHighNotes = 0;
		nLowNotes = 1;

		int firstPartLenLow = lowNoteLength;
		int secondPartLenLow = oneNoteLength - lowNoteLength;
			
		// just a sequence of one low note, then rest, that's it
		sequence[0].isRest = false;
		sequence[0].freq = lowNoteFreq;
		sequence[0].gain = lowNoteVolume;
		sequence[0].length = firstPartLenLow;

		sequence[1].isRest = true;
		sequence[1].freq = 20;
		sequence[1].gain = 0;
		sequence[1].length = secondPartLenLow;		
	}
	// in all other cases - 1/4 to 7/4
	else
	{
		
		// in all of these cases - build one high note at the beginning
		// pitched part followed by padding rest (total of 2 "notes" in OSC terms)
		
		int firstPartLenHigh = highNoteLength;
		int secondPartLenHigh = oneNoteLength - highNoteLength;
	
		for(int i=0; i<nHighNotes; i++)
		{
			sequence[0].isRest = false;
			sequence[0].freq = highNoteFreq;
			sequence[0].gain = highNoteVolume;
			sequence[0].length = firstPartLenHigh;
			
			sequence[1].isRest = true;
			sequence[1].freq = 20;
			sequence[1].gain = 0;
			sequence[1].length = secondPartLenHigh;
		}
		
		// now build low notes x nLowNotes (nBeats minus one)
		// pitched part followed by padding rest (total of 2 "notes" in OSC terms)	
		
		for(int i=0; i<nLowNotes; i++)
		{
			int firstPartLenLow = lowNoteLength;
			int secondPartLenLow = oneNoteLength - lowNoteLength;
			int index = i*2+2;
			
			sequence[index].isRest = false;
			sequence[index].freq = lowNoteFreq;
			sequence[index].gain = lowNoteVolume;
			sequence[index].length = firstPartLenLow;
			
			index++;

			sequence[index].isRest = true;
			sequence[index].freq = 20;
			sequence[index].gain = 0;
			sequence[index].length = secondPartLenLow;
		}
	}
	
	// now record how many elements went into the sequence vector
	// (should be nBeats x 2, except when 0/4 you have 2 elements)
	nElementsInSequence = (nHighNotes + nLowNotes) * 2;

	cout << "end of buildSeq..\n";
}

void Metronome::clearSequence()
{
	for(int i=0; i<20; i++)
	{
		sequence[i].isRest = false;
		sequence[i].freq = 20;
		sequence[i].gain = 0;
		sequence[i].length = 44100;
	}
}

// set volume in float value...
// GUI should write low enough value so signal won't distort
// like 0 to 0.6f??? (needs experimenting)
void Metronome::setVolume(float v)
{
	if(v <= 0.0f) v = 0.0f;
	else if(v>0.99f) v = 0.99f;
	metronomeVolume = v * VOLUME_SCALING_FACTOR; // let's not make it too loud
}

float Metronome::getVolume()
{
	return metronomeVolume;
}

// returns the pending blinker flash request 
bool Metronome::blinkerFlashRequested()
{
	bool ret = flashRequested;
	flashRequested = false;
	return ret;
}

// 0 for green, 1 for red
int Metronome::getBlinkerFlashColor()
{
	return blinkerColor;
}

//////////////////////
// DEBUG

void Metronome::report()
{
	for(int i=0; i<nElementsInSequence; i++)
	{
		cout << "ELEM " << i << " ... isRest= " << sequence[i].isRest << " freq=" << sequence[i].freq << endl;
		cout << "   gain=" << sequence[i].gain << " length=" << sequence[i].length << endl;
	}
	
	cout << "nElementsInSequence = " << nElementsInSequence << endl;
	cout << "met volume = " << metronomeVolume << endl;
	cout << "bpm = " << currentBPM << endl;
	
}