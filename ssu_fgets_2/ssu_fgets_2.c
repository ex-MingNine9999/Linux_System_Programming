#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "ssu_runtime.h"

#define BUFFER_MAX 256

int main(void)
{
	char command[BUFFER_MAX];
	char *prompt = "Prompt >>";

	gettimeofday(&begin_t, NULL);

	while (1) {
		fputs(prompt, stdout);

		if (fgets(command, sizeof(command), stdin) == NULL) {
			break;
		}

		system(command);
	}

	fprintf(stdout, "Good bye...\n");
	fflush(stdout);

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
