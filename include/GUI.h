#ifndef GUI_H
#define GUI_H

// GUI class /////////////////////////////////////////////////////////////////////

#include <SFML/Graphics.hpp>
#include <windows.h>
#include <vector>
#include <deque>
#include <iostream>
#include <string>
#include <sstream>
#include <cmath>
#include "Pitch.h"
#include "Audio.h"
#include "Tap.h"
#include "Metronome.h"

class Kbd
{
public:

	Kbd(){}
	~Kbd(){}

	bool right()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) return true; else return false; }
	bool left()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) return true; else return false; }
	bool up()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) return true; else return false; }
	bool down()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) return true; else return false; }
	bool ret()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::Return)) return true; else return false; }
	bool escape()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) return true; else return false; }
	bool f1()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::F1)) return true; else return false; }
	bool f2()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::F2)) return true; else return false; }
	bool f3()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::F3)) return true; else return false; }
	bool f4()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::F4)) return true; else return false; }
	bool f5()
		{ if(sf::Keyboard::isKeyPressed(sf::Keyboard::F5)) return true; else return false; }

private:

};

class Mouse
{

public:

	//static const int MOUSE_WINDOW_WIDTH = 840;
	//static const int MOUSE_WINDOW_HEIGHT = 640;

	sf::RenderWindow* w;
	int intX;
	int intY;
	int intPrevY;
	int intYDelta;
	int intPrevX;
	int intXDelta;

	Mouse()
	{}
	~Mouse(){}

	void bindWindow(sf::RenderWindow* windowObj)
	{
		if(windowObj){ w = windowObj; }
	}

	bool left()
		{ if( sf::Mouse::isButtonPressed(sf::Mouse::Left) ) return true; else return false;  }
	bool right()
		{ if( sf::Mouse::isButtonPressed(sf::Mouse::Right) ) return true; else return false; }
	int x()
		{ return sf::Mouse::getPosition(const_cast<sf::RenderWindow&>(*w)).x; }
	int y()
		{ intPrevY = intY; intY = sf::Mouse::getPosition(const_cast<sf::RenderWindow&>(*w)).y; return intY; }
	int getYDelta()
		{ intPrevY = intY; intY = sf::Mouse::getPosition(const_cast<sf::RenderWindow&>(*w)).y;
			intYDelta = intY - intPrevY; return intYDelta; }
	int getXDelta()
		{ intPrevX = intX; intX = sf::Mouse::getPosition(const_cast<sf::RenderWindow&>(*w)).x;
			intXDelta = intX - intPrevX; return intXDelta; }
};

class Button
{
public:

	float x, y, w, h;
	float bx, by, bw, bh;
	bool highlighted;
	bool showAlternate;
	bool pressed;
	bool pressedThenReleased;
	bool frontActionRequested;
	bool tailActionRequested;
	sf::Texture texture;
	sf::Texture textureActive;
	sf::Texture textureAlternate;
	sf::Texture textureAlternateActive;
	sf::Sprite sprite;
	sf::Sprite spriteActive;
	sf::Sprite spriteAlternate;
	sf::Sprite spriteAlternateActive;
	sf::RenderWindow* window;
	Mouse* mouse;
	
	Button()
	{}
	
	~Button(){}
	
	void create(const std::string& fName, Mouse* ptrMouse, sf::RenderWindow* ptrWindow, bool usingAlternate=false)
	{
		std::string fNameRegular = fName + ".png";
		std::string fNameActive = fName + "_active" + ".png";
		std::string fNameAlternate = fName + "_alt" + ".png";
		std::string fNameAlternateActive = fName + "_alt_active" + ".png";

		if (!texture.loadFromFile(fNameRegular))
			std::cout << "Error reading image: " << fNameRegular << std::endl;	
		if (!textureActive.loadFromFile(fNameActive))
			std::cout << "Error reading image: " << fNameActive << std::endl;
		if(usingAlternate)
		{
			if (!textureAlternate.loadFromFile(fNameAlternate))
				std::cout << "Error reading image: " << fNameAlternate << std::endl;	
			if (!textureAlternateActive.loadFromFile(fNameAlternateActive))
				std::cout << "Error reading image: " << fNameAlternateActive << std::endl;
		}
		
		sprite.setTexture(texture, true);
		spriteActive.setTexture(textureActive, true);	
		spriteAlternate.setTexture(textureAlternate, true);
		spriteAlternateActive.setTexture(textureAlternateActive, true);

		mouse = ptrMouse;
		window = ptrWindow;

		setClickableBoundary(4, 4, 42, 29); // set these for standard size
		highlighted = false;
		showAlternate = false;
		pressed = false;
		pressedThenReleased = false;
		frontActionRequested = false;
		tailActionRequested = false;
	}
	
	void setPosition(float xx, float yy)
	{
		x = xx; y = yy;
		sprite.setPosition(sf::Vector2f(x, y));	
		spriteActive.setPosition(sf::Vector2f(x, y));
		spriteAlternate.setPosition(sf::Vector2f(x, y));	
		spriteAlternateActive.setPosition(sf::Vector2f(x, y));
	}
	
	void setClickableBoundary(float left, float top, float width, float height)
	{
		bx = left; by = top; bw = width; bh = height;
	}
	
	bool hovering()
	{
		if((mouse->x()) > (x+bx) && (mouse->x()) < (x+bx+bw) 
						&& (mouse->y()) > (y+by) && (mouse->y()) < (y+by+bh))
			return true;
		else
			return false;
	}
	
	void update()
	{
		if(hovering())
		{
			highlighted = true;
			
			if(!pressed && !pressedThenReleased && highlighted && mouse->left())
			{
				pressed = true;
				frontActionRequested = true;
			}
			else if(pressed && !pressedThenReleased && highlighted && !mouse->left())
			{
				pressed = false;
				pressedThenReleased = true;
				tailActionRequested = true;
			}
			else if(!pressed && pressedThenReleased && !mouse->left())
				pressedThenReleased = false;
		}
		else
			highlighted = false;
	}
	
	void clearRequests()
	{
		frontActionRequested = false;
		tailActionRequested = false;
	}
	
	bool frontActionRequest()
	{
		bool ret = frontActionRequested;
		clearRequests();
		return ret;
	}
	
	bool pressedDown()
	{ return pressed; }
	
	bool tailActionRequest()
	{
		bool ret = tailActionRequested;
		clearRequests();
		return ret;
	}
	
	bool showAlternateImage()
	{
		showAlternate = true;
	}
	
	bool showDefaultImage()
	{
		showAlternate = false;
	}
	
	void draw()
	{
		if(!highlighted && !showAlternate) // regular non-highlited
			window->draw(sprite);
		else if(highlighted && !showAlternate) // regular active
			window->draw(spriteActive);
		else if(!highlighted && showAlternate) // alternate non-highlited
			window->draw(spriteAlternate);
		else if(highlighted && showAlternate) // alternate active
			window->draw(spriteAlternateActive);
	}
	
protected:
		
};

class Blinker
{
	
public:

	enum{ BLACK, GREEN, RED };
	
	std::string currentWorkingDir;
	
	sf::Texture blackTexture;
	sf::Texture greenTexture;
	sf::Texture redTexture;
	sf::Sprite blackSprite;
	sf::Sprite greenSprite;
	sf::Sprite redSprite;
	sf::RenderWindow* window;
	Mouse* mouse;
	
	float x, y;
	int state;
	
	Blinker()
	{
		setCurrentWorkingDir();
	}
	
	~Blinker(){}
	
	void setCurrentWorkingDir()
	{
		char buffer[MAX_PATH];
		GetModuleFileName( NULL, buffer, MAX_PATH );
		std::string::size_type pos = std::string( buffer ).find_last_of( "\\/" );
		currentWorkingDir =  std::string( buffer ).substr( 0, pos);	
	}
	
	void create(Mouse* ptrMouse, sf::RenderWindow* ptrWindow)
	{	
		std::string strFile;
		strFile = currentWorkingDir + "\\res\\blinker.png";
		if (!blackTexture.loadFromFile(strFile))
			std::cout << "Error reading image: blinker.png\n" << std::endl;
		strFile = currentWorkingDir + "\\res\\blinker_green.png";
		if (!greenTexture.loadFromFile(strFile))
			std::cout << "Error reading image: blinker_green.png\n" << std::endl;
		strFile = currentWorkingDir + "\\res\\blinker_red.png";
		if (!redTexture.loadFromFile(strFile))
			std::cout << "Error reading image: blinker_red.png\n" << std::endl;
		
		blackSprite.setTexture(blackTexture, true);
		greenSprite.setTexture(greenTexture, true);
		redSprite.setTexture(redTexture, true);
		
		mouse = ptrMouse;
		window = ptrWindow;
		state = BLACK;
	}
	
	void setPosition(float xx, float yy)
	{
		blackSprite.setPosition(sf::Vector2f(xx, yy));
		greenSprite.setPosition(sf::Vector2f(xx, yy));
		redSprite.setPosition(sf::Vector2f(xx, yy));
	}
	
	void turnGreen()
	{ state = GREEN; }
	void turnRed()
	{ state = RED; }
	void turnOff()
	{ state = BLACK; }
	
	bool isOn()
	{ if(state==GREEN || state==RED) return true; else return false; }
	
	void draw()
	{
		switch(state)
		{
			case BLACK:	window->draw(blackSprite); break;
			case GREEN:	window->draw(greenSprite); break;
			case RED  :	window->draw(  redSprite); break;
		}
	}
};

class Slider
{

public:

	static constexpr float HANDLE_WIDTH = 25;
	static constexpr float HANDLE_HEIGHT = 20;
	static constexpr float BAR_WIDTH = 81;

	std::string currentWorkingDir;
	
	sf::Texture barTexture;
	sf::Texture handleTexture;
	sf::Sprite barSprite;
	sf::Sprite handleSprite;
	sf::RenderWindow* window;
	Mouse* mouse;
	
	float movableRange;
	float x, y;
	float handleY;
	float pos;
	float valuePercent;
	bool dragging;
	bool stateUpdated;

	Slider()
	{
		setCurrentWorkingDir();
	}
	
	~Slider(){}
	
	void setCurrentWorkingDir()
	{
		char buffer[MAX_PATH];
		GetModuleFileName( NULL, buffer, MAX_PATH );
		std::string::size_type pos = std::string( buffer ).find_last_of( "\\/" );
		currentWorkingDir =  std::string( buffer ).substr( 0, pos);	
	}
	
	void create(Mouse* ptrMouse, sf::RenderWindow* ptrWindow)
	{
		std::string strFile;
		strFile = currentWorkingDir + "\\res\\slider_bar.png";
		if (!barTexture.loadFromFile(strFile))
			std::cout << "Error reading image: slider_bar.png\n" << std::endl;
		strFile = currentWorkingDir + "\\res\\slider_handle.png";		
		if (!handleTexture.loadFromFile(strFile))
			std::cout << "Error reading image: slider_handle.png\n" << std::endl;

		barSprite.setTexture(barTexture, true);
		handleSprite.setTexture(handleTexture, true);

		movableRange = (float)(BAR_WIDTH - HANDLE_WIDTH);
		mouse = ptrMouse;
		window = ptrWindow;
		x = 0;
		y = 0;
		pos = 0; // relative x pos of handle
		valuePercent = 0;
		handleY = -8; // relative y pos of handle (stays unchanged)
		dragging = false;
	}
	
	void setPosition(float xx, float yy)
	{
		x = xx; y = yy;
		barSprite.setPosition(sf::Vector2f(xx, yy));
		handleSprite.setPosition(sf::Vector2f(xx + pos, yy + handleY));
	}
	
	bool hovering()
	{
		float bx = x + pos;
		float by = y + handleY;
		float bw = HANDLE_WIDTH;
		float bh = HANDLE_HEIGHT;
		if( mouse->x() > bx && mouse->x() < (bx + bw)  
						&& mouse->y() > by && mouse->y() < (by + bh))
			return true;
		
		return false;
	}
	
	void update()
	{
		stateUpdated = false;
		if(!dragging && hovering() && mouse->left()) // just starting to drag
		{
			int dummy = mouse->getXDelta();
			dragging = true;
		}
		if(dragging && !mouse->left()) // just finished dragging
			dragging = false;
			
		if(dragging)
		{
			int xMoved = mouse->getXDelta();
			moveHandle((float)xMoved);
			if(xMoved!=0)
				stateUpdated = true;
		}
	}
	
	void moveHandle(float xDiff)
	{
		pos += xDiff;
		if(pos < 0.0f) pos = 0.0f;
		else if(pos > movableRange) pos = movableRange;
		handleSprite.setPosition(sf::Vector2f(x + pos, y + handleY));
		valuePercent = pos * 100.0f / movableRange;
	}
	
	bool setValuePercent(float newValue)
	{
		valuePercent = newValue;
		float newPos = newValue * movableRange / 100.0f;
		moveHandle(newPos - pos);
	}
	
	float getValuePercent()
	{ return valuePercent; }
	
	bool stateChanged()
	{ return stateUpdated; }
	
	void draw()
	{
		window->draw(barSprite);
		window->draw(handleSprite);
	}
};


class GUI
{
public:

static constexpr float PI = 3.1415926535897932384626433832795;

enum{ TUNER_MODE, METRONOME_MODE, RECORDER_MODE };

Audio audio;
Pitch pitch;
unsigned long long startedTime;
Kbd kbd;
Mouse mouse;
Tap tap;
Metronome metronome;

sf::Clock tapFlashTimer;
sf::Clock blinkerTimer;
sf::Clock tempoUpBtnFireTimer;
sf::Clock tempoDownBtnFireTimer;

sf::Font font;
sf::RenderWindow window;
sf::Event event;
sf::Text noteNameText;
sf::Text accidentalText;
sf::Text freqText;
sf::Text freqFracText;
sf::Text BPMText;
sf::Text nBeatsText;

sf::Texture tunerBaseTexture;
sf::Texture redCircleTexture;
sf::Texture tunerScreenTexture;
sf::Texture tunerScreenRedTexture;
sf::Texture metronomeScreenTexture;
sf::Texture metronomeScreenRedTexture;

sf::Sprite tunerBase;
sf::Sprite redCircle;
sf::Sprite tunerScreen;
sf::Sprite tunerScreenRed;
sf::Sprite metronomeScreen;
sf::Sprite metronomeScreenRed;
sf::ConvexShape needle;
sf::RectangleShape needleRect;

Button menuButton;
Button powerButton;
Button metronomeTapButton;
Button metronomePlayButton;
Button metronomeNBeatsButton;
Button metronomeTempoDownButton;
Button metronomeTempoUpButton;

Blinker blinker;
Slider metronomeVolSlider;
float metronomeVolume;
bool tapFlashOn;
bool metronomePlaying;
int metronomeNBeats;
int currentBPM;
bool tempoUpFiring;
double tempoUpNextUpdateTime;
bool tempoDownFiring;
double tempoDownNextUpdateTime;

float needleTopX;
float needleTopY;
int backgroundAlpha;
bool tunerStateChanged;

bool currentMode;
bool exitApp;

std::stringstream ss;
std::string currentWorkingDir;

GUI();
~GUI(){}

void initialize();
void start();
std::string getCurrentDir();
unsigned long long getTimeMs64();
void restartTimer();
long getElapsedTime();
void runTuner();
void runMetronome();
void refreshBPMText();
void runRecorder();

};

#endif
