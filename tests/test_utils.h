#ifndef TEST_UTILS_H
#define TEST_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Test result tracking */
typedef struct {
    int total_tests;
    int passed_tests;
    int failed_tests;
} TestResults;

static TestResults test_results = {0, 0, 0};

/* Assertion macros */
#define ASSERT_EQUAL(actual, expected) \
    do { \
        test_results.total_tests++; \
        if ((actual) == (expected)) { \
            test_results.passed_tests++; \
        } else { \
            test_results.failed_tests++; \
            fprintf(stderr, "FAIL: %s:%d - Expected %ld, got %ld\n", \
                    __FILE__, __LINE__, (long)(expected), (long)(actual)); \
        } \
    } while (0)

#define ASSERT_NOT_NULL(ptr) \
    do { \
        test_results.total_tests++; \
        if ((ptr) != NULL) { \
            test_results.passed_tests++; \
        } else { \
            test_results.failed_tests++; \
            fprintf(stderr, "FAIL: %s:%d - Expected non-NULL pointer\n", \
                    __FILE__, __LINE__); \
        } \
    } while (0)

#define ASSERT_NULL(ptr) \
    do { \
        test_results.total_tests++; \
        if ((ptr) == NULL) { \
            test_results.passed_tests++; \
        } else { \
            test_results.failed_tests++; \
            fprintf(stderr, "FAIL: %s:%d - Expected NULL pointer\n", \
                    __FILE__, __LINE__); \
        } \
    } while (0)

#define ASSERT_TRUE(condition) \
    do { \
        test_results.total_tests++; \
        if ((condition)) { \
            test_results.passed_tests++; \
        } else { \
            test_results.failed_tests++; \
            fprintf(stderr, "FAIL: %s:%d - Expected true, got false\n", \
                    __FILE__, __LINE__); \
        } \
    } while (0)

#define ASSERT_FALSE(condition) \
    do { \
        test_results.total_tests++; \
        if (!(condition)) { \
            test_results.passed_tests++; \
        } else { \
            test_results.failed_tests++; \
            fprintf(stderr, "FAIL: %s:%d - Expected false, got true\n", \
                    __FILE__, __LINE__); \
        } \
    } while (0)

/* Test reporting */
static void print_test_summary(void) {
    printf("\n========== TEST SUMMARY ==========\n");
    printf("Total Tests:  %d\n", test_results.total_tests);
    printf("Passed:       %d\n", test_results.passed_tests);
    printf("Failed:       %d\n", test_results.failed_tests);
    printf("==================================\n");
    
    if (test_results.failed_tests == 0) {
        printf("All tests passed!\n");
    } else {
        printf("Some tests failed.\n");
        exit(EXIT_FAILURE);
    }
}

#endif /* TEST_UTILS_H */
