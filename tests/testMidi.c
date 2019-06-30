#include "testMidi.h"

static int setup(void** state){
    char * name = "name";
    FILE * p = open_midi_file(name);
    if(p != NULL){
        (*state) = p;
        return 0;
    }
    return -1;
}

static int teardown(void** state){
    remove("name");
    remove("test");
    close_midi_file((*state));
    return 0;
}

static void test_midi_header_writer(void** state){

    int a;

    //bad arguments test
    FILE * fichier = fopen("test","wb+");
    MIDI_header_writer(fichier, 0, 3, noire);
    fseek(fichier,0,SEEK_SET);
    fread(&a,sizeof(char),2,fichier);
    assert_int_not_equal(a,0x544d);
    close_midi_file(fichier);

    fichier = fopen("test","wb+");
    MIDI_header_writer(fichier, 1, 3, noire);
    fseek(fichier,0,SEEK_SET);
    fread(&a,sizeof(char),2,fichier);
    assert_int_equal(a,0x544d);
    fread(&a,sizeof(char),2,fichier);
    assert_int_equal(a,0x6468);
    fread(&a,sizeof(char),2,fichier);
    assert_int_equal(a,0x0000);
    fread(&a,sizeof(char),2,fichier);
    assert_int_equal(a,0x0600);
    //les derniers bits non testes
    close_midi_file(fichier);
}

static void test_midi_header_track(void** state){

    char a;

    FILE * fichier = fopen("test","wb+");
    unsigned long ret = MIDI_header_track(fichier);

    //checking the return value
    assert_int_equal(8,ret);
    fseek(fichier,0,SEEK_SET);

    //checking what's written in the file
    fread(&a,sizeof(char),1,fichier);
    assert_int_equal(a,0x4d);
    fread(&a,sizeof(char),1,fichier);
    assert_int_equal(a,0x54);
    fread(&a,sizeof(char),1,fichier);
    assert_int_equal(a,0x72);
    fread(&a,sizeof(char),1,fichier);
    assert_int_equal(a,0x6b);

    close_midi_file(fichier);
}

static void test_midi_end_of_track(void** state){

    FILE * test = fopen("test","wb+");
    char a;

    MIDI_end_of_track(test);
    fseek(test,0,SEEK_SET);
    fread(&a,sizeof(char),1,test);
    assert_int_equal(a,0x00); //Midi_delta_time
    fread(&a,sizeof(char),1,test);
    assert_int_equal(a,0xffffffffffffffff); //attention aux bits de poids forts !!
    fread(&a,sizeof(char),1,test);
    assert_int_equal(a,0x2f);
    fread(&a,sizeof(char),1,test);
    assert_int_equal(a,0x00);
    close_midi_file(test);
}

static void test_midi_tempo(void** state){

    FILE * test = fopen("test","wb+");
    char a;
    unsigned long duree = 10;

    MIDI_tempo(test,duree);
    fseek(test,0,SEEK_SET);
    fread(&a,sizeof(char),1,test);
    assert_int_equal(a,0x00);
    fread(&a,sizeof(char),1,test);
    assert_int_equal(a,0xffffffffffffffff);
    fread(&a,sizeof(char),1,test);
    assert_int_equal(a,0x51);
    fread(&a,sizeof(char),1,test);
    assert_int_equal(a,0x03);
    fread(&a,sizeof(char),1,test);
    assert_int_equal(a,duree>>16);
    fread(&a,sizeof(char),1,test);
    assert_int_equal(a,duree>>8);
    fread(&a,sizeof(char),1,test);
    assert_int_equal(a,duree);
    close_midi_file(test);
}

static void test_midi_note(void** state){

    FILE * test = fopen("test","wb+");
    char a;
    unsigned char etat = ON;
    unsigned char bad_state = 5;
    unsigned char canal = 5;
    unsigned char Note_MIDI = 10;
    unsigned char velocite = 10;

    MIDI_Note(bad_state, test, canal, Note_MIDI, velocite);
    fread(&a,sizeof(char),1,test);
    assert_int_not_equal(a,bad_state + canal % 16);
    close_midi_file(test);

    test = fopen("test","wb+");
    MIDI_Note(etat, test, canal, Note_MIDI, velocite);
    fread(&a,sizeof(char),1,test);
    assert_int_equal(a,etat + canal % 16);
    fread(&a,sizeof(char),1,test);
    assert_int_equal(a,Note_MIDI % 128);
    fread(&a,sizeof(char),1,test);
    assert_int_equal(a,velocite % 128);
    close_midi_file(test);
}

static void test_midi_program_change(void** state){

    char a;
    FILE * test = open_midi_file("name");
    unsigned char canal_test = 5;
    unsigned char instrument_test = 30;
    MIDI_Program_Change(test, canal_test, instrument_test);
    fseek(test,1,SEEK_SET);
    fread(&a,sizeof(char),1,test);
    assert_int_equal(a,(0xC0 + canal_test % 16));
    fread(&a,sizeof(char),1,test);
    assert_int_equal(a,(instrument_test % 128));
    close_midi_file(test);

}

int run_UTmidi(void){
    const struct CMUnitTest tests_plateau[] = {

        cmocka_unit_test(test_midi_header_writer),
        cmocka_unit_test(test_midi_header_track),
        cmocka_unit_test(test_midi_end_of_track),
        cmocka_unit_test(test_midi_tempo),
        cmocka_unit_test(test_midi_note),
        cmocka_unit_test(test_midi_program_change)

    };

    return cmocka_run_group_tests(tests_plateau,setup,teardown);
}
