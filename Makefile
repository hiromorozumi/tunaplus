EXEFILEDEV = tunaplus_dev.exe
EXEFILE = tunaplus.exe
CPPFILES = GUI.cpp Audio.cpp Pitch.cpp Tap.cpp OSC.cpp Metronome.cpp tunerMain.cpp
ICONRES = tunaplus.res

INCLUDEPATHS = -I./include
LIBPATHS = -L./lib ./lib/portaudio_x86.lib
LIBFLAGS = -lfftw3 -lsfml-graphics -lsfml-window -lsfml-system

normal:
	g++ $(ICONRES) $(CPPFILES) $(INCLUDEPATHS) $(LIBPATHS) $(LIBFLAGS) -o $(EXEFILEDEV)
	
release:
	g++ $(ICONRES) $(CPPFILES) $(INCLUDEPATHS) $(LIBPATHS) $(LIBFLAGS) -o $(EXEFILE) -mwindows	

debug:
	g++ -g $(ICONRES) $(CPPFILES) $(INCLUDEPATHS) $(LIBPATHS) $(LIBFLAGS) -o $(EXEFILEDEV)
	
clean:
	rm $(EXEFILEDEV)
	rm $(EXEFILE)