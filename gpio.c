#include "main.h"

void read_file(const char* file_name, void handle_line(char *, size_t, void *), void *state) {
	FILE *fp;
	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	fp = fopen(file_name, "r");
	if (fp == NULL) {
		fprintf(stderr, "Failed opening file '%s'\n", file_name);
	}

	while((read = getline(&line, &len, fp)) != -1) {
		handle_line(line, read, state);
	}

	fclose(fp);
	if (line) {
		free(line);
	}
}

#include <dirent.h>
#include <string.h>
void list_files(const char *dir_name) {
	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir (dir_name)) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir (dir)) != NULL) {
			printf ("%s\n", ent->d_name);
		}
		closedir (dir);
	} else {
		/* could not open directory */
		perror ("");
	}
}
