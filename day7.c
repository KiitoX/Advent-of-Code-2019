#include "main.h"

t_program *init_amplifier(t_program *program_state) {
	t_program *program = malloc(sizeof(t_program) + sizeof(int[program_state->len]));
	program->instruction_pointer = 0;
	program->len = program_state->len;

	// initialise program
	memcpy(program->at, program_state->at, sizeof(int) * program->len);

	return program;
}

t_output *run_amplifier(t_program *program, int input_value) {
	t_input *input = NULL;
	input = malloc(sizeof(t_input) + sizeof(int[1]));
	input->pos = 0;
	input->len = 1;
	input->data[0] = input_value;

	t_output *output = NULL;
	output = run_program(program, input, CACHED);

	free(input);

	return output;
}

void part1(t_program *program_state) {
	int max = INT_MIN;
	char phase_setting[10];

	// honestly I don't really know why I don't have to reset the amps for each different phase setting in this non-feedback loop case...

	t_program *amp_a = init_amplifier(program_state);
	t_program *amp_b = init_amplifier(program_state);
	t_program *amp_c = init_amplifier(program_state);
	t_program *amp_d = init_amplifier(program_state);
	t_program *amp_e = init_amplifier(program_state);

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

						// init phases
						t_output *out_a = run_amplifier(amp_a, phase_a);
						t_output *out_b = run_amplifier(amp_b, phase_b);
						t_output *out_c = run_amplifier(amp_c, phase_c);
						t_output *out_d = run_amplifier(amp_d, phase_d);
						t_output *out_e = run_amplifier(amp_e, phase_e);

						free(out_a);
						free(out_b);
						free(out_c);
						free(out_d);
						free(out_e);

						// amplify values
						out_a = run_amplifier(amp_a, 0);
						out_b = run_amplifier(amp_b, out_a->data[0]);
						out_c = run_amplifier(amp_c, out_b->data[0]);
						out_d = run_amplifier(amp_d, out_c->data[0]);
						out_e = run_amplifier(amp_e, out_d->data[0]);

						if (out_e->data[0] > max) {
							max = out_e->data[0];
							sprintf(phase_setting, "%d,%d,%d,%d,%d", phase_a, phase_b, phase_c, phase_d, phase_e);
						}

						free(out_a);
						free(out_b);
						free(out_c);
						free(out_d);
						free(out_e);
					}
				}
			}
		}
	}

	printf("arg: %s for value: %d\n", phase_setting, max);

	free(amp_a);
	free(amp_b);
	free(amp_c);
	free(amp_d);
	free(amp_e);
}

void part2(t_program *program_state) {
	int max = INT_MIN;
	char phase_setting[10];

	for (int phase_a = 5; phase_a < 10; ++phase_a) {
		for (int phase_b = 5; phase_b < 10; ++phase_b) {
			if (phase_b == phase_a) {
				continue;
			}
			for (int phase_c = 5; phase_c < 10; ++phase_c) {
				if (phase_c == phase_a || phase_c == phase_b) {
					continue;
				}
				for (int phase_d = 5; phase_d < 10; ++phase_d) {
					if (phase_d == phase_c || phase_d == phase_b || phase_d == phase_a) {
						continue;
					}
					for (int phase_e = 5; phase_e < 10; ++phase_e) {
						if(phase_e == phase_d || phase_e == phase_c || phase_e == phase_b || phase_e == phase_a) {
							continue;
						}

						t_program *amp_a = init_amplifier(program_state);
						t_program *amp_b = init_amplifier(program_state);
						t_program *amp_c = init_amplifier(program_state);
						t_program *amp_d = init_amplifier(program_state);
						t_program *amp_e = init_amplifier(program_state);

						// init phases
						t_output *out_a = run_amplifier(amp_a, phase_a);
						t_output *out_b = run_amplifier(amp_b, phase_b);
						t_output *out_c = run_amplifier(amp_c, phase_c);
						t_output *out_d = run_amplifier(amp_d, phase_d);
						t_output *out_e = run_amplifier(amp_e, phase_e);

						int input = 0;
						while (out_a->reason != HAS_FINISHED
							&& out_b->reason != HAS_FINISHED
					        && out_c->reason != HAS_FINISHED
					        && out_d->reason != HAS_FINISHED
					        && out_e->reason != HAS_FINISHED) { // while waiting for input

							free(out_a);
							free(out_b);
							free(out_c);
							free(out_d);
							free(out_e);

							// amplify values
							out_a = run_amplifier(amp_a, input);
							out_b = run_amplifier(amp_b, out_a->data[0]);
							out_c = run_amplifier(amp_c, out_b->data[0]);
							out_d = run_amplifier(amp_d, out_c->data[0]);
							out_e = run_amplifier(amp_e, out_d->data[0]);

							input = out_e->data[0];
						}

						if (out_e->data[0] > max) {
							max = out_e->data[0];
							sprintf(phase_setting, "%d,%d,%d,%d,%d", phase_a, phase_b, phase_c, phase_d, phase_e);
						}

						free(out_a);
						free(out_b);
						free(out_c);
						free(out_d);
						free(out_e);

						free(amp_a);
						free(amp_b);
						free(amp_c);
						free(amp_d);
						free(amp_e);
					}
				}
			}
		}
	}

	printf("arg: %s for max looped value: %d\n", phase_setting, max);
}

void amp_out() {
	t_program *program_state = NULL;

	read_file("../input7", read_code, &program_state);

	part1(program_state);

	part2(program_state);

	free(program_state);
}