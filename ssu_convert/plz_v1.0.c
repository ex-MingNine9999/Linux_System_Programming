#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include "ssu_runtime.h"
#include "ssu_convert.h"

char filename[PATH_MAX];
char cfile[OBJ_MAX][PATH_MAX];
int cfunc[OBJ_MAX][FUNC_MAX];
int optFlag[6];

int main(int argc, char *argv[])
{
	gettimeofday(&begin_t, NULL);

	optionHandling(argc, argv);

	convert();

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}

void optionHandling(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "Usage : %s <filename> <options> ..\n", argv[0]);
		exit(1);
	}

	strcpy(filename, argv[1]);

	int opt;

	while ((opt = getopt(argc - 1, argv + 1, OPTION_DEF)) > 0) {
		switch (opt) {
			case 'j' :
				optFlag[OPT_J] = 1;
				break;
			case 'c' :
				optFlag[OPT_C] = 1;
				break;
			case 'p' :
				optFlag[OPT_P] = 1;
				break;
			case 'f' :
				optFlag[OPT_F] = 1;
				break;
			case 'l' :
				optFlag[OPT_L] = 1;
				break;
			case 'r' :
				optFlag[OPT_R] = 1;
				break;
		}
	}

	return;
}

void convert(void)
{
	int javaDes;
	int cDes;
	char buf[BUFLEN];
	char oneLineBuf[BUFLEN];
	char *cp;
	int obj = 0;
	int i, j = 0, len;
	int cb = 0, check = 0;
	int checkFunc = 0;
	int checkMain = 0;

	if ((javaDes = open(filename, O_RDONLY)) < 0) {
		fprintf(stderr, "open error for %s\n", filename);
		exit(1);
	}

	while ((len = read(javaDes, buf, BUFLEN - 1)) > 0) {
		buf[len] = 0;
		i = 0;

		while (1) {

			do {
				oneLineBuf[j] = buf[i++];

				if (i >= len) {
					break;
				}

				j++;

			} while (strchr(LINE_END, oneLineBuf[j - 1]) == NULL); 

			if (buf[i] == 0) {
				break;
			}

			oneLineBuf[j] = 0;

			if (oneLineBuf[j - 1] == '{') {
				cb++;
				if (check == 0) {
					check = 1;
				}
			}
			else if (oneLineBuf[j - 1] == '}') {
				cb--;
				if (cb == 1) {
					if (checkMain) {
						write(cDes, "\treturn 0;\n", 11);
						checkMain = 0;	
					}

					checkFunc = 0;
				}


				if (cb > 0) {
					insertTab(cDes, cb);
					write(cDes, "}\n", 2);
				}
				else {
					check = 0;
					close(cDes);
					continue;
				}
			}

			switch (oneLineConvert(oneLineBuf, checkMain)) {
				case 0 :
					break;
				case 1 :
					strcpy(cfile[obj++], oneLineBuf);

					strcat(oneLineBuf, ".c");

					if ((cDes = open(oneLineBuf, O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0) {
						fprintf(stderr, "creat error for %s\n", oneLineBuf);
						exit(1);
					}
					write(cDes, "\n", 1);


					break;
				case 2 :
					write(cDes, oneLineBuf, strlen(oneLineBuf));
					checkFunc = 1;
					checkMain = 1;
					break;

			}

			while (buf[i] == ' ' || buf[i] == '\t') {
				i++;
			}
			if (buf[i] == '\n') {
				i++;
			}

			j = 0;
		}
	}

}

int oneLineConvert(char oneLineBuf[BUFLEN], int checkFunc)
{
	char buf[BUFLEN];
	char *cp;
	int i;

	if (strncmp(oneLineBuf, "import", 6) == 0) {		// import 출현 시
		return 0;
	}
	else {
		for (i = 0; i < ACMD_NUM; i++) {
			if (strstr(oneLineBuf, AccessModifier[i]) != NULL) {
				strcpy(buf, oneLineBuf + strlen(AccessModifier[i] ) + 1);
				break;
			}
		}

		if (i == ACMD_NUM) {
			strcpy(buf, oneLineBuf);
		}

		memset(oneLineBuf, 0, BUFLEN);
	}


	if (checkFunc == 0) {
		if ((cp = strstr(buf, "class")) != NULL) {		// class 출현 시
			cp += 6;

			i = 0;
			while (*cp != '{' && *cp != ' ') {
				oneLineBuf[i++] = *cp;
				cp += 1;
			}
			oneLineBuf[i] = 0;

			for (i = 0; oneLineBuf[i] != 0; i++) {
				if (oneLineBuf[i] == ' ') {
					for (int j = i + 1; oneLineBuf[j - 1] != 0; j++) {
						oneLineBuf[j - 1] = oneLineBuf[j];
					}
					i--;
				}
			}

			return 1;
		}

		if (strstr(buf, "main") != NULL) {				// main 출현 시
			strcpy(oneLineBuf, "int main(int argc, char *argv[])\n{\n");
			return 2;
		}



	}
	else {

	}

	return 0;
}

void insertTab(int cDes, int cb)
{
	char buf[BUFLEN] = {0};

	for (int i = 1; i < cb; i++) {
		strcat(buf, "\t");
	}

	write(cDes, buf, cb - 1);
}

void convertDataDec(char *buf)
{
	char dataName[NAME_MAX];
	int dataTypeNum;
	int i;

	for (int i = 0; i < DATA_MAX; i++) {
		if (strstr(buf, DataType[i]) != NULL) {
			dataTypeNum = i;
			break;
		}
	}

	if (strstr(buf, "[]") != NULL) {

	}
}
