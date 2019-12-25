#include "main.h"

void test() {
	t_program *program = NULL;

	read_file("../input5", read_code, &program);
	//print_state(program);

	// part1 and part2
	exec_program(program);
	//print_state(program);

	free(program);
}