#include "testFrequencyHandler.h"

static int setup_frequencyHandler(void** state){
    /*Let us create an array of 1  real windows*/
    double** power_spectrum = (double**)safe_malloc(sizeof(double*));
    *power_spectrum = (double*)safe_malloc(6250*sizeof(double));

    *state = power_spectrum;
#include "testFile_power_spectrum_440Hz.txt"
    /*This window is supposed to contain a lonely 440Hz frequency*/

    fftConstants fftInfo;
    fftInfo.nbrWindow = 1;
    fftInfo.multiplierCoefficient = 0.672913; //Told by the provider of the testFile
    fftInfo.sizeWindow = 6250;
    fftInfo.samplingRate = 44100;
    return 0;
}

static int teardown_frequencyHandler(void** state){

    double** power_spectrum = *state;
    free(*power_spectrum);
    free(power_spectrum);

    return 0;
}

static void test_isALocalMaxima(void** state){
    int res;

    res = isALocalMaxima(3,5,7);
    assert_int_equal(res,0);

    res = isALocalMaxima(5,8,7);
    assert_int_equal(res,1);

    res = isALocalMaxima(1,1,0.5);
    assert_int_equal(res,1);

    res = isALocalMaxima(1,2,1);
    assert_int_equal(res,1);
}

static void test_getThresholdLocMax(void** state) {
    /*Let us create an array of two windows containing each one 10 frequencies*/
    double *power_spectrum[2];
    power_spectrum[0] = (double *) safe_malloc(10 * sizeof(double));
    power_spectrum[1] = (double *) safe_malloc(10 * sizeof(double));

    power_spectrum[0][0] = 2;
    power_spectrum[0][1] = 4;
    power_spectrum[0][2] = 8; //Local max 8
    power_spectrum[0][3] = 6;
    power_spectrum[0][4] = 3;
    power_spectrum[0][5] = 2;
    power_spectrum[0][6] = 2;
    power_spectrum[0][7] = 4; //Local max 4
    power_spectrum[0][8] = 1;
    power_spectrum[0][9] = 2;

    power_spectrum[1][0] = 4;
    power_spectrum[1][1] = 6;
    power_spectrum[1][2] = 12; //Local max 12
    power_spectrum[1][3] = 8;
    power_spectrum[1][4] = 5;
    power_spectrum[1][5] = 4;
    power_spectrum[1][6] = 4;
    power_spectrum[1][7] = 8; //Local max 8
    power_spectrum[1][8] = 3;
    power_spectrum[1][9] = 4;

    /* 8+4+12+8 = 32 , the avg is 8*/

    fftConstants fftInfo;
    fftInfo.sizeWindow = 10;
    fftInfo.nbrWindow =2;

    assert_int_equal(getThresholdLocMax(power_spectrum, fftInfo), 8);
    free(power_spectrum[0]);
    free(power_spectrum[1]);
}

static void test_getHighestAmplitude(void** state){
    /*Let us create an array of two windows containing each one 10 frequencies*/
    double *power_spectrum[2];
    power_spectrum[0] = (double *) safe_malloc(10 * sizeof(double));
    power_spectrum[1] = (double *) safe_malloc(10 * sizeof(double));

    power_spectrum[0][0] = 2;
    power_spectrum[0][1] = 4;
    power_spectrum[0][2] = 8;
    power_spectrum[0][3] = 6;
    power_spectrum[0][4] = 3;
    power_spectrum[0][5] = 2;
    power_spectrum[0][6] = 2;
    power_spectrum[0][7] = 4;
    power_spectrum[0][8] = 1;
    power_spectrum[0][9] = 2;

    power_spectrum[1][0] = 4;
    power_spectrum[1][1] = 6;
    power_spectrum[1][2] = 12; //max 12
    power_spectrum[1][3] = 8;
    power_spectrum[1][4] = 5;
    power_spectrum[1][5] = 4;
    power_spectrum[1][6] = 4;
    power_spectrum[1][7] = 8;
    power_spectrum[1][8] = 3;
    power_spectrum[1][9] = 4;


    fftConstants fftInfo;
    fftInfo.sizeWindow = 10;
    fftInfo.nbrWindow =2;

    assert_int_equal(getHighestAmplitude(power_spectrum,fftInfo), 12);
    free(power_spectrum[0]);
    free(power_spectrum[1]);
}

static void test_getMinFrequencyIndex(void** state){
    fftConstants fftInfo;
    fftInfo.multiplierCoefficient = 1.05;
    double delta = 0.025;
    double freq = 440;
    //index = 408.57 => 408
    assert_int_equal(getMinFrequencyIndex(freq,fftInfo,delta),408);
}

static void test_getMaxFrequencyIndex(void** state){
    fftConstants fftInfo;
    fftInfo.multiplierCoefficient = 1.05;
    fftInfo.sizeWindow = 6250; //The important thing is that it is more than 430
    double delta = 0.025;
    double freq = 440;
    //index = 429.52 => 430
    assert_int_equal(getMaxFrequencyIndex(freq,fftInfo,delta),430);

    fftInfo.sizeWindow = 428;
    assert_int_equal(getMaxFrequencyIndex(freq,fftInfo,delta),427);

}

static void test_normalizeAmplitude(void** state){
    double highest_amp = 110003;
    double amp = 4568;
    assert_int_equal(normalizeAmplitude(amp,highest_amp),10);
    assert_int_equal(normalizeAmplitude(highest_amp,highest_amp),255);
}

static void test_getRelevantFrequencies(void** state){

    /*Let us create an array of 1  real windows*/
    double ** power_spectrum = *state;

    fftConstants fftInfo;
    fftInfo.nbrWindow = 1;
    fftInfo.multiplierCoefficient = 0.672913; //Told by the provider of the testFile
    fftInfo.sizeWindow = 6250;
    fftInfo.samplingRate = 44100;

    double highest_amplitude = getHighestAmplitude(power_spectrum,fftInfo);
    int threshold = getThresholdFracHigh(highest_amplitude,THRESHOLD_FRACTION);

    frequencyGroup fg;
    fg = getRelevantFrequencies(power_spectrum[0], fftInfo, threshold, highest_amplitude);

    assert_int_equal(fg.more_than_10_flag,0);
    assert_int_equal(fg.nb_frequencies,1);
    assert_int_equal(fg.frequencies[0].frequency,440.0);
}

static void test_equalFrequencyGroup(void** state){
    frequencyGroup fg1;
    frequencyGroup fg2;

    fg1.nb_frequencies = 1;
    fg2.nb_frequencies = 4,
    assert_int_equal(equalFrequencyGroup(fg1,fg2),0);

    fg1.nb_frequencies = 2;
    fg1.frequencies[0].frequency = 440;
    fg1.frequencies[1].frequency = 880;
    fg2.nb_frequencies = 2;
    fg2.frequencies[0].frequency = 185;
    fg2.frequencies[1].frequency = 880;
    assert_int_equal(equalFrequencyGroup(fg1,fg2),0);

    fg1.nb_frequencies = 2;
    fg1.frequencies[0].frequency = 185;
    fg1.frequencies[1].frequency = 440;
    fg2.nb_frequencies = 2;
    fg2.frequencies[0].frequency = 185;
    fg2.frequencies[1].frequency = 440;
    assert_int_equal(equalFrequencyGroup(fg1,fg2),1);
}

static void test_getThresholdFracHigh(void** state) {
    /*Let us create an array of 2 windows of size 10*/
    double *power_spectrum[2];
    power_spectrum[0] = (double *) safe_malloc(10 * sizeof(double));
    power_spectrum[1] = (double *) safe_malloc(10 * sizeof(double));

    power_spectrum[0][0] = 25;
    power_spectrum[0][1] = 49;
    power_spectrum[0][2] = 88; //Max
    power_spectrum[0][3] = 61;
    power_spectrum[0][4] = 30;
    power_spectrum[0][5] = 20;
    power_spectrum[0][6] = 23;
    power_spectrum[0][7] = 49;//Max local
    power_spectrum[0][8] = 16;
    power_spectrum[0][9] = 28;
    power_spectrum[1][0] = 29;
    power_spectrum[1][1] = 47;
    power_spectrum[1][2] = 85;//Max local
    power_spectrum[1][3] = 63;
    power_spectrum[1][4] = 30;
    power_spectrum[1][5] = 29;
    power_spectrum[1][6] = 21;
    power_spectrum[1][7] = 46;//Max local
    power_spectrum[1][8] = 16;
    power_spectrum[1][9] = 27;

    fftConstants fftInfo;
    fftInfo.sizeWindow = 10;
    fftInfo.nbrWindow =2;

    double highest_amplitude = getHighestAmplitude(power_spectrum,fftInfo);
    assert_int_equal(getThresholdFracHigh(highest_amplitude,THRESHOLD_FRACTION),88*THRESHOLD_FRACTION);
    free(power_spectrum[0]);
    free(power_spectrum[1]);
}

static void test_preFrequencyConverter(void** state){
    double** power_spectrum = *state;

    fftConstants fftInfo;
    fftInfo.nbrWindow = 1;
    fftInfo.multiplierCoefficient = 0.672913; //Told by the provider of the testFile
    fftInfo.sizeWindow = 6250;
    fftInfo.samplingRate = 44100;

    frequencyGroup* notes_per_window;

    notes_per_window = preFrequencyConverter(power_spectrum,fftInfo);

    assert_int_equal(notes_per_window[0].frequencies[0].frequency,440);
    assert_int_equal(notes_per_window[0].frequencies[0].velocity,255);
    assert_int_equal(notes_per_window[0].nb_frequencies,1);
    assert_int_equal(notes_per_window[0].more_than_10_flag,0);

    free(notes_per_window);
}

static void test_frequencyConverter(void** state){

    fftConstants fftInfo;
    fftInfo.nbrWindow = 3;
    fftInfo.sizeWindow = 6250;
    fftInfo.samplingRate = 44100;

    frequencyGroup fg[3];
    fg[0].nb_frequencies = 2 ;
    fg[0].frequencies[0].frequency = 196;
    fg[0].frequencies[0].velocity = 178;
    fg[0].frequencies[1].frequency = 440;
    fg[0].frequencies[1].velocity = 232;
    fg[0].more_than_10_flag = 0;
    fg[1].nb_frequencies = 2 ;//Same frequencies as previous
    fg[1].frequencies[0].frequency = 196;
    fg[1].frequencies[0].velocity = 10;
    fg[1].frequencies[1].frequency = 440;
    fg[1].frequencies[1].velocity = 20;
    fg[1].more_than_10_flag = 0;
    fg[2].nb_frequencies = 1 ;
    fg[2].frequencies[0].frequency = 196;
    fg[2].frequencies[0].velocity = 145;
    fg[2].more_than_10_flag = 0;

    int nb_notes;
    NoteFreq* out = frequencyConverter(fg,fftInfo,&nb_notes);

    unsigned long window_duration = (unsigned long) (1000000*((double)fftInfo.sizeWindow/(double)fftInfo.samplingRate) );

    assert_int_equal(nb_notes,2);

    assert_int_equal(out[0].duree, 2*window_duration);
    assert_int_equal(out[0].frequence,196);
    assert_int_equal(out[0].amplitude,232);

    assert_int_equal(out[1].duree, 2*window_duration);
    assert_int_equal(out[1].frequence,440);
    assert_int_equal(out[1].amplitude,232);

    assert_int_equal(out[2].duree, 2*window_duration);
    assert_int_equal(out[2].frequence,432);
    assert_int_equal(out[2].amplitude,0);
    //...
    assert_int_equal(out[9].duree, 2*window_duration);
    assert_int_equal(out[9].frequence,432);
    assert_int_equal(out[9].amplitude,0);

    assert_int_equal(out[10].duree,window_duration);
    assert_int_equal(out[10].frequence,196);
    assert_int_equal(out[10].amplitude,145);

    assert_int_equal(out[11].duree, window_duration);
    assert_int_equal(out[11].frequence,432);
    assert_int_equal(out[11].amplitude,0);
    //...
    assert_int_equal(out[19].duree, window_duration);
    assert_int_equal(out[19].frequence,432);
    assert_int_equal(out[19].amplitude,0);

    free(out);
}

static void test_fillNoteMatrix(void** state){
    NoteFreq frequency_matrix[2*NOTE_AMOUNT];

    fftConstants fftInfo;
    fftInfo.nbrWindow = 2;
    fftInfo.sizeWindow = 6250;
    fftInfo.samplingRate = 44100;

    frequencyGroup fg[2];
    fg[0].nb_frequencies = 2 ;
    fg[0].frequencies[0].frequency = 196;
    fg[0].frequencies[0].velocity = 178;
    fg[0].frequencies[1].frequency = 440;
    fg[0].frequencies[1].velocity = 232;
    fg[0].more_than_10_flag = 0;
    fg[1].nb_frequencies = 1;
    fg[1].frequencies[0].frequency = 220;
    fg[1].frequencies[0].velocity = 68;
    fg[1].more_than_10_flag = 0;

    unsigned char velocity;
    unsigned long duration;

    duration = (unsigned long) (1000000*((double)fftInfo.sizeWindow/(double)fftInfo.samplingRate) );

    velocity = 232;
    fillNoteMatrix(frequency_matrix+0*NOTE_AMOUNT,fg[0],duration,velocity);
    velocity = 68;
    fillNoteMatrix(frequency_matrix+1*NOTE_AMOUNT,fg[1],duration,velocity);

    assert_int_equal(frequency_matrix[0].amplitude,232);
    assert_int_equal(frequency_matrix[0].frequence,196);
    assert_int_equal(frequency_matrix[0].duree, duration);

    assert_int_equal(frequency_matrix[1].amplitude,232);
    assert_int_equal(frequency_matrix[1].frequence,440);
    assert_int_equal(frequency_matrix[1].duree, duration);

    assert_int_equal(frequency_matrix[2].amplitude,0);
    assert_int_equal(frequency_matrix[2].frequence,432);//No note
    assert_int_equal(frequency_matrix[2].duree, duration);

    assert_int_equal(frequency_matrix[3].amplitude,0);
    assert_int_equal(frequency_matrix[3].frequence,432);//No note
    assert_int_equal(frequency_matrix[3].duree, duration);

    assert_int_equal(frequency_matrix[4].amplitude,0);
    assert_int_equal(frequency_matrix[4].frequence,432);//No note
    assert_int_equal(frequency_matrix[4].duree, duration);

    assert_int_equal(frequency_matrix[5].amplitude,0);
    assert_int_equal(frequency_matrix[5].frequence,432);//No note
    assert_int_equal(frequency_matrix[5].duree, duration);

    assert_int_equal(frequency_matrix[6].amplitude,0);
    assert_int_equal(frequency_matrix[6].frequence,432);//No note
    assert_int_equal(frequency_matrix[6].duree, duration);

    assert_int_equal(frequency_matrix[7].amplitude,0);
    assert_int_equal(frequency_matrix[7].frequence,432);//No note
    assert_int_equal(frequency_matrix[7].duree, duration);

    assert_int_equal(frequency_matrix[8].amplitude,0);
    assert_int_equal(frequency_matrix[8].frequence,432);//No note
    assert_int_equal(frequency_matrix[8].duree, duration);

    assert_int_equal(frequency_matrix[9].amplitude,0);
    assert_int_equal(frequency_matrix[9].frequence,432);//No note
    assert_int_equal(frequency_matrix[9].duree, duration);

    assert_int_equal(frequency_matrix[10].amplitude,68);
    assert_int_equal(frequency_matrix[10].frequence,220);
    assert_int_equal(frequency_matrix[10].duree, duration);

    assert_int_equal(frequency_matrix[11].amplitude,0);
    assert_int_equal(frequency_matrix[11].frequence,432);//No note
    assert_int_equal(frequency_matrix[11].duree, duration);
}


int run_UTfrequencyHandler(void){

const struct CMUnitTest frequencyHandlerUT[] = {
    cmocka_unit_test(test_isALocalMaxima),
    cmocka_unit_test(test_getThresholdLocMax),
    cmocka_unit_test(test_getHighestAmplitude),
    cmocka_unit_test(test_getMinFrequencyIndex),
    cmocka_unit_test(test_getMaxFrequencyIndex),
    cmocka_unit_test(test_normalizeAmplitude),
    cmocka_unit_test(test_getRelevantFrequencies),
    cmocka_unit_test(test_equalFrequencyGroup),
    cmocka_unit_test(test_getThresholdFracHigh),
    cmocka_unit_test(test_preFrequencyConverter),
    cmocka_unit_test(test_frequencyConverter),
    cmocka_unit_test(test_fillNoteMatrix)
};

int out=0;

out += cmocka_run_group_tests(frequencyHandlerUT, setup_frequencyHandler, teardown_frequencyHandler);

return out;
}