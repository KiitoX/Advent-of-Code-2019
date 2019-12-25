#include "main.h"

typedef struct t_node {
	char id[4];
	char parent_id[4];
	struct t_node *parent;
	size_t len_children;
	struct t_node **children;
} t_node;

typedef struct {
	t_node *com; // centre of mass
	size_t len;
	t_node **nodes;
} t_orbit_map;

t_node *find_node(t_orbit_map *orbit_map, char *id) {
	for (int i = 0; i < orbit_map->len; ++i) {
		if (strcmp(orbit_map->nodes[i]->id, id) == 0) {
			return orbit_map->nodes[i];
		}
	}
	return NULL;
}

void handle_relations(char *line, size_t len, void *state) {
	t_orbit_map *orbit_map = (t_orbit_map *)state;

	// three characters and terminating \0
	char parent[4];
	char identifier[4];

	sscanf(line, "%3[^)])%3s", parent, identifier);

	t_node *node = find_node(orbit_map, identifier);
	if (node == NULL) {
		node = malloc(sizeof(t_node));
		memcpy(node->id, identifier, sizeof(identifier));
		memcpy(node->parent_id, parent, sizeof(parent));
		node->len_children = 0;
		node->children = NULL;
		node->parent = NULL;
	}

	orbit_map->nodes = reallocarray(orbit_map->nodes, orbit_map->len++, sizeof(t_node));
	orbit_map->nodes[orbit_map->len - 1] = node;

	//printf("%s orbits %s\n", identifier, parent);
}

void link_relations(t_orbit_map *orbit_map) {
	for (int i = 0; i < orbit_map->len; ++i) {
		t_node *node = orbit_map->nodes[i];

		if (node->parent == NULL && node != orbit_map->com) {
			node->parent = find_node(orbit_map, node->parent_id);
			if (node->parent != NULL) {
				// we found our parent, please register
				node->parent->children = realloc(node->parent->children,
						sizeof(t_node) * node->parent->len_children++);
				node->parent->children[node->parent->len_children - 1] = node;
			} else {
				perror("well, this shouldn't happen...\n");
				printf("sumthin happened: %s\n", node->parent_id);
			}
		}
	}
}

int count_to_paren(t_node *node, t_node *paren) {
	int jumps = 0;

	while (node != paren) {
		jumps++;
		//printf("Going from %s to %s\n", node->id, node->parent->id);
		node = node->parent;
		if (node == NULL) {
			return -1; // we reached COM, without reaching paren
		}
	}

	return jumps;
}

void count_orbits(t_orbit_map *orbit_map) {
	int sum = 0;
	for (int i = 0; i < orbit_map->len; ++i) {
		t_node *node = orbit_map->nodes[i];
		int jumps = count_to_paren(node, orbit_map->com);
		sum += jumps;
	}

	printf("total orbits: %d\n", sum);
}

void fill_path(t_orbit_map *orbit_map, t_node **path, size_t len, t_node *tail) {
	t_node *node = tail;
	for (int i = len; i > 0; --i) {
		path[i - 1] = node;
		node = node->parent;
	}
}

t_node *find_common_parent(t_orbit_map *orbit_map, t_node *node_a, t_node *node_b) {
	int to_a = count_to_paren(node_a, orbit_map->com);
	int to_b = count_to_paren(node_b, orbit_map->com);

	t_node *path_to_a[to_a];
	fill_path(orbit_map, path_to_a, to_a, node_a);
	t_node *path_to_b[to_b];
	fill_path(orbit_map, path_to_b, to_b, node_b);

	printf("%s is %d away from COM\n", node_a->id, to_a);
	printf("%s is %d away from COM\n", node_b->id, to_b);

	t_node *common_ancestor = orbit_map->com;

	for (int i = 0; i < to_a && i < to_b; ++i) {
		t_node *node_to_a = path_to_a[i];
		t_node *node_to_b = path_to_b[i];
		if (node_to_a == node_to_b) {
			common_ancestor = node_to_a;
			printf("%s > ", common_ancestor->id);
		} else {
			printf("path diverges to %s and %s\n", node_to_a->id, node_to_b->id);
			break;
		}
	}

	return common_ancestor;
}

void calc_orbits() {
	t_node com = {"COM", "---", NULL, 0, NULL};

	t_orbit_map orbit_map = {&com, 1, NULL};
	orbit_map.nodes = reallocarray(orbit_map.nodes, orbit_map.len, sizeof(t_node));
	orbit_map.nodes[0] = &com;

	read_file("../input6", handle_relations, &orbit_map);

	link_relations(&orbit_map);

	// part1
	count_orbits(&orbit_map);

	// part2
	t_node *you = find_node(&orbit_map, "YOU");
	t_node *san = find_node(&orbit_map, "SAN");

	printf("%s orbits %s, and %s orbits %s\n", you->id, you->parent_id, san->id, san->parent_id);

	// find common parent
	t_node *parent = find_common_parent(&orbit_map, you, san);

	int to_you = count_to_paren(you->parent, parent);
	int to_san = count_to_paren(san->parent, parent);

	printf("%s is %d away from %s\n", you->id, to_you, parent->id);
	printf("%s is %d away from %s\n", san->id, to_san, parent->id);

	printf("total distance: %d\n", to_you + to_san);
}