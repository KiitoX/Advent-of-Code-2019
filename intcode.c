#include "main.h"

// expecting a pointer to a (t_program *) which will be malloc'ed accordingly
void read_code(char *line, size_t len, void *state) {
	t_program **program_pointer = (t_program **)state;
	int content[(len + 1) / 2]; // array with maximum amount of digits when input in format 1,2,3,4...

	int matches = 0;
	int index = 0;
	while ((matches = sscanf(line, "%d%*[,]%s", &(content[index]), line))) {
		//printf("%d: %d (%s) -> %d\n", index, content[index], line, matches);
		if (matches < 2) { // detect last match
			break;
		}
		index++;
	}

	*program_pointer = malloc(sizeof(t_program) + sizeof(int[index + 1]));
	t_program *program = *program_pointer;

	program->len = index + 1;

	for (size_t i = 0; i < program->len; ++i) {
		program->at[i] = content[i];
	}
}

void print_state(t_program *program) {

	for (size_t i = 0; i < program->len; i++) {
		if (i > 0) {
			putchar(',');
		}
		printf("%d", program->at[i]);
	}
	putchar('\n');
}

typedef enum {
	POSITION = 0, // parameters are pointers
	IMMEDIATE = 1, // parameters are literals
} e_parameter_mode;

int get_param(t_program *program, size_t pos, unsigned off) {
	unsigned op_code = program->at[pos];

	// op_code: ABCDE => A: pmode3, B: pmode2, C: pmode1, DE: instruction
	e_parameter_mode pmode = (op_code / (int)pow(10, 1 + off)) % 10;

	int arg = 0; // fallback value, since we always try to retrieve 3 args, even when we don't need to, or can't
	if ((pos + off) < program->len) {
		arg = program->at[pos + off];
	}

	//printf("(%ld + %d) -> %d w/ %d\n", pos, off, arg, pmode);

	switch (pmode) {
		case POSITION:
			if(arg < program->len) { // another fallback for wild numbers
				return program->at[arg];
			} else {
				return 0;
			}
		case IMMEDIATE:
			return arg;
		default:
			fprintf(stderr, "Invalid pmode: %d (%d at %ld)\n", pmode, op_code, pos);
	}
}

typedef enum {
	ADDITION = 1, // in1, in2, dest
	MULTIPLICATION = 2, // in1, in2, dest
	INPUT = 3, // dest
	OUTPUT = 4, // in1
	JUMP_IF_TRUE = 5, // in1, jmp
	JUMP_IF_FALSE = 6, // in1, jmp
	LESS_THAN = 7, // in1, in2, dest
	EQUALS = 8, // in1, in2, dest
	TERMINATION = 99,
} e_instruction;

void exec_program(t_program *program) {
	t_input input = {true, 0, 0};
	run_program(program, &input, true);
}

t_output *run_program(t_program *program, t_input *input, bool output_direct_mode) {
	// instruction pointer
	size_t pos = 0;
	// initial instruction
	int op_code = program->at[pos];

	t_output *output = NULL;
	if (!output_direct_mode) {
		output = malloc(sizeof(t_output) + sizeof(int[0]));
		output->direct_mode = output_direct_mode;
		output->pos = 0;
		output->len = 0;
	}

	forever {
		// amount for moving the instruction pointer
		int offset = 1;

		e_instruction instr = op_code % 100;
		// resolve argument values
		int arg1 = get_param(program, pos, 1);
		int arg2 = get_param(program, pos, 2);
		int arg3 = get_param(program, pos, 3);
		// direct values to be used for writing
		// NOTE: Parameters that an instruction writes to will never be in immediate mode
		size_t dest1 = program->at[pos + 1];
		size_t dest2 = program->at[pos + 2];
		size_t dest3 = program->at[pos + 3];

		//printf("[%ld] <%d> | [%ld]:%d, [%ld]:%d, [%ld]:%d\n", pos, instr, dest1, arg1, dest2, arg2, dest3, arg3);

		// handle instructions
		switch (instr) {
			case ADDITION:
				program->at[dest3] = arg1 + arg2;
				offset += 3;
				break;
			case MULTIPLICATION:
				program->at[dest3] = arg1 * arg2;
				offset += 3;
				break;
			case INPUT:
				if (input->direct_mode) {
					printf("Input a value: ");
					scanf("%d", &program->at[dest1]);
				} else {
					// read from input struct
					program->at[dest1] = input->data[input->pos];
					input->pos++;
				}
				offset += 1;
				break;
			case OUTPUT:
				if (output_direct_mode) {
					printf("%d\n", arg1);
				} else {
					// put data into output struct
					//printf("saving %d\n", arg1);
					output->len++;
					output = realloc(output, sizeof(t_output) + sizeof(int[output->len]));
					output->data[output->len - 1] = arg1;
				}
				offset += 1;
				break;
			case JUMP_IF_TRUE:
				if (arg1 != 0) {
					offset = 0;
					pos = arg2;
				} else {
					offset += 2;
				}
				break;
			case JUMP_IF_FALSE:
				if (arg1 == 0) {
					offset = 0;
					pos = arg2;
				} else {
					offset += 2;
				}
				break;
			case LESS_THAN:
				program->at[dest3] = (arg1 < arg2) ? 1 : 0;
				offset += 3;
				break;
			case EQUALS:
				program->at[dest3] = (arg1 == arg2) ? 1 : 0;
				offset += 3;
				break;
			case TERMINATION:
				return output; // stop execution
			default:
				fprintf(stderr, "Undefined op_code: %d (%d at %ld)\n", instr, op_code, pos);
		}

		// move instruction pointer
		pos += offset;
		op_code = program->at[pos];
	}
}