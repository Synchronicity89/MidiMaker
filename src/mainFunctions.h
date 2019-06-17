/**\file mainFunctions.h
 * \brief this is the header file associated to mainFunctions.c
 * \author Brice Croix
 * \date 24.05.2019
 */
#ifndef MAINFUNCTIONS_H
#define MAINFUNCTIONS_H

#include "wavHandler.h"
#include "FFT.h"
#include "frequencyHandler.h"
#include "notes.h"
#include "MIDIHandler.h"
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include "gui.h"

#define VERBOSE_NONE 0
#define VERBOSE_LIGHT 1
#define VERBOSE_HEAVY 2


/**\enum mainExit
 * \brief this enum allows other functions to know what may have gone wrong while converting file
 */
typedef enum mainExit{
    MAIN_EXIT_SUCCESS,
    MAIN_EXIT_WAV_OPEN_ERROR,
    MAIN_EXIT_WAV_NOT_WAV,
    MAIN_EXIT_WAV_NOT_PROPER_WAV,
    MAIN_EXIT_WAV_EMPTY,
    MAIN_EXIT_WAV_UK_CHUNK,
    MAIN_EXIT_FOURIER_ABANDON,
    MAIN_EXIT_MIDI_FAILURE,
    MAIN_EXIT_UNKNOWN_ERROR
}mainExit;

mainExit mainFunction(char* file_in, char* file_out, unsigned long tempo, unsigned char instrument, unsigned char verbose);

void printHelp();

#endif