cmake_minimum_required(VERSION 3.13)
project(MidiMaker C) #Creating the file MidiMaker.cbp

set(CMAKE_C_STANDARD 99)

execute_process(COMMAND apt-get install libfreetype6-dev libsdl2-dev libsdl2-ttf-dev)


find_package(SDL2 REQUIRED)
find_library(CMOCKA_LIBRARY cmocka HINTS /usr/local/lib/)
find_library(FFTw_LIBRARY fftw3 HINTS libfftw3/)


# The target for tests
add_executable(mainTests tests/mainTests.c
		tests/testWavHandler.c tests/testWavHandler.h src/wavHandler.c src/wavHandler.h
		tests/testMidi.c tests/testMidi.h src/MIDIHandler.c src/MIDIHandler.h
		tests/testNotes.c tests/testNotes.h src/notes.c src/notes.h
		tests/testFFT.c tests/testFFT.h src/FFT.c src/FFT.h libfftw3/fftw3.h
		tests/testFrequencyHandler.c tests/testFrequencyHandler.h src/frequencyHandler.c src/frequencyHandler.h
		src/global.h src/global.c
		)

target_link_libraries(mainTests ${CMOCKA_LIBRARY} ${FFTw_LIBRARY} m)

# The main target
add_executable(MidiMaker src/main.c
        src/FFT.h src/FFT.c
        src/frequencyHandler.h src/frequencyHandler.c
        src/mainFunctions.h src/mainFunctions.c
        src/MIDIHandler.h src/MIDIHandler.c
        src/notes.h src/notes.c
		src/gui.h src/gui.c
        src/wavHandler.h src/wavHandler.c
		src/global.h src/global.c
        )
include_directories(MidiMaker /usr/include/SDL2)
target_link_libraries(MidiMaker ${FFTw_LIBRARY} m ${SDL2_LIBRARIES} SDL2_ttf)




