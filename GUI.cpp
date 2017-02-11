// GUI class implementation /////////////////////////////////////////////////////////////

#include "GUI.h"

using namespace std;

GUI::GUI()
{
	initialize();
}

void GUI::initialize()
{
	currentWorkingDir = getCurrentDir();
	string filePrefix = currentWorkingDir + "\\res\\";
	
	// set the general font
	std::string fontFileName = filePrefix + "AUDIMB.TTF";
	
	cout << "font file path:\n" << fontFileName << endl;
	if (!font.loadFromFile(fontFileName))
		cout << "Error reading font " << fontFileName << endl;
	
	referencePitch = 440;
	restartTimer();
	pitch.bindAudio(&audio);
	audio.bindMetronome(&metronome);
	
	audioInBoost = 15;
	double newFactor = ((double)(audioInBoost) * 49.0 / 100.0) + 1.0; // scale to 1.0 to 50.0
	pitch.setAudioInBoostFactor(newFactor);	
	
	// Create the main window
	window.create(sf::VideoMode(240, 240), "Tuna+", 2); // 3rd param is for sf::Style
	
	mouse.bindWindow(&window);
	
	// Load GUI images to display
	string strFile;
	strFile = filePrefix + "base.png";
	if (!tunerBaseTexture.loadFromFile(strFile))
		cout << "Error reading image: base.png\n";
	strFile = filePrefix + "red_circle.png";
	if (!redCircleTexture.loadFromFile(strFile))
		cout << "Error reading image: red_circle.png\n";
	strFile = filePrefix + "tuner_screen.png";
	if (!tunerScreenTexture.loadFromFile(strFile))
		cout << "Error reading image: tuner_screen.png\n";
	strFile = filePrefix + "tuner_screen_red.png";
	if (!tunerScreenRedTexture.loadFromFile(strFile))
		cout << "Error reading image: tuner_screen_red.png\n";
	strFile = filePrefix + "metronome_screen.png";
	if (!metronomeScreenTexture.loadFromFile(strFile))
		cout << "Error reading image: metronome_screen.png\n";

	strFile = filePrefix + "metronome_screen_red.png";	
	if (!metronomeScreenRedTexture.loadFromFile(strFile))
		cout << "Error reading image: metronome_screen_red.png\n";
	
	// common assets set up
	tunerBase.setTexture(tunerBaseTexture, true);
	tunerBase.setPosition(sf::Vector2f(0,0));

	strFile = filePrefix + "menu_button";
	menuButton.create(strFile, &mouse, &window);

	menuButton.setPosition(71, 185);
	
	strFile = filePrefix + "power_button";
	powerButton.create(strFile, &mouse, &window);
	powerButton.setPosition(126, 185);

	// tuner assets set up
	redCircle.setTexture(redCircleTexture, true);
	redCircle.setPosition(sf::Vector2f(80, 145));
	tunerScreen.setTexture(tunerScreenTexture, true);
	tunerScreen.setPosition(sf::Vector2f(0, 0));
	tunerScreenRed.setTexture(tunerScreenRedTexture, true);
	tunerScreenRed.setPosition(sf::Vector2f(0, 0));

	needleRect.setSize(sf::Vector2f(80,0));
	needleRect.setPosition(sf::Vector2f(0,0));
	needleRect.setOrigin(sf::Vector2f(0,0));
	needleRect.setOutlineThickness(2);
	needleRect.setFillColor(sf::Color(200, 160, 160));
	needleRect.setOutlineColor(sf::Color(200, 160, 160));

	noteNameText.setFont(font);
	noteNameText.setString("A");
	noteNameText.setFillColor(sf::Color(220, 180, 180));
	noteNameText.setCharacterSize(80);
	noteNameText.setPosition(sf::Vector2f(34, -4));
	accidentalText.setFont(font);
	accidentalText.setString("#");
	accidentalText.setFillColor(sf::Color(220, 180, 180));
	accidentalText.setCharacterSize(42);
	accidentalText.setPosition(sf::Vector2f(74,0));
	freqText.setFont(font);
	freqText.setString("440");
	freqText.setFillColor(sf::Color(170, 150, 150));
	freqText.setCharacterSize(52);
	freqText.setPosition(sf::Vector2f(96, 24));
	freqFracText.setFont(font);
	freqFracText.setString(".0");
	freqFracText.setFillColor(sf::Color(170, 150, 150));
	freqFracText.setCharacterSize(28);
	freqFracText.setPosition(sf::Vector2f(176, 48));

	// metronome assets set up
	metronomeScreen.setTexture(metronomeScreenTexture, true);
	metronomeScreen.setPosition(sf::Vector2f(0, 0));
	metronomeScreenRed.setTexture(metronomeScreenRedTexture, true);
	metronomeScreenRed.setPosition(sf::Vector2f(0, 0));
	metronomeScreenRed.setColor(sf::Color(255,255,255,150));

	strFile = filePrefix + "nbeats_button";
	metronomeNBeatsButton.create(strFile, &mouse, &window);
	metronomeNBeatsButton.setPosition(42, 21);
	strFile = filePrefix + "tap_button";
	metronomeTapButton.create(strFile, &mouse, &window);
	metronomeTapButton.setPosition(96, 21);
	strFile = filePrefix + "play_button";
	metronomePlayButton.create(strFile, &mouse, &window, true); // last bool arg -> using alt image
	metronomePlayButton.setPosition(150, 21);
	
	strFile = filePrefix + "tempo_down_button";
	metronomeTempoDownButton.create(strFile, &mouse, &window);
	metronomeTempoDownButton.setPosition(152, 152);	
	metronomeTempoDownButton.setClickableBoundary(5, 4, 24, 21);
	
	strFile = filePrefix + "tempo_up_button";
	metronomeTempoUpButton.create(strFile, &mouse, &window);
	metronomeTempoUpButton.setPosition(183, 152);	
	metronomeTempoUpButton.setClickableBoundary(1, 4, 23, 21);	
	
	BPMText.setFont(font);
	BPMText.setString("120");
	BPMText.setFillColor(sf::Color(200, 160, 160));
	BPMText.setCharacterSize(56);
	BPMText.setPosition(sf::Vector2f(102, 70));

	nBeatsText.setFont(font);
	nBeatsText.setString("4/4");
	nBeatsText.setFillColor(sf::Color(160, 140, 140));
	nBeatsText.setCharacterSize(20);
	nBeatsText.setPosition(sf::Vector2f(48, 102));	

	blinker.create(&mouse, &window);
	blinker.setPosition(36, 157);
	blinker.turnOff();
	
	metronomeVolSlider.create(&mouse, &window);
	metronomeVolSlider.setPosition(64, 166);

	// setup mode assets
	strFile = filePrefix + "setup_button";
	setupButton.create(strFile, &mouse, &window);
	setupButton.setPosition(71, 21);
	strFile = filePrefix + "help_button";
	helpButton.create(strFile, &mouse, &window);
	helpButton.setPosition(125, 21);
	
	referencePitchSlider.create(&mouse, &window);
	referencePitchSlider.setPosition(69, 104);	
	referencePitchTextLabel.setFont(font);
	referencePitchTextLabel.setString("Reference pitch:");
	referencePitchTextLabel.setFillColor(sf::Color(40, 40, 40));
	referencePitchTextLabel.setCharacterSize(13);
	referencePitchTextLabel.setPosition(sf::Vector2f(69, 77));	
	referencePitchText.setFont(font);
	referencePitchText.setString("440");
	referencePitchText.setFillColor(sf::Color(40, 40, 40));
	referencePitchText.setCharacterSize(14);
	referencePitchText.setPosition(sf::Vector2f(156, 97));

	audioInBoostSlider.create(&mouse, &window);
	audioInBoostSlider.setPosition(69, 156); // was 104 so add 52	
	audioInBoostTextLabel.setFont(font);
	audioInBoostTextLabel.setString("Audio-in boost:");
	audioInBoostTextLabel.setFillColor(sf::Color(40, 40, 40));
	audioInBoostTextLabel.setCharacterSize(13);
	audioInBoostTextLabel.setPosition(sf::Vector2f(69, 129));	
	audioInBoostText.setFont(font);
	audioInBoostText.setString(" 15");
	audioInBoostText.setFillColor(sf::Color(40, 40, 40));
	audioInBoostText.setCharacterSize(14);
	audioInBoostText.setPosition(sf::Vector2f(156, 149));
	
	// tuner variables
	backgroundAlpha = 0;
	tunerStateChanged = false;
	
	// metronome variables
	tapFlashOn = false;
	metronomePlaying = false;
	metronomePlayButton.showDefaultImage();
	metronomeVolume = 70;
	metronomeVolSlider.setValuePercent(metronomeVolume);
	metronomeNBeats = 4;
	currentBPM = 120;
	metronome.setBPM((double)currentBPM);
	tempoUpFiring = false;
	tempoUpNextUpdateTime = 0.5;
	tempoDownFiring = false;
	tempoDownNextUpdateTime = 0.5;

	// setup mode variables
	referencePitchSlider.setValuePercent(50);
	audioInBoostSlider.setValuePercent(audioInBoost);
	
	// getting ready...
	exitApp = false;
	currentMode = TUNER_MODE;
	audio.setRecordingMode();

}

void GUI::start()
{
	audio.start();
	
	restartTimer();
	
	//
	// main loop
	//
	
	while(!exitApp)
	{
		
		//
		// GUI related stuff
		//
		
		// if window has been closed, exit
		if(!window.isOpen())
			exitApp = true;
		
		// process events
		while (window.pollEvent(event))
		{
			// window closed - exit now
			if(event.type == sf::Event::Closed)
				window.close();
			
			// if resizing requested - well, force the constant size
			if (event.type == sf::Event::Resized)
			{
				window.setSize(sf::Vector2u(240,240));
			}			
		}
		
		// check for keyboard inputs...
		
		// ESC key for exiting app
		if(kbd.escape())
			exitApp = true;
		
		// run the current MODE selected
		if     (currentMode==TUNER_MODE)
			runTuner();
		else if(currentMode==METRONOME_MODE)
			runMetronome();
		else if(currentMode==RECORDER_MODE)
			runRecorder();
		else if(currentMode==SETUP_MODE)
			runSetup();
	}
}

// get current directory
std::string GUI::getCurrentDir()
{
    char buffer[MAX_PATH];
    GetModuleFileName( NULL, buffer, MAX_PATH );
    string::size_type pos = string( buffer ).find_last_of( "\\/" );
    return string( buffer ).substr( 0, pos);
}

unsigned long long GUI::getTimeMs64()
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
	ret /= 10000; /* From 100 nano seconds (10^-7) to 1 millisecond (10^-3) intervals */
	
	return ret;
}

void GUI::restartTimer()
{
	startedTime = getTimeMs64();
}

long GUI::getElapsedTime()
{
	long elapsedTime = (long)(getTimeMs64() - startedTime);
	if(elapsedTime<0) elapsedTime = 0;
	return elapsedTime;
}

///////////////////////////////////////////////
//
// tuner main loop
//
///////////////////////////////////////////////

void GUI::runTuner()
{
	// process for power button
	powerButton.update();
	if(powerButton.tailActionRequest())
	{
		cout << "Power off - exiting app...\n";
		exitApp = true;
	}

	// process for menu button
	menuButton.update();
	if(menuButton.tailActionRequest())
	{
		cout << "Mode change request, going to Next Mode...\n";
		
		// now change mode to next one
		currentMode = METRONOME_MODE;
		metronomePlaying = false;
		metronomePlayButton.showDefaultImage();
		audio.setMetronomeMode();
	}

	//
	//	Backend stuff (pitch detection and FFT)
	//
	
	// update to make sure the buffer vector size stays the same size
	// and we keep purging the oldest sample frames
	audio.update();

	if(getElapsedTime()>150)
	{
		pitch.detect();
		if(pitch.loudEnough() && pitch.currentNoteNumber > 10)
		{
			cout << pitch.currentFrequency << "Hz (" << pitch.currentNoteName 
				<< " " << pitch.currentNoteCenterFrequency << "Hz) AVG " << pitch.getFrequencyAvg() 
				<< "Hz Div " << (int)(pitch.currentNoteDeltaPercent) << "% (AVG " << pitch.getDeltaPercentAvg() 
				<< "%) Amp " << (int)pitch.currentPeakAmplitude << " " << pitch.currentNoteNumber 
				<< endl;
		}
		restartTimer();
		tunerStateChanged = true;
	}
	else
		tunerStateChanged = false;
	
	// determine needle's position - based on the averaged history of note diviation percentage
	if(tunerStateChanged)
	{

		// if signal's not loud enough... place at left most position (angle = 180)
		if(!pitch.loudEnough() || pitch.currentNoteNumber <= 10)
		{
			needleRect.setPosition(sf::Vector2f(0,0));
			needleRect.setOrigin(sf::Vector2f(0,0));
			needleRect.setRotation(180.0f);
			needleRect.move(sf::Vector2f(120, 170));				
		}
		else // signal IS loud enough, rotate/move the needle to the right position				
		{
			// needleRect.setSize(sf::Vector2f(80,0));
			needleRect.setPosition(sf::Vector2f(0,0));
			needleRect.setOrigin(sf::Vector2f(0,0));			
			float rotationAngle = (float)(((pitch.getDeltaPercentAvg() + 50.0) / 100.0) * 180.0) + 180.0f;
			if(rotationAngle > 360.0f)
				rotationAngle = 360.0f;
			else if(rotationAngle < 180.0f)
				rotationAngle = 180.0f;
			needleRect.setRotation(rotationAngle);
			needleRect.move(sf::Vector2f(120, 170));
		}
		
	}
	
	// when close to center freq of target note, make background red (using alpha blending)
	if(tunerStateChanged && pitch.loudEnough() && pitch.currentNoteNumber > 10)
	{
		double absDiv = abs(pitch.getDeltaPercentAvg());
		if(absDiv < 8.0)
		{
			backgroundAlpha = (int)(((8.0 - absDiv) / 8.0) * 350.0);
			if(backgroundAlpha>200) backgroundAlpha = 200;
		}
		else
			backgroundAlpha = 0;
	}
	
	// now according to the alpha amount, set the background "sprite" color
	tunerScreenRed.setColor(sf::Color(255,255,255,backgroundAlpha));
	
	// get the current pitch name, freq etc..
	
	if(tunerStateChanged)
	{	
		
		if(pitch.loudEnough() && pitch.currentNoteNumber > 10)
		{
			// freqText display size + position might get adjusted, so set default everytime to begin
			freqText.setCharacterSize(52); // default character size
			freqText.setPosition(sf::Vector2f(96, 24));				
			
			string str = pitch.getCurrentNoteName();
			string strNote = "";
			strNote += str.at(0);
			noteNameText.setString(strNote);
			
			// now to accidental..
			if(str.length()>1 && str.at(1)=='#')
			{
				char chAcc = str.at(1);
				accidentalText.setString(chAcc);
			}
			else
				accidentalText.setString("");
			
			// get the frequency number
			double freq = pitch.getFrequencyAvg(); // used history averaged freq!
			freq = (round(freq * 10.0)) / 10.0; // round to ###.# form (1st decimal place, rounded)		
			
			// stringstream ss; // will convert int to string
			ss.str("");
			ss << (int)freq; // get only the integer part
			string strFreq = ss.str();			
	
			if(strFreq.size()==1) // pad spaces..
				strFreq = "  " + strFreq;
			else if(strFreq.size()==2)
				strFreq = " " + strFreq;
			else if(strFreq.size()>=4) // if 4-digits, adjust character size to fit
			{
				freqText.setCharacterSize(44); 
				freqText.setPosition(sf::Vector2f(86, 32)); // bring position down a little
			}
			
			// get the fraction part of frequency
			double dblFrac = freq - floor(freq);
			int fraqDigit = (int)(round(dblFrac * 10.0));
			ss.str("");
			ss << "." << fraqDigit;
			string strFrac = ss.str();

			freqText.setString(strFreq);
			freqFracText.setString(strFrac);
			
			// DEBUG
			// cout << "rounded freq = " << freq << " int = " << (int)freq << " frac = " << fraqDigit << endl;				
		}
		// tunerStateChanged, but not loud enough...
		else
		{
			noteNameText.setString("");
			accidentalText.setString("");
			freqText.setString("");
			freqFracText.setString("");
			backgroundAlpha = 0;
		}
		
	}

	// draw screen...
	window.clear();
	window.draw(tunerBase); // 1 - draw base GUI
	window.draw(tunerScreen);
	
	menuButton.draw();
	powerButton.draw();		

	window.draw(tunerScreenRed);	// 2 - draw red background mask(as needed)
	window.draw(needleRect);	// 3 - draw needle
	window.draw(redCircle);	// 4 - draw red circle
	window.draw(noteNameText);
	window.draw(accidentalText);
	window.draw(freqText);
	window.draw(freqFracText);
	
	window.display();
}

///////////////////////////////////////////////
//
// metronome main loop (TODO)
//
///////////////////////////////////////////////

void GUI::runMetronome()
{
	// process for power button
	powerButton.update();
	if(powerButton.tailActionRequest())
	{
		cout << "Power off - exiting app...\n";
		exitApp = true;
	}

	// process for menu button
	menuButton.update();
	if(menuButton.tailActionRequest())
	{
		cout << "Mode change request, going to Next Mode...\n";
		
		// now change mode to next one
		metronomePlaying = false;
		metronome.stop();
		currentMode = SETUP_MODE;
		// audio.refresh();
		// audio.setRecordingMode();
		// pitch.refresh(); // refresh audio buffer for gentle restarting
		// backgroundAlpha = 0;
	}

	// process for nbeats button - n/4 time
	metronomeNBeatsButton.update();
	if(metronomeNBeatsButton.frontActionRequest())
	{
		metronomeNBeats++;
		if(metronomeNBeats > 7) metronomeNBeats = 0;
		
		// send this new nbeats info to metronome
		metronome.setNBeats(metronomeNBeats);
		
		// set display for n/4 time signature
		ss.str("");
		ss << metronomeNBeats;
		string strNBeats = ss.str();
		strNBeats += "/4";
		nBeatsText.setString(strNBeats);
		
		cout << "nBeats changed to " << metronomeNBeats << endl;
	}	
	
	// process for tap button
	metronomeTapButton.update();

	if(metronomeTapButton.frontActionRequest())
	{
		
		// Tap class - "tap" once now to calculate BPM
		tap.tap();
		if(!tap.isBPMReady())
		{
			cout << "NOT READY YET - listening... -\n";
			BPMText.setString("...");
		}
		else
		{
			// get the calculated bpm from Tap class
			int newBPM = (int)(round(tap.getBPM()));
			cout << "current BPM = " << tap.getBPM() << " (History size = " << tap.getHistorySize() << ")\n";
			
			// send this new bpm info to metronome
			metronome.setBPM((double)newBPM);
			currentBPM = newBPM;
			refreshBPMText();
			
			// if metronome's already playing, restart at top of beat one
			if(metronomePlaying)
				metronome.start();
			
			// make screen flash right on each tap
			tapFlashTimer.restart();
		}		
	}
	
	// check if screen should flash or not for TAPPing
	if(tapFlashTimer.getElapsedTime().asSeconds() < 0.05)
		tapFlashOn = true;
	else
		tapFlashOn = false;	

	// process for menu button
	metronomePlayButton.update();
	if(metronomePlayButton.frontActionRequest())
	{
		if(!metronomePlaying)
		{
			metronomePlaying = true;
			metronomePlayButton.showAlternateImage();
			cout << "metronome - play!\n";
			
			// DEBUG
			// metronome.report();			
			
			metronome.start();
			refreshBPMText();
		}
		else
		{
			metronomePlaying = false;
			metronomePlayButton.showDefaultImage();
			cout << "metronome - stop.\n";
			
			metronome.stop();
			refreshBPMText();
		}
	}
	
	// process for tempo up button
	metronomeTempoUpButton.update();
	if(metronomeTempoUpButton.frontActionRequest())
	{
		currentBPM++;
		if(currentBPM>600) currentBPM=600;
		metronome.setBPM((double)currentBPM);
		cout << "tempo++, new BPM = " << currentBPM << endl;
		refreshBPMText();
		tempoUpBtnFireTimer.restart();
		tempoUpFiring = true;
		tempoUpNextUpdateTime = 0.6;
	}
	
	// handle tempo up button "firing"
	double upBtnTimeNow = (double)(tempoUpBtnFireTimer.getElapsedTime().asSeconds());	
	if(upBtnTimeNow>0.6 && tempoUpFiring)
	{
		if(upBtnTimeNow > tempoUpNextUpdateTime)
		{
			currentBPM++;
			if(currentBPM>600) currentBPM=600;
			metronome.setBPM((double)currentBPM);
			cout << "tempo++, new BPM = " << currentBPM << endl;
			refreshBPMText();			
			tempoUpNextUpdateTime = upBtnTimeNow + 0.06;
		}
	}
	if(tempoUpFiring && !metronomeTempoUpButton.pressedDown())
		tempoUpFiring = false;

	// process for tempo down button
	metronomeTempoDownButton.update();
	if(metronomeTempoDownButton.frontActionRequest())
	{
		currentBPM--;
		if(currentBPM<30) currentBPM=30;
		metronome.setBPM((double)currentBPM);
		cout << "tempo--, new BPM = " << currentBPM << endl;
		refreshBPMText();
		tempoDownBtnFireTimer.restart();
		tempoDownFiring = true;
		tempoDownNextUpdateTime = 0.6;
	}
	
	// handle tempo down button "firing"
	double downBtnTimeNow = (double)(tempoDownBtnFireTimer.getElapsedTime().asSeconds());	
	if(downBtnTimeNow>0.6 && tempoDownFiring)
	{
		if(downBtnTimeNow > tempoDownNextUpdateTime)
		{
			currentBPM--;
			if(currentBPM<30) currentBPM=30;
			metronome.setBPM((double)currentBPM);
			cout << "tempo--, new BPM = " << currentBPM << endl;
			refreshBPMText();			
			tempoDownNextUpdateTime = downBtnTimeNow + 0.06;
		}
	}
	if(tempoDownFiring && !metronomeTempoDownButton.pressedDown())
		tempoDownFiring = false;
	
	// process volume slider
	metronomeVolSlider.update();
	if(metronomeVolSlider.stateChanged())
	{
		metronomeVolume = metronomeVolSlider.getValuePercent();
		
		// adjust to 0 to 0.6f scale
		float volAmountToPass = metronomeVolume / 100.0f;
		metronome.setVolume(volAmountToPass);
		cout << "Volume slider: " << metronomeVolume << endl;
	}

	// get the blinker flash request from Metronome
	if(metronome.blinkerFlashRequested())
	{
		// first beat is always green, EXCEPT in 0/4 time
		if(metronome.getBlinkerFlashColor()==0) // green
			blinker.turnGreen();
		else if(metronome.getBlinkerFlashColor()==1)// red
			blinker.turnRed();
		if(metronome.getNBeats()==0) // force red for 0/4 time
			blinker.turnRed();
		
		blinkerTimer.restart();
	}
	
	if(blinkerTimer.getElapsedTime().asSeconds() > 0.07)
		blinker.turnOff();
	
	// draw screen...

	window.clear();
	window.draw(tunerBase); // 1 - draw base GUI
	
	menuButton.draw();
	powerButton.draw();
	
	window.draw(metronomeScreen);
	if(tapFlashOn)
		window.draw(metronomeScreenRed);
	window.draw(BPMText);
	window.draw(nBeatsText);
	
	metronomeNBeatsButton.draw();
	metronomeTapButton.draw();
	metronomePlayButton.draw();
	metronomeTempoDownButton.draw();
	metronomeTempoUpButton.draw();
	blinker.draw();
	metronomeVolSlider.draw();
	
	window.display();
}

// helper function to set bpm text display
void GUI::refreshBPMText()
{
	// convert the BPM value to string
	ss.str("");
	ss << currentBPM;
	string strNewBPM = ss.str();
	if(strNewBPM.length()==1)
		strNewBPM = "  " + strNewBPM;
	else if(strNewBPM.length()==2)
		strNewBPM = " " + strNewBPM;
	
	// now set the string
	BPMText.setString(strNewBPM);	
}

///////////////////////////////////////////////
//
// recorder main loop (TODO)
//
///////////////////////////////////////////////

void GUI::runRecorder()
{
	// process for menu button
	menuButton.update();
	if(menuButton.tailActionRequest())
	{
		cout << "Menu: tail action!\n";
		menuButton.clearRequests();
		
		// now change mode to next one
		currentMode = TUNER_MODE;
	}		
}

///////////////////////////////////////////////
//
// setup mode main loop (TODO)
//
///////////////////////////////////////////////

void GUI::runSetup()
{
	// process for power button
	powerButton.update();
	if(powerButton.tailActionRequest())
	{
		cout << "Power off - exiting app...\n";
		exitApp = true;
	}

	// process for menu button
	menuButton.update();
	if(menuButton.tailActionRequest())
	{
		cout << "Mode change request, going to Next Mode...\n";
		
		// now change mode to next one
		currentMode = TUNER_MODE;
		audio.refresh();
		audio.setRecordingMode();
		pitch.refresh(); // refresh audio buffer for gentle restarting
		backgroundAlpha = 0;
	}

	// process for setup button
	setupButton.update();
	if(setupButton.frontActionRequest())
	{
		string exeCommand = "mmsys.cpl";
		ShellExecute( NULL, NULL, exeCommand.c_str(), NULL, NULL, SW_SHOWNORMAL );	
		cout << "Entering sound device setup...\n";
	}	

	// process for help button
	helpButton.update();
	if(helpButton.frontActionRequest())
	{
		// let's open the manual in 'documentation' folder that's in cwd
		string docPath = "documentation\\tunaplus_manual.html";
		string docPath2 = currentWorkingDir + "\\documentation\\tunaplus_manual.html";
		cout << "Opening Doc at: " << docPath2 << endl;		
		ShellExecute( NULL, NULL, docPath2.c_str(), NULL, NULL, SW_SHOWNORMAL );		
	}

	// reference pitch slider
	referencePitchSlider.update();
	if(referencePitchSlider.stateChanged())
	{
		referencePitch = 435 + (int)(round((double)referencePitchSlider.getValuePercent()/10.0));
		
		// now let Pitch class rebuild the frequency table (used for referencing in tuner mode)
		pitch.buildFrequencyTable((double)referencePitch);
		
		// convert the value to string
		ss.str("");
		ss << referencePitch;
		string strNewRefPitch = ss.str();

		// now set the string
		referencePitchText.setString(strNewRefPitch);	
		cout << "Reference Pitch: " << referencePitch << endl;
	}	

	// audio-in boost slider
	audioInBoostSlider.update();
	if(audioInBoostSlider.stateChanged())
	{
		audioInBoost = (int)(audioInBoostSlider.getValuePercent());
		
		// slider value range 0-100 -> audio-in boost factor 1.0 to 50.0
		double newFactor = ((double)(audioInBoost) * 49.0 / 100.0) + 1.0;
		pitch.setAudioInBoostFactor(newFactor);
		
		// DEBUG
		cout << "Audio-in Boost factor is now: " << newFactor << endl;
		
		// determine how many space needs to be padded to the left of amt display
		string spacePad = "";
		if(audioInBoost<10) spacePad = "  ";
		else if(audioInBoost<100) spacePad = " ";
		
		// convert the value to string
		ss.str("");
		ss << spacePad << audioInBoost;
		string strNewAudioInBoost = ss.str();

		// now set the string
		audioInBoostText.setString(strNewAudioInBoost);	
		cout << "Audio-in boost: " << audioInBoost << endl;
	}
	
	// draw screen...

	window.clear();
	window.draw(tunerBase); // 1 - draw base GUI
	
	menuButton.draw();
	powerButton.draw();
	
	setupButton.draw();
	helpButton.draw();
	window.draw(referencePitchTextLabel);
	window.draw(referencePitchText);
	referencePitchSlider.draw();
	window.draw(audioInBoostTextLabel);
	window.draw(audioInBoostText);
	audioInBoostSlider.draw();
	
	window.display();	
}