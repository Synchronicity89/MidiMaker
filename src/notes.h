#ifndef NOTES_H
#define NOTES_H

#include "MIDIHandler.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "global.h"
#include "frequencyHandler.h"


#define NB_NOTES_MAX 100000
#define NOTE_FREQ_MUL 1.059463094
//This latter corresponds to pow(2, 1/12 )

typedef struct notes notes;

struct notes{

unsigned char valeur; // 60 = C3;
unsigned long duree; //128 = noire;
unsigned char velocite;

};

typedef struct monotone_track monotone_track;

struct monotone_track{

int nb_notes;
notes tab_notes[NB_NOTES_MAX];

};

int read_monotone_track(FILE * fichier, notes * tab_notes,int nb_notes);
int write_a_note(unsigned char valeur, unsigned char velocite, unsigned long duree,notes * tab_notes, int nb_notes);
monotone_track * init_a_track();
monotone_track * write_monotone_track(NoteFreq * tf, int nb_notes, unsigned long tempo, int num_track);
int free_monotone_track(monotone_track * mono);

#endif
