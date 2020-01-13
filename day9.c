#include "main.h"

void boost() {
	t_program *program = NULL;
	read_file("../input9", read_code, &program);

	exec_program(program);

	free(program);
}
