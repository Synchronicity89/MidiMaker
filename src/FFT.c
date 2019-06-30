/** \file FFT.c
 * \author clement
 * \date 28/02/19.
 * \headerfile FFT.h
 * \brief That file aims at converting a temporal C data structure (signal loaded from the .wav file) into a frequency C data structure (double pointer on frequency spectrums)
 */

#include "FFT.h"

 /**
 * \fn int size_window(int samplingRate)
 * \brief This function calculate the optimized size of windows for this .wav file, it is called by "get_fft_constants"
 * \param[in] Sampling rate of the signal
 * \return Number of samples in each window
 */
int size_window(int samplingRate){
    int sizeWindow = (int)((samplingRate*NBMINPERIOD)/(double)LOWFREQ); /* conservation of at least NBMINPERIOD periods of all considered frequency in each window */
    return sizeWindow;
}

 /**
 * \fn int size_spectrum_input(int samplingRate)
 * \brief This function calculate the optimized size of input arrays of the fft, it is called by "get_fft_constants"
 * \param[in] Sampling rate of the signal
 * \return Size of input arrays of the fft or EXIT_FAILURE
 */
int size_spectrum_input(int samplingRate){
    int sizeSpectrumInput = samplingRate/STEPSPECTRUM;
    /* The size of input arrays of the fft has to be a power of 2 */
    for(int i=0; i<30; i++){
        if(sizeSpectrumInput < pow(2,i)){
            sizeSpectrumInput = pow(2,i);
            return sizeSpectrumInput;
        }
    }
    return EXIT_FAILURE;
}

/**
 * \fn int nbr_window(int lenghtMusic, int sizeWindow)
 * \brief This function calculate the number of windows in which the signal loaded from the .wav file is divided, it is called by "get_fft_constants"
 * \param[in] Number of samples in the signal
 * \param[in] Size of windows
 * \return Number of windows
 */
int nbr_window(int lenghtMusic, int sizeWindow){
    int nbrWindow = lenghtMusic/sizeWindow;
    return nbrWindow;
}

/**
 * \fn int get_flag(int nbrWindow)
 * \brief This function check if the program should stop or continue and return a flag, it is called by "get_fft_constants"
 * \param[in] Number of windows
 * \param[in] Sampling rate
 * \return A flag which contains CONTINUE or ABANDON value
 */
int get_flag(int nbrWindow, int samplingRate){
    int abandonFlag;
    if(nbrWindow > 0 && samplingRate > 2*(MAXFREQ+BUFFER))
        abandonFlag = CONTINUE;
    else
        abandonFlag = ABANDON;
    return abandonFlag;
}

/**
 * \fn int fill_fft_data(int indexWindow, fftConstants constFDS, int * samples, double * fftData)
 * \brief This function fills the sizeSpectrumInput-sized fftData array with window-sized data initially stored in samples array, it is called by "fft_one_window"
 * \param[in] Index of the window which is being treated
 * \param[in] fftConstants structure constFDS containing all useful constants to execute the Fast Fourier Transform
 * \param[in] samples int array where all samples are initially stored, dynamically allocated
 * \param[out] fftData double array where to store samples of one window, dynamically allocated
 * \return EXIT_SUCCESS
 */
int fill_fft_data(int indexWindow, fftConstants constFDS, int * samples, double * fftData){
    int x=0;
    for(int i=0; i<constFDS.sizeSpectrumInput; i++){
        if(i-x*constFDS.sizeWindow < constFDS.sizeWindow){
            /* fftData array being much bigger than the size of a window, it is filled by a repetition of the signal portion in the window.
               This allows to have a better quality spectrum at the end of the Fast Fourier Transform. */
            fftData[i]=(double)samples[i-x*constFDS.sizeWindow+indexWindow*constFDS.sizeWindow];
        }else{
            fftData[i]=fftData[0];
            x++;
        }
    }
    return EXIT_SUCCESS;
}

 /**
 * \fn int set_fft(fftConstants constFDS)
 * \brief This function initialize the Fast Fourier Transform by creating the initial plan and allocating the in and out arrays, , it is called by "fft_main_function"
 * \param[in] fftConstants structure constFDS containing all useful constants to execute the Fast Fourier Transform
 * \param[out] pointer to fftIOP structure which contains the plan p which is being initialized and in/out arrays of the Fast Fourier Transform which are being allocated in this function
 * \return EXIT_SUCCESS
 */
int set_fft(fftConstants constFDS, fftIOP * iop_1d) {

    iop_1d->in = (double * ) fftw_malloc(sizeof(double) * constFDS.sizeSpectrumInput );
    iop_1d->out = (fftw_complex * ) fftw_malloc(sizeof(fftw_complex) * constFDS.sizeSpectrumInput );
    iop_1d->p = fftw_plan_dft_r2c_1d(constFDS.sizeSpectrumInput, iop_1d->in , iop_1d->out, FFTW_ESTIMATE);

    /* /!\ WARNING : plan has to be created before initializing the inputs in and out arrays */
    /* fftw_plan_dft_r2c_1d(int n, double *in, fftw_complex *out, unsigned flags) creates a plan, which is an object that contains all the data that FFTW needs to compute the FFT

        arg 1 : the size of the transform we are trying to compute
        arg 2 : pointers to the input arrays of the transform
        arg 3 : pointers to the output array of the transform
        arg 4 : FFTW_MEASURE or FFTW_ESTIMATE (flags argument)
                -> FFTW_MEASURE instructs FFTW to run and measure the execution time of several FFTs in order to find the best way to compute the transform of size n.
                   This process takes some time (usually a few seconds) -> to use if initialization time is not important
                -> FFTW_ESTIMATE does not run any computation and just builds a reasonable plan that is probably sub-optimal -> to use if initialization time is important
    */
    return EXIT_SUCCESS;
}

 /**
 * \fn int fft(fftConstants constFDS, double * fftData, double * power_spectrum_1d)
 * \brief This function fills in array with data in fftData array, execute the Fast Fourier Transform, and store the spectrum in power_spectrum_1d array, it is called by "fft_one_window"
 * \param[in] fftConstants structure constFDS containing all useful constants to execute the Fast Fourier Transform
 * \param[in] fftData double array where samples of the window which is being treated are stored, dynamically allocated
 * \param[out] power_spectrum_1d double array where to store the spectrum, dynamically allocated
 * \param[out] pointer to fftIOP structure which contains the plan p and in/out arrays of the Fast Fourier Transform
 * \return EXIT_SUCCESS
 */
int fft(fftConstants constFDS, double * fftData, double * power_spectrum_1d, fftIOP * iop_1d) {

    /* Transfer of data from fftData array to in array */
    for (int i = 0; i < constFDS.sizeSpectrumInput; i++) {
        iop_1d->in[i] = (fftData[i]/(255.0))-1.0;
    }

    /* Execution of the Fast Fourier Transform */
    fftw_execute(iop_1d->p);

    /* Reorganization of data from out array */
    for (int i = 0; i< constFDS.sizeSpectrumUseful; i++){
        power_spectrum_1d[i] = sqrt(iop_1d->out[i][0]*iop_1d->out[i][0] + iop_1d->out[i][1]*iop_1d->out[i][1]);
    }

    return EXIT_SUCCESS;
}

 /**
 * \fn int end_fft()
 * \brief This function free memory used during the Fast Fourier Transform, it is called by "fft_main_function"
 * \param[out] pointer to fftIOP structure which contains the plan p which is being destroyed and in/out arrays of the Fast Fourier Transform which are being deallocated in this function
 * \return EXIT_SUCCESS
 */
int end_fft(fftIOP * iop_1d) {
    fftw_destroy_plan(iop_1d->p);
    fftw_free(iop_1d->in);
    fftw_free(iop_1d->out);
   return EXIT_SUCCESS;
}

 /**
 * \fn fftConstants get_fft_constants(WavHeader h)
 * \brief This function calculates all needed constants to execute the Fast Fourier Tranform and stores them in a fftConstants structure called constFDS, it is called by "fft_main_function"
 * \param[in] WavHeader struct where are store header data
 * \return fftConstants structure constFDS
 */
fftConstants get_fft_constants(WavHeader h){
    fftConstants constFDS;
    constFDS.lengthMusic = h.dwSampleLength;
    constFDS.samplingRate = h.nSamplesPerSec;
    constFDS.sizeSpectrumInput = size_spectrum_input(constFDS.samplingRate);
    constFDS.sizeSpectrumOutput = constFDS.sizeSpectrumInput/2;
    constFDS.sizeWindow = size_window(constFDS.samplingRate);
    constFDS.nbrWindow = nbr_window(constFDS.lengthMusic, constFDS.sizeWindow);
    constFDS.sizeSpectrumUseful = (MAXFREQ+BUFFER)*constFDS.sizeSpectrumInput/constFDS.samplingRate;
    constFDS.multiplierCoefficient = (double)constFDS.samplingRate/(double)constFDS.sizeSpectrumInput;
    constFDS.abandonFlag = get_flag(constFDS.nbrWindow, constFDS.samplingRate);
    return constFDS;
}

 /**
 * \fn int fft_one_window(int indexWindow, fftConstants constFDS, int * samples, double * fftData, double ** power_spectrum_2d)
 * \brief This function execute the Fast Fourier Transform for a single window, it is called by "fft_main_function"
 * \param[in] Index of the window which is being treated
 * \param[in] fftConstants structure constFDS containing all useful constants to execute the Fast Fourier Transform
 * \param[in] samples int array where all samples are initially stored, dynamically allocated
 * \param[out] fftData double array where to store samples of the window which is being treated, dynamically allocated
 * \param[out] double pointer power_spectrum_2d, which is an array of arrays, those last containing each one the spectrum of a single window, dynamically allocated
 * \param[out] pointer to fftIOP structure which contains the plan p and in/out arrays of the Fast Fourier Transform
 * \return EXIT_SUCCESS
 */
int fft_one_window(int indexWindow, fftConstants constFDS, int * samples, double * fftData, double ** power_spectrum_2d, fftIOP * iop_1d){
    power_spectrum_2d[indexWindow] = (double * ) safe_calloc(constFDS.sizeSpectrumUseful, sizeof(double));
    fill_fft_data(indexWindow, constFDS, samples, fftData);
    fft(constFDS, fftData, power_spectrum_2d[indexWindow], iop_1d);
    return EXIT_SUCCESS;
}

 /**
 * \fn fftConstants fft_main_function(WavHeader h, int* samples, double *** power_spectrum_3d)
 * \brief This function execute the Fast Fourier Transform for a complete signal stored in a .wav file
 * \param[in] WavHeader struct where are store header data
 * \param[in] samples int array where all samples of the signal are initially stored, dynamically allocated
 * \param[out] double pointer power_spectrum_2d address, which is an array of arrays, those last containing each one the spectrum of a single window, dynamically allocated
 * \return fftConstants structure constFDS
 */
fftConstants fft_main_function(WavHeader h, int* samples, double *** power_spectrum_3d){

    /* Creation of the fftConstants structure and obtaining fields value */
    fftConstants constFDS = get_fft_constants(h);

    /* Test if there is at least one window in this file and if the sampling rate is higher than MAXFREQ+BUFFER */
    if(!constFDS.abandonFlag){

        /* Creation of the fftIOP structure */
        fftIOP iop;
        /* Memory allocation */
        double * fftData = (double * ) safe_calloc(constFDS.sizeSpectrumInput, sizeof(double));
        *power_spectrum_3d = (double ** ) safe_calloc(constFDS.nbrWindow,sizeof(double*));


        /* Computing the Fast Fourier Transform */
        set_fft(constFDS, &iop);
        for(int indexWindow=0; indexWindow<constFDS.nbrWindow; indexWindow++){
            fft_one_window(indexWindow, constFDS, samples, fftData, *power_spectrum_3d, &iop);
        }
        end_fft(&iop);

        /* Free memory */
        free(fftData);
        //free(samples);
    }

    return constFDS;
}

/**
 * \fn void freePowerSpectrum(double** power_spectrum, int nb_windows)
 * \brief this function frees the power_spectrum array
 * \param power_samples the matrix to free
 * \param nb_windows size of the first array
 */
void freePowerSpectrum(double** power_spectrum, int nb_windows){
    int i;

    for(i = 0 ; i < nb_windows ; i++){
        free(power_spectrum[i]);
    }
    free(power_spectrum);
}

