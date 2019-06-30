#include "notes.h"

/**
* \fn void read_monotone_track(FILE * fichier, notes * tab_notes,int nb_notes)
* \brief Read the notes in the tab given, to write a track in a .mid file, useful for the monoton.
*
* \param[out] fichier is the MIDI file to be modified.
* \param[in] tab_notes is the tab to be read to write into the MIDI file.
* \param[in] nb_notes is the number of notes in the tab.
* \return Don't return anything.
*/
int read_monotone_track(FILE * fichier, notes * tab_notes,int nb_notes){

    if(fichier == NULL || tab_notes == NULL || nb_notes<=0){

	return -1;

    }

    for(int i = 0; i<nb_notes; i++){
        MIDI_Unique_Note(fichier, 0, tab_notes[i].valeur, tab_notes[i].velocite, tab_notes[i].duree) ;
    }

    return 0;
}

/**
* \fn int write_a_note(unsigned char valeur, unsigned char velocite, unsigned long duree,notes * tab_notes, int nb_notes)
* \brief Write a note int a tab of notes.
*
* \param[in] valeur is is the ID of the value (in terms of music, like a C, or a A) of a note.
* \param[in] velocite is the value that determine how loud the note will be played.
* \param[in] duree represents the time the note will be played.
* \param[in] tab_notes in which we write the note.
* \param[in] nb_notes is the number of notes in the tab.
* \return Return the number of notes in the tab after the addition of new note into it.
*/
int write_a_note(unsigned char valeur, unsigned char velocite, unsigned long duree,notes * tab_notes, int nb_notes){

    notes newnote;

    if(valeur>88 || valeur<4 || nb_notes<0){

	return -1;

    }

    newnote.valeur = valeur;
    newnote.velocite = velocite;
    newnote.duree = duree;

    tab_notes[nb_notes] = newnote;

    nb_notes = nb_notes+1;
    return nb_notes;

}

/**
* \fn monotone_track * init_a_track()
* \brief Initialize a track that will be filled after.
*
* \param None.
* \return Return the initialized monoton track.
*/
monotone_track * init_a_track(){

    monotone_track * track = NULL;
    track = (monotone_track *)safe_malloc(sizeof(monotone_track));

    track->nb_notes = 0;

    return track;

}

/**
* \fn int free_monotone_track(monotone_track * mono)
* \brief Free the memory used for the structure monotone_track.
*
* \param[out] mono is the monoton track to be freed.
* \return Return 0 if the track is free, -1 otherwise.
*/
int free_monotone_track(monotone_track * mono){

	if(mono == NULL){

		return (EXIT_FAILURE);

	}

	//free(mono->tab_notes);
	free(mono);

	return(EXIT_SUCCESS);
}

/**
* \fn monotone_track * write_monotone_track(NoteFreq * tf, int nb_notes, unsigned long tempo, int num_track)
* \brief Write a track with the informations given in the parameters.
*
* \param[in] tf is the tab with the informations describing the caracteristics of the notes we have to write into the MIDI file.
* \param[in] nb_notes is the number of notes that will be written in the track. unsigned long :tempo, int :num_track
* \param[in] tempo is the tempo of the music.
* \param[in] num_track is the ID of the track in which I write the notes.
* \return Return the filled track.
*/
monotone_track * write_monotone_track(NoteFreq * tf, int nb_notes, unsigned long tempo, int num_track){

    if(tf == NULL || nb_notes <=0 || num_track<0 || num_track>10){

	return NULL;

    }

    monotone_track * track = init_a_track();

    int i = 0;
    int j = 1;
    double a,b;

    while(i<nb_notes){

        a = LOW_C1*(1-DELTA_FREQ); //fréquence limite basse de la note la plus grave
        b = LOW_C1*(1+DELTA_FREQ); //fréquence limite haute de la note la plus grave

        while(j<=KNOWN_NOTES_AMOUNT){
            if((tf[i*NOTE_AMOUNT+num_track].frequence>a) && (tf[i*NOTE_AMOUNT+num_track].frequence<b)){

                track->nb_notes = write_a_note((unsigned char)(C3-36+j),(tf[i*NOTE_AMOUNT+num_track].amplitude),(tf[i*NOTE_AMOUNT+num_track].duree/tempo*noire),track->tab_notes,track->nb_notes); //Do1 à Do7

            }
        a = a*NOTE_FREQ_MUL;
        b = b*NOTE_FREQ_MUL; //la fonction pow donne lieu à une erreur, cependant cette valeur correspond à une approximation suffisante pour cette application de la valeur : racine douzième de deux.
        j++;
        }
    j=1;
    i++;
    }

    return track;

}





