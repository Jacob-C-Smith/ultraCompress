#include "../src/data_structures/frequency_counter.h"
#include "test_utils.h"
#include <stdio.h>
#include <string.h>

void test_frequency_counter_creation(void) {
    printf("Testing frequency counter creation...\n");
    
    FrequencyCounter *counter = fc_create();
    ASSERT_NOT_NULL(counter);
    ASSERT_EQUAL(fc_get_total_symbols(counter), 0);
    
    /* Check all frequencies are initialized to 0 */
    for (int i = 0; i < 256; i++) {
        ASSERT_EQUAL(fc_get_frequency(counter, i), 0);
    }
    
    fc_free(counter);
}

void test_frequency_counter_process_data(void) {
    printf("Testing frequency counter data processing...\n");
    
    FrequencyCounter *counter = fc_create();
    
    uint8_t data[] = "hello world";
    ASSERT_EQUAL(fc_process_data(counter, data, strlen((const char *)data)), 0);
    
    ASSERT_EQUAL(fc_get_total_symbols(counter), 11);
    ASSERT_EQUAL(fc_get_frequency(counter, 'h'), 1);
    ASSERT_EQUAL(fc_get_frequency(counter, 'e'), 1);
    ASSERT_EQUAL(fc_get_frequency(counter, 'l'), 3);
    ASSERT_EQUAL(fc_get_frequency(counter, 'o'), 2);
    ASSERT_EQUAL(fc_get_frequency(counter, ' '), 1);
    ASSERT_EQUAL(fc_get_frequency(counter, 'w'), 1);
    ASSERT_EQUAL(fc_get_frequency(counter, 'r'), 1);
    ASSERT_EQUAL(fc_get_frequency(counter, 'd'), 1);
    
    fc_free(counter);
}

void test_frequency_counter_repeated_data(void) {
    printf("Testing frequency counter with repeated data...\n");
    
    FrequencyCounter *counter = fc_create();
    
    uint8_t data1[] = "aaa";
    uint8_t data2[] = "aa";
    
    fc_process_data(counter, data1, 3);
    fc_process_data(counter, data2, 2);
    
    ASSERT_EQUAL(fc_get_frequency(counter, 'a'), 5);
    ASSERT_EQUAL(fc_get_total_symbols(counter), 5);
    
    fc_free(counter);
}

void test_frequency_counter_empty_data(void) {
    printf("Testing frequency counter with empty data...\n");
    
    FrequencyCounter *counter = fc_create();
    
    uint8_t data[] = "";
    ASSERT_EQUAL(fc_process_data(counter, data, 0), 0);
    ASSERT_EQUAL(fc_get_total_symbols(counter), 0);
    
    fc_free(counter);
}

void test_frequency_counter_binary_data(void) {
    printf("Testing frequency counter with binary data...\n");
    
    FrequencyCounter *counter = fc_create();
    
    uint8_t data[] = {0x00, 0xFF, 0x00, 0xFF, 0x00};
    ASSERT_EQUAL(fc_process_data(counter, data, 5), 0);
    
    ASSERT_EQUAL(fc_get_frequency(counter, 0x00), 3);
    ASSERT_EQUAL(fc_get_frequency(counter, 0xFF), 2);
    ASSERT_EQUAL(fc_get_total_symbols(counter), 5);
    
    fc_free(counter);
}

void test_frequency_counter_get_frequencies_array(void) {
    printf("Testing frequency counter array retrieval...\n");
    
    FrequencyCounter *counter = fc_create();
    
    uint8_t data[] = "aabbcc";
    fc_process_data(counter, data, 6);
    
    uint32_t freqs[256];
    ASSERT_EQUAL(fc_get_frequencies(counter, freqs), 0);
    
    ASSERT_EQUAL(freqs['a'], 2);
    ASSERT_EQUAL(freqs['b'], 2);
    ASSERT_EQUAL(freqs['c'], 2);
    ASSERT_EQUAL(freqs['d'], 0);
    
    fc_free(counter);
}

void test_frequency_counter_reset(void) {
    printf("Testing frequency counter reset...\n");
    
    FrequencyCounter *counter = fc_create();
    
    uint8_t data[] = "test";
    fc_process_data(counter, data, 4);
    ASSERT_EQUAL(fc_get_total_symbols(counter), 4);
    
    ASSERT_EQUAL(fc_reset(counter), 0);
    ASSERT_EQUAL(fc_get_total_symbols(counter), 0);
    
    for (int i = 0; i < 256; i++) {
        ASSERT_EQUAL(fc_get_frequency(counter, i), 0);
    }
    
    fc_free(counter);
}

void test_frequency_counter_large_data(void) {
    printf("Testing frequency counter with large data...\n");
    
    FrequencyCounter *counter = fc_create();
    
    /* Create a buffer with repeated pattern */
    uint8_t buffer[1000];
    for (int i = 0; i < 1000; i++) {
        buffer[i] = (i % 10) + '0';  /* '0' through '9' */
    }
    
    ASSERT_EQUAL(fc_process_data(counter, buffer, 1000), 0);
    
    ASSERT_EQUAL(fc_get_total_symbols(counter), 1000);
    for (int i = 0; i < 10; i++) {
        ASSERT_EQUAL(fc_get_frequency(counter, '0' + i), 100);
    }
    
    fc_free(counter);
}

void test_frequency_counter_invalid_operations(void) {
    printf("Testing frequency counter invalid operations...\n");
    
    /* NULL operations */
    ASSERT_EQUAL(fc_process_data(NULL, NULL, 0), -1);
    ASSERT_EQUAL(fc_get_frequencies(NULL, NULL), -1);
    ASSERT_EQUAL(fc_reset(NULL), -1);
    ASSERT_EQUAL(fc_get_frequency(NULL, 'a'), 0);
    ASSERT_EQUAL(fc_get_total_symbols(NULL), 0);
    
    fc_free(NULL);  /* Should not crash */
    
    ASSERT_TRUE(1);
}

int main(void) {
    printf("======= FREQUENCY COUNTER TESTS =======\n\n");
    
    test_frequency_counter_creation();
    test_frequency_counter_process_data();
    test_frequency_counter_repeated_data();
    test_frequency_counter_empty_data();
    test_frequency_counter_binary_data();
    test_frequency_counter_get_frequencies_array();
    test_frequency_counter_reset();
    test_frequency_counter_large_data();
    test_frequency_counter_invalid_operations();
    
    print_test_summary();
    
    return EXIT_SUCCESS;
}
