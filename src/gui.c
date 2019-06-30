/**\file view.c
 * \brief this file aims to do the graphic interface of the program
 * \author Ayaan Toorab & Clement Landais
 * \date 02.05.2019
 */

#include "gui.h"


int mainView(){
    char path[PATH_MAX];
	SDL_Window *window = NULL;
	SDL_Renderer * renderer = NULL;

	//Initialisation du programme
	if(SDL_Init(SDL_INIT_VIDEO) != 0){

		SDL_ExitWithError("ERREUR : Initialisation SDL");

	}

	//Execution du programme

	if(SDL_CreateWindowAndRenderer(WINDOW_WIDTH,WINDOW_HEIGHT,SDL_WINDOW_SHOWN,&window,&renderer) != 0){

		SDL_ExitWithError("Impossible de créer la fenêtre et le rendu");

	}

	inputStructure input;
    initializeInputStructure(&input, FIRSTLOAD);
    mainExit main_exit;
    int tempo;
    unsigned char instrument;

	SDL_bool program_launched = SDL_TRUE;

	SDL_InitRenderer(window, renderer, input);

	SDL_Event event;

	//Gestion des évènements
	while(program_launched){

		while(SDL_PollEvent(&event))
		{

			switch(event.type){
				case SDL_MOUSEBUTTONDOWN:
                    if(SDL_BUTTON(SDL_GetMouseState(NULL, NULL))==SDL_BUTTON_LEFT){
                        actualizeInputFieldBorders(1, renderer);
                    }
                    getcwd(path,PATH_MAX);
                    strcat(path,"/gui/musical-note.bmp");
                    switch(SDL_BUTTON(SDL_GetMouseState(NULL, NULL))){

                        case SDL_BUTTON_LEFT:
                            if(event.button.x>=INPUT1POSX && event.button.x<=INPUT1POSX+INPUT1WIDTH && event.button.y>=INPUT1POSY && event.button.y<=INPUT1POSY+INPUT1HEIGHT){
                                input.inputFlag = 1;
                                actualizeInputFieldBorders(input.inputFlag, renderer);
                            } else if(event.button.x>=INPUT2POSX && event.button.x<=INPUT2POSX+INPUT2WIDTH && event.button.y>=INPUT2POSY && event.button.y<=INPUT2POSY+INPUT2HEIGHT){
                                input.inputFlag = 2;
                                actualizeInputFieldBorders(input.inputFlag, renderer);
                            } else if(event.button.x>=INPUT3POSX && event.button.x<=INPUT3POSX+INPUT3WIDTH && event.button.y>=INPUT3POSY && event.button.y<=INPUT3POSY+INPUT3HEIGHT){
                                input.inputFlag = 3;
                                actualizeInputFieldBorders(input.inputFlag, renderer);
                            } else if(event.button.x>=INPUTCASE_POSX && event.button.x<=INPUTCASE_POSX+INPUTCASEDIM && event.button.y>=INPUTCASE_POSY1 && event.button.y<=INPUTCASE_POSY1+INPUTCASEDIM){
                                input.inputFlag = 4;
                                input.instrumentFlag = PIANO;
                                actualizeInputFieldBorders(input.inputFlag, renderer);
                                SDL_CreateRectWithDimensionsFilled(renderer, INPUTCASEDIM, INPUTCASEDIM, INPUTCASE_POSX, INPUTCASE_POSY2, INPUTBORDERWIDTH, 0, 0, 0,240,240,240);
                                SDL_CreateRectWithDimensionsFilled(renderer, INPUTCASEDIM, INPUTCASEDIM, INPUTCASE_POSX, INPUTCASE_POSY3, INPUTBORDERWIDTH, 0, 0, 0,240,240,240);
                                SDL_NewTexture(window, renderer, path, INPUTCASE_POSX, INPUTCASE_POSY1);
                            } else if(event.button.x>=INPUTCASE_POSX && event.button.x<=INPUTCASE_POSX+INPUTCASEDIM && event.button.y>=INPUTCASE_POSY2 && event.button.y<=INPUTCASE_POSY2+INPUTCASEDIM){
                                input.inputFlag = 5;
                                input.instrumentFlag = GUITARE;
                                actualizeInputFieldBorders(input.inputFlag, renderer);
                                SDL_CreateRectWithDimensionsFilled(renderer, INPUTCASEDIM, INPUTCASEDIM, INPUTCASE_POSX, INPUTCASE_POSY1, INPUTBORDERWIDTH, 0, 0, 0,240,240,240);
                                SDL_CreateRectWithDimensionsFilled(renderer, INPUTCASEDIM, INPUTCASEDIM, INPUTCASE_POSX, INPUTCASE_POSY3, INPUTBORDERWIDTH, 0, 0, 0,240,240,240);
                                SDL_NewTexture(window, renderer, path, INPUTCASE_POSX, INPUTCASE_POSY2);

                            } else if(event.button.x>=INPUTCASE_POSX && event.button.x<=INPUTCASE_POSX+INPUTCASEDIM && event.button.y>=INPUTCASE_POSY3 && event.button.y<=INPUTCASE_POSY3+INPUTCASEDIM){
                                input.inputFlag = 6;
                                input.instrumentFlag = SAXOPHONE;
                                actualizeInputFieldBorders(input.inputFlag, renderer);
                                SDL_CreateRectWithDimensionsFilled(renderer, INPUTCASEDIM, INPUTCASEDIM, INPUTCASE_POSX, INPUTCASE_POSY1, INPUTBORDERWIDTH, 0, 0, 0,240,240,240);
                                SDL_CreateRectWithDimensionsFilled(renderer, INPUTCASEDIM, INPUTCASEDIM, INPUTCASE_POSX, INPUTCASE_POSY2, INPUTBORDERWIDTH, 0, 0, 0,240,240,240);
                                SDL_NewTexture(window, renderer, path, INPUTCASE_POSX, INPUTCASE_POSY3);
                            } else {
                                input.inputFlag = 0;
                                actualizeInputFieldBorders(input.inputFlag, renderer);
                            }

                            if(event.button.x>=HELPPOSX && event.button.x<=HELPPOSX+HELPSIZE && event.button.y>=HELPPOSY && event.button.y<=HELPPOSY+HELPSIZE){
                                    SDL_Help(window, renderer, &program_launched, &input);
                                    SDL_InitRenderer(window, renderer, input);
                            }

                            if(event.button.x>=RUNPOSX && event.button.x<=RUNPOSX+RUNSIZE && event.button.y>=RUNPOSY && event.button.y<=RUNPOSY+RUNSIZE){
                                    checkStructureValues(&input, window, renderer, &program_launched);

                                    if(input.correctFlag == CORRECT){
                                        tempo = (unsigned long)strtol(input.textInput3, NULL, 10);
                                        if(tempo==0){
                                            SDL_Popup(TEMPO_EQUAL_TO_0_POPUP, window, renderer, &program_launched, &input);
                                        } else {
                                            switch(input.instrumentFlag){
                                                case PIANO:
                                                    instrument = 0;
                                                    break;
                                                case GUITARE:
                                                    instrument = 25;
                                                    break;
                                                case SAXOPHONE:
                                                    instrument = 64;
                                                    break;
                                                default:
                                                    break;
                                            }

                                            char * wavFile = safe_calloc(PATH_MAX, sizeof (wavFile));
                                            char * midFile = safe_calloc(PATH_MAX, sizeof (midFile));

                                            getPath(&wavFile, &midFile, input);

                                            /* appel du programme */
                                            main_exit = mainFunction(wavFile, midFile, tempo, instrument, VERBOSE_NONE);


                                            free(wavFile);
                                            free(midFile);

                                            switch(main_exit){
                                                case MAIN_EXIT_SUCCESS:
                                                    SDL_Popup(MAIN_EXIT_SUCCESS_POPUP, window, renderer, &program_launched, &input);
                                                    initializeInputStructure(&input, RELOAD);
                                                    break;
                                                case MAIN_EXIT_WAV_OPEN_ERROR:
                                                    SDL_Popup(MAIN_EXIT_WAV_OPEN_ERROR_POPUP, window, renderer, &program_launched, &input);
                                                    break;
                                                case MAIN_EXIT_WAV_NOT_WAV:
                                                    SDL_Popup(MAIN_EXIT_WAV_NOT_WAV_POPUP, window, renderer, &program_launched, &input);
                                                    break;
                                                case MAIN_EXIT_WAV_NOT_PROPER_WAV:
                                                    SDL_Popup(MAIN_EXIT_WAV_NOT_PROPER_WAV_POPUP, window, renderer, &program_launched, &input);
                                                    break;
                                                case MAIN_EXIT_WAV_EMPTY:
                                                    SDL_Popup(MAIN_EXIT_WAV_EMPTY_POPUP, window, renderer, &program_launched, &input);
                                                    break;
                                                case MAIN_EXIT_FOURIER_ABANDON:
                                                    SDL_Popup(MAIN_EXIT_FOURIER_ABANDON_POPUP, window, renderer, &program_launched, &input);
                                                    break;
                                                case MAIN_EXIT_MIDI_FAILURE:
                                                    SDL_Popup(MAIN_EXIT_MIDI_FAILURE_POPUP, window, renderer, &program_launched, &input);
                                                    break;
                                                case MAIN_EXIT_UNKNOWN_ERROR:
                                                    SDL_Popup(MAIN_EXIT_UNKNOWN_ERROR_POPUP, window, renderer, &program_launched, &input);
                                                    break;
                                                case MAIN_EXIT_WAV_UK_CHUNK:
                                                    SDL_Popup(MAIN_EXIT_WAV_UK_CHUNK_POPUP, window, renderer, &program_launched, &input);
                                                    break;
                                                default:
                                                    break;
                                            }
                                        }
                                        SDL_InitRenderer(window, renderer, input);
                                    }
                            }

                            break;
                        case SDL_BUTTON_MIDDLE:
                            break;
                        case SDL_BUTTON_RIGHT:
                            break;
                        default:
                            break;

                    }
					break;
                case SDL_TEXTINPUT:
                    SDL_inputTextField(&input, window, renderer, event, 0);
                    break;
                case SDL_KEYDOWN:
                    switch(input.inputFlag){
                        case 0:
                            break;
                        case 1:
                            if(event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE)
                                SDL_inputTextField(&input, window, renderer, event, 1);
                            break;
                        case 2:
                            if(event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE)
                                SDL_inputTextField(&input, window, renderer, event, 1);
                            break;
                        case 3:
                            if(event.key.keysym.scancode == SDL_SCANCODE_BACKSPACE)
                                SDL_inputTextField(&input, window, renderer, event, 1);
                            break;
                        default:
                            break;
                    }

                    break;
				case SDL_QUIT:
					program_launched = SDL_FALSE;
					break;
				default:
					break;
			}

		}

	}
    free(input.textInput1);
    free(input.textInput2);
    free(input.textInput3);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	return EXIT_SUCCESS;


}

//Compilation : gcc view.c -o prog $(sdl2-config --cflags --libs)
//avec sdl_ttf : gcc view.c -o prog $(sdl2-config --cflags --libs) -lSDL2_ttf
//Si j'ai besoin de savoir comment "dessiner" regarder la vidéo sur le rendu.

//exemples :

/*if(SDL_SetRenderDrawColor(renderer, 112, 168, 237, SDL_ALPHA_OPAQUE) !=0){

		SDL_ExitWithError("Impossible de changer la couleur");

	}

	if(SDL_RenderDrawPoint(renderer, 100, 450) != 0){

		SDL_ExitWithError("Impossible d'afficher le point");

	}

	if(SDL_RenderDrawLine(renderer,50, 50, 500, 500) != 0){

		SDL_ExitWithError("Impossible d'afficher la ligne");

	}

	SDL_RenderPresent(renderer);*/

void getPath(char ** wavFile, char ** midFile, inputStructure input){

    strcat(*wavFile, getenv("HOME"));
    strcat(*midFile, getenv("HOME"));

    strcat(*wavFile, "/");
    strcat(*midFile, "/");

    char * input1 = safe_calloc(PATH_MAX, sizeof (input1));;
    char * input2 = safe_calloc(PATH_MAX, sizeof (input2));;

    for(int i = 0; i<PATH_MAX-2; i++){
        input1[i]=input.textInput1[i+2];
        input2[i]=input.textInput2[i+2];
    }

    strcat(*wavFile, input1);
    strcat(*midFile, input2);

    free(input1);
    free(input2);
}

void initializeInputStructure(inputStructure * input, int loadFlag){
    if(loadFlag == RELOAD){
        free(input->textInput1);
        free(input->textInput2);
        free(input->textInput3);
    }
    input->textInput1 = safe_calloc(PATH_MAX, sizeof (input->textInput1));
    input->textInput2 = safe_calloc(PATH_MAX, sizeof (input->textInput2));
    input->textInput3 = safe_calloc(PATH_MAX, sizeof (input->textInput3));
    input->textInput1[0]=126;
    input->textInput1[1]=47;
    input->textInput2[0]=126;
    input->textInput2[1]=47;
    input->textInput3[0]=32;
    input->indexInput1 = 2;
    input->indexInput2 = 2;
    input->indexInput3 = 0;
    input->inputFlag = 0;
    input->instrumentFlag = NOINSTRUMENT;
    input->correctFlag=INCORRECT;
}

int notOnlyNumbers(char * text, int sizeText){
    for(int i=0; i<sizeText; i++){
        if(text[i]<48 || text[i]>57)
            return 1;
    }
    return 0;
}

void checkStructureValues(inputStructure * input, SDL_Window * window, SDL_Renderer * renderer, SDL_bool * program_launched){
    if(input->indexInput1==2 || input->indexInput2==2) /* il manque le chemin pour un des fichiers .wav et .mid */
        SDL_Popup(NO_INPUT_FILES_POPUP, window, renderer, program_launched, input);
    else if(input->indexInput3==0) /* il manque le tempo */
        SDL_Popup(NO_TEMPO_POPUP, window, renderer, program_launched, input);
    else if(notOnlyNumbers(input->textInput3, input->indexInput3))/* le tempo n'est pas uniquement composé que de chiffres */
        SDL_Popup(NOT_ONLY_NUMBERS_IN_TEMPO_POPUP, window, renderer, program_launched, input);
    else if (input->instrumentFlag == NOINSTRUMENT)/* aucun instrument d'a été choisi */
        SDL_Popup(NO_INSTRUMENT_POPUP, window, renderer, program_launched, input);
    else /* tout est bon */
        input->correctFlag = CORRECT;

    SDL_InitRenderer(window, renderer, *input);
}

void SDL_inputTextField(inputStructure * input, SDL_Window * window, SDL_Renderer * renderer, SDL_Event event, int delFlag){

    switch(input->inputFlag){
        case 0:
            break;
        case 1:
            if(delFlag){
                if(input->indexInput1>=3){
                    input->indexInput1--;
                    input->textInput1[input->indexInput1]=0;
                }
            }else{
                input->indexInput1++;
                strcat(input->textInput1, event.text.text);
            }
            SDL_CreateRectWithDimensionsFilled(renderer, INPUT1HEIGHT, INPUT1WIDTH, INPUT1POSX, INPUT1POSY, INPUTBORDERWIDTH, SELECTEDBORDERCOLOR_R, SELECTEDBORDERCOLOR_G, SELECTEDBORDERCOLOR_B, 240, 240, 240);
            SDL_CreateText(window, renderer, INPUT1POSX+5, INPUT1POSY+5, input->textInput1, 20, ISANINPUT);
            break;
        case 2:
            if(delFlag){
                if(input->indexInput2>=3){
                    input->indexInput2--;
                    input->textInput2[input->indexInput2]=0;
                }
            }else{
                input->indexInput2++;
                strcat(input->textInput2, event.text.text);
            }
            SDL_CreateRectWithDimensionsFilled(renderer, INPUT2HEIGHT, INPUT2WIDTH, INPUT2POSX, INPUT2POSY, INPUTBORDERWIDTH, SELECTEDBORDERCOLOR_R, SELECTEDBORDERCOLOR_G, SELECTEDBORDERCOLOR_B, 240, 240, 240);
            SDL_CreateText(window, renderer, INPUT2POSX+5, INPUT2POSY+5, input->textInput2, 20, ISANINPUT);
            break;
        case 3:
            if(delFlag){
                if(input->indexInput3>=2){
                    input->indexInput3--;
                    input->textInput3[input->indexInput3]=0;
                } else if(input->indexInput3==1){
                    input->indexInput3--;
                    input->textInput3[input->indexInput3]=32;
                }
            }else{
                input->indexInput3++;
                if(input->textInput3[0]==32)
                    input->textInput3[0]=event.text.text[0];
                else
                    strcat(input->textInput3, event.text.text);
            }
                SDL_CreateRectWithDimensionsFilled(renderer, INPUT3HEIGHT, INPUT3WIDTH, INPUT3POSX, INPUT3POSY, INPUTBORDERWIDTH, SELECTEDBORDERCOLOR_R, SELECTEDBORDERCOLOR_G, SELECTEDBORDERCOLOR_B, 240, 240, 240);
                SDL_CreateText(window, renderer, INPUT3POSX+5, INPUT3POSY+5, input->textInput3, 20, ISANINPUT);
            break;
        default:
            break;
    }
    SDL_RenderPresent(renderer);
}

void actualizeInputFieldBorders(int fieldNumber, SDL_Renderer * renderer){

    switch(fieldNumber){
        case 0 :
            SDL_CreateRectWithDimensions(renderer, INPUT1HEIGHT, INPUT1WIDTH, INPUT1POSX, INPUT1POSY, INPUTBORDERWIDTH, 0, 0, 0);
            SDL_CreateRectWithDimensions(renderer, INPUT2HEIGHT, INPUT2WIDTH, INPUT2POSX, INPUT2POSY, INPUTBORDERWIDTH, 0, 0, 0);
            SDL_CreateRectWithDimensions(renderer, INPUT3HEIGHT, INPUT3WIDTH, INPUT3POSX, INPUT3POSY, INPUTBORDERWIDTH, 0, 0, 0);
            SDL_StopTextInput();
            break;
        case 1 :
            SDL_CreateRectWithDimensions(renderer, INPUT1HEIGHT, INPUT1WIDTH, INPUT1POSX, INPUT1POSY, INPUTBORDERWIDTH, SELECTEDBORDERCOLOR_R, SELECTEDBORDERCOLOR_G, SELECTEDBORDERCOLOR_B);
            SDL_CreateRectWithDimensions(renderer, INPUT2HEIGHT, INPUT2WIDTH, INPUT2POSX, INPUT2POSY, INPUTBORDERWIDTH, 0, 0, 0);
            SDL_CreateRectWithDimensions(renderer, INPUT3HEIGHT, INPUT3WIDTH, INPUT3POSX, INPUT3POSY, INPUTBORDERWIDTH, 0, 0, 0);
            SDL_StartTextInput();
            break;
        case 2 :
            SDL_CreateRectWithDimensions(renderer, INPUT1HEIGHT, INPUT1WIDTH, INPUT1POSX, INPUT1POSY, INPUTBORDERWIDTH, 0, 0, 0);
            SDL_CreateRectWithDimensions(renderer, INPUT2HEIGHT, INPUT2WIDTH, INPUT2POSX, INPUT2POSY, INPUTBORDERWIDTH, SELECTEDBORDERCOLOR_R, SELECTEDBORDERCOLOR_G, SELECTEDBORDERCOLOR_B);
            SDL_CreateRectWithDimensions(renderer, INPUT3HEIGHT, INPUT3WIDTH, INPUT3POSX, INPUT3POSY, INPUTBORDERWIDTH, 0, 0, 0);
            SDL_StartTextInput();
            break;
        case 3 :
            SDL_CreateRectWithDimensions(renderer, INPUT1HEIGHT, INPUT1WIDTH, INPUT1POSX, INPUT1POSY, INPUTBORDERWIDTH, 0, 0, 0);
            SDL_CreateRectWithDimensions(renderer, INPUT2HEIGHT, INPUT2WIDTH, INPUT2POSX, INPUT2POSY, INPUTBORDERWIDTH, 0, 0, 0);
            SDL_CreateRectWithDimensions(renderer, INPUT3HEIGHT, INPUT3WIDTH, INPUT3POSX, INPUT3POSY, INPUTBORDERWIDTH, SELECTEDBORDERCOLOR_R, SELECTEDBORDERCOLOR_G, SELECTEDBORDERCOLOR_B);
            SDL_StartTextInput();
            break;
        case 4 :
            SDL_CreateRectWithDimensions(renderer, INPUT1HEIGHT, INPUT1WIDTH, INPUT1POSX, INPUT1POSY, INPUTBORDERWIDTH, 0, 0, 0);
            SDL_CreateRectWithDimensions(renderer, INPUT2HEIGHT, INPUT2WIDTH, INPUT2POSX, INPUT2POSY, INPUTBORDERWIDTH, 0, 0, 0);
            SDL_CreateRectWithDimensions(renderer, INPUT3HEIGHT, INPUT3WIDTH, INPUT3POSX, INPUT3POSY, INPUTBORDERWIDTH, 0, 0, 0);
            SDL_StopTextInput();
            break;
        case 5 :
            SDL_CreateRectWithDimensions(renderer, INPUT1HEIGHT, INPUT1WIDTH, INPUT1POSX, INPUT1POSY, INPUTBORDERWIDTH, 0, 0, 0);
            SDL_CreateRectWithDimensions(renderer, INPUT2HEIGHT, INPUT2WIDTH, INPUT2POSX, INPUT2POSY, INPUTBORDERWIDTH, 0, 0, 0);
            SDL_CreateRectWithDimensions(renderer, INPUT3HEIGHT, INPUT3WIDTH, INPUT3POSX, INPUT3POSY, INPUTBORDERWIDTH, 0, 0, 0);
            SDL_StopTextInput();
            break;
        case 6 :
            SDL_CreateRectWithDimensions(renderer, INPUT1HEIGHT, INPUT1WIDTH, INPUT1POSX, INPUT1POSY, INPUTBORDERWIDTH, 0, 0, 0);
            SDL_CreateRectWithDimensions(renderer, INPUT2HEIGHT, INPUT2WIDTH, INPUT2POSX, INPUT2POSY, INPUTBORDERWIDTH, 0, 0, 0);
            SDL_CreateRectWithDimensions(renderer, INPUT3HEIGHT, INPUT3WIDTH, INPUT3POSX, INPUT3POSY, INPUTBORDERWIDTH, 0, 0, 0);
            SDL_StopTextInput();
            break;
        default:
            break;
    }
    SDL_RenderPresent(renderer);
}

int SDL_InitRenderer(SDL_Window * window, SDL_Renderer * renderer, inputStructure input){
    /*Recovery of the executable path*/
    char path[PATH_MAX];

	if(renderer == NULL){

		SDL_ExitWithError("Renderer de valeur NULL");

	}

	//Création de la texture d'arrière plan

	SDL_CreateBackground(window, renderer);

	//Tempo, fichier .wav, fichier .mid

	SDL_CreateText(window, renderer, 170, 350, ".wav", 90, ISNOTANINPUT);
	SDL_CreateText(window, renderer, 900, 350, ".mid", 90, ISNOTANINPUT);
	SDL_CreateText(window, renderer,80, 550, "tempo en bpm", 70, ISNOTANINPUT);

	//Aide
	SDL_CreateRectWithDimensions(renderer, HELPSIZE, HELPSIZE, HELPPOSX, HELPPOSY, 3, 0, 0, 0);
	SDL_CreateText(window, renderer, HELPPOSX+5, HELPPOSY+10, "?", 80, ISNOTANINPUT);

	//Nom de l'appli
	SDL_CreateText(window, renderer,(WINDOW_WIDTH - 580)/ 2, 120, "Midi Maker", 120, ISNOTANINPUT);

	//Input fichier .wav
    SDL_CreateRectWithDimensionsFilled(renderer, INPUT1HEIGHT, INPUT1WIDTH, INPUT1POSX, INPUT1POSY, INPUTBORDERWIDTH, 0, 0, 0, 240, 240, 240);
    SDL_CreateText(window, renderer, INPUT1POSX+5, INPUT1POSY+5, input.textInput1, 20, ISANINPUT);
	//Input fichier .mid
    SDL_CreateRectWithDimensionsFilled(renderer, INPUT2HEIGHT, INPUT2WIDTH, INPUT2POSX, INPUT2POSY, INPUTBORDERWIDTH, 0, 0, 0, 240, 240, 240);
    SDL_CreateText(window, renderer, INPUT2POSX+5, INPUT2POSY+5, input.textInput2, 20, ISANINPUT);
	//Input tempo
    SDL_CreateRectWithDimensionsFilled(renderer, INPUT3HEIGHT, INPUT3WIDTH, INPUT3POSX, INPUT3POSY, INPUTBORDERWIDTH, 0, 0, 0, 240, 240, 240);
    SDL_CreateText(window, renderer, INPUT3POSX+5, INPUT3POSY+5, input.textInput3, 20, ISANINPUT);
	//Bouton run (dimensions 120x120)
    getcwd(path,PATH_MAX);
	strcat(path,"/gui/play.bmp");
    SDL_NewTexture(window, renderer ,path ,RUNPOSX, RUNPOSY);
	//Choix Instrument
    SDL_CreateRectWithDimensionsFilled(renderer, INPUTCASEDIM, INPUTCASEDIM, INPUTCASE_POSX, INPUTCASE_POSY1, INPUTBORDERWIDTH, 0, 0, 0, 240, 240, 240);
    SDL_CreateRectWithDimensionsFilled(renderer, INPUTCASEDIM, INPUTCASEDIM, INPUTCASE_POSX, INPUTCASE_POSY2, INPUTBORDERWIDTH, 0, 0, 0, 240, 240, 240);
    SDL_CreateRectWithDimensionsFilled(renderer, INPUTCASEDIM, INPUTCASEDIM, INPUTCASE_POSX, INPUTCASE_POSY3, INPUTBORDERWIDTH, 0, 0, 0, 240, 240, 240);
    SDL_CreateText(window, renderer, INPUTCASE_POSX+110, INPUTCASE_POSY1-10, "piano", 50, ISNOTANINPUT);
    SDL_CreateText(window, renderer, INPUTCASE_POSX+100, INPUTCASE_POSY2-10, "guitare", 50, ISNOTANINPUT);
    SDL_CreateText(window, renderer, INPUTCASE_POSX+70, INPUTCASE_POSY3-10, "saxophone", 50, ISNOTANINPUT);

    /*Creating the image to open path*/
    getcwd(path,PATH_MAX);
    strcat(path,"/gui/musical-note.bmp");
    switch(input.instrumentFlag){
        case NOINSTRUMENT:
            break;
        case PIANO:
            SDL_NewTexture(window, renderer, path, INPUTCASE_POSX, INPUTCASE_POSY1);
            break;
        case GUITARE:
            SDL_NewTexture(window, renderer, path, INPUTCASE_POSX, INPUTCASE_POSY2);
            break;
        case SAXOPHONE:
            SDL_NewTexture(window, renderer, path, INPUTCASE_POSX, INPUTCASE_POSY3);
            break;
        default:
            break;
    }

	//Affichage du rendu, à gérer à la fin
	SDL_RenderPresent(renderer);

}

void SDL_Popup(errorCode err_code, SDL_Window * window, SDL_Renderer * renderer, SDL_bool * program_launched, inputStructure * input){


	if(renderer == NULL || window == NULL){

		SDL_ExitWithError("Window ou renderer fournis non valides");

	}

	SDL_bool subprogram_launched = SDL_TRUE;

	//Création de la texture de crédits

	SDL_CreateRectWithDimensionsFilled(renderer, POPUPWINDOW_HEIGHT, POPUPWINDOW_WIDTH, (WINDOW_WIDTH-POPUPWINDOW_WIDTH)/2, (WINDOW_HEIGHT-POPUPWINDOW_HEIGHT)/2, 3, 0, 0, 0, 255, 255, 255);

	//Texte
    switch(err_code){
        case MAIN_EXIT_SUCCESS_POPUP:
            SDL_CreateText(window, renderer, (WINDOW_WIDTH-POPUPWINDOW_WIDTH)/2+105, WINDOW_HEIGHT/2-35, "votre fichier a ete", 30, ISNOTANINPUT);
            SDL_CreateText(window, renderer, (WINDOW_WIDTH-POPUPWINDOW_WIDTH)/2+95, WINDOW_HEIGHT/2+5, "converti avec succes", 30, ISNOTANINPUT);
            break;
        case NO_INPUT_FILES_POPUP:
            SDL_CreateText(window, renderer, (WINDOW_WIDTH-POPUPWINDOW_WIDTH)/2+45, WINDOW_HEIGHT/2-35, "veuillez renseigner des chemins", 30, ISNOTANINPUT);
            SDL_CreateText(window, renderer, (WINDOW_WIDTH-POPUPWINDOW_WIDTH)/2+35, WINDOW_HEIGHT/2+5, "pour les fichiers .wav et .mid", 30, ISNOTANINPUT);
            break;
        case NO_TEMPO_POPUP:
            SDL_CreateText(window, renderer, (WINDOW_WIDTH-POPUPWINDOW_WIDTH)/2+55, WINDOW_HEIGHT/2-15, "veuillez renseigner un tempo", 30, ISNOTANINPUT);
            break;
        case NOT_ONLY_NUMBERS_IN_TEMPO_POPUP:
            SDL_CreateText(window, renderer, (WINDOW_WIDTH-POPUPWINDOW_WIDTH)/2+65, WINDOW_HEIGHT/2-35, "veuillez mettre uniquement", 30, ISNOTANINPUT);
            SDL_CreateText(window, renderer, (WINDOW_WIDTH-POPUPWINDOW_WIDTH)/2+65, WINDOW_HEIGHT/2+5, "des chiffres dans le tempo", 30, ISNOTANINPUT);
            break;
        case NO_INSTRUMENT_POPUP:
            SDL_CreateText(window, renderer, (WINDOW_WIDTH-POPUPWINDOW_WIDTH)/2+50, WINDOW_HEIGHT/2-15, "veuillez choisir un instrument", 30, ISNOTANINPUT);
            break;
        case TEMPO_EQUAL_TO_0_POPUP:
            SDL_CreateText(window, renderer, (WINDOW_WIDTH-POPUPWINDOW_WIDTH)/2+95, WINDOW_HEIGHT/2-35, "le tempo ne peut pas", 30, ISNOTANINPUT);
            SDL_CreateText(window, renderer, (WINDOW_WIDTH-POPUPWINDOW_WIDTH)/2+120, WINDOW_HEIGHT/2+5, "etre egal a zero", 30, ISNOTANINPUT);
            break;
        case MAIN_EXIT_WAV_OPEN_ERROR_POPUP:
            SDL_CreateText(window, renderer, (WINDOW_WIDTH-POPUPWINDOW_WIDTH)/2+105, WINDOW_HEIGHT/2-35, "impossible d'ouvrir", 30, ISNOTANINPUT);
            SDL_CreateText(window, renderer, (WINDOW_WIDTH-POPUPWINDOW_WIDTH)/2+120, WINDOW_HEIGHT/2+5, "le fichier .wav", 30, ISNOTANINPUT);
            break;
        case MAIN_EXIT_WAV_NOT_WAV_POPUP:
            SDL_CreateText(window, renderer, (WINDOW_WIDTH-POPUPWINDOW_WIDTH)/2+105, WINDOW_HEIGHT/2-35, "le fichier n'est pas", 30, ISNOTANINPUT);
            SDL_CreateText(window, renderer, (WINDOW_WIDTH-POPUPWINDOW_WIDTH)/2+115, WINDOW_HEIGHT/2+5, "un fichier .wav", 30, ISNOTANINPUT);
            break;
        case MAIN_EXIT_WAV_NOT_PROPER_WAV_POPUP:
            SDL_CreateText(window, renderer, (WINDOW_WIDTH-POPUPWINDOW_WIDTH)/2+90, WINDOW_HEIGHT/2-35, "la structure interne de", 30, ISNOTANINPUT);
            SDL_CreateText(window, renderer, (WINDOW_WIDTH-POPUPWINDOW_WIDTH)/2+80, WINDOW_HEIGHT/2+5, "ce fichier est incorrecte", 30, ISNOTANINPUT);
            break;
        case MAIN_EXIT_WAV_EMPTY_POPUP:
            SDL_CreateText(window, renderer, (WINDOW_WIDTH-POPUPWINDOW_WIDTH)/2+85, WINDOW_HEIGHT/2-15, "le fichier .wav est vide", 30, ISNOTANINPUT);
            break;
        case MAIN_EXIT_FOURIER_ABANDON_POPUP:
            SDL_CreateText(window, renderer, (WINDOW_WIDTH-POPUPWINDOW_WIDTH)/2+70, WINDOW_HEIGHT/2-55, "le fichier est trop court ou", 30, ISNOTANINPUT);
            SDL_CreateText(window, renderer, (WINDOW_WIDTH-POPUPWINDOW_WIDTH)/2+25, WINDOW_HEIGHT/2-15, "alors la frequence d'echantillonage", 30, ISNOTANINPUT);
            SDL_CreateText(window, renderer, (WINDOW_WIDTH-POPUPWINDOW_WIDTH)/2+125, WINDOW_HEIGHT/2+25, "est trop faible", 30, ISNOTANINPUT);
            break;
        case MAIN_EXIT_MIDI_FAILURE_POPUP:
            SDL_CreateText(window, renderer, (WINDOW_WIDTH-POPUPWINDOW_WIDTH)/2+85, WINDOW_HEIGHT/2-35, "erreur lors de l'ecriture", 30, ISNOTANINPUT);
            SDL_CreateText(window, renderer, (WINDOW_WIDTH-POPUPWINDOW_WIDTH)/2+115, WINDOW_HEIGHT/2+5, "du fichier .mid", 30, ISNOTANINPUT);
            break;
        case MAIN_EXIT_UNKNOWN_ERROR_POPUP:
            SDL_CreateText(window, renderer, (WINDOW_WIDTH-POPUPWINDOW_WIDTH)/2+100, WINDOW_HEIGHT/2-35, "une erreur inconnue", 30, ISNOTANINPUT);
            SDL_CreateText(window, renderer, (WINDOW_WIDTH-POPUPWINDOW_WIDTH)/2+125, WINDOW_HEIGHT/2+5, "s'est produite", 30, ISNOTANINPUT);
            break;
        case MAIN_EXIT_WAV_UK_CHUNK_POPUP:
            SDL_CreateText(window, renderer, (WINDOW_WIDTH-POPUPWINDOW_WIDTH)/2+90, WINDOW_HEIGHT/2-35, "la structure interne de", 30, ISNOTANINPUT);
            SDL_CreateText(window, renderer, (WINDOW_WIDTH-POPUPWINDOW_WIDTH)/2+80, WINDOW_HEIGHT/2+5, "ce fichier est incorrecte", 30, ISNOTANINPUT);
            break;
        default:
            break;
    }
	//Affichage
	SDL_RenderPresent(renderer);


	//Gestion des évènements
	while(subprogram_launched){

		SDL_Event subevent;

		while(SDL_PollEvent(&subevent))
		{

			switch(subevent.type){

				case SDL_MOUSEBUTTONDOWN:
                    switch(SDL_BUTTON(SDL_GetMouseState(NULL, NULL))){
                        case SDL_BUTTON_LEFT:
                            if(subevent.button.x>=(WINDOW_WIDTH+POPUPWINDOW_WIDTH)/2 || subevent.button.x<=(WINDOW_WIDTH-POPUPWINDOW_WIDTH)/2 || subevent.button.y>=(WINDOW_HEIGHT+POPUPWINDOW_HEIGHT)/2 || subevent.button.y<=(WINDOW_HEIGHT-POPUPWINDOW_HEIGHT)/2){
                                subprogram_launched = SDL_FALSE;
                            }
                            break;
                        default:
                            break;

					}
					break;
                 case SDL_QUIT:
                    subprogram_launched = SDL_FALSE;
					*program_launched = SDL_FALSE;
					break;
				default:
					break;
			}

		}

	}
    SDL_InitRenderer(window, renderer, *input);
	return;

}

void SDL_Help(SDL_Window * window, SDL_Renderer * renderer, SDL_bool * program_launched, inputStructure * input){


	if(renderer == NULL || window == NULL){

		SDL_ExitWithError("Window ou renderer fournis non valides");

	}

	SDL_bool subprogram_launched = SDL_TRUE;


	//Création de la texture de crédits

	SDL_CreateRectWithDimensionsFilled(renderer, SUBWINDOW_HEIGHT, SUBWINDOW_WIDTH, (WINDOW_WIDTH-SUBWINDOW_WIDTH)/2, (WINDOW_HEIGHT-SUBWINDOW_HEIGHT)/2, 3, 0, 0, 0, 255, 255, 255);

	//Texte

	SDL_CreateText(window, renderer, WINDOW_WIDTH/2-75, 110, "consignes", 60, ISNOTANINPUT);

	SDL_CreateText(window, renderer, WINDOW_WIDTH/2-75, 110, "consignes", 60, ISNOTANINPUT);
	SDL_CreateText(window, renderer, WINDOW_WIDTH/2-300, 180, "indiquez le chemin du fichier source et du fichier cible depuis", 30, ISNOTANINPUT);
 	SDL_CreateText(window, renderer, WINDOW_WIDTH/2-230, 210, "/home/user afin de proceder a la conversion", 30, ISNOTANINPUT);
	SDL_CreateText(window, renderer, WINDOW_WIDTH/2-250, 270, "indiquez le tempo en battements par minute (bpm)", 30, ISNOTANINPUT);
	SDL_CreateText(window, renderer, WINDOW_WIDTH/2-280, 330, "choisissez l'instrument a utiliser dans le fichier midi", 30, ISNOTANINPUT);
	SDL_CreateText(window, renderer, WINDOW_WIDTH/2-290, 390, "appuyez sur le bouton \"play\" pour debuter la conversion ", 30, ISNOTANINPUT);

	SDL_CreateText(window, renderer, WINDOW_WIDTH/2-250, 450, "pour quitter la fenetre d'aide, appuyez sur \"? \"", 30, ISNOTANINPUT);

	SDL_CreateText(window, renderer, WINDOW_WIDTH/2-70, 530, "credits", 60, ISNOTANINPUT);
	SDL_CreateText(window, renderer, WINDOW_WIDTH/2-360, 600, "landais clement", 30, ISNOTANINPUT);
	SDL_CreateText(window, renderer, WINDOW_WIDTH/2-160,600, "croix brice", 30, ISNOTANINPUT);
	SDL_CreateText(window, renderer, WINDOW_WIDTH/2-20,600, "toorab ayaan", 30, ISNOTANINPUT);
	SDL_CreateText(window, renderer, WINDOW_WIDTH/2+140,600, "four--pony milan", 30, ISNOTANINPUT);

	SDL_CreateText(window, renderer, WINDOW_WIDTH/2-200, 640, "'creez vos fichiers midi a votre guise!'", 30, ISNOTANINPUT);

	//Affichage

	SDL_RenderPresent(renderer);


	//Gestion des évènements
	while(subprogram_launched){

		SDL_Event subevent;

		while(SDL_PollEvent(&subevent))
		{

			switch(subevent.type){

				case SDL_MOUSEBUTTONDOWN:
                    switch(SDL_BUTTON(SDL_GetMouseState(NULL, NULL))){
                        case SDL_BUTTON_LEFT:
                            if(subevent.button.x>=HELPPOSX && subevent.button.x<=HELPPOSX+HELPSIZE && subevent.button.y>=HELPPOSY && subevent.button.y<=HELPPOSY+HELPSIZE){
                                subprogram_launched = SDL_FALSE;
                            }
                            break;
                        default:
                            break;

					}
					break;
                case SDL_QUIT:
                    subprogram_launched = SDL_FALSE;
					*program_launched = SDL_FALSE;
					break;
				default:
					break;
			}

		}

	}
    SDL_InitRenderer(window, renderer, *input);
	return;

}

void SDL_CreateText(SDL_Window *window, SDL_Renderer *renderer, int posx, int posy, const char * mots, int taille, int isAnInput){
    char path[PATH_MAX];

	if(TTF_Init() == -1)
	{
    		fprintf(stderr, "Erreur d'initialisation de TTF_Init : %s\n", TTF_GetError());
    		exit(EXIT_FAILURE);
	}

	SDL_Texture * texture = NULL;
	SDL_Surface * texte = NULL;
	TTF_Font *police = NULL;
	SDL_Color couleurNoire = {0, 0, 0};

    /*Creating the image to open path*/
    getcwd(path,PATH_MAX);

    if(isAnInput){
        strcat(path,"/gui/cour.ttf");
        police = TTF_OpenFont(path, taille);
    }else {
        strcat(path,"/gui/birds.ttf");
        police = TTF_OpenFont(path, taille);
    }

	texte = TTF_RenderText_Blended(police, mots, couleurNoire);

	texture = SDL_CreateTextureFromSurface(renderer, texte);
	SDL_FreeSurface(texte);

	if(texture == NULL){

		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_ExitWithError("Impossible de créer la texture");

	}

	SDL_Rect rect;

	if(SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h)!=0){

		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_ExitWithError("Impossible de charger la texture");

	}

	rect.x = posx;
	rect.y = posy;

	if(SDL_RenderCopy(renderer, texture, NULL, &rect)!=0){

		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_ExitWithError("Impossible d'afficher la texture");

	}

	TTF_CloseFont(police); /* Doit être avant TTF_Quit() */
	TTF_Quit();


}

void SDL_ExitWithError(const char * message){

	SDL_Log("%s > %s\n", message, SDL_GetError());
	SDL_Quit();
	exit(EXIT_FAILURE);

}

/*Initialise un rectangle, donc juste les bords avec un intérieur transparent, de hauteur height, de largeur width, et dont les du point (0;0), donc le sommet en haut à gauche, sont définies par posx et posy, avec SDL_RenderDrawRect(renderer,&rect) on dessine le rectangle voulu sur le rendu*/
SDL_Rect SDL_CreateRectWithDimensions(SDL_Renderer *renderer, int height, int width, int posx, int posy, int draw_width, int rBorder, int gBorder, int bBorder){

	int i = 0;
	SDL_Rect rect;

	if(SDL_SetRenderDrawColor(renderer, rBorder, gBorder, bBorder, SDL_ALPHA_OPAQUE) !=0){

		SDL_ExitWithError("Impossible de changer la couleur");

	}

	while(i<draw_width){

		rect.x = posx-i;
		rect.y = posy-i;
		rect.h = height+2*i;
		rect.w = width+2*i;

		if(SDL_RenderDrawRect(renderer,&rect) != 0){

			SDL_ExitWithError("Impossible d'afficher le rectangle");

		}
		i++;
	}

        return rect;

}

SDL_Rect SDL_CreateRectWithDimensionsFilled(SDL_Renderer *renderer, int height, int width, int posx, int posy, int draw_width, int rBorder, int gBorder, int bBorder, int rFill, int gFill, int bFill){

    SDL_Rect rect = SDL_CreateRectWithDimensions(renderer, height, width, posx, posy, draw_width, rBorder, gBorder, bBorder);

    if(SDL_SetRenderDrawColor(renderer, rFill, gFill, bFill, SDL_ALPHA_OPAQUE) !=0){

		SDL_ExitWithError("Impossible de changer la couleur");

	}

	SDL_RenderFillRect(renderer, &rect);

	if(SDL_SetRenderDrawColor(renderer, rBorder, gBorder, bBorder, SDL_ALPHA_OPAQUE) !=0){

		SDL_ExitWithError("Impossible de changer la couleur");

	}

	SDL_Rect rectBorder = SDL_CreateRectWithDimensions(renderer, height, width, posx, posy, draw_width, rBorder, gBorder, bBorder);

	return rectBorder;
}

void SDL_NewTexture(SDL_Window *window, SDL_Renderer *renderer, const char * bmp, int posx, int posy){

	SDL_Surface * image = NULL;
	SDL_Texture * texture = NULL;

	image = SDL_LoadBMP(bmp);

	if(image == NULL){

		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_ExitWithError("Impossible de charger l'image");

	}

	texture = SDL_CreateTextureFromSurface(renderer, image);
	SDL_FreeSurface(image);

	if(texture == NULL){

		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_ExitWithError("Impossible de créer la texture");

	}

	SDL_Rect rect;

	if(SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h)!=0){

		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_ExitWithError("Impossible de charger la texture");

	}

	rect.x = posx;
	rect.y = posy;

	if(SDL_RenderCopy(renderer, texture, NULL, &rect)!=0){

		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_ExitWithError("Impossible d'afficher la texture");

	}

	SDL_RenderPresent(renderer);

}

void SDL_CreateBackground(SDL_Window *window, SDL_Renderer *renderer) {
    char path[PATH_MAX];

	SDL_Surface *image = NULL;
	SDL_Texture *texture = NULL;

	getcwd(path,PATH_MAX);
	strcat(path,"/gui/background.bmp");
	image = SDL_LoadBMP(path);

	if (image == NULL) {

		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_ExitWithError("Impossible de charger l'image");

	}

	texture = SDL_CreateTextureFromSurface(renderer, image);
	SDL_FreeSurface(image);

	if (texture == NULL) {

		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_ExitWithError("Impossible de créer la texture");

	}

	SDL_Rect background;

	if (SDL_QueryTexture(texture, NULL, NULL, &background.w, &background.h) != 0) {

		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_ExitWithError("Impossible de charger la texture");

	}

	background.x = (WINDOW_WIDTH - background.w) / 2;
	background.y = (WINDOW_HEIGHT - background.h) / 2;

	if (SDL_RenderCopy(renderer, texture, NULL, &background) != 0) {

		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_ExitWithError("Impossible d'afficher la texture");

	}

}
