/** \file FFT.h
 * \author clement
 * \date 28/02/19.
 * \brief header file associated to the FFT.c file.
 */

#ifndef FFT_H
#define FFT_H

#include <stdio.h>
#include <stdlib.h>
#include "../libfftw3/fftw3.h"
#include <complex.h>
#include <math.h>
#include "wavHandler.h"

/**The constants that are needed for the Fast Fourier Transform.*/
#define MAXFREQ 4186 /**< Frequency of the highest considered music note : 4186Hz */
#define LOWFREQ 32 /**< Frequency of the lowest considered music note : 32Hz */
#define NBMINPERIOD 2 /**< Minimum number of period(s) in a window for the lowest considered frequency : 2 periods */
#define STEPSPECTRUM 0.75 /**< Maximum sampling step allowed in power_spectrum_1d arrays in Hz */
#define BUFFER 20 /**< Security buffer */
#define ABANDON 1 /**< Abandon value of the abandonFlag */
#define CONTINUE 0 /**< Continue value of the abandonFlag */

/**\struct fft_constants FFT.h
 * \brief All useful constants used during the Fast Fourier Transform
 */
typedef struct{
    // Directly taken from the data in the WavHeader
    int lengthMusic; /**< Number of samples in the signal loaded from the .wav file*/
    int samplingRate; /**< Sampling frequency of the signal, usually 44100Hz or 48000Hz*/
    // Calculated from the previous data
    int sizeWindow; /**< Number of samples in a window. Each window will be treated by the fft one by one*/
    int sizeSpectrumInput; /**< Number of samples in the input array of the fft*/
    int sizeSpectrumOutput; /**< Number of useful samples in the output array of the fft*/
    int sizeSpectrumUseful; /**< Index of the highest music note considered in the program : 4186Hz, aka number of samples which are stored in power_spectrum_1d arrays*/
    int nbrWindow; /**< Number of windows in the piece of music*/
    double multiplierCoefficient; /**< Coefficient that allows to calculate the frequency from the index in power_spectrum_1d arrays. Frequency = multiplierCoefficient * index*/
    int abandonFlag; /**< Flag that indicates if the program have to continue or to stop in case the signal in .wav file is too short (less than one window)*/
}fftConstants;

/**\struct fft_iop FFT.h
 * \brief Plan p and in/out arrays of the Fast Fourier Transform
 */
typedef struct{
    double * in; /**< Input double array of the Fast Fourier Transform */
    fftw_complex * out; /**< Output fftw_complex array of the Fast Fourier Transform */ /* type fftw_complex : a double[2] composed of the real (example[0]) and imaginary (example[1]) parts of a complex number */
    fftw_plan p; /**< Plan of the Fast Fourier Transform */
}fftIOP;

int size_window(int samplingRate);

int size_spectrum_input(int samplingRate);

int nbr_window(int lenghtMusic, int sizeWindow);

int get_flag(int sizeWindow, int samplingRate);

int fill_fft_data(int indexWindow, fftConstants constFDS, int * samples, double * fftData);

int set_fft(fftConstants constFDS, fftIOP * iop_1d);

int fft(fftConstants constFDS, double * fftData, double * power_spectrum_1d, fftIOP * iop_1d);

int end_fft(fftIOP * iop_1d);

fftConstants get_fft_constants(WavHeader h);

int fft_one_window(int indexWindow, fftConstants constFDS, int * samples, double * fftData, double ** power_spectrum_2d, fftIOP * iop);

fftConstants fft_main_function(WavHeader h, int* samples, double *** power_spectrum_3d);

void freePowerSpectrum(double** power_spectrum, int nb_windows);

#endif //FFT_H
