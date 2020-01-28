#include "main.h"

typedef struct {
	int width;
	int height;
	int asteroids; // amount of asteroids
	int points[];
} t_asteroid_map;

typedef struct {
	int x, y;
} t_point;

typedef struct {
	union {
		t_point pt;
		struct {
			int x, y;
		};
	};
	double angle;
	double dist;
	bool destroyed;
} t_asteroid;

double dist(t_point p0, t_point p1) {
	double dx = p0.x - p1.x;
	double dy = p0.y - p1.y;
	return sqrt(dx * dx + dy * dy);
}

double angle(t_point p0, t_point p1) {
	// guaranteed to be in range (-pi, pi]
	return atan2(p0.y - p1.y, p0.x - p1.x);
}

int compare_asteroids(const void *_a0, const void *_a1) {
	t_asteroid a0 = *(t_asteroid *)_a0;
	t_asteroid a1 = *(t_asteroid *)_a1;

	// sort first by angle
	if (a0.angle < a1.angle) {
		return -1; // a0 should be put before a1
	} else if (a0.angle > a1.angle) {
		return 1; // a0 should be put after a1
	} else {
		// then by distance, if necessary
		if (a0.dist < a1.dist) {
			return -1; // a0 should be put before a1
		} else if (a0.dist > a1.dist) {
			return 1; // a0 should be put after a1
		} else {
			return 0;
		}
	}
}

void load_asteroid_map(char *line, size_t len, void *state) {
	t_asteroid_map **asteroid_map_pointer = (t_asteroid_map **)state;

	t_asteroid_map *asteroid_map = *asteroid_map_pointer;
	if (asteroid_map->width + 1 < len) {
		// init map width (- newline), we expect this to stay the same
		asteroid_map->width = (int)len - 1;
	} else if (len < asteroid_map->width) {
		// skip too short lines
		return;
	}
	// add one horizontal line to map
	asteroid_map->height++;
	size_t num_points = asteroid_map->width * asteroid_map->height;

	*asteroid_map_pointer = realloc(asteroid_map, sizeof(t_asteroid_map) + sizeof(int[num_points]));
	asteroid_map = *asteroid_map_pointer;

	// go over each character to convert and store it
	for (int i = 0; i < asteroid_map->width; ++i) {
		char c = line[i];
		int val = c == '#'; // 1 if '#' else 0
		asteroid_map->asteroids += val;

		int index = (asteroid_map->height - 1) * asteroid_map->width + i;
		asteroid_map->points[index] = val;
	}
}

void watch() {
	t_asteroid_map *asteroid_map = NULL;
	asteroid_map = malloc(sizeof(t_asteroid_map) + sizeof(int[0]));
	asteroid_map->width = 0;
	asteroid_map->height = 0;
	asteroid_map->asteroids = 0;

	read_file("../input10", load_asteroid_map, &asteroid_map);

	t_point asteroids[asteroid_map->asteroids];

	int u = 0;
	for (int y = 0; y < asteroid_map->height; ++y) {
		for (int x = 0; x < asteroid_map->width; ++x) {
			int index = y * asteroid_map->width + x;
			int point = asteroid_map->points[index];
			if (point == 1) {
				t_point p = {x, y};
				asteroids[u] = p;
				u++;
			}
		}
	}

	// part1

	int max_visible_asteroids = -1;
	int max_visible_index = -1;

	for (int i = 0; i < asteroid_map->asteroids; ++i) {
		// this point is being considered as a candidate for the monitoring station
		t_point p0 = asteroids[i];
		int visible_asteroids = 0;
		for (int j = 0; j < asteroid_map->asteroids; ++j) {
			// do not compare point to itself
			if (i != j) {
				// we want to check if this point is visible from p0
				t_point p1 = asteroids[j];

				bool blocked = false;
				// check if any of the other asteroids are blocking line of sight
				for (int k = 0; k < asteroid_map->asteroids; ++k) {
					// exclude the two points we're considering
					if (i != k && j != k) {
						t_point p2 = asteroids[k];
						// get direction angle of p1 and p2
						double angle1 = round(angle(p0, p1) * 1000);
						double angle2 = round(angle(p0, p2) * 1000);
						// check if p2 is on the same line of sight as p1
						if (angle1 == angle2) {
							double dist1 = dist(p0, p1);
							double dist2 = dist(p0, p2);
							if (dist2 < dist1) {
								// p2 is on line of sight, and closer, so p1 is not visible
								//printf("(%d, %d) -> (%d, %d) is blocked by (%d, %d) [d1: %f, d2: %f]\n", p0.x, p0.y, p1.x, p1.y, p2.x, p2.y, dist1, dist2);
								blocked = true;
								break;
							}
						}
					}
				}
				if (!blocked) {
					visible_asteroids++;
				}
			}
		}

		if (visible_asteroids > max_visible_asteroids) {
			max_visible_asteroids = visible_asteroids;
			max_visible_index = i;
		}
	}

	t_point max = asteroids[max_visible_index];
	printf("best location: (%d, %d) [%d] with %d visible asteroids\n", max.x, max.y, max_visible_index, max_visible_asteroids);

	// part2

	// all asteroids excluding the pivot
	t_asteroid asteroids_by_angle[asteroid_map->asteroids];

	printf("mapping...\n");
	for (int l = 0; l < asteroid_map->asteroids; ++l) {
		t_point pt = asteroids[l];
		// rotate 90deg anti-clockwise to start at 12 o'clock
		double a = angle(max, pt) - M_PI/2;
		// bring angle in range [0, 2pi)
		a = fmod(a + 2 * M_PI, 2 * M_PI);
		// transform to deg [0, 360)
		a = a / M_PI * 180;

		asteroids_by_angle[l].pt = pt;
		asteroids_by_angle[l].angle = a;
		asteroids_by_angle[l].dist = dist(max, pt);
		asteroids_by_angle[l].destroyed = false;
	}

	printf("sorting...\n");
	qsort(asteroids_by_angle, asteroid_map->asteroids, sizeof(t_asteroid), compare_asteroids);

	/*
	for (int i = 0; i < asteroid_map->asteroids; ++i) {
		if (i > 0) {
			printf(", ");
		}
		t_asteroid *as = &asteroids_by_angle[i];
		printf("(%d, %d)", as->x, as->y);
	}
	printf("\n");
	*/

	printf("zapping...\n");
	int destruction_index = -1;

	bool asteroids_left = true;
	while (asteroids_left) {
		bool have_destroyed_some = false;
		double last_angle = -1;
		for (int m = 0; m < asteroid_map->asteroids; ++m) {
			t_asteroid *as = &asteroids_by_angle[m];

			// skip centre
			if (as->x == max.x && as->y == max.y) {
				continue;
			}

			if (!as->destroyed && as->angle > last_angle) {
				as->destroyed = true;
				destruction_index++;
				have_destroyed_some = true;
				last_angle = as->angle;

				printf("%ith: (%d, %d)\n", destruction_index + 1, as->x, as->y);
			}
		}
		if (!have_destroyed_some) {
			asteroids_left = false;
		}
	}

	free(asteroid_map);
}
