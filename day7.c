#include "main.h"

int run_amplifier(t_program *program_state, int phase_setting, int input_signal) {
	t_program *program = malloc(sizeof(t_program) + sizeof(int[program_state->len]));
	program->len = program_state->len;

	// initialise program
	memcpy(program->at, program_state->at, sizeof(int) * program->len);

	t_input *input = NULL;
	input = malloc(sizeof(t_input) + sizeof(int[2]));
	input->direct_mode = false;
	input->pos = 0;
	input->len = 2;
	input->data[0] = phase_setting;
	input->data[1] = input_signal;

	t_output *output = NULL;
	output = run_program(program, input, false);

	int output_signal = output->data[0];

	free(input);
	free(output);
	free(program);

	return output_signal;
}

void amp_out() {
	t_program *program_state = NULL;

	read_file("../input7", read_code, &program_state);

	t_program *program = malloc(sizeof(t_program) + sizeof(int[program_state->len]));
	program->len = program_state->len;

	// part1
	int max = INT_MIN;
	char phase_setting[10];

	for (int phase_a = 0; phase_a < 5; ++phase_a) {
		for (int phase_b = 0; phase_b < 5; ++phase_b) {
			if (phase_b == phase_a) {
				continue;
			}
			for (int phase_c = 0; phase_c < 5; ++phase_c) {
				if (phase_c == phase_a || phase_c == phase_b) {
					continue;
				}
				for (int phase_d = 0; phase_d < 5; ++phase_d) {
					if (phase_d == phase_c || phase_d == phase_b || phase_d == phase_a) {
						continue;
					}
					for (int phase_e = 0; phase_e < 5; ++phase_e) {
						if(phase_e == phase_d || phase_e == phase_c || phase_e == phase_b || phase_e == phase_a) {
							continue;
						}

						int out_a = run_amplifier(program_state, phase_a, 0);
						int out_b = run_amplifier(program_state, phase_b, out_a);
						int out_c = run_amplifier(program_state, phase_c, out_b);
						int out_d = run_amplifier(program_state, phase_d, out_c);
						int out_e = run_amplifier(program_state, phase_e, out_d);

						if (out_e > max) {
							max = out_e;
							sprintf(phase_setting, "%d,%d,%d,%d,%d", phase_a, phase_b, phase_c, phase_d, phase_e);
						}
					}
				}
			}
		}
	}

	printf("arg: %s for value: %d\n", phase_setting, max);

	free(program_state);
}