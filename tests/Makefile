CC = gcc
CFLAGS = -Wall -Wextra -g -std=c99
SRC_DIR = ../src/data_structures
ENCODER_SRC = ../src
TEST_DIR = .

# Audio libraries
AUDIO_LIBS = -lmpg123 -lmp3lame -lm
AUDIO_CFLAGS = $(shell pkg-config --cflags libmpg123 libmp3lame 2>/dev/null)

# Test executables
TESTS = test_priority_queue test_huffman_tree test_frequency_counter test_huffman_encoder test_mp3_io

# Source files
PRIORITY_QUEUE_SRC = $(SRC_DIR)/priority_queue.c
HUFFMAN_TREE_SRC = $(SRC_DIR)/huffman_tree.c
FREQUENCY_COUNTER_SRC = $(SRC_DIR)/frequency_counter.c
HUFFMAN_ENCODER_SRC = $(ENCODER_SRC)/huffman_encoder.c
AUDIO_MP3_SRC = $(ENCODER_SRC)/audio_io/audio_io_mp3.c

.PHONY: all clean run

all: $(TESTS)

test_priority_queue: test_priority_queue.c $(PRIORITY_QUEUE_SRC) $(HUFFMAN_TREE_SRC)
	$(CC) $(CFLAGS) -o $@ $^

test_huffman_tree: test_huffman_tree.c $(HUFFMAN_TREE_SRC) $(PRIORITY_QUEUE_SRC)
	$(CC) $(CFLAGS) -o $@ $^

test_frequency_counter: test_frequency_counter.c $(FREQUENCY_COUNTER_SRC)
	$(CC) $(CFLAGS) -o $@ $^

test_huffman_encoder: test_huffman_encoder.c $(HUFFMAN_ENCODER_SRC) $(HUFFMAN_TREE_SRC) $(PRIORITY_QUEUE_SRC) $(FREQUENCY_COUNTER_SRC)
	$(CC) $(CFLAGS) -I$(ENCODER_SRC) -o $@ $^

test_mp3_io: test_mp3_io.c $(AUDIO_MP3_SRC) $(HUFFMAN_ENCODER_SRC) $(HUFFMAN_TREE_SRC) $(PRIORITY_QUEUE_SRC) $(FREQUENCY_COUNTER_SRC)
	$(CC) $(CFLAGS) $(AUDIO_CFLAGS) -I$(ENCODER_SRC) -o $@ $^ $(AUDIO_LIBS)

run: all
	@echo "\n========== Running Priority Queue Tests =========="
	@./test_priority_queue
	@echo "\n========== Running Huffman Tree Tests =========="
	@./test_huffman_tree
	@echo "\n========== Running Frequency Counter Tests =========="
	@./test_frequency_counter
	@echo "\n========== Running Huffman Encoder Tests =========="
	@./test_huffman_encoder
	@echo "\n========== Running MP3 I/O & Compression Tests =========="
	@./test_mp3_io

clean:
	rm -f $(TESTS) *.o

.DEFAULT_GOAL := all
