EXEFILEDEV = tunaplus_dev.exe
EXEFILE = tunaplus.exe
CPPFILES = Audio.cpp Pitch.cpp Tap.cpp OSC.cpp Metronome.cpp GUI.cpp tunerMain.cpp

INCLUDEPATHS = -I./include
LIBPATHS = -L./lib ./lib/portaudio_x86.lib
LIBFLAGS = -lfftw3 -lsfml-graphics -lsfml-window -lsfml-system

normal:
	g++ $(CPPFILES) $(INCLUDEPATHS) $(LIBPATHS) $(LIBFLAGS) -o $(EXEFILEDEV)
	
release:
	g++ $(CPPFILES) $(INCLUDEPATHS) $(LIBPATHS) $(LIBFLAGS) -o $(EXEFILE) -mwindows	
	
clean:
	rm $(EXEFILEDEV)
	rm $(EXEFILE)