#include "testMidi.h"
#include "testNotes.h"
#include "testWavHandler.h"
#include "testFFT.h"
#include "testFrequencyHandler.h"


int main(int argc, char** argv){
    int out;
    out = 0;

    out += run_UTwavHandler();
    out += run_UTnotes();
    out += run_UTmidi();
    out += run_UTFFT();
    out += run_UTfrequencyHandler();



    return out;
}