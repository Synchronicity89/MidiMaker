/**
 * \file main.c
 * \brief this file is the main file of this program, it provides the main function required to run.
 * \author Brice Croix
 * \date 18.05.2019
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
#include "FFT.h"
 //#include <unistd.h>
#include <windows.h>
#include "frequencyHandler.h"
#include "mainFunctions.h"
#include "MIDIHandler.h"
#include "notes.h"
#include "wavHandler.h"
#include "gui.h"


/**
 * \fn int main(int argc, char** argv)
 * \brief main function of this program
 * \author Brice Croix
 * \param argc Number of arguments given to the program
 * \param argv string array containing the program arguments
 * \return EXIT_SUCCESS or EXIT_FAILURE
 */
int main(int argc, char** argv){

    /*Variables declaration*/
    unsigned long tempo; /**<\var tempo of the wav file, in bpm*/
    char file_out[PATH_MAX]; /**<\var name of the wav file to transform*/
    char file_in[PATH_MAX]; /**<\var name of the output file*/
    int file_in_length; /**<\var Length of file_in*/
    unsigned char instrument; /**<\var MIDI code corresponding to an instrument*/
    int main_exit; /**<\var a flag returned by mainFunction to inform how conversion went*/
    int opt;/**<\var a variable to handle options*/
    int nb_opt_arg;/**<\var number of options and option parameters given to the program*/
    unsigned char verbose;/**<\var a flag to handle different levels of verbose*/
    /*End of Variables declaration*/

    /*Adding an empty file_out name*/
    file_out[0] = '\0';

    /*Recovery of program options*/
    instrument = 0;
    nb_opt_arg=0;
    verbose = VERBOSE_NONE;
    while((opt=getopt(argc,argv,"vVhi:o:")) != -1){
        nb_opt_arg++;
        switch(opt){
            case 'h':
                printHelp();
                return EXIT_SUCCESS;
            case 'v':
                verbose = VERBOSE_LIGHT;
                break;
            case 'V':
                verbose = VERBOSE_HEAVY;
                break;
            case 'i':
                nb_opt_arg++;
                instrument = getInstrumentMIDICode(optarg);
                if(instrument==0xFF){
                    fprintf(stdout,"'%s' is not a valid instrument, piano was chosen by default.\n",optarg);
                    instrument = 0;
                }
                break;
            case 'o':
                nb_opt_arg++;
                strcpy(file_out, optarg);
                break;
            default:
                fprintf(stdout,"%c is not a valid option, see 'MIDIMaker -h' for help\n",optopt);
                break;
        }
    }

    if(argc==1){ /*No arguments were given, the graphical program must be used*/

        mainView();
        return EXIT_SUCCESS;

    }else if(argc==2){ /*The program only has the wav file to use, it is missing the tempo value*/

        fprintf(stdout, "Too few arguments. This program must be called as follow :\n\tMIDIMaker <wav_file> <tempo_value> [OPTIONS]\n"
                        "See 'MIDIMaker -h' for help\n");

        return EXIT_FAILURE;
    }else if(argc-nb_opt_arg == 3){ /* The program has been called properly, it is normally able to run as planned*/
        /*Recovery of the file_in name*/
        strcpy(file_in,argv[argc-2]);

        /*Recovery of the tempo*/
        tempo = (unsigned long)strtol(argv[argc-1], NULL, 10);
        if(tempo==0){
            fprintf(stdout,"The tempo %s given as parameter is not valid. This program must be called as follow :\n\tMIDIMaker <wav_file> <tempo_value> [OPTIONS]\n"
                           "See 'MIDIMaker -h' for help\n", argv[argc-1]);
            return EXIT_FAILURE;
        }

        /*Generating the output file name if no -o option*/
        if(file_out[0]=='\0') {
            file_in_length = (int) strlen(file_in);
            strcpy(file_out, file_in);
            if (file_in[file_in_length - 4] == '.' && file_in[file_in_length - 3] == 'w' &&
                file_in[file_in_length - 2] == 'a' && file_in[file_in_length - 1] == 'v') {
                /*If file_in ends by ".wav"*/
                file_out[file_in_length - 3] = 'M';
                file_out[file_in_length - 2] = 'I';
                file_out[file_in_length - 1] = 'D';
            } else {
                /*If for some reason the input file does not have an extension*/
                strcat(file_out, ".MID");
            }
        }

        /*Starting main program*/
        main_exit = mainFunction(file_in,file_out,tempo,instrument,verbose);

        /*Return a coherent EXIT value*/
        if(main_exit!=MAIN_EXIT_SUCCESS){
            if(verbose==VERBOSE_NONE)fprintf(stdout,"Error occurred, file was not created\n");
            return EXIT_FAILURE;
        }else{
            if(verbose==VERBOSE_NONE)fprintf(stdout,"File successfully created\n");
            return EXIT_SUCCESS;
        }
    }else{
        fprintf(stdout, "Invalid number of arguments. This program must be called as follow :\n\tMIDIMaker <wav_file> <tempo_value> [OPTIONS]\n"
                        "See 'MIDIMaker -h' for help\n");
        return EXIT_FAILURE;
    }
}
