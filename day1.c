#include "main.h"

typedef struct {
	long fuel;
} t_reqs;

void handle_mass(char *line, size_t len, void *state) {
	t_reqs *reqs = (t_reqs *)state;

	long mass = strtol(line, NULL, 10);
	//sscanf(line, "%ld", &mass);

	// part 1:
	long fuel = mass / 3 - 2;
	reqs->fuel += fuel;

	// part 2:
	long ff_sum = 0;
	long fuel_fuel = fuel / 3 - 2;
	while (fuel_fuel > 0) {
		ff_sum += fuel_fuel;
		reqs->fuel += fuel_fuel;
		fuel_fuel = fuel_fuel / 3 - 2;
	}

	printf("mass: %ld, fuel: %ld, ff_sum: %ld, total: %ld\n", mass, fuel, ff_sum, reqs->fuel);
}

void tally_up() {
	t_reqs reqs = {0};

	read_file("../input1", handle_mass, &reqs);

	printf("Required fuel: %ld", reqs.fuel);
}
