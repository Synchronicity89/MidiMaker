#include "MIDIHandler.h"


/**
* \fn void MIDI_header_writer(FILE *fichier, unsigned char SMF, unsigned short pistes, unsigned short nbdiv)
* \brief Write the main header of the MIDI file.
*
* \param[in] fichier is the MIDI file to be modified., unsigned char :SMF, unsigned short :pistes, unsigned short :nbdiv
* \param[in] SMF is the type of MIDI file we want to write, unsigned short :pistes, unsigned short :nbdiv
* \param[in] pistes is the number of tracks that will be present in this file., unsigned short :nbdiv
* \param[in] nbdiv is the ...
* \return Don't return any value.
*/
void MIDI_header_writer(FILE *fichier, unsigned char SMF, unsigned short pistes, unsigned short nbdiv){

	if(SMF==0 && pistes>1){
		fprintf(stderr,"Format de fichier non adapté, en SMF 0, une seule piste peut être gérée\n");
		return;
	}

	unsigned char octets[14] = {0x4d, 0x54, 0x68, 0x64, 0x00, 0x00, 0x00, 0x06};
	//ligne Mthd ci dessus, "code" normalisé permettant de déterminer si le fichier est un fichier midi
	octets[8] = 0;
	octets[9] = SMF;
	octets[10] = pistes >> 8;
	octets[11] = pistes;
	octets[12] = nbdiv >> 8;
	octets[13] = nbdiv;

	int verif = (int)fwrite(octets,1,14,fichier);
	if(verif != 14){
		fprintf(stderr,"Error : something went wrong when writing 14 bytes in output file\n");
	}
}

/**
* \fn unsigned long MIDI_header_track(FILE* fichier)
* \brief Write the header associated to a track.
*
* \param[in] fichier is the MIDI file to be modified.
* \return Return the position of the pointer in the .mid file.
*/
unsigned long MIDI_header_track(FILE* fichier){
	unsigned char octets[8] = {0x4d, 0x54, 0x72, 0x6B, 0x00, 0x00, 0x00, 0x00}; //les 4 octets de la fin permettent de coder le nombre d'octet composant la suite de la piste;
	fwrite(&octets,8,1,fichier);
	return (unsigned long)ftell(fichier);
}

/**
* \fn void MIDI_end_of_track(FILE * fichier)
* \brief Write the bytes that define the end of a track in the .mid file.
*
* \param fichier is the MIDI file to be modified.
* \return Don't return anything.
*/
void MIDI_end_of_track(FILE * fichier){
	MIDI_delta_time(fichier,0); //fonction vue par la suite
	unsigned char octets[3] = {0xff,0x2f,0x00};
	fwrite(&octets,3,1,fichier);
}

/**
* \fn void final_sizeoftrack_writer(FILE* fichier, unsigned long marque)
* \brief Give in the .mid file the size in terms of bytes of a track.
*
* \param[in] fichier is the MIDI file to be modified.
* \param[in] marque is a position of the pointer that is used to know from which point of the file we have started to write the informations about this track.
* \return Don't return anything.
*/
void final_sizeoftrack_writer(FILE* fichier, unsigned long marque){
	unsigned char octets[4];
	unsigned long taille = ftell(fichier) - marque;
	fseek(fichier, marque-4, SEEK_SET);
	octets[0] = taille >> 24;
	octets[1] = taille >> 16;
	octets[2] = taille >> 8;
	octets[3] = taille;
	fwrite(&octets, 4, 1, fichier);
	fseek(fichier, 0, SEEK_END);
}

/**
* \fn void MIDI_tempo(FILE* fichier,unsigned long duree)
* \brief Write in the .mid file the value of the tempo of the music.
*
* \param[in] fichier is the MIDI file to be modified, unsigned long :duree
* \param[in] duree is the time in ms of a quarter note.
* \return Don't return anything.
*/
void MIDI_tempo(FILE* fichier,unsigned long duree){
	MIDI_delta_time(fichier, 0);
	unsigned char octets[6] = {0xFF, 0x51, 0x03};
	octets[3] = duree >> 16;
	octets[4] = duree >> 8;
	octets[5] = duree;
	fwrite(&octets,6,1,fichier);
}

/**
* \fn void write_track1(FILE* fichier, unsigned long duree)
* \brief Write in the .mid file the "track1" a description of the general caracteristics of a track, that presents the tempo chosen for this file.
*
* \param[in] fichier is the MIDI file to be modified.
* \param[in] duree
* \return Ne retourne rien mais écrit dans le fichier.
*/
void write_track1(FILE* fichier, unsigned long duree){
	unsigned long marque = MIDI_header_track(fichier);
	MIDI_tempo(fichier, duree);
	MIDI_end_of_track(fichier);
	final_sizeoftrack_writer(fichier, marque);
}

/**
* \fn void write_variable_length_quantity(FILE *fichier, unsigned long i)
* \brief Write in a MIDI file the time between the launch of two distinct notes.
*
* \param[in] fichier is the MIDI file to be modified.
* \param[in] i represents the time between the two notes.
* \return Don't return anything.
*/
void write_variable_length_quantity(FILE *fichier, unsigned long i) {

     int continuer ;

     if (i > 0x0FFFFFFF) {
         fprintf(stderr,"ERREUR : delai > 0x0FFFFFFF ! \n") ;
         exit(EXIT_FAILURE) ;
     }

     unsigned long filo = i & 0x7F ;
     i = i >> 7 ;

     while (i != 0) {
         filo = (filo << 8)  + ((i & 0x7F) | 0x80) ;
         i = i >> 7 ;
     }

     do {
         fwrite(&filo, 1, 1, fichier) ;
         continuer = filo & 0x80 ;
         if (continuer) filo = filo >> 8 ;
     } while (continuer) ;

 }

/**
* \fn void MIDI_delta_time(FILE *fichier, unsigned long duree)
* \brief Call write_variable_length_quantity.
*
* \param[in] fichier is the MIDI file to be modified.
* \param[in] Don't return anything.
*/
 void MIDI_delta_time(FILE *fichier, unsigned long duree) {

     write_variable_length_quantity(fichier, duree) ;

}


/**
* \fn void MIDI_Program_Change(FILE *fichier, unsigned char canal, unsigned char instrument)
* \brief Modify the informations about the chosen instrument and the chanel used to generate music.
*
* \param[out] fichier is the MIDI file to be modified.
* \param[in] canal is the ID of the chanel we want to use.
* \param[in] instrument is the ID of the instrument we want to use.
* \return Don't return anything.
*/
//permet de changer d'instrument et de mettre un canal en polyton si mode = 1
void MIDI_Program_Change(FILE *fichier, unsigned char canal, unsigned char instrument) {

     unsigned char octets[2] ;
     MIDI_delta_time(fichier, 0) ;
     octets[0] = 0xC0 + canal % 16 ;
     octets[1] = instrument % 128 ;

     fwrite(&octets, 2, 1, fichier) ;

 }

/**
* \fn void MIDI_Note(unsigned char etat, FILE *fichier, unsigned char canal, unsigned char Note_MIDI, unsigned char velocite)
* \brief Write the data about a note in the MIDI file.
*
* \param[in] etat is the state of the note (ON or OFF).
* \param[out] fichier is the MIDI file to be modified.
* \param[in] canal is the chanel on which we want to play the note.
* \param[in] Note_MIDI is the ID of the value (in terms of music, like a C, or a A) of a note.
* \param[in] velocite is the value that determine how loud the note will be played.
* \return Ne retourne rien mais écrit dans le fichier.
*/
void MIDI_Note(unsigned char etat, FILE *fichier, unsigned char canal, unsigned char Note_MIDI, unsigned char velocite) {

	unsigned char octets[3] ;
	octets[0] = etat + canal % 16 ;
	octets[1] = Note_MIDI % 128 ;
	octets[2] = velocite % 128 ;
	fwrite(&octets, 3, 1, fichier) ;
}

/**
* \fn void MIDI_Unique_Note(FILE *fichier, unsigned char canal, unsigned char Note_MIDI, unsigned char velocite, unsigned long duree)
* \brief Write the data corresponding to a note in a monoton track. Help to avoid the errors in the delta-time, as the time of the note is given.
*
* \param[in] etat is the state of the note (ON or OFF).
* \param[out] fichier is the MIDI file to be modified.
* \param[in] canal is the chanel on which we want to play the note.
* \param[in] Note_MIDI is the ID of the value (in terms of music, like a C, or a A) of a note.
* \param[in] velocite is the value that determine how loud the note will be played.
* \param[in] duree represents the time the note will be played.
* \return Ne retourne rien mais écrit dans le fichier.
*/
void MIDI_Unique_Note(FILE *fichier, unsigned char canal, unsigned char Note_MIDI, unsigned char velocite, unsigned long duree) {

	MIDI_delta_time(fichier, 0) ;
	MIDI_Note(ON,  fichier, canal, Note_MIDI, velocite) ;
	MIDI_delta_time(fichier, duree) ;
	MIDI_Note(OFF, fichier, canal, Note_MIDI, 0) ;

}

/**
* \fn void write_midi_1_voice(FILE *fichier,monotone_track * track, unsigned char instrument)
* \brief Write in the file one musical voice.
*
* \param[out] fichier is the MIDI file to be modified.
* \param[in] track is the track that we use to write the correct musical voice into the MIDI file.
* \param[in] instrument is the ID of the instrument we chose to play the musical voice.
* \return Don't return anything.
*/
 void write_midi_1_voice(FILE *fichier,monotone_track * track, unsigned char instrument) {

	unsigned long marque = MIDI_header_track(fichier);
	MIDI_Program_Change(fichier, 0, instrument);

	read_monotone_track(fichier,track->tab_notes,track->nb_notes);

	MIDI_end_of_track(fichier);
	final_sizeoftrack_writer(fichier, marque);
}

/**
* \fn void write_midi_1_voice(FILE *fichier,monotone_track * track, unsigned char instrument)
* \brief Write in the file a polyton music with 10 voices playing at the same time in the "worst" case.
*
* \param[out] fichier is the MIDI file to be modified.
* \param[in] mono is the track that is read to write the caracteristics of a voice in the MIDI file.
* \param[in] canal is the chanel on which we gonna play this voice.
* \return Don't return anything.
*/
void write_midi_n_voices(FILE *fichier, monotone_track * mono, unsigned char canal, unsigned char instrument){

	unsigned long marque = MIDI_header_track(fichier);
	MIDI_Program_Change(fichier, canal, (unsigned char)instrument);

	for(int i=0; i<(mono->nb_notes); i++){
        MIDI_delta_time(fichier, 0) ;
        MIDI_Note(ON,  fichier, canal, mono->tab_notes[i].valeur, mono->tab_notes[i].velocite) ;
        MIDI_delta_time(fichier, mono->tab_notes[i].duree) ;
        MIDI_Note(OFF, fichier, canal, mono->tab_notes[i].valeur, 0) ;
	}


    	MIDI_end_of_track(fichier);
	final_sizeoftrack_writer(fichier, marque);

}

/**
* \fn void midi_file_1_voice(NoteFreq * tf, int nb_notes, char * nom_fichier_midi, unsigned long tempo, unsigned char instrument)
* \brief Take as input the output of the last block of the project, and make a MIDI file out of the data given and the instrument chosen.
*
* \param[in] tf is the tab with the informations describing the caracteristics of the notes we have to write into the MIDI file.
* \param[in] nb_notes is the number of notes in this music (the notes can be silences too).
* \param[in] nom_fichier_midi is the name of the MIDI file we want to create.
* \param[in] tempo is the tempo of the music.
* \param[in] instrument is the ID of the instrument chosen to play the music.
* \return Don't return anything.
*/
void midi_file_1_voice(NoteFreq * tf, int nb_notes, char * nom_fichier_midi, unsigned long tempo, unsigned char instrument){

    monotone_track * track = NULL;
    track = write_monotone_track(tf,nb_notes, tempo,0);

    FILE *fichier_midi = open_midi_file(nom_fichier_midi) ;

    MIDI_header_writer(fichier_midi, 1, 2, noire) ;

    write_track1(fichier_midi, tempo) ; //500000 correspond à la durée d'une noire ici car tempo = 500000;

    write_midi_1_voice(fichier_midi,track,instrument);

    close_midi_file(fichier_midi) ;

    free_monotone_track(track);

    return;

}

/**
* \fn void midi_file_n_voices(NoteFreq * tf, int nb_notes, char * nom_fichier_midi, unsigned long tempo, unsigned char nb_voix)
* \brief Take as input the output of the last block of the project, and make a MIDI file out of the data given and the instrument chosen. The number of maximum distinct voices is 10.
*
* \param[in] tf is the tab with the informations describing the caracteristics of the notes we have to write into the MIDI file.
* \param[in] nb_notes is the number of notes in this music (the notes can be silences too).
* \param[in] nom_fichier_midi is the name of the MIDI file we want to create.
* \param[in] tempo is the tempo of the music.
* \param[in] nb_voix is the number of voices in this music.
* \return EXIT_SUCCESS or EXIT_FAILURE
*/
int midi_file_n_voices(NoteFreq * tf, int nb_notes, char * nom_fichier_midi, unsigned long tempo, unsigned char nb_voix, unsigned char instrument){

    unsigned char i;

    if(tf==NULL){
        fprintf(stderr,"Cannot use previous data to create MIDI file.\n");
        return EXIT_FAILURE;
    }

    monotone_track * track = NULL;
    FILE *fichier_midi = open_midi_file(nom_fichier_midi) ;

    /*File opening verification*/
    if(fichier_midi==NULL){
        fprintf(stderr,"Cannot create file %s\n",nom_fichier_midi);
        return EXIT_FAILURE;
    }

    MIDI_header_writer(fichier_midi, 1, (unsigned short)(nb_voix+1), noire) ;

    write_track1(fichier_midi, tempo) ; //500000 correspond à la durée d'une noire ici car tempo = 500000;

    for(i = 0; i<nb_voix; i++){

        track = NULL;
        track = write_monotone_track(tf,nb_notes, tempo,(int)i);

        write_midi_n_voices(fichier_midi, track, i, instrument);
        free_monotone_track(track);
    }

    close_midi_file(fichier_midi) ;

    return EXIT_SUCCESS;

}

/**
* \fn FILE * open_midi_file(char * nom_fichier_midi)
* \brief Take as input the name of the MIDI file and initialize it.
* \param[in] nom_fichier_midi is the name of the MIDI file we want to create.
* \param nom_fichier_midi is the name of the MIDI file we want to create.
* \return Don't return anything.
*/
FILE * open_midi_file(char * nom_fichier_midi){

	FILE * newmidi = fopen(nom_fichier_midi, "wb") ;
	if(newmidi == NULL){

		fprintf(stderr,"%s",strerror(errno));
		return(NULL);

	}

	return newmidi;

}

/**
* \fn FILE * close_midi_file(FILE * midi_file)
* \brief Take as input the file to be closed, and handle the case of error.
* \param[in] nom_fichier_midi is the name of the MIDI file we want to close.
* \param midi_file is the MIDI file to be closed.
* \return Return EXIT_SUCCESS if the close action succeeded, EXIT_FAILURE otherwise.
*/
int close_midi_file(FILE * midi_file){

	if(fclose(midi_file)==EOF){

		fprintf(stderr,"%s",strerror(errno));
		return(EXIT_FAILURE);

	}

        return(EXIT_SUCCESS);
}

/**
 * \fn unsigned char getInstrumentMIDICode(char* i)
 * \brief gives MIDI code corresponding to instrument name
 * \author Brice Croix
 * \param i string containing instrument name
 * \return MIDI code corresponding to input instrument or 0xFF on error
 */
unsigned char getInstrumentMIDICode(char* i) {
	/*pianos*/
	if (strcasecmp(i, "piano") == 0 || strcasecmp(i, "acoustic_grand_piano") == 0) {
		return 0;
	} else if (strcasecmp(i, "bright_acoustic_piano") == 0) {
		return 1;
	} else if (strcasecmp(i, "electric_grand_piano") == 0) {
		return 2;
	} else if (strcasecmp(i, "honky-tonk_piano") == 0) {
		return 3;
	} else if (strcasecmp(i, "electric_piano") == 0 || strcasecmp(i, "electric_piano_1") == 0) {
		return 4;
	} else if (strcasecmp(i, "electric_piano_2") == 0) {
		return 5;
	} else if (strcasecmp(i, "harpsichord") == 0) {
		return 6;
	} else if (strcasecmp(i, "clavinet") == 0) {
		return 7;

		/*chromatic percussions*/
	} else if (strcasecmp(i, "celesta") == 0) {
		return 8;
	} else if (strcasecmp(i, "glockenspiel") == 0) {
		return 9;
	} else if (strcasecmp(i, "music_box") == 0) {
		return 10;
	} else if (strcasecmp(i, "vibraphone") == 0) {
		return 11;
	} else if (strcasecmp(i, "marimba") == 0) {
		return 12;
	} else if (strcasecmp(i, "xylophone") == 0) {
		return 13;
	} else if (strcasecmp(i, "tubular_bells") == 0) {
		return 14;
	} else if (strcasecmp(i, "dulcimer") == 0) {
		return 15;

		/*Organs*/
	} else if (strcasecmp(i, "organ") == 0 || strcasecmp(i, "drawbar_organ") == 0) {
		return 16;
	} else if (strcasecmp(i, "percussive_organ") == 0) {
		return 17;
	} else if (strcasecmp(i, "rock_organ") == 0) {
		return 18;
	} else if (strcasecmp(i, "church_organ") == 0) {
		return 19;
	} else if (strcasecmp(i, "reed_organ") == 0) {
		return 20;
	} else if (strcasecmp(i, "accordion") == 0) {
		return 21;
	} else if (strcasecmp(i, "harmonica") == 0) {
		return 22;
	} else if (strcasecmp(i, "tango_accordion") == 0) {
		return 23;

		/*Guitars*/
	} else if (strcasecmp(i, "acoustic_guitar_nylon") == 0) {
		return 24;
	} else if (strcasecmp(i, "guitar") == 0 || strcasecmp(i, "acoustic_guitar") == 0 ||
			   strcasecmp(i, "acoustic_guitar_steel") == 0) {
		return 25;
	} else if (strcasecmp(i, "electric_guitar_jazz") == 0) {
		return 26;
	} else if (strcasecmp(i, "electric_guitar_clean") == 0 || strcasecmp(i, "electric_guitar") == 0) {
		return 27;
	} else if (strcasecmp(i, "electric_guitar_muted") == 0) {
		return 28;
	} else if (strcasecmp(i, "overdriven_guitar") == 0) {
		return 29;
	} else if (strcasecmp(i, "distortion_guitar") == 0) {
		return 30;
	} else if (strcasecmp(i, "guitar_harmonics") == 0) {
		return 24;

	//TODO: finish this function
	/*Bass*/
	/*strings*/
	/*ensemble*/
	/*Brass*/
	/*Reed*/
	/*Pipe*/
	/*synth lead*/
	/*synth pads*/
	/*synth effects*/
	/*ethnics*/
	/*percussive*/
	/*sound effects*/

	}else{
		return 0xFF;
	}
}



