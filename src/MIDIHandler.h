/** \file wavHandler.h
 * \author ayaan
 * \date 21/02/19.
 * \brief header file associated to the MIDIHandler.c file.
 */

#ifndef MIDIHANDLER_H
#define MIDIHANDLER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "notes.h"
#include <errno.h>
#include "wavHandler.h"

#ifndef MIDIMaker_old
#include "frequencyHandler.h"
#endif
#ifdef MIDIMaker_old
#include "matrice.h"
#include "tabNotes.h"
#endif



#define noire 128
#define ON  0x90
#define OFF 0x80
#define C3  60

#define NB_NOTES_MAX 100000

/**\definition of data sizes uses in a MIDI file
*/

typedef struct monotone_track monotone_track;


void MIDI_header_writer(FILE *fichier, unsigned char SMF, unsigned short pistes, unsigned short nbdiv);
unsigned long MIDI_header_track(FILE* fichier);
void write_track1(FILE *fichier, unsigned long duree);
void MIDI_end_of_track(FILE * fichier);
void write_variable_length_quantity(FILE *fichier, unsigned long i);
void MIDI_delta_time(FILE *fichier, unsigned long duree);
void MIDI_tempo(FILE* fichier,unsigned long duree);
void MIDI_Program_Change(FILE *fichier, unsigned char canal, unsigned char instrument);
void MIDI_Note(unsigned char etat, FILE *fichier, unsigned char canal, unsigned char Note_MIDI, unsigned char velocite);
void MIDI_Unique_Note(FILE *fichier, unsigned char canal, unsigned char Note_MIDI, unsigned char velocite, unsigned long duree);
void write_midi_1_voice(FILE *fichier, monotone_track * track, unsigned char instrument);
void midi_file_1_voice(NoteFreq * tf, int nb_notes, char * nom_fichier_midi, unsigned long tempo, unsigned char instrument);
int midi_file_n_voices(NoteFreq * tf, int nb_notes, char * nom_fichier_midi, unsigned long tempo, unsigned char nb_voix, unsigned char instrument);
FILE * open_midi_file(char * nom_fichier_midi);
int close_midi_file(FILE * midi_file);
void write_midi_n_voices(FILE *fichier, monotone_track * mono, unsigned char canal, unsigned char instrument);

unsigned char getInstrumentMIDICode(char* i);

#endif

