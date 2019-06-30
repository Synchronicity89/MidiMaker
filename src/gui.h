/**\file view.h
 * \brief this is the header file associated to view.c
 * \author Ayaan Toorab & Clement Landais
 * \date 02.05.2019
 */

#ifndef VIEW_H
#define VIEW_H

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL_ttf.h>
#include <limits.h>
#include "global.h"
#include "mainFunctions.h"
#include <unistd.h>

#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800

#define SUBWINDOW_WIDTH 800
#define SUBWINDOW_HEIGHT 600

#define POPUPWINDOW_WIDTH 400
#define POPUPWINDOW_HEIGHT 150

#define ISANINPUT 1
#define ISNOTANINPUT 0

#define CORRECT 1
#define INCORRECT 0
#define RELOAD 1
#define FIRSTLOAD 0

#define INPUT1POSX 10
#define INPUT1POSY 430
#define INPUT2POSX 730
#define INPUT2POSY 430
#define INPUT3POSX 170
#define INPUT3POSY 630
#define INPUT1HEIGHT 30
#define INPUT1WIDTH 450
#define INPUT2HEIGHT 30
#define INPUT2WIDTH 450
#define INPUT3HEIGHT 30
#define INPUT3WIDTH 140
#define INPUTBORDERWIDTH 2
#define SELECTEDBORDERCOLOR_R 44
#define SELECTEDBORDERCOLOR_G 51
#define SELECTEDBORDERCOLOR_B 204
#define INPUTCASEDIM 30
#define INPUTCASE_POSX 790
#define INPUTCASE_POSY1 540
#define INPUTCASE_POSY2 620
#define INPUTCASE_POSY3 700
#define RUNPOSX 530
#define RUNPOSY 320
#define RUNSIZE 120
#define HELPPOSX 1110
#define HELPPOSY 20
#define HELPSIZE 82

#define NOINSTRUMENT 0
#define PIANO 1
#define GUITARE 2
#define SAXOPHONE 3

typedef struct{
    char * textInput1;
    char * textInput2;
    char * textInput3;
    int indexInput1;
    int indexInput2;
    int indexInput3;
    int inputFlag;
    int instrumentFlag;
    int correctFlag;
}inputStructure;

typedef enum errorCode{
    MAIN_EXIT_SUCCESS_POPUP,
    MAIN_EXIT_WAV_OPEN_ERROR_POPUP,
    MAIN_EXIT_WAV_NOT_WAV_POPUP,
    MAIN_EXIT_WAV_NOT_PROPER_WAV_POPUP,
    MAIN_EXIT_WAV_EMPTY_POPUP,
    MAIN_EXIT_FOURIER_ABANDON_POPUP,
    MAIN_EXIT_MIDI_FAILURE_POPUP,
    MAIN_EXIT_UNKNOWN_ERROR_POPUP,
    MAIN_EXIT_WAV_UK_CHUNK_POPUP,
    NO_INPUT_FILES_POPUP,
    NO_TEMPO_POPUP,
    NOT_ONLY_NUMBERS_IN_TEMPO_POPUP,
    NO_INSTRUMENT_POPUP,
    TEMPO_EQUAL_TO_0_POPUP,
}errorCode;

int mainView();

void getPath(char ** wavFile, char ** midFile, inputStructure input);
void SDL_ExitWithError(const char * message);
void SDL_CreateBackground(SDL_Window *window, SDL_Renderer *renderer);
SDL_Rect SDL_CreateRectWithDimensions(SDL_Renderer *renderer, int height, int width, int posx, int posy, int draw_width, int rBorder, int gBorder, int bBorder);
SDL_Rect SDL_CreateRectWithDimensionsFilled(SDL_Renderer *renderer, int height, int width, int posx, int posy, int draw_width, int rBorder, int gBorder, int bBorder, int rFill, int gFill, int bFill);
void SDL_NewTexture(SDL_Window *window, SDL_Renderer *renderer, const char * bmp, int posx, int posy);
void SDL_CreateText(SDL_Window *window, SDL_Renderer *renderer,int posx, int posy, const char * mots, int taille, int isAnInput);
void SDL_Help(SDL_Window * window, SDL_Renderer * renderer, SDL_bool * program_launched, inputStructure * input);
int SDL_InitRenderer(SDL_Window * window, SDL_Renderer * renderer, inputStructure input);
void actualizeInputFieldBorders(int fieldNumber, SDL_Renderer * renderer);
void SDL_inputTextField(inputStructure * input, SDL_Window * window, SDL_Renderer * renderer, SDL_Event event, int delFlag);
void initializeInputStructure(inputStructure * input, int loadFlag);
void checkStructureValues(inputStructure * input, SDL_Window * window, SDL_Renderer * renderer, SDL_bool * program_launched);
void SDL_Popup(errorCode err_code, SDL_Window * window, SDL_Renderer * renderer, SDL_bool * program_launched, inputStructure * input);
int notOnlyNumbers(char * text, int sizeText);

#endif
