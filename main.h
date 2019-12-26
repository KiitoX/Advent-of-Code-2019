
#ifndef ADVENT2019_MAIN_H
#define ADVENT2019_MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <stdbool.h>

#define forever for(;;)

// misc.c
const char *sbool(bool b);

// gpio.c
void read_file(const char* file_name, void handle_line(char *, size_t, void *), void *state);

// intcode.c
typedef struct {
	size_t instruction_pointer;
	size_t len;
	int at[];
} t_program;

typedef enum {
	HAS_FINISHED = 1,
	NEEDS_INPUT = 2,
} e_reason;

typedef struct {
	union {
		size_t pos; // used for cached input
		e_reason reason; // termination reason in output
	};
	size_t len; // modified only in output
	int data[];
} t_input, t_output;

typedef enum {
	DIRECT = 1, // use stdin/stdout
	CACHED = 2, // use t_input and t_output
} e_io_mode;

void read_code(char *line, size_t len, void *state);
void print_state(t_program *program);
void exec_program(t_program *program);
t_output *run_program(t_program *program, t_input *input, e_io_mode io_mode);

// these are the functions that each complete the two assignments for every day
// day1.c
void tally_up();

// day2.c
void process();

// day3.c
void crisscross();

// day4.c
void break_pw();

// day5.c
void test();

// day6.c
void calc_orbits();

// day7.c
void amp_out();

#endif //ADVENT2019_MAIN_H
