/**\file mainFunctions.c
 * \brief This file provides the main function of this program, by calling successively all useful functions
 * \author Brice Croix
 * \date 24.05.2019
 */

#include "mainFunctions.h"

/**
 * \fn int mainFunction(char* file_in, char* file_out, int tempo, unsigned char instrument)
 * \brief Main function of this program, converts a wav file given as file_in to MIDI file given as file_out
 * \author Brice Croix
 * \param file_in name of the input file
 * \param file_out name of the output file
 * \param tempo tempo of this input file
 * \param instrument MIDI code of an instrument
 * \param verbose How much informations do have to be printed on stdout
 * \return mainExit enum
 */
mainExit mainFunction(char* file_in, char* file_out, unsigned long tempo, unsigned char instrument, unsigned char verbose){
    /*Variables declaration*/

    int* samples; /**<\var samples array to store the wav samples*/
    WavHeader h; /**<\var wavHeader structure to store the information given by the wav file*/
    __uint32_t nb_samples; /**<\var The number of samples properly loaded in the samples array*/
    double ** power_spectrum_2d; /**<\var An array of arrays, those last containing each one the spectrum of a single window*/
    fftConstants fftInfo; /**<\var All useful constants used during the Fast Fourier Transform*/
    int nb_notes; /**<\var The number of different "configurations" in the file*/
    NoteFreq * frequency_matrix; /**<\var a double dimension matrix giving the different consecutive "configurations" of notes in the file*/
    unsigned char nb_voix; /**<\var number of channels in the output file, by now it is always NOTE_AMOUNT*/
    frequencyGroup* notes_per_window; /**<\var An array with notes informations for every window*/
    int write_midi_exit_value; /**<\var A flag with value EXIT_SUCCESS or EXIT_FAILURE for the midi writer function*/

    /*End of Variables declaration*/

    /*Let us begin to convert the file*/
    if(verbose) fprintf(stdout,"----- Starting conversion -----\n\n");

    /*Load the wav file data in the wavHeader structure and samples array*/
    nb_samples = load(file_in, &h, &samples);

    /*Verification that all data were properly loaded*/
    if((h.dwSampleLength != nb_samples) || (h.exit!=WAV_EXIT_SUCCESS)){
        if(verbose) fprintf(stdout,"Could not read the file %s, execution stopped.\n\n----- End of conversion -----\n",file_in);
        switch(h.exit){
            case WAV_EXIT_OPEN_ERROR:
                return MAIN_EXIT_WAV_OPEN_ERROR;
            case WAV_EXIT_NOT_WAV:
                return MAIN_EXIT_WAV_NOT_WAV;
            case WAV_EXIT_NOT_PROPER_WAV:
                return MAIN_EXIT_WAV_NOT_PROPER_WAV;
            case WAV_EXIT_EMPTY:
                return MAIN_EXIT_WAV_EMPTY;
            case WAV_EXIT_UNKNOWN_CHUNK:
                return MAIN_EXIT_WAV_UK_CHUNK;
            default:
                return MAIN_EXIT_UNKNOWN_ERROR;
        }
    }else{
        if(verbose) fprintf(stdout,"File %s successfully loaded. It is sampled at %d Hz and contains %d samples.\n",file_in,h.nSamplesPerSec,nb_samples);
    }

    /*Do the Fourier analysis of the loaded samples*/
    fftInfo= fft_main_function(h, samples, &power_spectrum_2d);

    /*Free the useless samples array*/
    free(samples);

    /*Verification that the Fourier analysis performed properly*/
    if(fftInfo.abandonFlag==ABANDON){
        if(verbose) fprintf(stdout,"Not enough data, either %d samples is too few to perform Fourier transform or %d Hz sampling rate is too low.\n\n----- End of conversion -----\n",nb_samples,fftInfo.samplingRate);
        return MAIN_EXIT_FOURIER_ABANDON;
    }else{
        if(verbose) fprintf(stdout,"Fourier transform done successfully. The file was divided into %d windows of %d samples.\n",fftInfo.nbrWindow,fftInfo.sizeWindow);
    }

    /*Interpret the results of the Fourier transformation*/
    nb_notes = 0;
    notes_per_window = preFrequencyConverter(power_spectrum_2d, fftInfo);
    //notes_per_window=NULL;

    /*free the useless power_spectrum matrix*/
    freePowerSpectrum(power_spectrum_2d,fftInfo.nbrWindow);

    /*Interpret the results of the Fourier transformation, part 2*/
    frequency_matrix = frequencyConverter(notes_per_window,fftInfo,&nb_notes);

    /* free the useless notes_per_window array*/
    free(notes_per_window);

    /*Theoretically these functions cannot fail since the signal has at least one window*/
    if(verbose) fprintf(stdout,"Fourier data were successfully converted to notes information. This file contains %d notes.\n",nb_notes);

    /*Let us print all notes*/
    if(verbose==VERBOSE_HEAVY) printFrequencyMatrix(frequency_matrix, nb_notes);

    /*Let us create the output file*/
    nb_voix = NOTE_AMOUNT;
    write_midi_exit_value = midi_file_n_voices(frequency_matrix,nb_notes,file_out,tempo,nb_voix,instrument);

    /*free the useless frequency_matrix array*/
    free(frequency_matrix);

    /*Verification that the file has properly been written*/
    if(write_midi_exit_value==EXIT_FAILURE){
        if(verbose) fprintf(stdout,"Failed to create file %s\n\n----- End of conversion -----\n",file_out);
        return MAIN_EXIT_MIDI_FAILURE;
    }else {
        if(verbose) fprintf(stdout, "The file %s has successfully been created.\n", file_out);
    }

    if(verbose) fprintf(stdout,"\n----- End of conversion -----\n");

    return MAIN_EXIT_SUCCESS;
}

/**
 * \fn void printHelp()
 * \author Brice Croix
 * \brief This function prints on stdout the help page that is in help.txt
 */
void printHelp(){
    FILE* fptr;
    char c;
    char path[PATH_MAX];
    getcwd(path,PATH_MAX);
    strcat(path,"/help.txt");
    fptr = fopen(path, "r");
    if (fptr == NULL){
        fprintf(stderr,"Missing or corrupted help.txt file\n");
    }

    // Read contents from file
    c = (char)fgetc(fptr);
    while (c != EOF)    {
        fprintf (stdout,"%c", c);
        c = (char)fgetc(fptr);
    }
    fprintf(stdout,"\n");

    fclose(fptr);
}