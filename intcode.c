#include "main.h"

// expecting a pointer to a (t_program *) which will be malloc'ed accordingly
void read_code(char *line, size_t len, void *state) {
	t_program **program_pointer = (t_program **)state;
	size_t max_program_length = (len + 1) / 2; // maximum amount of numbers when input is in format 1,2,3,4...
	int64_t content[max_program_length]; // array buffer to hold parsing result

	int matches = 0;
	int index = 0;
	while ((matches = sscanf(line, "%ld%*[,]%s", &(content[index]), line))) {
		//printf("%d: %ld (%s) -> %d\n", index, content[index], line, matches);
		if (matches < 2) { // detect last match
			break;
		}
		index++;
	}

	size_t program_length = index + 1;
	// since day8 we need more memory than the program needs itself
	size_t program_memory_size = program_length + 102400;

	*program_pointer = malloc(sizeof(t_program) + sizeof(int64_t[program_memory_size]));
	t_program *program = *program_pointer;

	// initialise program memory to 0
	memset(program->at, 0, sizeof(int64_t[program_memory_size]));

	program->instruction_pointer = 0;
	program->relative_base = 0;
	program->mem_size = program_memory_size;

	for (size_t i = 0; i < program_length; ++i) {
		program->at[i] = content[i];
	}
}

void print_state(t_program *program, bool stop_early) {
	int count_zeroes = 0;

	for (size_t i = 0; i < program->mem_size; i++) {
		int c = program->at[i];
		if (stop_early) {
			if (c == 0) {
				count_zeroes++;
			} else {
				count_zeroes = 0;
			}
			if (count_zeroes > 10) {
				break;
			}
		}

		if (i > 0) {
			putchar(',');
		}
		if (i % 80 == 0) {
			printf("\n⮑ ");
		}
		printf("%d", c);
	}
	putchar('\n');
}

typedef enum {
	POSITION = 0, // parameters are pointers
	IMMEDIATE = 1, // parameters are literals
	RELATIVE = 2, // parameters are pointers relative to a base address
} e_parameter_mode;

int64_t get_param(t_program *program, size_t pos, unsigned off, bool dest) {
	unsigned op_code = program->at[pos];

	// op_code: ABCDE => A: pmode3, B: pmode2, C: pmode1, DE: instruction
	e_parameter_mode pmode = (op_code / (int)pow(10, 1 + off)) % 10;

	int64_t argument = 0; // fallback value, since we always try to retrieve 3 args, even when we don't need to, or can't
	if ((pos + off) < program->mem_size) {
		argument = program->at[pos + off];
	}

	//printf("(%ld + %d) -> %d w/ %d\n", pos, off, argument, pmode);

	switch (pmode) {
		case RELATIVE:
			argument += program->relative_base;
		case POSITION:
			if (!dest) { // destination arguments will never want destination resolution
				if(argument < program->mem_size) { // another fallback for wild numbers, to avoid segfaults
					return program->at[argument];
				} else {
					//fprintf(stderr, "Illegal address <%ld> in POSITION mode (%ld)\n", argument, program->mem_size);
					return 0;
				}
			}
		case IMMEDIATE:
			return argument;
		default:
			fprintf(stderr, "Invalid pmode: %d (%d at %ld)\n", pmode, op_code, pos);
	}
}

void exec_program(t_program *program) {
	run_program(program, NULL, DIRECT);
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
	MOVE_REL_BASE = 9, // in1
	TERMINATION = 99,
} e_instruction;

t_output *run_program(t_program *program, t_input *input, e_io_mode io_mode) {
	// instruction pointer
	size_t pos = program->instruction_pointer;
	// initial instruction
	unsigned op_code = program->at[pos];

	t_output *output = NULL;
	if (io_mode == CACHED) {
		output = malloc(sizeof(t_output) + sizeof(int[0]));
		output->pos = 0;
		output->len = 0;
	}

	bool stop = false;
	while (!stop) {
		// amount for moving the instruction pointer
		int offset = 1;

		e_instruction instr = op_code % 100;

		// resolve argument values
		int64_t arg1 = get_param(program, pos, 1, false);
		int64_t arg2 = get_param(program, pos, 2, false);
		int64_t arg3 = get_param(program, pos, 3, false);
		// direct values to be used for writing
		// NOTE: Parameters that an instruction writes to will never be in immediate mode
		size_t dest1 = get_param(program, pos, 1, true);
		size_t dest2 = get_param(program, pos, 2, true);
		size_t dest3 = get_param(program, pos, 3, true);

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
				switch (io_mode) {
					case DIRECT:
						printf("Input a value: ");
						scanf("%ld", &program->at[dest1]);
						break;
					case CACHED:
						if (input->pos < input->len) {
							// read from input struct
							program->at[dest1] = input->data[input->pos];
							input->pos++;
						} else {
							// we don't have enough input, wait for more
							// do not include skip for this instruction,
							// as we want to rerun it with new input
							program->instruction_pointer = pos;
							output->reason = NEEDS_INPUT;
							stop = true;
						}
						break;
					default:
						fprintf(stderr, "Invalid io_mode: %d\n", io_mode);
				}
				offset += 1;
				break;
			case OUTPUT:
				switch (io_mode) {
					case DIRECT:
						printf("%ld\n", arg1);
						break;
					case CACHED:
						// put data into output struct
						//printf("saving %d\n", arg1);
						output->len++;
						output = realloc(output, sizeof(t_output) + sizeof(int64_t[output->len]));
						output->data[output->len - 1] = arg1;
						break;
					default:
						fprintf(stderr, "Invalid io_mode: %d\n", io_mode);
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
			case MOVE_REL_BASE:
				program->relative_base += arg1;
				offset += 1;
				break;
			case TERMINATION:
				program->instruction_pointer = 0;
				if (output != NULL) {
					output->reason = HAS_FINISHED;
				}
				stop = true; // stop execution
				break;
			default:
				fprintf(stderr, "Undefined op_code: %d (%d at %ld)\n", instr, op_code, pos);
		}

		// move instruction pointer
		pos += offset;
		op_code = program->at[pos];
	}

	return output;
}