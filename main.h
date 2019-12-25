
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
	size_t len;
	int at[];
} t_program;

void read_code(char *line, size_t len, void *state);
void print_state(t_program *program);
void exec_program(t_program *program);

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
