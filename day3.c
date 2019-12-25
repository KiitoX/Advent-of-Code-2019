#include "main.h"

typedef struct {
	int x;
	int y;
} t_point;

typedef struct {
	int len1;
	int len2;
	t_point *line1;
	t_point *line2;
} t_lines;

void handle_directions(char *line, size_t len, void *state) {
	t_lines *lines = (t_lines *)state;

	t_point content[(len + 1) / 3]; // array with maximum amount of points

	// start at origin;
	content[0].x = 0;
	content[0].y = 0;

	int matches;
	int index = 1;

	char direction;
	int amount;

	while ((matches = sscanf(line, "%c%d%*[,]%s", &direction, &amount, line))) {
		//printf("%d: %d (%s)\n", index, content[index], pos);
		if (direction == 'U') {
			content[index].x = content[index - 1].x;
			content[index].y = content[index - 1].y + amount;
		} else if (direction == 'D') {
			content[index].x = content[index - 1].x;
			content[index].y = content[index - 1].y - amount;
		} else if (direction == 'L') {
			content[index].x = content[index - 1].x - amount;
			content[index].y = content[index - 1].y;
		} else if (direction == 'R') {
			content[index].x = content[index - 1].x + amount;
			content[index].y = content[index - 1].y;
		} else {
			perror("Unexpected direction\n");
		}
		//printf("%d: %c%d -> (%d, %d) (%d)\n", index, direction, amount, content[index].x, content[index].y, matches);
		if (matches < 3) { // detect last match
			break;
		}
		index++;
	}

	size_t clen = index + 1;
	t_point *cline = NULL;
	if (lines->line1 == NULL) {
		lines->len1 = clen;
		lines->line1 = malloc(sizeof(t_point) * (lines->len1));
		cline = lines->line1;
	} else {
		lines->len2 = clen;
		lines->line2 = malloc(sizeof(t_point) * (lines->len2));
		cline = lines->line2;
	}

	memcpy(cline, content, sizeof(t_point) * clen);
}

void crisscross() {
	t_lines lines = {0, 0, NULL, NULL};

	read_file("../input3", handle_directions, &lines);

	size_t len = 150;
	int index = 0;
	t_point *crossings = calloc(len, sizeof(t_point));
	int *cross_time = malloc(sizeof(int) * len);

	printf("%d, %d\n", lines.len1, lines.len2);

	// part1:
	// find collisions
	for (int i = 1; i < lines.len1; ++i) {
		for (int j = 1; j < lines.len2; ++j) {
			t_point line1A = lines.line1[i - 1];
			t_point line1B = lines.line1[i];

			t_point line2A = lines.line2[j - 1];
			t_point line2B = lines.line2[j];

			// memory management, disabled in favour of constant length
			/*
			if (index >= len) {
				len += 50;
				printf("realloc: %ld, %ld\n", len, sizeof(t_point) * len);
				t_point *recross = realloc(crossings, sizeof(t_point) * len);
				if (recross) {
					crossings = recross;
				} else {
					perror("realloc failed");
				}
			}*/

			// for a crossing there must be a vertical
			t_point vLineA = {};
			t_point vLineB = {};
			// and a horizontal line
			t_point hLineA = {};
			t_point hLineB = {};

			bool has_crossing = false;

			if (line1A.x == line1B.x && line2A.y == line2B.y) { // line1 vertical and line2 horizontal
				vLineA = line1A;
				vLineB = line1B;
				hLineA = line2A;
				hLineB = line2B;
				has_crossing = true;
			} else if (line1A.y == line1B.y && line2A.x == line2B.x) { // line 1 horizontal and line2 vertical
				hLineA = line1A;
				hLineB = line1B;
				vLineA = line2A;
				vLineB = line2B;
				has_crossing = true;
			}

			// The following conditionals are extremely gory, continue on you own risk
			if (has_crossing) {
				// hLine in y-Area of vLine
				if ((vLineA.y <= hLineA.y && hLineA.y <= vLineB.y)
				    || (vLineA.y >= hLineA.y && hLineA.y >= vLineB.y)) {
					// vLine in x-Area of hLine
					if ((hLineA.x <= vLineA.x && vLineA.x <= hLineB.x)
					    || (hLineA.x >= vLineA.x && vLineA.x >= hLineB.x)) {
						crossings[index].x = vLineA.x;
						crossings[index].y = hLineA.y;
						//printf("%d: (%d, %d) %d\n", index, crossings[index].x, crossings[index].y, abs(crossings[index].x) + abs(crossings[index].y));
						index++;
					}
				}
			}
		}
	}

	// get minimum distance part1
	int dist1 = INT_MAX;

	for (int k = 0; k <= index; ++k) {
		t_point p = crossings[k];
		int d = abs(p.x) + abs(p.y);
		if (d > 0 && d < dist1) {
			dist1 = d;
			printf("(%d, %d) -> %d\n", p.x, p.y, d);
		}
	}

	printf("%d\n", dist1);

	// get minimum latency part2
	for (int k = 0; k <= index; ++k) {
		t_point p = crossings[k];

		int d1 = 0;
		int i = 0;
		t_point last = lines.line1[i++];

		while (i > 0) {
			t_point node = lines.line1[i++];
			// first add tiles up to current node
			int dist = abs(node.x - last.x) + abs(node.y - last.y);

			// check if the target is between last and current node
			if (last.x == node.x) { // vertical line
				if (last.x == p.x) { // we may hit our target
					if ((last.y <= p.y && p.y <= node.y)
						|| (last.y >= p.y && p.y >= node.y)) { // we hit our target
						dist = abs(p.x - last.x) + abs(p.y - last.y);
						i = 0;
					}
				}
			} else if (last.y == node.y) { // horizontal line
				if (last.y == p.y) { // we may hit the target
					if ((last.x <= p.x && p.x <= node.x)
						|| (last.x >= p.x && p.x >= node.x)) { // we hit our target
						dist = abs(p.x - last.x) + abs(p.y - last.y);
						i = 0;
					}
				}
			}
			last = node;
			d1 += dist;
		}

		int d2 = 0;
		last = lines.line2[i++];
		while (i > 0) {
			t_point node = lines.line2[i++];
			// first add tiles up to current node
			int dist = abs(node.x - last.x) + abs(node.y - last.y);

			// check if the target is between last and current node
			if (last.x == node.x) { // vertical line
				if (last.x == p.x) { // we may hit our target
					if ((last.y <= p.y && p.y <= node.y)
					    || (last.y >= p.y && p.y >= node.y)) { // we hit our target
						dist = abs(p.x - last.x) + abs(p.y - last.y);
						i = 0;
					}
				}
			} else if (last.y == node.y) { // horizontal line
				if (last.y == p.y) { // we may hit the target
					if ((last.x <= p.x && p.x <= node.x)
					    || (last.x >= p.x && p.x >= node.x)) { // we hit our target
						dist = abs(p.x - last.x) + abs(p.y - last.y);
						i = 0;
					}
				}
			}
			last = node;
			d2 += dist;
		}

		printf("d1: %d, d2: %d, -> %d\n", d1, d2, d1+d2);
		cross_time[k] = d1 + d2;
	}

	int dist2 = INT_MAX;

	for (int k = 0; k <= index; ++k) {
		t_point p = crossings[k];
		int t = cross_time[k];
		if (t > 0 && t < dist2) {
			dist2 = t;
			printf("(%d, %d) -> %d\n", p.x, p.y, t);
		}
	}

	printf("%d\n", dist2);

	free(crossings);
	free(cross_time);

	free(lines.line1);
	free(lines.line2);
}
