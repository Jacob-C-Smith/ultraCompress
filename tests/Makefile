CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99
SRC_DIR = ../src/data_structures
TEST_DIR = .

# Test executables
TESTS = test_priority_queue test_huffman_tree test_frequency_counter

# Source files
PRIORITY_QUEUE_SRC = $(SRC_DIR)/priority_queue.c
HUFFMAN_TREE_SRC = $(SRC_DIR)/huffman_tree.c
FREQUENCY_COUNTER_SRC = $(SRC_DIR)/frequency_counter.c

.PHONY: all clean run

all: $(TESTS)

test_priority_queue: test_priority_queue.c $(PRIORITY_QUEUE_SRC) $(HUFFMAN_TREE_SRC)
	$(CC) $(CFLAGS) -o $@ $^

test_huffman_tree: test_huffman_tree.c $(HUFFMAN_TREE_SRC) $(PRIORITY_QUEUE_SRC)
	$(CC) $(CFLAGS) -o $@ $^

test_frequency_counter: test_frequency_counter.c $(FREQUENCY_COUNTER_SRC)
	$(CC) $(CFLAGS) -o $@ $^

run: all
	@echo "\n========== Running Priority Queue Tests =========="
	@./test_priority_queue
	@echo "\n========== Running Huffman Tree Tests =========="
	@./test_huffman_tree
	@echo "\n========== Running Frequency Counter Tests =========="
	@./test_frequency_counter

clean:
	rm -f $(TESTS) *.o

.DEFAULT_GOAL := all
