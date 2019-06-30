#include "testNotes.h"

static int setup(void** state){
    monotone_track * p = init_a_track();
    if(p != NULL){
        (*state) = p;
        return 0;
    }
    return -1;
}

static int teardown(void** state){
    free_monotone_track((*state));
    return 0;
}

static void test_init_a_track_retourNonNul(void** state){
    monotone_track * p = init_a_track();
    assert_ptr_not_equal(p,NULL);
    free(p);
}

static void test_free_monotone_track_trackNULL(void** state){
    assert_int_equal(-1,free_monotone_track(NULL));
}

static void test_read_monotone_track_BadAttributes(void** state){

    notes * tab = (notes*)(*state);
    FILE * f = (FILE*)(*state);
    assert_int_equal(-1,read_monotone_track(NULL, tab,10));
    assert_int_equal(-1,read_monotone_track(f, NULL,10));
    assert_int_equal(-1,read_monotone_track(f, tab,-1));

}

static void test_write_note_BadAttributes(void** state){
    notes* tab = (notes*) (*state);
    assert_int_equal(-1,write_a_note(3, 10, 10, tab, 10));
    assert_int_equal(-1,write_a_note(89, 10, 10, tab, 10));
    assert_int_equal(-1,write_a_note(10, 10, 10, tab, -1));
    assert_int_equal(-1,write_a_note(10, 10, 10, NULL, -1));
}

static void test_write_monotone_track_BadAttributes(void** state){

    NoteFreq * tf = (NoteFreq*)(*state);
    assert_ptr_equal(NULL,write_monotone_track(NULL, 10, 50000, 1));
    assert_ptr_equal(NULL,write_monotone_track(tf, 0, 50000, 1));
    assert_ptr_equal(NULL,write_monotone_track(tf, 10, 50000, -1));
    assert_ptr_equal(NULL,write_monotone_track(tf, 10, 50000, 11));

}

int run_UTnotes(void){
    const struct CMUnitTest tests_plateau[] = {
            cmocka_unit_test(test_init_a_track_retourNonNul),
            cmocka_unit_test(test_free_monotone_track_trackNULL),
            cmocka_unit_test(test_write_note_BadAttributes),
            cmocka_unit_test(test_read_monotone_track_BadAttributes),
            cmocka_unit_test(test_write_monotone_track_BadAttributes)
    };
    return cmocka_run_group_tests(tests_plateau,setup,teardown);
}
