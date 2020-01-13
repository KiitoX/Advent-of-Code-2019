#include "main.h"

typedef struct {
	int num_pixels;
	int pixels[];
} t_image_layer ;

typedef struct {
	int width;
	int height;
	int num_layers;
	t_image_layer *layers[];
} t_space_image;

void load_space_image(char *line, size_t len, void *state) {
	t_space_image **image_pointer = (t_space_image **)state;

	t_space_image *image = *image_pointer;
	int num_pixels = image->width * image->height;

	*image_pointer = realloc(image, sizeof(t_space_image) + sizeof(typeof(t_image_layer *)[num_pixels]));
	image = *image_pointer;
	image->num_layers = (int)(len / num_pixels);

	//printf("nums: %ld, pixels: %d, layers: %d\n", mem_size, num_pixels, image->num_layers);

	// make enough space for all layers
	// allocate memory for each layer
	for (int i = 0; i < image->num_layers; ++i) {
		image->layers[i] = malloc(sizeof(t_image_layer) + sizeof(int[num_pixels]));
		image->layers[i]->num_pixels = num_pixels;
	}

	int current_layer = 0;
	int current_pixel = 0;

	// go over each character to convert and store it
	for (int j = 0; j < len; ++j) {
		char c = line[j];
		int val = c - '0';
		if (val < 0 || val > 9) {
			// character out of bounds, we expect this to happen at the newline
			break;
		}
		image->layers[current_layer]->pixels[current_pixel] = val;

		// advance position counter
		current_pixel++;
		if (current_pixel >= num_pixels) {
			current_pixel = 0;
			current_layer++;
		}
	}
}

void imagine() {
	t_space_image *image = NULL;
	image = malloc(sizeof(t_space_image) + sizeof(typeof(t_image_layer *)[0]));
	image->width = 25;
	image->height = 6;
	image->num_layers = 0;

	read_file("../input8", load_space_image, &image);

	// part1
	// count zeroes on each layer
	int min_zeroes = INT_MAX;
	int ones = -1;
	int twos = -1;
	int layer_index = -1;

	for (int i = 0; i < image->num_layers; ++i) {
		t_image_layer *layer = image->layers[i];
		int zero_count = 0;
		int one_count = 0;
		int two_count = 0;
		for (int j = 0; j < layer->num_pixels; ++j) {
			int px = layer->pixels[j];
			if (px == 0) {
				zero_count++;
			} else if (px == 1) {
				one_count++;
			} else if (px == 2) {
				two_count++;
			}
		}
		//printf("layer: %d with %d zeroes, and 1: %d, 2: %d as prod: %d\n", i, zero_count, one_count, two_count, one_count*two_count);

		if (zero_count < min_zeroes) {
			min_zeroes = zero_count;
			ones = one_count;
			twos = two_count;
			layer_index = i;
		}
	}

	printf("min zero layer: %d with %d zeroes, and 1: %d, 2: %d and their product: %d\n", layer_index, min_zeroes, ones, twos, ones*twos);

	// part2
	t_image_layer *lowest_layer = image->layers[image->num_layers - 1];
	t_image_layer *sum_layer = NULL;
	sum_layer = malloc(sizeof(t_image_layer) + sizeof(int[lowest_layer->num_pixels]));

	// init layer
	for (int y = 0; y < image->height; ++y) {
		for (int x = 0; x < image->width; ++x) {
			sum_layer->pixels[x + y*image->width] = 2;
		}
	}

	for (int i = 0; i < image->num_layers; ++i) {
		t_image_layer *layer = image->layers[i];
		for (int j = 0; j < layer->num_pixels; ++j) {
			int px = layer->pixels[j];
			int dest = sum_layer->pixels[j];

			if (dest == 2 && px != 2) { // two is transparent
				sum_layer->pixels[j] = px;
			}
		}
	}

	// print result layer
	for (int y = 0; y < image->height; ++y) {
		for (int x = 0; x < image->width; ++x) {
			printf("%s", sum_layer->pixels[x + y*image->width] == 0 ? "  " : "██");
		}
		printf("\n");
	}

	// free layers
	for (int i = 0; i < image->num_layers; ++i) {
		free(image->layers[i]);
	}

	free(sum_layer);

	free(image);
}
