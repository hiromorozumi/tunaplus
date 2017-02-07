// Pitch class implementation /////////////////////////////////////////////////////////////

#include <iostream>
#include <algorithm>
#include "Pitch.h"

using namespace std;

// constructor
Pitch::Pitch()
{
	initialize();
}

void Pitch::initialize()
{
	semitoneRatio = pow(2.0, 1.0/12.0);
	middleC = 220.0 * pow(semitoneRatio, 3.0); // middle C is C4
	cZero = middleC * pow(0.5, 4.0);
	buildFreqencyTable(); // used for looking up frequency of a given musical note
	
	currentFrequency = 0;
	currentPeakAmplitude = 0;
	thresholdAmplitude = 800;
	currentNoteNumberDouble = 0;
	currentNoteDeltaPercent = 0;
	
	// initializing fftw3 library related variables
	in  = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * N);
	out = (fftw_complex *) fftw_malloc(sizeof(fftw_complex) * N); 
	p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
	
	// set all imaginary part to zero
	for(int i=0; i<N; i++)
		in[i][1] = 0;
		
}

// clear up data when restarting tuner mode
void Pitch::refresh()
{
	for(int i=0; i<N; i++)
	{
		in[i][0] = 0;
		in[i][1] = 0;
	}
	currentFrequency = 0;
	currentPeakAmplitude = 0;
	currentNoteNumberDouble = 0;
	currentNoteDeltaPercent = 0;
}

// binds the Audio object
void Pitch::bindAudio(Audio* r)
{
	audio = r;
}

double Pitch::detect()
{
	// first get the peak amplitude
	int peakAmp = 0;
	for(int i=0; i<audio->captured.size(); i++)
	{
		if(abs(audio->captured[i])>peakAmp)
			peakAmp = abs(audio->captured[i]);
	}
	currentPeakAmplitude = (double)peakAmp / 100000;
	
	// first copy the captured data in the Audio class to fftw-ready array
	int capturedSize = audio->captured.size();
	for(int i=0; i<N; i++)
	{
		if(i>=capturedSize)
			in[i][0] = 0; // zero pad..
		else
			in[i][0] = (double)audio->captured[i];
	}
	
	// now perform fft!
	fftw_execute(p);
	
	// get the magnitudes
	for(int i=0; i<N/4; i++)
		magnitude[i] = sqrt(pow(out[i][0], 2.0) + pow(out[i][1], 2.0));


	// find the peak
	double biggestMag = 0;
	int indexAtPeak = 0;
	double adjustedIndex = 0;
	double peakFrequency = 0;
	
	for(int i=1; i<N/4; i++)
	{
		if(magnitude[i] > biggestMag)
		{
			biggestMag = magnitude[i];
			indexAtPeak = i;
		}
	}	
		
	// interporate and estimate the exact peak freqency
	
	double binDiviation;

	// Quinn's Second Estimator
	// see... http://dspguru.com/dsp/howtos/how-to-interpolate-fft-peak

	int k = indexAtPeak;				
	double divider = pow(out[k][0], 2.0) + pow(out[k][1], 2.0);
	double ap = (out[k+1][0] * out[k][0] + out[k+1][1] * out[k][1]) / divider;
	double dp = -ap  / (1.0 - ap);
	double am = (out[k-1][0] * out[k][0] + out[k-1][1] * out[k][1]) / divider;
	
	double dm = am / (1.0 - am);
	double d = (dp + dm) / 2 + tau(dp * dp) - tau(dm * dm);
	binDiviation = d;
		
	double adjustedBinLocation = (double)indexAtPeak + binDiviation;
	peakFrequency = (sampleRate * adjustedBinLocation /(double)N );
	
	// try to find a fundamental candidate near half point
	// let's first determine the search range
	
	// first get the biggestEightMags
	vector<double> biggestEightMags;
	biggestEightMags.push_back(magnitude[1]);
	biggestEightMags.push_back(magnitude[2]);
	biggestEightMags.push_back(magnitude[3]);
	biggestEightMags.push_back(magnitude[4]);
	biggestEightMags.push_back(magnitude[5]);
	biggestEightMags.push_back(magnitude[6]);
	biggestEightMags.push_back(magnitude[7]);
	biggestEightMags.push_back(magnitude[8]);
	sort(biggestEightMags.begin(), biggestEightMags.end()); // sort, smallest first
	
	for(int i=5; i<N/4; i++)
	{
		if(biggestEightMags[0]<magnitude[i]) // compare with smallest of four
		{
			biggestEightMags.push_back(magnitude[i]);
			if(biggestEightMags.size()>8) // if overflowing, remove the smallest one (first one)
				biggestEightMags.erase(biggestEightMags.begin());
			sort(biggestEightMags.begin(), biggestEightMags.end()); // sort, smallest first
		}
	}
	
	int startSearchPos = indexAtPeak / 2 - 4;
	int endSearchPos = indexAtPeak / 2 + 4;
	if(startSearchPos<1) startSearchPos = 1;
	if(endSearchPos >= N-1) endSearchPos = N-1;
	
	fundamentalFreqAtHalf = 0;
	fundamentalFreqAtOneThird = 0;
	fundamentalFoundAtHalf = false;
	fundamentalFoundAtOneThird = false;
	
	double divRatio = 1.0;
	double smallestRatioSoFar = 1.0;
	
	// search through biggestEightMagsList
	for(int i=0; i<8; i++)
	{	
		// get the corresponding bin number having this exact magnitude value
		int candidateBinNumber = 1;
		for(int j=1; j<N/4; j++)
		{
			if(biggestEightMags[i]==magnitude[j])
				candidateBinNumber = j;
		}
		
		// let's do this once again...
		// Quinn's Second Estimator
		// see... http://dspguru.com/dsp/howtos/how-to-interpolate-fft-peak
		
		k = candidateBinNumber;				
		divider = pow(out[k][0], 2.0) + pow(out[k][1], 2.0);
		ap = (out[k+1][0] * out[k][0] + out[k+1][1] * out[k][1]) / divider;
		dp = -ap  / (1.0 - ap);
		am = (out[k-1][0] * out[k][0] + out[k-1][1] * out[k][1]) / divider;
		
		dm = am / (1.0 - am);
		d = (dp + dm) / 2 + tau(dp * dp) - tau(dm * dm);
		binDiviation = d;
		
		double thisBinNumAdjusted = (double)candidateBinNumber + binDiviation;
		double freqAtThisBinAdjusted = (sampleRate * thisBinNumAdjusted /(double)N );	

		// check 1/2 point for fundamental
		divRatio = abs((peakFrequency/2.0) - freqAtThisBinAdjusted ) / freqAtThisBinAdjusted;
		if(divRatio < 0.02 && divRatio > -0.02 && divRatio < smallestRatioSoFar)
		{
			smallestRatioSoFar = divRatio;
			fundamentalFoundAtHalf = true;
			fundamentalFreqAtHalf = freqAtThisBinAdjusted;
		}
		// check 1/3 point for fundamental
		divRatio = abs((peakFrequency/3.0) - freqAtThisBinAdjusted ) / freqAtThisBinAdjusted;
		if(divRatio < 0.02 && divRatio > -0.02 && divRatio < smallestRatioSoFar)
		{
			smallestRatioSoFar = divRatio;
			fundamentalFoundAtOneThird = true;
			fundamentalFreqAtOneThird = freqAtThisBinAdjusted;
		}			
			
	}
	
	// if indeed there was a strong fundamental at exact half point, rewrite the peakFrequency
	if(fundamentalFoundAtHalf)
	{
		cout << peakFrequency << "Hz, adj w/ fundamental @ 1/2 pt " << fundamentalFreqAtHalf << "Hz\n";
		peakFrequency = fundamentalFreqAtHalf;
	}
	else if(fundamentalFoundAtOneThird) // if not, was 1/3 point good?
	{
		cout << peakFrequency << "Hz, adj w/ fundamental @ 1/3 pt " << fundamentalFreqAtOneThird << "Hz\n";
		peakFrequency = fundamentalFoundAtOneThird;
	}
	
	// find the closest note and get the diviation amount
	double diviationAtClosest = peakFrequency;
	int indexAtClosest = 0;
	for(int i=0;i<108;i++)
	{
		double tempDiviation = abs(peakFrequency - noteToFrequency(i));
		if(tempDiviation < diviationAtClosest)
		{
			diviationAtClosest = tempDiviation;
			indexAtClosest = i;
		}
	}

	currentNoteNumber = indexAtClosest;
	currentNoteCenterFrequency = frequencyTable[currentNoteNumber];
	currentNoteName = noteToString(currentNoteNumber);
	currentFreqencyDelta = peakFrequency - noteToFrequency(indexAtClosest);
	
	// store the result, return the result
	currentFrequency = peakFrequency;
	currentNoteNumberDouble = frequencyToNote(currentFrequency);
	currentNoteDeltaPercent = getCurrentNoteDeltaPercent();

	// push the current data to history (for averaging)
	pushHistory();

	return currentFrequency;
	
}

double Pitch::tau(double x)
{
	double p1 = log(3 * pow(x, 2.0) + 6 * x + 1);
	double part1 = x + 1 - sqrt(2.0/3.0);
	double part2 = x + 1 + sqrt(2.0/3.0);
	double p2 = log(part1 / part2);	
	return (1.0/4.0 * p1 - sqrt(6) / 24 * p2);
}

void Pitch::buildFreqencyTable()
{
	for(int i=0;i<108;i++)
	{
		frequencyTable[i] = noteToFrequency(i);
	}
}

double Pitch::noteToFrequency(int noteNum)
{
	return cZero * pow(semitoneRatio, (double)noteNum);
}

double Pitch::frequencyToNote(double freq)
{
	double x = freq/cZero;
	return (log10(x) / log10(semitoneRatio));
}

double Pitch::getCurrentNoteDeltaPercent()
{
	double noteDelta = currentNoteNumberDouble - (double)currentNoteNumber;
	return (noteDelta * 100.0);
}

string Pitch::noteToString(int noteNum)
{
	int noteNumInOct = noteNum % 12;
	string str = "";
	switch(noteNumInOct)
	{
		case 0: str = "C"; break;
		case 1: str = "C#"; break;
		case 2: str = "D"; break;
		case 3: str = "D#"; break;
		case 4: str = "E"; break;
		case 5: str = "F"; break;
		case 6: str = "F#"; break;
		case 7: str = "G"; break;
		case 8: str = "G#"; break;
		case 9: str = "A"; break;
		case 10: str = "A#"; break;
		case 11: str = "B"; break;
	}
	
	if(noteNum<108)
	{
		string numStr = "0123456789";
		int octNum = noteNum / 12;
		char c = numStr.at(octNum);
		str.append(1, c);
	}
	
	return str;
}

bool Pitch::loudEnough()
{
	bool answer = false;
	if(currentPeakAmplitude > thresholdAmplitude)
		answer = true;
	return answer;
}

string Pitch::getCurrentNoteName()
{
	return currentNoteName;
}

//
void Pitch::pushHistory()
{
	deltaPercentHistory.push_back(currentNoteDeltaPercent);
	if(deltaPercentHistory.size()>HISTORY_SIZE)
		deltaPercentHistory.pop_front();
	
	frequencyHistory.push_back(currentFrequency);
	if(frequencyHistory.size()>HISTORY_SIZE)
		frequencyHistory.pop_front();
}

double Pitch::getDeltaPercentAvg()
{
	int accum = 0;
	for(int i=0; i<HISTORY_SIZE; i++)
		accum += deltaPercentHistory[i];
	return ((double)accum / (double)HISTORY_SIZE); 
}

double Pitch::getFrequencyAvg()
{
	double accum = 0;
	for(int i=0; i<HISTORY_SIZE; i++)
		accum += frequencyHistory[i];
	return (accum / (double)HISTORY_SIZE); 	
}
