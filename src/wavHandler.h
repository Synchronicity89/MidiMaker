/** \file wavHandler.h
 * \author brice
 * \date 19/02/19.
 * \brief header file associated to the wavHandler.c file.
 */

#ifndef WAVHANDLER_H
#define WAVHANDLER_H

#include "stdintgcc.h"
#include <stdio.h>
#include <stdlib.h>
#include "global.h"

/**The standard format codes for waveform data. Only WAVE_FORMAT_PCM and WAVE_FORMAT_EXTENSIBLE seems to be useful theses days.*/
#define WAVE_FORMAT_UNDEFINED 0x0000
#define WAVE_FORMAT_PCM 0x0001
#define WAVE_FORMAT_IEEE_FLOAT 0x0003
#define WAVE_FORMAT_ALAW 0x0006
#define WAVE_FORMAT_MULAW 0x0007
#define WAVE_FORMAT_EXTENSIBLE 0xFFFE


/**\enum wavExit
 * \brief This enum allows to know what could go wrong while reading the wav file
 */
typedef enum wavExit{
    WAV_EXIT_SUCCESS,
    WAV_EXIT_OPEN_ERROR,
    WAV_EXIT_NOT_WAV,
    WAV_EXIT_NOT_PROPER_WAV,
    WAV_EXIT_EMPTY,
    WAV_EXIT_UNKNOWN_CHUNK
}wavExit;

/**\struct WavHeader wavHandler.h
 * \brief All data possibly encoded in the header of a .wav file
 */
typedef struct WavHeader{
    //In the fmt chunk:
    __uint32_t file_size; /**< Size of the file minus 8 bytes*/
    __uint16_t wFormatTag; /**< Specifies the size of the rest of the header (PCM, non-PCM, or Extensible)*/
    __uint16_t nChannels; /**< How many channels are encoded for each sample*/
    __uint32_t nSamplesPerSec; /**< How many samples have to be read each second for this recording, aka sampling frequency*/
    __uint32_t nAvgBytesPerSec; /**< How many bytes have to be read each second for this recording*/
    __uint16_t nBlockAlign; /**< How many byte for one data block (a sample of each channel) (nBytesPerSample*nChannels) */
    __uint16_t wBitsPerSample; /**< How wide is one sample of one channel (IN BITS)*/
    /*If the file is not PCM we also have these fields :*/
    __uint16_t cbSize;/**< How wide is the extension in bytes*/
    __uint16_t wValidBitsPerSample;/**< Informational only*/
    __uint32_t dwChannelMask;/**< Speakers channel mask, uses 18 bits, each bit corresponding to a speaker position*/
    __uint16_t subFormat; /**< GUID, the actual size of this field is 16 bytes, but the 14 remainings are always 0x000000001000800000aa00389b71*/
    /*In the fact chunk :*/
    __uint32_t dwSampleLength; /**< Number of samples per channel*/

    /*Additional info on whether the data were loaded or not*/
    wavExit exit;/**< an additional field to inform other functions of a possible error*/

}WavHeader;

/*
 * \struct MetaData wavHandler.h
 * \brief Metadatas contained in a .wav file
 */
 /*
typedef struct{
    char* title;
    char* artist;
    char* album;
    int date;
    char* genre;
    int track_number;
    int track_total;
    char* language;
    char* publisher;
    char* copyright;
    char* encoded_by;
    char* comments;
    //There is a "Now playing" metadata, but I have no idea what it is.
}MetaData;
*/


void initWavHeader(WavHeader* h);

__uint32_t load(char* filename, WavHeader* header, int** samples);

void printWavHeader(WavHeader* h);

#endif //WAVHANDLER_H
