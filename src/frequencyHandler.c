/**
 * \file frequencyHandler.c
 * \brief This file aims at converting FFT informations into useful frequency informations.
 * \author Brice Croix
 * \date 19.05.19
 */

#include "frequencyHandler.h"

/**
 * \fn int getThreshold(double** power_spectrum, fftConstants fftInfo)
 * \author Brice Croix
 * \brief this functions returns the threshold above which all frequencies will be considered played. Calculated with the average of all signal's local maximums
 * \param[in] power_spectrum A double array containing a certain number of samples windows, and for each window the amplitude of each frequency as follow : power_samples[window_nb][frequency_index]
 * \param[in] fftInfo an fftConstants structure containing all informations on the windows size, number of windows...
 * \return a threshold above which all frequencies will be considered played notes. This is the average of all local maximums.
 */
int getThresholdLocMax(double** power_spectrum, fftConstants fftInfo){
    int window,frequency_index;
    long double local_max_amplitude_sum; /**< The sum of all local maximums*/
    unsigned int nb_local_max; /**< How many local maximum does this file includes*/
    int nb_window; /**< How many windows does this signal include*/
    int size_window;/**< How many frequencies does a window have the amplitude*/

    int threshold;

    nb_window = fftInfo.nbrWindow;
    size_window = fftInfo.sizeWindow;

    local_max_amplitude_sum = 0;
    nb_local_max = 0;
    for(window = 0; window < nb_window; window++){/* Let us run through all windows*/
        for(frequency_index = 1; frequency_index < size_window-1; frequency_index++){/* Let us run through all this windows frequencies*/
            /*Is this a local maxima? */
            if(isALocalMaxima(power_spectrum[window][frequency_index-1],power_spectrum[window][frequency_index],power_spectrum[window][frequency_index+1])){
                local_max_amplitude_sum += power_spectrum[window][frequency_index];
                nb_local_max++;
            }
        }
    }

    threshold =(int) (local_max_amplitude_sum/nb_local_max);

    return threshold;

}

/**
 * \fn int getThresholdFracHigh(double highest_amplitude, double divider)
 * \author Brice Croix
 * \brief this functions returns the threshold above which all frequencies will be considered played. Calculated with a fraction of the highest amplitude.
 * \param[in] highest_amplitude the highest amplitude encountered in the file
 * \param[in] ratio the ratio between the highest_amplitude and the threshold
 * \return a threshold above which all frequencies will be considered played notes. This is a fraction of the highest velocity in this signal.
 */
double getThresholdFracHigh(double highest_amplitude, double ratio){
    double threshold;
    threshold = (ratio*highest_amplitude);

    return threshold;
}

/**
 * \fn int isALocalMaxima(double previous,double current, double next)
 * \author Brice Croix
 * \brief This function tells if the double 'current' is between 'previous' and 'next'
 * \param[in] previous
 * \param[in] current
 * \param[in] next
 * \return 1 if 'current' is between 'previous' and 'next', 0 otherwise
 */
int isALocalMaxima(double previous,double current, double next){
    if(previous<=current && current>=next){
        return 1;
    }else{
        return 0;
    }
}

/**
 * \fn frequencyGroup getRelevantFrequencies(double* window, fftConstants fftInfo, double threshold, double highest_amplitude)
 * \author Brice Croix
 * \brief This function returns, for a given window, the relevant played frequencies in a frequencyGroup struct.
 * \param[in] window The window to analyze
 * \param[in] fftInfo All the informations about the window, suze as its length or how to convert the index to a frequency
 * \param[in] threshold A treshold above which frequencies are considered played
 * \param[in] highest_amplitude The highest amplitude in this signal
 * \return a frequencyGroup containing the 10 or less played notes in this window
 */
frequencyGroup getRelevantFrequencies(double* window, fftConstants fftInfo, double threshold, double highest_amplitude){
    //double known_notes[97];
    frequencyGroup out;/**<\var The output frequencyGroup*/
    int known_notes_index;/**<\var The index used to look through known_notes*/
    double frequency_here; /**<\var a simple flag to indicate that a frequency is here or not, if yes also contains its velocity*/

    double known_notes[] = {
            32.703 , 34.648 , 36.708 , 38.891 , 41.203 , 43.654 , 46.249 , 48.999 , 51.913 , 55.000 , 58.270 , 61.735 , //C1 to B2
            65.406 , 69.296 , 73.416 , 77.782 , 82.407 , 87.307 , 92.499 , 97.999 , 103.83 , 110.00 , 116.54 , 123.47 , //C2 to B3
            130.81 , 138.59 , 146.83 , 155.56 , 164.81 , 174.61 , 185.00 , 196.00 , 207.65 , 220.00 , 233.08 , 246.94 , //C3 to B4
            261.63 , 277.18 , 293.67 , 311.13 , 329.63 , 349.23 , 369.99 , 392.00 , 415.30 , 440.00 , 466.16 , 493.88 , //C4 to B5
            523.25 , 554.37 , 587.33 , 622.25 , 659.26 , 698.46 , 739.99 , 783.99 , 830.61 , 880.00 , 932.33 , 987.77 , //C5 to B6
            1046.5 , 1108.7 , 1174.7 , 1244.5 , 1318.5 , 1396.9 , 1480.0 , 1568.0 , 1661.2 , 1760.0 , 1864.7 , 1975.5 , //C6 to B7
            2093.0 , 2217.5 , 2349.3 , 2489.0 , 2637.0 , 2793.0 , 2960.0 , 3136.0 , 3322.4 , 3520.0 , 3729.3 , 3951.1 , //C7 to B8
            4186.0 //C8
    };/**<\var All frequencies known to be notes from C1 to C8 (85 notes = KNOWN_NOTES_AMOUNT)*/
    /*We may add an option to recognize notes from the A-432Hz convention... Just kidding why would we do that*/
    //TODO : maybe extend this array for lower or higher notes such as A0 and B0

    /*Initialize the frequencyGroup output*/
    out.nb_frequencies = 0;
    out.more_than_10_flag = 0;


    /*Let us 'look' which ones of theses frequency are above the threshold in this window*/
    for(known_notes_index = 0 ; known_notes_index < KNOWN_NOTES_AMOUNT ; known_notes_index++){

        /*Is the current frequency here?*/
        frequency_here = isFrequencyHere(window,known_notes[known_notes_index],fftInfo,threshold);
        if(frequency_here){
            if(out.nb_frequencies<NOTE_AMOUNT) {
                out.frequencies[out.nb_frequencies].frequency = known_notes[known_notes_index];
                out.frequencies[out.nb_frequencies].velocity = normalizeAmplitude(frequency_here,highest_amplitude);
                out.nb_frequencies++;

            }else{
                out.more_than_10_flag = 1;
            }
        }
        if(out.more_than_10_flag) return out; //There are already 10 found frequencies, cannot return more
    }
    return out;
}

/**
 * \fn int getMinFrequencyIndex(double freq, fftConstants fftInfo, double delta)
 * \author Brice Croix
 * \brief This function returns the index that correspond to freq_min when you want all frequencies between (freq*(1-delta)) and (freq*(1+delta))
 * \param[in] freq the center frequency
 * \param[in] fftInfo informations about the Fourier Transform
 * \param[in] delta how wide is the interval
 * \return the index corresponding to freq_min
 */
int getMinFrequencyIndex(double freq, fftConstants fftInfo, double delta){
    int minIndex;
    double index_to_freq_coeff = fftInfo.multiplierCoefficient;
    /* We have :
     * freq = index*coeff
     * freqMin = freq*(1-delta)
     *
     * Indeed :
     * indexMin = ( freq*(1-delta) )/coeff
     */
    minIndex = (int)floor(( freq*(1-delta) )/index_to_freq_coeff);
    if(minIndex < 0){
        return 0;
    }else{
        return minIndex;
    }
}

/**
 * \fn int getMaxFrequencyIndex(double freq, fftConstants fftInfo, double delta)
 * \author Brice Croix
 * \brief This function returns the index that correspond to freq_max when you want all frequencies between (freq*(1-delta)) and (freq*(1+delta))
 * \param[in] freq the center frequency
 * \param[in] fftInfo informations about the Fourier Transform
 * \param[in] delta how wide is the interval
 * \return the index corresponding to freq_max
 */
int getMaxFrequencyIndex(double freq, fftConstants fftInfo, double delta){
    int maxIndex;
    double index_to_freq_coeff = fftInfo.multiplierCoefficient;
    /* We have :
     * freq = index*coeff
     * freqMin = freq*(1+delta)
     *
     * Indeed :
     * indexMin = ( freq*(1+delta) )/coeff
     */

    maxIndex = (int)ceil(( freq*(1+delta) )/index_to_freq_coeff);
    if(maxIndex >= fftInfo.sizeWindow){
        return fftInfo.sizeWindow-1;
    }else{
        return  maxIndex;
    }
}

/**
 * \fn double getHighestAmplitude(double** power_spectrum, fftConstants fftInfo)
 * \author Brice Croix
 * \param[in] power_spectrum A double array containing a certain number of samples windows, and for each window the amplitude of each frequency as follow : power_samples[window_nb][frequency_index]
 * \param[in] fftInfo an fftConstants structure containing all information on the windows size, number of windows...
 * \return The highest amplitude in all the file
 */
double getHighestAmplitude(double** power_spectrum, fftConstants fftInfo){
    int nb_window; /**< How many windows does this signal include*/
    int size_window;/**< How many frequencies does a window have the amplitude*/

    int window,frequency_index;

    double out = 0;

    /*Recovery of the fftInfo*/
    nb_window = fftInfo.nbrWindow;
    size_window = fftInfo.sizeWindow;

    for(window = 0; window < nb_window; window++){/* Let us run through all windows*/
        for(frequency_index = 1; frequency_index < size_window; frequency_index++){/* Let us run through all this windows frequencies*/
            if(power_spectrum[window][frequency_index] > out) out = power_spectrum[window][frequency_index];
        }
    }
    return out;
}

/**
 * \fn __uint8_t normalizeAmplitude(double amplitude, double highestAmplitude)
 * \author Brice Croix
 * \brief Normalize the value of 'amplitude' between 0 and 255, 'highestAmplitude' being 255
 * \param amplitude amplitude to normalize
 * \param highestAmplitude highest amplitude in this signal
 * \return normalized value of 'amplitude', between 0 and 255
 */
__uint8_t normalizeAmplitude(double amplitude, double highest_amplitude){
    return (__uint8_t)(255*(amplitude / highest_amplitude));
}

/**
 * \fn int equalFrequencyGroup(frequencyGroup fg1, frequencyGroup fg2)
 * \author Brice Croix
 * \brief This function compares the two frequency groups given as parameters
 * \param fg1 First frequency group to compare
 * \param fg2 Second Frequency group to compare
 * \return 1 if fg1 and fg2 contain the same played frequencies (velocity does not matter), 0 otherwise
 */
int equalFrequencyGroup(frequencyGroup fg1, frequencyGroup fg2){
    int i;

    /*First let us look if the two have the same number of played Frequencies*/
    if(fg1.nb_frequencies != fg2.nb_frequencies) return 0;

    /*Now let us look if they do have the same played frequencies*/

    /*As regard to how they are created, all frequencyGroups will always have their frequencies sorted from the lowest to the highest note*/
    for(i=0 ; i<fg1.nb_frequencies ; i++){
        if(fg1.frequencies[0].frequency != fg2.frequencies[0].frequency) return 0;
    }

    /*By here they must be equal*/
    return 1;
}

/**
 * \fn unsigned char getHighestVelocity(frequencyGroup fg)
 * \author Brice Croix
 * \brief This function gives the highest velocity in a frequencyGroup
 * \param fg The frequencyGroup to process
 * \return The highest velocity
 */
unsigned char getHighestVelocity(frequencyGroup fg){
    unsigned char output;
    int i;

    output = 0;

    for(i=0 ; i<fg.nb_frequencies ; i++){
        if(fg.frequencies[i].velocity > output) output = fg.frequencies[i].velocity;
    }
    return output;
}

/**
 * \fn unsigned long getNbNotes(frequencyGroup fg, int nb_window)
 * \author Brice Croix
 * \brief This function returns the number of notes in a given frequencyGroup array, or to be more precise, the number of change of notes +1
 * \param fg_array array of frequencyGroups of size nb_window
 * \param nb_window Length of the array
 * \return The number of notes
 */
unsigned long getNbNotes(frequencyGroup* fg_array, int nb_window){
    unsigned long nb_notes;
    int i;

    nb_notes = 1;
    for(i = 1 ; i<nb_window ; i++){
        if(!equalFrequencyGroup(fg_array[i-1],fg_array[i]) ) nb_notes++;
    }
    return nb_notes;
}

/**
 * \fn double isFrequencyHere(double* window, double frequency, fftConstants fftInfo, double threshold)
 * \author Brice Croix
 * \brief This function indicates if a given frequency is played in a given window, i.e if its velocity is above threshold.
 * \param frequency frequency to look for
 * \param fftInfo informations about the Fourier analysis
 * \param threshold threshold above which a frequency velocity is considered valid
 * \return 0 if the frequency is absent, its velocity if present
 */
double isFrequencyHere(double* window, double frequency, fftConstants fftInfo, double threshold){
    int freq_index;/**<\var The index we are looking at the moment, its corresponding frequency is given by freq_index*fftInfo.multiplierCoefficient */
    double i_to_f;/**<\var equals fftInfo.multiplierCoefficient*/

    /*The player is allowed to be a little out of tune, this is why we will look at multiple indexes and not only one*/
    for(freq_index = getMinFrequencyIndex(frequency,fftInfo,DELTA_FREQ) ; freq_index <= getMaxFrequencyIndex(frequency,fftInfo,DELTA_FREQ); freq_index++){

        /*Is the frequency present in this window ?*/
        if(window[freq_index] >= threshold){
            return window[freq_index]; /*This frequency has been found, there's no need to keep searching*/
        }
    }
    return 0;
}

/**
 * \fn NoteFreq* frequencyConverter(frequencyGroup* notes_per_window, fftConstants fftInfo, int* nb_notes_ptr)
 * \author Brice Croix
 * \brief This function converts the fft result into a matrix that is understandable for the MIDI writer
 * \param notes_per_window var array containing all played notes for each window
 * \param[in] fftInfo an fftConstants structure containing all informations on the windows size, number of windows...
 * \param[out] nb_notes_ptr pointer to the number of notes in this signal
 * \return a NoteFreq Matrix (yes not an array), in each slot there are 10 NoteFreq.
 */
NoteFreq* frequencyConverter(frequencyGroup* notes_per_window, fftConstants fftInfo, int* nb_notes_ptr){
    int nb_window; /**<\var How many windows does this signal include*/
    int i_win; /**<\var An index to go through all windows*/
    unsigned long duration; /**<\var duration, in microsecond, of a group of notes*/
    unsigned long window_duration;  /**<\var duration, in microsecond, of a window*/
    unsigned char velocity;
    NoteFreq* output;
    unsigned char velocity_temp;

    /*Adding some security to this function*/
    if(notes_per_window==NULL) return NULL;

    /*Initialize the number of notes*/
    *nb_notes_ptr = 0;

    /*generate the output matrix*/
    output = NULL;

    /*we will reevaluate velocity for each window in order to set each note at its maximum velocity*/
    velocity = getHighestVelocity(notes_per_window[0]);

    /*Recovery of fft informations*/
    nb_window = fftInfo.nbrWindow;

    /*Fill the output matrix*/
    window_duration = (unsigned long) (1000000*((double)fftInfo.sizeWindow/(double)fftInfo.samplingRate) ); /*This may not be pertinent due to numeric approximations*/

    duration = window_duration;
    for(i_win = 1; i_win < nb_window ; i_win++){

        /*We shall write in the output matrix only when the frequencyGroup changes*/
        if(equalFrequencyGroup( notes_per_window[i_win-1] , notes_per_window[i_win] )){
            /*If this window contains the same played_notes as the previous one*/

            duration += window_duration; //Again, we may lose information by adding values and not calculating the duration each time

            /*We may want to reevaluate velocity*/
            velocity_temp = getHighestVelocity(notes_per_window[i_win]);
            if(velocity_temp>velocity) velocity = velocity_temp;

        }else{
            /*If this window contains different notes than the previous one*/

            /*Let us allocate memory to store this new group of notes (which is the previous index)*/
            *nb_notes_ptr+=1;
            output = safe_realloc(output,(*nb_notes_ptr) * NOTE_AMOUNT * sizeof(NoteFreq));

            /*Let us write this group of notes with its duration*/
            fillNoteMatrix(&output[NOTE_AMOUNT*((*nb_notes_ptr) -1)], notes_per_window[i_win-1], duration, velocity);

            /*Initialize velocity and duration for next group of notes (which is the current index)*/
            velocity = getHighestVelocity(notes_per_window[i_win]);
            duration = window_duration;
        }
    }
    /*The last group of notes has not been written*/
    *nb_notes_ptr+=1;
    output = safe_realloc(output,(*nb_notes_ptr) * NOTE_AMOUNT * sizeof(NoteFreq));
    fillNoteMatrix(&output[NOTE_AMOUNT*((*nb_notes_ptr) -1)], notes_per_window[nb_window-1], duration, velocity);

    /*We no longer need this array*/
    //free(notes_per_window);

    return output;
}

/**
 * \fn void fillNoteMatrix(NoteFreq* dest, frequencyGroup fg, unsigned long duration)
 * \author Brice Croix
 * \brief This function assign to dest the frequency group given as parameter
 * \param dest address where to store the NOTE_AMOUNT NoteFreqs
 * \param fg input frequencyGroup
 * \param duration duration of this group of notes
 * \param velocity velocity of this group of notes
 */
void fillNoteMatrix(NoteFreq* dest, frequencyGroup fg, unsigned long duration, unsigned char velocity){
    int i;

    for(i=0 ; i<fg.nb_frequencies ; i++){
        dest[i].duree = duration;
        dest[i].frequence = fg.frequencies[i].frequency;
        dest[i].amplitude = velocity;
    }
    for(i=fg.nb_frequencies ; i<NOTE_AMOUNT ; i++){
        /*These notes are not played but they must be written in this array with 0 velocity*/
        dest[i].duree = duration;
        dest[i].frequence = NO_NOTE_FREQUENCY;
        dest[i].amplitude = 0;
    }
}

/**
 * \fn frequencyGroup* preFrequencyConverter(double** power_spectrum,fftConstants fftInfo)
 * \author Brice Croix
 * \brief This functions returns an array of size nb_window containing the played notes for each window. It is called by frequencyConverter.
 * \param[in] power_spectrum A double array containing a certain number of samples windows, and for each window the amplitude of each frequency as follow : power_samples[window_nb][frequency_index]
 * \param[in] fftInfo an fftConstants structure containing all informations on the windows size, number of windows...
 * \return array of played notes per window
 */
frequencyGroup* preFrequencyConverter(double** power_spectrum,fftConstants fftInfo){
    double threshold; /**<\var A treshold above which frequencies are considered played */
    double highest_amplitude; /**<\var The highest amplitude in this signal*/
    int nb_window; /**<\var How many windows does this signal include*/
    int i_win; /**<\var An index to go through all windows*/
    unsigned int more_than_10_simultaneous_notes_flag;/**<\var a flag to tell the user that this signal is not conform*/
    frequencyGroup* notes_per_window; /**<\var Each window will reveal what notes it contains*/
    frequencyGroup fg_temp;

    /*Recovery of fft informations*/
    nb_window = fftInfo.nbrWindow;

    /*Let us allocate memory to store all data*/
    notes_per_window = (frequencyGroup*)safe_malloc(nb_window * sizeof(frequencyGroup));

    /*Determination of the highest amplitude*/
    highest_amplitude = getHighestAmplitude(power_spectrum, fftInfo);

    /*Determination of the threshold, as this function is called in getHighestAmplitude, it may be relevant to optimize this*/
    threshold = getThresholdFracHigh(highest_amplitude,THRESHOLD_FRACTION);

    /*Initialize the error flag*/
    more_than_10_simultaneous_notes_flag = 0;

    /*Get, for each window, the played frequencies in it, and put it in the notes_per_window array*/
    for(i_win = 0 ; i_win < nb_window ; i_win++){
        fg_temp = getRelevantFrequencies(power_spectrum[i_win],fftInfo,threshold,highest_amplitude);
        notes_per_window[i_win] = fg_temp;
        if(notes_per_window[i_win].more_than_10_flag) more_than_10_simultaneous_notes_flag++;
    }

    /*free the useless power_spectrum matrix*/
    //freePowerSpectrum(power_spectrum, fftInfo.nbrWindow);

    /*Inform the user of a possible error*/
    if(more_than_10_simultaneous_notes_flag) fprintf(stderr,"Warning : this signal sometimes contains more than %d simultaneous notes, only the lowest 10 will be processed.\n",NOTE_AMOUNT);

    return notes_per_window;
}

/**
 * \fn printFrequencyMatrix(NoteFreq* frequency_matrix)
 * \author Brice Croix
 * \param frequency_matrix matrix to print
 * \param nb_notes number of notes in this file
 */
void printFrequencyMatrix(NoteFreq* frequency_matrix, int nb_notes){
    int i,j;

    printf("\t>>Printing notes\n");
    for(i=0;i<nb_notes;i++){
        printf("\tFor %8ld ms, amp %3d :\t\t",frequency_matrix[i*NOTE_AMOUNT].duree,frequency_matrix[i*NOTE_AMOUNT].amplitude);
        for(j=0;j<NOTE_AMOUNT;j++){
            if(frequency_matrix[i*NOTE_AMOUNT+j].frequence!= NO_NOTE_FREQUENCY) printf("%5d Hz\t",(int)frequency_matrix[i*NOTE_AMOUNT+j].frequence);
        }
        printf("\n");
    }
    printf("\t>>End of printing notes\n");
}


