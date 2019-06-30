#include "testFFT.h"

static int setup_FFT(void** state){
    return 0;
}

static int teardown_FFT(void** state){
    return 0;
}

static void test_size_window(void** state){
    int res;
    /* 120*2/32 = 7.5 */
    res = size_window(120);
    assert_int_equal(res,7);
    /* 20000*2/32 = 1250 */
    res = size_window(20000);
    assert_int_equal(res,1250);
    /* 44100*2/32 = 2756.25 */
    res = size_window(44100);
    assert_int_equal(res,2756);
    /* 48000*2/32 = 3000 */
    res = size_window(48000);
    assert_int_equal(res,3000);
}

static void test_size_spectrum_input(void** state){
    int res;
    /* 120/0.75 = 160 -> 2^8 = 256 */
    res = size_spectrum_input(120);
    assert_int_equal(res,256);
    /* 20000/0.75 = 26666 -> 2^15 = 32768 */
    res = size_spectrum_input(20000);
    assert_int_equal(res,32768);
    /* 44100/0.75 = 58800 -> 2^16 = 65536 */
    res = size_spectrum_input(44100);
    assert_int_equal(res,65536);
    /* 48000/0.75 = 64000 -> 2^16 = 65536 */
    res = size_spectrum_input(48000);
    assert_int_equal(res,65536);
}

static void test_nbr_window(void** state){
    int res;
    /* 70/5 = 14 */
    res = nbr_window(70, 5);
    assert_int_equal(res,14);
    /* 10/3 = 3.33 */
    res = nbr_window(10, 3);
    assert_int_equal(res,3);
    /* 5000/200 = 25 */
    res = nbr_window(5000, 200);
    assert_int_equal(res,25);
    /* 5000/215 = 23.26 */
    res = nbr_window(5000, 215);
    assert_int_equal(res,23);
}

static void test_get_flag(void** state){
    int res;

    res = get_flag(-1, 200);
    assert_int_equal(res,ABANDON);

    res = get_flag(0, 10000);
    assert_int_equal(res,ABANDON);

    res = get_flag(1, 10000);
    assert_int_equal(res,CONTINUE);

    res = get_flag(125, 200);
    assert_int_equal(res,ABANDON);
}

static void test_fill_fft_data(void** state){
    /* focus on the 4th window of samples */
    int indexWindow = 3;
    /*Let us create an array of five windows containing each one 5 samples */
    int * samples;
    samples = (int *) safe_malloc(25 * sizeof(int));
    /*Let us create an array which will contain the repetition of one window. The size of this array is the size of the input (and so output) arrays of the Fast Fourier Transform fixed here at 500 */
    double * fftData;
    fftData = (double *) safe_malloc(500 * sizeof(double));

    samples[0] = 214;
    samples[1] = 1562;
    samples[2] = 2462;
    samples[3] = 5684;
    samples[4] = 4766;
    samples[5] = 2992;
    samples[6] = 4854;
    samples[7] = 1521;
    samples[8] = 540;
    samples[9] = -154;
    samples[10] = -178;
    samples[11] = -874;
    samples[12] = -2568;
    samples[13] = -7461;
    samples[14] = -6355;
    samples[15] = -2594; // concerned window
    samples[16] = 1471; // concerned window
    samples[17] = 4985; // concerned window
    samples[18] = 9510; // concerned window
    samples[19] = 10185; // concerned window
    samples[20] = 15675;
    samples[21] = 14655;
    samples[22] = 23085;
    samples[23] = 20045;
    samples[24] = 42064;

    fftConstants fftInfo;
    fftInfo.sizeWindow = 5;
    fftInfo.sizeSpectrumInput = 500;

    assert_int_equal(fill_fft_data(indexWindow, fftInfo, samples, fftData), EXIT_SUCCESS);

    assert_int_equal((double)samples[15], fftData[0]);
    assert_int_equal((double)samples[16], fftData[1]);
    assert_int_equal((double)samples[17], fftData[2]);
    assert_int_equal((double)samples[18], fftData[3]);
    assert_int_equal((double)samples[19], fftData[4]);

    assert_int_equal((double)samples[15], fftData[5]);
    assert_int_equal((double)samples[16], fftData[6]);
    assert_int_equal((double)samples[17], fftData[7]);
    assert_int_equal((double)samples[18], fftData[8]);
    assert_int_equal((double)samples[19], fftData[9]);

    assert_int_equal((double)samples[15], fftData[125]);
    assert_int_equal((double)samples[16], fftData[126]);
    assert_int_equal((double)samples[17], fftData[127]);
    assert_int_equal((double)samples[18], fftData[128]);
    assert_int_equal((double)samples[19], fftData[129]);

    assert_int_equal((double)samples[15], fftData[310]);
    assert_int_equal((double)samples[16], fftData[311]);
    assert_int_equal((double)samples[17], fftData[312]);
    assert_int_equal((double)samples[18], fftData[313]);
    assert_int_equal((double)samples[19], fftData[314]);

    free(samples);
    free(fftData);
}

static void test_set_fft(void** state){

    /* Memory allocation and deallocation are tested with valgrind */

    fftConstants fftInfo;
    fftInfo.sizeSpectrumInput = 500; // size of the input (and so output) arrays of the Fast Fourier Transform here fixed at 500

    /*Let us create a fftIOP structure which will be send to the set_fft function */
    fftIOP fftiop;

    assert_int_equal(set_fft(fftInfo, &fftiop), EXIT_SUCCESS);

    assert_non_null(fftiop.in);
    assert_non_null(fftiop.out);
    assert_non_null(fftiop.p);

    fftw_destroy_plan(fftiop.p);
    fftw_free(fftiop.in);
    fftw_free(fftiop.out);

}

static void test_fft(void** state){

    /* initialization of the fftConstants fftInfo corresponding to the testFile_samples_440Hz.txt file */
    fftConstants fftInfo;
    fftInfo.sizeSpectrumInput = 65536;
    fftInfo.sizeSpectrumOutput = 32768;
    fftInfo.sizeSpectrumUseful = 6250;

    /* allocation and initialization of the different needed arrays */
    double * fftData;
    fftData = (double *) safe_calloc(65536, sizeof(double));
    #include "testFile_fftData_440Hz.txt" // This file initializes the fftData array with the 3rd window of testFile_samples_440Hz.txt file

    double * power_spectrum_1d;
    power_spectrum_1d = (double * ) safe_calloc(6250, sizeof(double));

    fftIOP iop;
    iop.in = (double * ) fftw_malloc(sizeof(double) * 65536 );
    iop.out = (fftw_complex * ) fftw_malloc(sizeof(fftw_complex) * 65536 );
    iop.p = fftw_plan_dft_r2c_1d(65536, iop.in , iop.out, FFTW_ESTIMATE);

    assert_int_equal(fft(fftInfo, fftData, power_spectrum_1d, &iop), EXIT_SUCCESS);

    int maxIndex = 0;
    int max = 0;
    for(int i=0; i<6250; i++){
        if(power_spectrum_1d[i]>max){
            max=power_spectrum_1d[i];
            maxIndex=i;
        }
    }
    assert_true(maxIndex>639 && maxIndex<669); // test if the highest peak is at the index 654 (440Hz*multiplierCoefficient=654) with a margin of 15 samples

    free(fftData);
    free(power_spectrum_1d);
    fftw_destroy_plan(iop.p);
    fftw_free(iop.in);
    fftw_free(iop.out);
}

static void test_end_fft(void** state){

    /* Memory allocation and deallocation are tested with valgrind */

}
static void test_get_fft_constants(void** state){

    WavHeader wavHeader1;
    wavHeader1.dwSampleLength = 1000; // number of samples in the signal
    wavHeader1.nSamplesPerSec = 32; // number of samples per second

    fftConstants fftInfoTest1;
    fftInfoTest1.lengthMusic = 1000; // number of samples in the signal
    fftInfoTest1.samplingRate = 32; // sampling rate
    fftInfoTest1.sizeSpectrumInput = 64; // 32/0.75 = 42.67 -> 2^6=64
    fftInfoTest1.sizeSpectrumOutput = 32; // 64/2 = 32
    fftInfoTest1.sizeWindow = 2; // 32*2/32 = 2
    fftInfoTest1.nbrWindow = 500; // 1000/2 = 500
    fftInfoTest1.sizeSpectrumUseful = 8412; // (4186+20)*64/32
    fftInfoTest1.multiplierCoefficient = 0.5; // 32/64 = 0.5
    fftInfoTest1.abandonFlag = ABANDON; // because samplingRate < 2*(MAXFREQ+BUFFER)

    fftConstants fftInfo1 = get_fft_constants(wavHeader1);

    assert_int_equal(fftInfo1.lengthMusic, fftInfoTest1.lengthMusic);
    assert_int_equal(fftInfo1.samplingRate, fftInfoTest1.samplingRate);
    assert_int_equal(fftInfo1.sizeSpectrumInput, fftInfoTest1.sizeSpectrumInput);
    assert_int_equal(fftInfo1.sizeSpectrumOutput, fftInfoTest1.sizeSpectrumOutput);
    assert_int_equal(fftInfo1.sizeWindow, fftInfoTest1.sizeWindow);
    assert_int_equal(fftInfo1.nbrWindow, fftInfoTest1.nbrWindow);
    assert_int_equal(fftInfo1.sizeSpectrumUseful, fftInfoTest1.sizeSpectrumUseful);
    assert_int_equal(fftInfo1.multiplierCoefficient, fftInfoTest1.multiplierCoefficient);
    assert_int_equal(fftInfo1.abandonFlag, fftInfoTest1.abandonFlag);


    WavHeader wavHeader2;
    wavHeader2.dwSampleLength = 200000; // number of samples in the signal
    wavHeader2.nSamplesPerSec = 44100; // number of samples per second

    fftConstants fftInfoTest2;
    fftInfoTest2.lengthMusic = 200000; // number of samples in the signal
    fftInfoTest2.samplingRate = 44100; // sampling rate
    fftInfoTest2.sizeSpectrumInput = 65536; // 44100/0.75 = 58800 -> 2^16=65536
    fftInfoTest2.sizeSpectrumOutput = 32768; // 665536/2 = 32768
    fftInfoTest2.sizeWindow = 2756; // 44100*2/32 = 2756.25
    fftInfoTest2.nbrWindow = 72; // 200000/2756 = 72.57
    fftInfoTest2.sizeSpectrumUseful = 6250; // (4186+20)*65536/44100 = 6250.44
    fftInfoTest2.multiplierCoefficient = 44100/65536; // 44100/65536 = 0.6729125977...
    fftInfoTest2.abandonFlag = CONTINUE; // because not less than one window and samplingRate > 2*(MAXFREQ+BUFFER)

    fftConstants fftInfo2 = get_fft_constants(wavHeader2);

    assert_int_equal(fftInfo2.lengthMusic, fftInfoTest2.lengthMusic);
    assert_int_equal(fftInfo2.samplingRate, fftInfoTest2.samplingRate);
    assert_int_equal(fftInfo2.sizeSpectrumInput, fftInfoTest2.sizeSpectrumInput);
    assert_int_equal(fftInfo2.sizeSpectrumOutput, fftInfoTest2.sizeSpectrumOutput);
    assert_int_equal(fftInfo2.sizeWindow, fftInfoTest2.sizeWindow);
    assert_int_equal(fftInfo2.nbrWindow, fftInfoTest2.nbrWindow);
    assert_int_equal(fftInfo2.sizeSpectrumUseful, fftInfoTest2.sizeSpectrumUseful);
    assert_int_equal(fftInfo2.multiplierCoefficient, fftInfoTest2.multiplierCoefficient);
    assert_int_equal(fftInfo2.abandonFlag, fftInfoTest2.abandonFlag);

}

static void test_fft_one_window(void** state){

    /* initialization of the fftConstants fftInfo corresponding to the testFile_samples_440Hz.txt file */
    fftConstants fftInfo;
    fftInfo.lengthMusic = 22050; // number of samples in the signal
    fftInfo.samplingRate = 44100; // sampling rate
    fftInfo.sizeSpectrumInput = 65536; // 44100/0.75 = 58800 -> 2^16=65536
    fftInfo.sizeSpectrumOutput = 32768; // 665536/2 = 32768
    fftInfo.sizeWindow = 2756; // 44100*2/32 = 2756.25
    fftInfo.nbrWindow = 8; // 22050/2756 = 8.0007
    fftInfo.sizeSpectrumUseful = 6250; // (4186+20)*65536/44100 = 6250.44
    fftInfo.multiplierCoefficient = 44100/65536; // 44100/65536 = 0.6729125977...
    fftInfo.abandonFlag = CONTINUE; // because not less than one window and samplingRate > 2*(MAXFREQ+BUFFER)

    /* allocation and initialization of the different needed arrays */
    int * samples;
    samples = (int *) safe_malloc(22050 * sizeof(int));
    #include "testFile_samples_440Hz.txt" // This file initializes the samples array with a signal which is a sinusoid at 440Hz

    double * fftData;
    fftData = (double *) safe_calloc(65536, sizeof(double));

    double ** power_spectrum_2d;
    power_spectrum_2d = (double ** ) safe_calloc(8, sizeof(double*));

    fftIOP iop;
    iop.in = (double * ) fftw_malloc(sizeof(double) * 65536 );
    iop.out = (fftw_complex * ) fftw_malloc(sizeof(fftw_complex) * 65536 );
    iop.p = fftw_plan_dft_r2c_1d(65536, iop.in , iop.out, FFTW_ESTIMATE);

    /* focus on the 4th window */
    int indexWindow = 3;

    assert_int_equal(fft_one_window(indexWindow, fftInfo, samples, fftData, power_spectrum_2d, &iop), EXIT_SUCCESS);
    assert_non_null(power_spectrum_2d[3]);
    assert_null(power_spectrum_2d[1]);
    assert_null(power_spectrum_2d[5]);
    assert_null(power_spectrum_2d[6]);

    int maxIndex = 0;
    int max = 0;
    for(int i=0; i<6250; i++){
        if(power_spectrum_2d[3][i]>max){
            max=power_spectrum_2d[3][i];
            maxIndex=i;
        }
    }
    assert_true(maxIndex>639 && maxIndex<669); // test if the highest peak is at the index 654 (440Hz*multiplierCoefficient=654) with a margin of 15 samples

    /* focus on the 8th window */
    indexWindow = 7;

    assert_int_equal(fft_one_window(indexWindow, fftInfo, samples, fftData, power_spectrum_2d, &iop), EXIT_SUCCESS);
    assert_non_null(power_spectrum_2d[3]);
    assert_non_null(power_spectrum_2d[7]);

    maxIndex = 0;
    max = 0;
    for(int i=0; i<6250; i++){
        if(power_spectrum_2d[7][i]>max){
            max=power_spectrum_2d[7][i];
            maxIndex=i;
        }
    }
    assert_true(maxIndex>639 && maxIndex<669); // test if the highest peak is at the index 654 (440Hz*multiplierCoefficient=654) with a margin of 15 samples

    free(power_spectrum_2d[3]);
    free(power_spectrum_2d[7]);
    free(power_spectrum_2d);
    free(samples);
    free(fftData);
    fftw_destroy_plan(iop.p);
    fftw_free(iop.in);
    fftw_free(iop.out);
}

static void test_fft_main_function(void** state){

    /* initialization of the WavHeader wavHeader corresponding to the testFile_samples_440Hz.txt file */
    WavHeader wavHeader;
    wavHeader.dwSampleLength = 22050; // number of samples in the signal
    wavHeader.nSamplesPerSec = 44100; // number of samples per second

    /* initialization of the fftConstants fftInfoTest corresponding to the testFile_samples_440Hz.txt file */
    fftConstants fftInfoTest;
    fftInfoTest.lengthMusic = 22050; // number of samples in the signal
    fftInfoTest.samplingRate = 44100; // sampling rate
    fftInfoTest.sizeSpectrumInput = 65536; // 44100/0.75 = 58800 -> 2^16=65536
    fftInfoTest.sizeSpectrumOutput = 32768; // 665536/2 = 32768
    fftInfoTest.sizeWindow = 2756; // 44100*2/32 = 2756.25
    fftInfoTest.nbrWindow = 8; // 22050/2756 = 8.0007
    fftInfoTest.sizeSpectrumUseful = 6250; // (4186+20)*65536/44100 = 6250.44
    fftInfoTest.multiplierCoefficient = 44100/65536; // 44100/65536 = 0.6729125977...
    fftInfoTest.abandonFlag = CONTINUE; // because not less than one window and samplingRate > 2*(MAXFREQ+BUFFER)

    /* allocation and initialization of the different needed arrays */
    int * samples;
    samples = (int *) safe_malloc(22050 * sizeof(int));
    #include "testFile_samples_440Hz.txt"

    double ** power_spectrum_2d;

    fftConstants fftInfo = fft_main_function(wavHeader, samples, &power_spectrum_2d);

    assert_int_equal(fftInfo.lengthMusic, fftInfoTest.lengthMusic);
    assert_int_equal(fftInfo.samplingRate, fftInfoTest.samplingRate);
    assert_int_equal(fftInfo.sizeSpectrumInput, fftInfoTest.sizeSpectrumInput);
    assert_int_equal(fftInfo.sizeSpectrumOutput, fftInfoTest.sizeSpectrumOutput);
    assert_int_equal(fftInfo.sizeWindow, fftInfoTest.sizeWindow);
    assert_int_equal(fftInfo.nbrWindow, fftInfoTest.nbrWindow);
    assert_int_equal(fftInfo.sizeSpectrumUseful, fftInfoTest.sizeSpectrumUseful);
    assert_int_equal(fftInfo.multiplierCoefficient, fftInfoTest.multiplierCoefficient);
    assert_int_equal(fftInfo.abandonFlag, fftInfoTest.abandonFlag);

    /* focus on the 1st window */
    int maxIndex0 = 0;
    int max0 = 0;
    /* focus on the 2nd window */
    int maxIndex1 = 0;
    int max1 = 0;
    /* focus on the 3rd window */
    int maxIndex2 = 0;
    int max2 = 0;
    /* focus on the 4th window */
    int maxIndex3 = 0;
    int max3 = 0;
    /* focus on the 5th window */
    int maxIndex4 = 0;
    int max4 = 0;
    /* focus on the 6th window */
    int maxIndex5 = 0;
    int max5 = 0;
    /* focus on the 7th window */
    int maxIndex6 = 0;
    int max6 = 0;
    /* focus on the 8th window */
    int maxIndex7 = 0;
    int max7 = 0;

    for(int i=0; i<6250; i++){
        if(power_spectrum_2d[0][i]>max0){
            max0=power_spectrum_2d[0][i];
            maxIndex0=i;
        }
        if(power_spectrum_2d[1][i]>max1){
            max1=power_spectrum_2d[1][i];
            maxIndex1=i;
        }
        if(power_spectrum_2d[2][i]>max2){
            max2=power_spectrum_2d[2][i];
            maxIndex2=i;
        }
        if(power_spectrum_2d[3][i]>max3){
            max3=power_spectrum_2d[3][i];
            maxIndex3=i;
        }
        if(power_spectrum_2d[4][i]>max4){
            max4=power_spectrum_2d[4][i];
            maxIndex4=i;
        }
        if(power_spectrum_2d[5][i]>max5){
            max5=power_spectrum_2d[5][i];
            maxIndex5=i;
        }
        if(power_spectrum_2d[6][i]>max6){
            max6=power_spectrum_2d[6][i];
            maxIndex6=i;
        }
        if(power_spectrum_2d[7][i]>max7){
            max7=power_spectrum_2d[7][i];
            maxIndex7=i;
        }
    }
    assert_true(maxIndex0>639 && maxIndex0<669); // test if the highest peak is at the index 654 (440Hz*multiplierCoefficient=654) with a margin of 15 samples
    assert_true(maxIndex1>639 && maxIndex1<669);
    assert_true(maxIndex2>639 && maxIndex2<669);
    assert_true(maxIndex3>639 && maxIndex3<669);
    assert_true(maxIndex4>639 && maxIndex4<669);
    assert_true(maxIndex5>639 && maxIndex5<669);
    assert_true(maxIndex6>639 && maxIndex6<669);
    assert_true(maxIndex7>639 && maxIndex7<669);

    free(samples);
    for(int i = 0 ; i < 8 ; i++){
        free(power_spectrum_2d[i]);
    }
    free(power_spectrum_2d);
}

static void test_freePowerSpectrum(void** state){

    /* Memory allocation and deallocation are tested with valgrind */

}

int run_UTFFT(void){

const struct CMUnitTest FFTUT[] = {
    cmocka_unit_test(test_size_window),
    cmocka_unit_test(test_size_spectrum_input),
    cmocka_unit_test(test_nbr_window),
    cmocka_unit_test(test_get_flag),
    cmocka_unit_test(test_fill_fft_data),
    cmocka_unit_test(test_set_fft),
    cmocka_unit_test(test_fft),
    cmocka_unit_test(test_end_fft),
    cmocka_unit_test(test_get_fft_constants),
    cmocka_unit_test(test_fft_one_window),
    cmocka_unit_test(test_fft_main_function),
    cmocka_unit_test(test_freePowerSpectrum)
};

int out=0;

out += cmocka_run_group_tests(FFTUT, setup_FFT, teardown_FFT);

return out;
}
