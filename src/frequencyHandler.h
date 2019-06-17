/**
 * \file frequencyHandler.h
 * \brief header file associated with frequencyHandler.c
 * \author Brice Croix
 * \date 19.05.19
 */

#ifndef FREQUENCYHANDLER_H
#define FREQUENCYHANDLER_H

#include <stdio.h>
#include <stdlib.h>
#include "FFT.h"
#include "wavHandler.h"

/*How out of tune is the player allowed to be*/
#define DELTA_FREQ 0.017

#define THRESHOLD_FRACTION 0.4
/*This value has a HUGE importance*/

#define NOTE_AMOUNT 10
#define NO_NOTE_FREQUENCY 432
#define KNOWN_NOTES_AMOUNT 85
#define LOW_C1 32.7032

/**
 * \struct playedFrequency
 * \brief This structure describe a played frequency with its frequency and its velocity
 */
typedef struct playedFrequency{
    double frequency;
    unsigned char velocity;
} playedFrequency;


/**
 * \struct frequencyGroup
 * \brief this structure will be useful to group all played notes in one window.
 */
typedef struct frequencyGroup{
    __uint8_t nb_frequencies;/**< How many notes are relevant in frequencies array*/
    playedFrequency frequencies[NOTE_AMOUNT]; /**< The relevant frequencies for one window*/
    __uint8_t more_than_10_flag; /**< Set to 1 if there was more than 10 played frequencies*/
} frequencyGroup;


/**
* \struct Notefreq
* \brief a structure defining a note
*/
typedef struct
{
    double frequence; /**< the frequency of the note, given in Hz */
    unsigned long duree; /**< the duration of the note, given in ms */
    unsigned char amplitude; /**< the high of the note, normalized between 0 and 255 */
} NoteFreq;



int getThresholdLocMax(double** power_spectrum, fftConstants fftInfo);

double getThresholdFracHigh(double highest_amplitude, double ratio);

int isALocalMaxima(double previous,double current, double next);

frequencyGroup getRelevantFrequencies(double* window, fftConstants fftInfo, double threshold, double highest_amplitude);

int getMinFrequencyIndex(double freq, fftConstants fftInfo, double delta);

int getMaxFrequencyIndex(double freq, fftConstants fftInfo, double delta);

double getHighestAmplitude(double** power_spectrum, fftConstants fftInfo);

__uint8_t normalizeAmplitude(double amplitude, double highest_amplitude);

NoteFreq* frequencyConverter(frequencyGroup* notes_per_window, fftConstants fftInfo, int* nb_notes_ptr);

int equalFrequencyGroup(frequencyGroup fg1, frequencyGroup fg2);

unsigned char getHighestVelocity(frequencyGroup fg);

unsigned long getNbNotes(frequencyGroup* fg, int nb_window);

double isFrequencyHere(double* window, double frequency, fftConstants fftInfo, double threshold);

void fillNoteMatrix(NoteFreq* dest, frequencyGroup fg, unsigned long duration, unsigned char velocity);

frequencyGroup* preFrequencyConverter(double** power_spectrum,fftConstants fftInfo);

void printFrequencyMatrix(NoteFreq* frequency_matrix, int nb_notes);

#endif