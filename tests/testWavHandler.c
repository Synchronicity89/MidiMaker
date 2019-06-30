#include "testWavHandler.h"

static int setup_wav_mono_1byte(void** state) {

    /*give the wav file path*/
    *state = malloc(PATH_MAX* sizeof(char));
    getcwd(*state, PATH_MAX);
    strcat(*state,"/unit_tests/testFile_mono_2samples_1byte.wav");

    return 0;
}

static void test_load_mono_1byte_header(void** state){
    char* path = (char*) *state;

    WavHeader h;
    int* samples = NULL;
    load(path,&h,&samples);
    free(samples);
    assert_int_equal(h.file_size,0x26);
    assert_int_equal(h.wFormatTag,WAVE_FORMAT_PCM);
    assert_int_equal(h.nChannels,1);
    assert_int_equal(h.nSamplesPerSec,0xac44);
    assert_int_equal(h.nAvgBytesPerSec,0xac44);
    assert_int_equal(h.nBlockAlign,1);
    assert_int_equal(h.wBitsPerSample,0x08);

    assert_int_equal(h.cbSize,0x0);
    assert_int_equal(h.wValidBitsPerSample,0x08);
    assert_int_equal(h.dwChannelMask,0);
    assert_int_equal(h.subFormat,WAVE_FORMAT_UNDEFINED);

    assert_int_equal(h.dwSampleLength,2);
}

static void test_load_mono_1byte_samples(void** state){
    char* path = (char*) *state;

    WavHeader h;
    int* samples = NULL;
    load(path,&h,&samples);

    assert_int_equal(samples[0],0x7c -0X80);
    assert_int_equal(samples[1],0x9e -0x80);

    free(samples);
}

static int setup_wav_mono_2bytes(void** state) {

    /*give the wav file path*/
    *state = malloc(PATH_MAX* sizeof(char));
    getcwd(*state, PATH_MAX);
    strcat(*state,"/unit_tests/testFile_mono_2samples_2bytes.wav");

    return 0;
}

static void test_load_mono_2bytes_header(void** state){
    char* path = (char*) *state;

    WavHeader h;
    int* samples = NULL;
    load(path,&h,&samples);
    free(samples);
    assert_int_equal(h.file_size,0x28);
    assert_int_equal(h.wFormatTag,WAVE_FORMAT_PCM);
    assert_int_equal(h.nChannels,1);
    assert_int_equal(h.nSamplesPerSec,0xac44);
    assert_int_equal(h.nAvgBytesPerSec,0x15888);
    assert_int_equal(h.nBlockAlign,2);
    assert_int_equal(h.wBitsPerSample,0x10);

    assert_int_equal(h.cbSize,0x0);
    assert_int_equal(h.wValidBitsPerSample,0x10);
    assert_int_equal(h.dwChannelMask,0);
    assert_int_equal(h.subFormat,WAVE_FORMAT_UNDEFINED);

    assert_int_equal(h.dwSampleLength,2);
}

static void test_load_mono_2bytes_samples(void** state){
    char* path = (char*) *state;

    WavHeader h;
    int* samples = NULL;
    load(path,&h,&samples);

    assert_int_equal(samples[0],0xffffffffffff9e7c); //Two's complement
    assert_int_equal(samples[1],0x3467);

    free(samples);
}

static int setup_wav_mono_4bytes(void** state) {

    /*give the wav file path*/
    *state = malloc(PATH_MAX* sizeof(char));
    getcwd(*state, PATH_MAX);
    strcat(*state,"/unit_tests/testFile_mono_2samples_4bytes.wav");

    return 0;
}

static void test_load_mono_4bytes_header(void** state){
    char* path = (char*) *state;

    WavHeader h;
    int* samples = NULL;
    load(path,&h,&samples);
    free(samples);
    assert_int_equal(h.file_size,0x2b);
    assert_int_equal(h.wFormatTag,WAVE_FORMAT_PCM);
    assert_int_equal(h.nChannels,1);
    assert_int_equal(h.nSamplesPerSec,0xac44);
    assert_int_equal(h.nAvgBytesPerSec,0x20B110);
    assert_int_equal(h.nBlockAlign,4);
    assert_int_equal(h.wBitsPerSample,0x20);

    assert_int_equal(h.cbSize,0);
    assert_int_equal(h.wValidBitsPerSample,0x20);
    assert_int_equal(h.dwChannelMask,0);
    assert_int_equal(h.subFormat,WAVE_FORMAT_UNDEFINED);

    assert_int_equal(h.dwSampleLength,2);
}

static void test_load_mono_4bytes_samples(void** state){
    char* path = (char*) *state;

    WavHeader h;
    int* samples = NULL;
    load(path,&h,&samples);

    assert_int_equal(samples[0],0x6d10669);
    assert_int_equal(samples[1],0x56898989);

    free(samples);
}

static int setup_wav_stereo_2bytes(void** state) {

    /*give the wav file path*/
    *state = malloc(PATH_MAX* sizeof(char));
    getcwd(*state, PATH_MAX);
    strcat(*state,"/unit_tests/testFile_stereo_2samples_2bytes.wav");

    return 0;
}

static void test_load_stereo_2bytes_header(void** state){
    char* path = (char*) *state;

    WavHeader h;
    int* samples = NULL;
    load(path,&h,&samples);
    free(samples);
    assert_int_equal(h.file_size,0x54);
    assert_int_equal(h.wFormatTag,WAVE_FORMAT_EXTENSIBLE);
    assert_int_equal(h.nChannels,2);
    assert_int_equal(h.nSamplesPerSec,0xac44);
    assert_int_equal(h.nAvgBytesPerSec,0x2b110);
    assert_int_equal(h.nBlockAlign,4);
    assert_int_equal(h.wBitsPerSample,0x10);

    assert_int_equal(h.cbSize,0x16);
    assert_int_equal(h.wValidBitsPerSample,0x10);
    assert_int_equal(h.dwChannelMask,0xfffffffe);
    assert_int_equal(h.subFormat,WAVE_FORMAT_PCM);

    assert_int_equal(h.dwSampleLength,2);
}

static void test_load_stereo_2bytes_samples(void** state){
    char* path = (char*) *state;

    WavHeader h;
    int* samples = NULL;
    load(path,&h,&samples);

    assert_int_equal(samples[0],0x4);
    assert_int_equal(samples[1],0x563);

    free(samples);
}

static int setup_wav_fileDoNotExists(void** state) {

    /*give the wav file path*/
    *state = malloc(PATH_MAX* sizeof(char));
    getcwd(*state, PATH_MAX);
    strcat(*state,"/unit_tests/fileDoNotExists.wav");

    return 0;
}
static void test_load_fileDoNotExists(void** state){
    char* path = (char*) *state;

    WavHeader h;
    int* samples = NULL;
    load(path,&h,&samples);
    assert_int_equal(h.file_size,0);
    assert_int_equal(h.wFormatTag,WAVE_FORMAT_UNDEFINED);
    assert_int_equal(h.nChannels,0);
    assert_int_equal(h.nSamplesPerSec,0);
    assert_int_equal(h.nAvgBytesPerSec,0);
    assert_int_equal(h.nBlockAlign,0);
    assert_int_equal(h.wBitsPerSample,0);

    assert_int_equal(h.cbSize,0);
    assert_int_equal(h.wValidBitsPerSample,0);
    assert_int_equal(h.dwChannelMask,0);
    assert_int_equal(h.subFormat,WAVE_FORMAT_UNDEFINED);

    assert_int_equal(h.dwSampleLength,0);

    assert_int_equal(samples,NULL);

    free(samples);
}

static int setup_wav_notAWavFile(void** state) {

    /*give the wav file path*/
    *state = malloc(PATH_MAX* sizeof(char));
    getcwd(*state, PATH_MAX);
    strcat(*state,"/unit_tests/testFile_notAWavFile.txt");

    return 0;
}
static void test_load_notAWavFile(void** state){
    char* path = (char*) *state;

    WavHeader h;
    int* samples = NULL;
    load(path,&h,&samples);
    assert_int_equal(h.file_size,0);
    assert_int_equal(h.wFormatTag,WAVE_FORMAT_UNDEFINED);
    assert_int_equal(h.nChannels,0);
    assert_int_equal(h.nSamplesPerSec,0);
    assert_int_equal(h.nAvgBytesPerSec,0);
    assert_int_equal(h.nBlockAlign,0);
    assert_int_equal(h.wBitsPerSample,0);

    assert_int_equal(h.cbSize,0);
    assert_int_equal(h.wValidBitsPerSample,0);
    assert_int_equal(h.dwChannelMask,0);
    assert_int_equal(h.subFormat,WAVE_FORMAT_UNDEFINED);

    assert_int_equal(h.dwSampleLength,0);

    assert_int_equal(samples,NULL);

    free(samples);
}

static int setup_wav_notAProperWavFile(void** state) {

    /*give the wav file path*/
    *state = malloc(PATH_MAX* sizeof(char));
    getcwd(*state, PATH_MAX);
    strcat(*state,"/unit_tests/testFile_notAProperWavFile.wav");

    return 0;
}
static void test_load_notAProperWavFile(void** state){
    char* path = (char*) *state;

    WavHeader h;
    int* samples = NULL;
    load(path,&h,&samples);
    assert_int_equal(h.file_size,0xb);
    assert_int_equal(h.wFormatTag,WAVE_FORMAT_UNDEFINED);
    assert_int_equal(h.nChannels,0);
    assert_int_equal(h.nSamplesPerSec,0);
    assert_int_equal(h.nAvgBytesPerSec,0);
    assert_int_equal(h.nBlockAlign,0);
    assert_int_equal(h.wBitsPerSample,0);

    assert_int_equal(h.cbSize,0);
    assert_int_equal(h.wValidBitsPerSample,0);
    assert_int_equal(h.dwChannelMask,0);
    assert_int_equal(h.subFormat,WAVE_FORMAT_UNDEFINED);

    assert_int_equal(h.dwSampleLength,0);

    assert_int_equal(samples,NULL);

    free(samples);
}


static int teardown_wav(void** state){
    free(*state);
    return 0;
}

int run_UTwavHandler(void) {

    const struct CMUnitTest test_load_mono_1byte[] = {
            cmocka_unit_test(test_load_mono_1byte_header),
            cmocka_unit_test(test_load_mono_1byte_samples)
    };
    const struct CMUnitTest test_load_mono_2bytes[] = {
            cmocka_unit_test(test_load_mono_2bytes_header),
            cmocka_unit_test(test_load_mono_2bytes_samples)
    };
    const struct CMUnitTest test_load_mono_4bytes[] = {
            cmocka_unit_test(test_load_mono_4bytes_header),
            cmocka_unit_test(test_load_mono_4bytes_samples)
    };
    const struct CMUnitTest test_load_stereo[] = {
            cmocka_unit_test(test_load_stereo_2bytes_header),
            cmocka_unit_test(test_load_stereo_2bytes_samples)
    };
    const struct CMUnitTest test_load_fileDoNotExists_struct[] = {
            cmocka_unit_test(test_load_fileDoNotExists)
    };
    const struct CMUnitTest test_load_notAWavFile_struct[] = {
            cmocka_unit_test(test_load_notAWavFile)
    };
    const struct CMUnitTest test_load_notAProperWavFile_struct[] = {
            cmocka_unit_test(test_load_notAProperWavFile)
    };

    int out = 0;

    out += cmocka_run_group_tests(test_load_mono_1byte, setup_wav_mono_1byte, teardown_wav);
    out += cmocka_run_group_tests(test_load_mono_2bytes, setup_wav_mono_2bytes, teardown_wav);
    out += cmocka_run_group_tests(test_load_mono_4bytes, setup_wav_mono_4bytes, teardown_wav);
    out += cmocka_run_group_tests(test_load_stereo, setup_wav_stereo_2bytes, teardown_wav);
    out += cmocka_run_group_tests(test_load_fileDoNotExists_struct, setup_wav_fileDoNotExists, teardown_wav);
    out += cmocka_run_group_tests(test_load_notAWavFile_struct, setup_wav_notAWavFile, teardown_wav);
    out += cmocka_run_group_tests(test_load_notAProperWavFile_struct, setup_wav_notAProperWavFile, teardown_wav);


    return out;
}