#include "main.h"

/*
 * WHAT I LEARNED THAT DAY:
 *
 * You can't just use the amount of entries an array has
 * to copy it's content with memcpy. In hindsight this makes
 * perfect sense, since we're talking about copying MEMORY
 * and not >things<, but lil'-brained me took ages (and quite
 * a few StackOverflow (cause that's literally what I've been causing)
 * posts) to understand that...
 */


void process() {
	t_program *state = NULL;

	// we are expecting exactly ONE line of code,
	// anything preceding the last line will be discarded, allocated memory will NOT be freed
	read_file("../input2", read_code, &state);

	t_program *program = NULL;
	program = malloc(sizeof(t_program) + sizeof(int[state->len]));
	program->instruction_pointer = 0;
	program->len = state->len;

	memcpy(program->at, state->at, sizeof(int) * state->len);

	// part1:
	program->at[1] = 12;
	program->at[2] = 2;

	exec_program(program);

	/*
	// print list
	for (size_t i = 0; i < program->len; i++) {
		if (i > 0) {
			putchar(',');
		}
		printf("%d", program->at[i]);
	}
	puts("\n");
	*/

	// part2:
	for (int i = 0; i < 100; ++i) {
		for (int j = 0; j < 100; ++j) {
			// initialise state
			memcpy(program->at, state->at, sizeof(int) * state->len);
			program->instruction_pointer = 0;

			// set noun and verb
			program->at[1] = i; // noun
			program->at[2] = j; // verb

			exec_program(program);

			printf("i: %d, j: %d, 0: %d, k: %d\n", i, j, program->at[0], 100 * i + j);
			if (program->at[0] == 19690720) { // part2:
				goto end;
			}
		}
	}
	end:
	puts("got it\n");

	free(state);
	free(program);
}
