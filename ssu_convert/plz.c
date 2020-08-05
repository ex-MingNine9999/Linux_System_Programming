#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "ssu_runtime.h"
#include "ssu_convert.h"

char filename[PATH_MAX];					// 인자로 들어온 자바 파일의 이름 저장
char cFile[OBJ_MAX][PATH_MAX];				// c파일들의 이름이 저장되어 있는 배열
int NumOfcFile;								// c파일의 개수
char javaFunc[FUNC_MAX][NAME_MAX];			// 자바의 함수들이 저장 되어 있는 배열
char cFunc[FUNC_MAX][NAME_MAX];				// 대응되는 C의 함수들이 저장 되어 있는 배열
char headerMap[HEADER_MAX][NAME_MAX];		// 헤더를 정의해둔 배열
int headerMapping[HEADER_MAX];				// 사용 되는 헤더를 저장해두는 배열
int funcMapping[FUNC_MAX][10];				// 함수에 필요한 헤더의 번호가 저장되어 있는 배열
int usedFunc[FUNC_MAX];						// 사용된 함수를 저장해둔 배열
int NumOfFunc;								// 헤더 테이블에 정의 되어있는 함수의 총 개수
int NumOfHeader;							// 헤더 테이블에 정의 되어있는 헤더의 총 개수
int NumOfUsed;								// 사용된 함수의 개수
char userHeaderTable[DEF_MAX][BUFLEN];		// 사용자 헤더파일
int NumOfUsedH;								// 사용자 헤더파일의 개수
char defineMap[DEF_MAX][BUFLEN];			// 전처리문을 저장할 배열
int NumOfDef;                               // 전처리문의 개수
int optFlag[OPT_MAX];                       // 옵션 Flag
char classVar[OBJ_MAX][VARLEN];             // 하나의 클래스안에 존재하는 변수들
int NumOfVar;                               // 클래스안에 존재하는 변수의 개수
char fdTable[OBJ_MAX][VARLEN];              // 파일 입출력의 예외를 위한 파일디스크립터 변수 저장
int NumOfFd;                                // 파일디스크립터의 개수

int main(int argc, char *argv[])
{
	gettimeofday(&begin_t, NULL);

	optionHandling(argc, argv);
	initializing();

	convert();
	printf("\n%s convert Success!\n", filename);

	optionProcessing();
	makeMakefile();

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

	strcpy(filename, argv[1]);                                      // 첫 인자를 변환할 파일로 전역변수에 저장

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
			default :
				fprintf(stderr, "option error for %c\n", optopt);	
				exit(1);
		}
	}

	return;
}

void setFuncTable(void)
{
	int i = 0; 

	strcpy(javaFunc[i], "printf");
	strcpy(cFunc[i], "printf");
	i++;

	strcpy(javaFunc[i], "nextInt");
	strcpy(cFunc[i], "scanf");
	i++;

	strcpy(javaFunc[i], "FileWriter");
	strcpy(cFunc[i], "open");
	i++;

	strcpy(javaFunc[i], "write");
	strcpy(cFunc[i], "write");
	i++;

	strcpy(javaFunc[i], "flush");
	strcpy(cFunc[i], "fsync");
	i++;

	strcpy(javaFunc[i], "close");
	strcpy(cFunc[i], "close");
	i++;

	strcpy(javaFunc[i], "new");
	strcpy(cFunc[i], "calloc");
	i++;

	strcpy(javaFunc[i], "IOException");
	strcpy(cFunc[i], "exit");
	i++;

	NumOfFunc = i;
}

void setHeaderTable(void)
{
	char buf[BUFLEN];
	char func[NAME_MAX];
	char head[NAME_MAX];
	int fd;
	int funcNum;
	int fc = 1, hc = 0;
	int len;
	int i, j, k, h;

	if ((fd = open(HEADER_TABLE, O_RDONLY)) < 0) {                  // 헤더파일이 정의 되어있는 파일을 open()
		fprintf(stderr, "open error for %s\n", HEADER_TABLE);
		exit(1);
	}

	while ((len = read(fd, buf, BUFLEN - 1)) > 0) {                 // 한 굴자씩 파싱하여 헤더 테이블에 저장
		buf[len] = 0;

		i = 0, j = 0;

		while (i < len) {
			while (buf[i] == '\n' || buf[i] == ' ') {
				i++;
			}

			if (fc == 1) {
				while (buf[i] != ' ' && i < len) {
					func[j] = buf[i];

					i++, j++;
				}

				if (i == len) {
					break;
				}

				func[j] = 0;

				fc = 0;
				hc = 1;
				h = 0;
				j = 0;
			}

			if (hc == 1) {
				for (k = 0; k < NumOfFunc; k++) {
					if (strcmp(cFunc[k],func) == 0) {
						funcNum = k;
						break;
					}
				}

				if (k == NumOfFunc) {
					fprintf(stderr, "Don't find C function for %s\n", func);
					exit(1);
				}

				while (buf[i] != '\n' && i < len) {
					if (buf[i] == ' ') {
						i++;
						continue;
					}

					if (buf[i] == '<') {
						head[j++] = ' ';
					}

					head[j] = buf[i];

					if (head[j] == '>') {
						head[j + 1] = 0;

						for (j = 0; j < NumOfHeader; j++) {
							if (strcmp(headerMap[j], head) == 0) {
								funcMapping[funcNum][h++] = j;
								break;
							}
						}

						if (j == NumOfHeader) {
							strcpy(headerMap[j], head);
							funcMapping[funcNum][h++] = j;
							NumOfHeader++;
						}

						j = -1;
					}

					j++;
					i++;
				}

				if (i == len) {
					break;
				}

				fc = 1;
				hc = 0;
			}
		}
	}

	close(fd);
}

void initializing(void)
{
	memset(funcMapping, -1, sizeof(funcMapping));               // -1로 초기화

	setFuncTable();
	setHeaderTable();
}

void convert()
{
	FILE *javaFp, *cFp = NULL;
	char buf[BUFLEN];
	char buf2[BUFLEN];
	char *sbuf[100];
	char c;
	int i, j, len, ret = 0;
	int cb = 0, scb = -1, sck = 0, scbr = 0, savebuf = 0;
	int checkKey = 0, checkCb = 0;
	int javaLine = 0, cLine = 0;

	if ((javaFp = fopen(filename, "r")) == NULL) {              // 자바파일 open()
		fprintf(stderr, "fopen error for %s\n", filename);
		exit(1);
	}

	while (fgets(buf, BUFLEN - 1, javaFp) != NULL) {
		len = strlen(buf) - 1;
		buf[len] = 0;							// 개행 문자를 NULL로
		javaLine++;

		if (optFlag[OPT_R]) {                   // R옵션이 들어왔을때
			pid_t pid;
			int status;

			if ((pid = fork()) == 0) {          // fork()하여 자식은
				system("clear");                // 터미널을 clear
				printf("-----%s-----\n", filename);     // 자바파일의 라인 하나를 출력
				printf("%d %s\n", javaLine, buf);
				exit(0);                        // 자식 종료
			}
			else if (pid > 0) {
				wait(&status);                  // 자식의 종료를 기다린다
				sleep(1);
			}
		}


		for (i = 0; buf[i] == '\t' || buf[i] == ' '; i++);	// 문자열 앞에 탭과 공백을 삭제
		for (j = 0; j + i <= len; j++) {
			buf[j] = buf[j + i];
		}

		for (i = 0; i < ACMD_NUM; i++) {					// 접근 제어자 삭제
			int tlen = strlen(AccessModifier[i]);

			if (strncmp(buf, AccessModifier[i], tlen) == 0) {
				for (j = 0; j <= len - tlen + 1; j++) {
					buf[j] = buf[j + tlen + 1];
				}
				break;
			}
		}

		switch(ret = convertLine(buf, cb)) {                // 한 줄 변환후 반환값을 이용한 처리
			case RET_CLASS :                                // 클래스는 파일 생성
				sprintf(buf2, "%s.c", buf);

				if ((cFp = fopen(buf2, "w")) == NULL) {
					fprintf(stderr, "fopen error for %s\n", buf2);
					exit(1);
				}

				fprintf(cFp, "\n");

				strcpy(cFile[NumOfcFile++], buf);
				break;
			case RET_KEY :
				writeLine(cFp, buf, cb + checkKey);
				checkKey++;
				break;
			case RET_SKIP :
				continue;
			case RET_OPCB :
				checkKey--;
				scb = cb;
				sck = checkKey;
				writeLine(cFp, buf, cb + checkKey);
				cb++;
				break;
			default :
				if (cFp == NULL) {
					continue;
				}
				if (cb == 0 && ret < 0) {                           // 파일이 끝나면
					fflush(cFp);
					fclose(cFp);

					sprintf(buf2, "%s.c", cFile[NumOfcFile - 1]);
					int cFd = open(buf2, O_RDWR);

					off_t off;
					for (i = 0; i < NumOfDef; i++) {                // 전처리기를 파일 앞에 삽입
						sprintf(buf, "#define %s\n", defineMap[i]);
						insertFile(cFd, buf);

						if (optFlag[OPT_R]) {
							sbuf[savebuf] = (char *)malloc(BUFLEN);
							strcpy(sbuf[savebuf], buf);
							savebuf++;
						}

						memset(defineMap[i], 0, sizeof(defineMap[0]));
					}

					for (i = 0; i < NumOfUsedH; i++) {              // 사용자 헤더를 파일앞에 삽입
						sprintf(buf, "#include \"%s\"\n", userHeaderTable[i]);

						insertFile(cFd, buf);

						if (optFlag[OPT_R]) {
							sbuf[savebuf] = (char *)malloc(BUFLEN);
							strcpy(sbuf[savebuf], buf);
							savebuf++;
						}
					}

					if (optFlag[OPT_P]) {
						printf("%s.c\n", cFile[NumOfcFile - 1]);
					}
                
					for (i = 0; i < NumOfUsed; i++) {               // 사용된 함수 확인
						for (j = 0; funcMapping[usedFunc[i]][j] >= 0; j++) {
							headerMapping[funcMapping[usedFunc[i]][j]] = 1;
						}

						if (optFlag[OPT_P]) {                       // P옵션 출력
							if (usedFunc[i] == FUNC_EXIT) {
								printf("%d %s -> %s()\n", i + 1, javaFunc[usedFunc[i]], cFunc[usedFunc[i]]);
							}
							else {
								printf("%d %s() -> %s()\n", i + 1, javaFunc[usedFunc[i]], cFunc[usedFunc[i]]);
							}
						}
						usedFunc[i] = 0;
					}

					int hc = 0;
					for (i = NumOfHeader - 1; i >= 0; i--) {        // 헤더를 파일앞에 삽입
						if (headerMapping[i] == 1) {
							memset(buf, 0, BUFLEN);
							len = strlen(headerMap[i]);
							strcpy(buf, headerMap[i]);

							if (hc == 0) {
								strcat(buf, "\n");
								len++;
								hc = 1;
							}

							insertFile(cFd, buf);
						if (optFlag[OPT_R]) {
							sbuf[savebuf] = (char *)malloc(BUFLEN);
							strcpy(sbuf[savebuf], buf);
							savebuf++;
						}
							headerMapping[i] = 0;
						}
					}

					for (i = 0; i < NumOfVar; i++) {
						memset(classVar[i], 0, VARLEN);
					}
					NumOfVar = 0;
					NumOfDef = 0;
					NumOfUsed = 0;

					fsync(cFd);
					close(cFd);

					break;
				}

				if (ret < 0) {
					cb += ret;
				}
				writeLine(cFp, buf, cb + checkKey);             // 한 줄을 파일에 삽입
				scbr = cb + checkKey;
				if (ret > 0) {
					cb += ret;
				}

				if (scb == -1) {
					if (checkKey > 0) {
						checkKey--;
					}
				}
				else if (scb > 0 && cb > scb) {
					checkCb = 1;
					if (checkKey > sck) {
						checkKey--;
					}
				}
				else if (checkCb) {
					cb = scb;
					checkKey = 0;
					checkCb = 0;
					scb = -1;
				}
		}

		if (optFlag[OPT_R]) {                                       // R옵션이 들어왔다면
			fflush(cFp);
			pid_t pid;
			int status;

			if ((pid = fork()) == 0) {                              // fork()하여 자식 프로세서가
				char *tok;                                          // 변환 된 코드 출력

				if (cb == 0 && ret < 0) {
					printf("-----%s.c-----\n", cFile[NumOfcFile - 1]);
					for (i = savebuf - 1; i >= 0; i--) {
						cLine++;
						printf("%d %s", cLine, sbuf[i]);
					}

					exit(cLine);
				}

				strcpy(buf2, buf);
                
				if (NumOfcFile > 0) {
					printf("-----%s.c-----\n", cFile[NumOfcFile - 1]);
				}
				if (buf2[0] == 0) {
					cLine++;
					printf("%d (SKIP)\n", cLine);
					exit(cLine);
				}
				tok = strtok(buf2, "\n");

				do {
					cLine++;
					printf("%d ", cLine);
					for (j = 0; j < scbr; j++) {
						printf("\t");
					}
					printf("%s\n", tok);
				} while ((tok = strtok(NULL, "\n")) != NULL);
				exit(cLine);
			}
			else if (pid > 0) {
				wait(&status);
				cLine = (status >> 8);
				sleep(2);

				for (i = 0; i < savebuf; i++) {
					free(sbuf[i]);
				}
			}
		}

	}

	fclose(javaFp);
}

int convertLine(char oneLineBuf[BUFLEN], int cb)
{
	char buf[BUFLEN];
	char tbuf[BUFLEN];
	char *sbuf[10];
	char var[VARLEN] = {0};
	char *cp;
	int i, j, k, t, st, sp;
	int len, l;
	int ret = 0, che = 0;
	static int checkMain, checkStruct, checkCmt, checkNew;
	static int userHeader, hLine;

	// 라인 전처리 시작!
	strcpy(buf, oneLineBuf);
	memset(oneLineBuf, 0, BUFLEN);

	len = strlen(buf);

	st = t = 1;

	if (checkCmt == 0) {                                        // 모든 주석과 문자열을 대체문자로 대체
		for (i = 0; i < len; i++) {
			if (buf[i] == '"') {
				i += extraction(tbuf, buf + i, buf[i], t);
				t++;
			}
			else if (buf[i] == '/') {
				i++;
				if (buf[i] == '/') {
					i += extraction(tbuf, buf + i, 0, t);
					t++;
				}
				else if (buf[i] == '*') {
					checkCmt = 1;

					i += extraction(tbuf, buf + i, buf[i], t);
					t++;

					if (buf[i + 1] == '/') {
						checkCmt = 0;
					}
				}
			}

			if (st != t) {
				sbuf[st] = (char *)malloc(BUFLEN);
				strcpy(sbuf[st], tbuf);
				st = t;
			}	
		}
	}
	else {
		i = extraction(tbuf, buf - 1, '*', t);

		if (i - 1 == len) {
			strcpy(oneLineBuf, tbuf);
			return ret;
		}
		else {
			t++;
			checkCmt = 0;
		}

		sbuf[st] = (char *)malloc(BUFLEN);
		strcpy(sbuf[st], tbuf);
		st = t;
	}

	for (i = 0; buf[i] != 0; i++) {
		if (buf[i] == '{') {
			ret++;
			che = 1;
		}
		else if (buf[i] == '}') {
			ret--;
		}
	}
	// 라인 전처리 끝!

	// 한 줄씩 변환 시작!
	if (strncmp(buf, "import", 6) == 0) {
		ret = RET_SKIP;
	}
	else if (strncmp(buf, "Scanner", 7) == 0) {
		ret = RET_SKIP;
	}

	if (ret == RET_SKIP) {
		buf[0] = 0;
		return ret;
	}

	for (i = 0; buf[i] == ' ' || buf[i] == '\n' || buf[i] == '\t'; i++)
		;
	if (i == len) {
		strcpy(oneLineBuf, buf);
		return ret;
	}

	if (cb == 0) {										// 함수의 외부
		if (strncmp(buf, "class", 5) == 0) {            // 클래스가 선언됐을때
			for (i = len - 1; buf[i] == ' ' || buf[i] == '{'; i--) {
				buf[i] = 0;
			}

			strcpy(buf, buf + 6);
			len = strlen(buf);

			checkMain = 0;

			ret = RET_CLASS; 
		}
		else if (strncmp(buf, "static void main", 16) == 0) {       // main()이 등장했을때
			if (strstr(buf, "IOException") != NULL) {
				checkMain++;
			}

			strcpy(buf, "int main(int argc, char* argv[])");

			if (ret) {
				strcat(buf, "\n{");
			}

			len = strlen(buf);

			checkMain++;
		}
		else if (strchr(buf, '(') != NULL) {                        // 그 외 함수들이 등장 했을때
			if (checkStruct == 1) {
				sprintf(tbuf, "} %s;", cFile[NumOfcFile - 1]);
				checkStruct = -2;

				write(userHeader, tbuf, strlen(tbuf));
			}
			else {
				checkStruct = -1;
			}

			cp = strchr(buf, '(');
			varParsing(buf, var, (int)(cp - buf));
			if (strcmp(var, cFile[NumOfcFile - 1]) == 0) {
				while (IS_VAR(*(cp - 1))) {
					cp--;
				}
				strInsert(cp, cFile[NumOfcFile - 1], 3);
				checkNew = 1;
			}

			cp = strchr(buf, '(');
			strInsert(cp + 1, cFile[NumOfcFile - 1], 2);
		}
		else {                                                      // 함수가 아닌 변수가 등장했을때
			if (checkStruct == 0 && checkMain == 0) { 
				sprintf(tbuf, "typedef struct _%s {\n\t", cFile[NumOfcFile - 1]);

				char hbuf[BUFLEN];
				sprintf(hbuf, "%s.h", cFile[NumOfcFile - 1]);

				userHeader = open(hbuf, O_WRONLY | O_CREAT | O_TRUNC, 0644);

				strcpy(userHeaderTable[NumOfUsedH++], hbuf);
				checkStruct = 1;
			}
			else {
				if (ret == 0) {
					strcpy(tbuf, "\t");
				}
				else {
					tbuf[0] = 0;
				}
			}

			if (checkStruct == 1) {
				for (i = 0; i < DATA_MAX; i++) {
					if (strncmp(buf, DataType[i], strlen(DataType[i])) == 0) {
						che = convertDataDec(buf, i);
						break;
					}
				}

				if (che != 1) {
					varParsing(buf, var, strlen(buf));
					strcpy(classVar[NumOfVar++], var);
				}

				strcat(tbuf, buf);
				strcpy(buf, tbuf);

				strcat(buf, "\n");
				write(userHeader, buf, strlen(buf));

				if (optFlag[OPT_R]) {
					fsync(userHeader);
					pid_t pid;
					int status;

					if ((pid = fork()) == 0) {
							
						char *tok;
						if (NumOfcFile > 0) {
							printf("-----%s.h-----\n", cFile[NumOfcFile - 1]);
						}

						if (tbuf[0] == 0) {
							hLine++;
							printf("%d (SKIP)\n", hLine);
							exit(hLine);
						}
						tok = strtok(tbuf, "\n");
						do {
							hLine++;
							printf("%d %s\n", hLine, tok);
						} while ((tok = strtok(NULL, "\n")) != NULL);
						exit(hLine);
					}
					else if (pid > 0) {
						wait(&status);
						hLine = (status >> 8);
						sleep(2);
					}
				}

				memset(buf, 0, BUFLEN);
				ret = RET_SKIP;
			}
		}
	}
	else {											// 함수의 내부
		if ((cp = strstr(buf, "null")) != NULL) {   // null을 NULL로 변환
			for (i = 0; i < 4; i++) {
				UPPER(*(cp + i));
			}
		}

		for (i = 0; i < DATA_MAX; i++) {            // 자료형 선언에 대한 처리
			int l = strlen(DataType[i]);
			if (strncmp(buf, DataType[i], l) == 0 && (buf[l] == ' ' || buf[l] == '[')) {
				convertDataDec(buf, i);

				break;
			}
		}

		for (i = 0; i < KEY_MAX; i++) {             // if, else, for ,while에 대한 처리
			if (strncmp(buf, KeyWord[i], strlen(KeyWord[i])) == 0) {
				break;
			}
		}

		if (i != KEY_MAX) {
			if (i == KEY_IF) {
				for (j = 0; j < NumOfFd; j++) {
					if ((cp = strstr(buf, fdTable[i])) != NULL) {
						cp += strlen(fdTable[i]);

						while (*cp == ' ') {
							cp++;
						}

						if (strncmp(cp, "!=", 2) == 0) {
							cp += 2;
							while (*cp == ' ') {
								cp++;
							}

							if (strncmp(cp, "NULL", 4) == 0) {
								cp = strchr(buf, '(') + 1;

								sprintf(cp, "%s >= 0)", fdTable[i]);
							}
						}
						else if (strncmp(cp, "==", 2) == 0) {
							cp += 2;
							while (*cp == ' ') {
								cp++;
							}

							if (strncmp(cp, "NULL", 4) == 0) {
								cp = strchr(buf, '(') + 1;

								sprintf(cp, "%s < 0)", fdTable[i]);
							}
						}
					}
				}
			}

			if (che == 0) {
				for (j = len - 1; buf[j] != ')'; j--) {
					if (buf[j] == ';') {
						break;
					}
				}

				if (buf[j] == ')') {
					ret = RET_KEY;
				}
			}
		}
		else {
			len = strlen(buf);

			for (i = 0; i < len; i++) {
				if (buf[i] == '(') {
					sp = i;
					j = i - 1;
					strcpy(tbuf, buf + sp);
					while (buf[j] == ' ') {			// 괄호앞에 띄어쓰기 예외 처리
						j--;
					}

					while (IS_VAR(buf[j - 1])) {
						j--;
					}

					for (k = 0; k < NumOfFunc; k++) {
						if (strncmp(buf + j, javaFunc[k], strlen(javaFunc[k])) == 0) {
							break;
						}
					}

					switch (k) {                                        // 라이브러리 함수가 등장했을때
						case 0 :
							cp = strstr(buf, "System.out.printf(");

							strcpy(cp, cFunc[k]);
							strcat(buf, tbuf);

							len = strlen(buf);
							i = 7;
							break;

						case 1 :
							for (j = 0; buf[j] != ' ' && buf[j] != '='; j++) {
								var[j] = buf[j];
							}

							sprintf(buf, "%s(\"%s\", &%s);", cFunc[k], "%d", var);

							break;
						case 2 :
							cp = strchr(tbuf, ',') + 1;
							while (*cp == ' ') {
								cp++;
							}

							if (strncmp(cp, "true", 4) == 0) {
								strcpy(cp, "O_WRONLY | O_APPEND);");
							}
							else if (strncmp(cp, "false", 5) == 0) {
								strcpy(cp, "O_WRONLY | O_CREAT | O_TRUNC, 0644);");
							}

							cp = strstr(buf, "new FileWriter(");
							sprintf(cp, "%s%s", cFunc[k], tbuf);

							if (checkMain > 1) {
								IOException(buf, cb);
							}

							cp = strchr(buf, '=');

							varParsing(buf, var, (int)(cp - buf));

							strcpy(fdTable[NumOfFd++], var);
							break;
						case 3 :
							varParsing(buf, var, j);

							l = strlen(tbuf) - 1;
							while (tbuf[l] != '"') {
								tbuf[l--] = 0;
							}
							l = 0;
							while (tbuf[l] != '"') {
								l++;
							}

							t = tbuf[l + 1] * -1;
							int sl;
							int tlen = sl = strlen(sbuf[t]);

							int bsc = 0;
							for (j = 0; j <	tlen; j++) {
								if (sbuf[t][j] == '\\') {
									if (sbuf[t][j + 1] == '\\' && bsc == 0) {
										bsc = 1;
									}
									else {
										bsc = 0;
										sl--;
									}
								}
							}

							sprintf(buf, "%s(%s, %s, %d);", cFunc[k], var, tbuf + l, st);

							break;
						case 4 :
						case 5 :
							varParsing(buf, var, j);

							sprintf(buf, "%s(%s);", cFunc[k], var);

							break;
						case 6 :
							break;
						default :                           // 그 외 함수들이 등장했을때
							if (strncmp(buf + j, "File(", 5) == 0) {
								while (buf[j] != '=') {
									j--;
								}
								buf[++j] = ' ';

								l = strlen(tbuf) - 1;
								while (tbuf[l] != '"') {
									tbuf[l--] = 0;
								}
								l = 0;
								while (tbuf[l] != '"') {
									l++;
								}

								sprintf(buf + j + 1, "%s;", tbuf + l);
							}
							else {
								if (buf[j - 1] == '.') {
									varParsing(buf, var, j);

									if (tbuf[1] == ')') {
										sprintf(buf + sp, "(&%s%s", var, tbuf + 1);

									}
									else {
										sprintf(buf + sp, "(&%s, %s", var, tbuf + 1);
									}

									strcpy(tbuf, buf + j);
									strcpy(buf + j - strlen(var) - 1, tbuf);
								}
								else if ((cp = strstr(buf, "new")) != NULL) {
									char *scp;

									scp = strchr(buf, '=');
									varParsing(buf, var, scp - buf);

									cp += 3;
									scp = cp;

									while (*cp == ' ') {
										cp++;
									}

									j = 0;
									while (*cp != '(') {
										tbuf[j++] = *cp;
										cp++;
									}
									tbuf[j] = 0;

									scp = strchr(buf, '=');
									while(!IS_VAR(*(scp - 1))) {
										scp--;
									}

									sprintf(scp, ";\n");
									for (j = 0; j < cb; j++) {
										strcat(buf, "\t");
									}
									scp = buf + strlen(buf);
									sprintf(scp, "new%s(&%s);", tbuf, var);
								}
							}
					}

					if (k < NumOfFunc) {
						addUsedFunc(k);                             // 사용된 라이브러리 함수를 저장
					}
				}
			}
		}

		if ((cp = strstr(buf, javaFunc[FUNC_NEW])) != NULL) {       // new가 나왔을때
			char *tp = cp;
			cp += 3;
			while (*(++cp) == ' ');

			for (i = 0; i < C_DATA; i++) {
				if (strncmp(cp, DataType[i], strlen(DataType[i])) == 0) {
					cp = strchr(cp, '[') + 1;
					j = 0;
					while (*cp != ']') {
						tbuf[j++] = *cp;
						cp += 1;
					}
					tbuf[j] = 0;

					sprintf(tp, "(%s *)calloc(%s, sizeof(%s));",	DataType[i], tbuf, DataType[i]);    // calloc으로 변환

					addUsedFunc(FUNC_NEW);

					break;
				}
			}
		}

		if (buf[0] == '{') {
			ret = RET_OPCB;
		}

		for (i = 0; i < NumOfVar; i++) {
			cp = buf;
			while ((cp = strstr(cp, classVar[i])) != NULL) {        // 구조체의 정의된 변수들이 나오면
				cp += strInsert(cp, cFile[NumOfcFile - 1], 1) + strlen(classVar[i]);    // 참조할 수 있도록 변환
			}
		}
	}

	if (checkMain > 0 && strncmp(buf, "return", 6) == 0) {          // main()에서 return이 나오면 변환
		strcpy(buf + 7, "0;");
		checkMain = -1;	
	}

	for (i = 0; i < len; i++) {                     // 추출해뒀던 문자열, 주석을 원래대로 돌려줌
		if (buf[i] < 0) {
			t = buf[i] * -1;
			j = 0;
			do {
				buf[i] = sbuf[t][j];
				sbuf[t][j] = 0;
				i++, j++;
			} while (buf[i] < 0);
		}
	}

	for (i = 1; i < st; i++) {                      // 메모리 할당 해제
		free(sbuf[i]);
	}

	if (cb + ret <= 0 && ret < 0) {                 // 한 파일이 종료될때 처리
		if (checkNew == 1) {
			sprintf(buf + len - 1, "\n\treturn *%c;\n}", cFile[NumOfcFile - 1][0]);
			checkNew = 0;
		}

		if (userHeader != 0) {
			close(userHeader);
			userHeader = 0;
		}

		if (checkMain > 0) {
			strcpy(buf + len - 1, "\n\treturn 0;\n}");
		}
	}

	strcpy(oneLineBuf, buf);                        // convert()에서 전해줬던 인자에 복사

	return ret;	
}

void insertTab(FILE *cFp, int cb)
{
	char buf[BUFLEN] = {0};

	for (int i = 0; i < cb; i++) {                  // 중괄호의 수를 계산해서 \t을 삽입
		strcat(buf, "\t");
	}

	fprintf(cFp, "%s", buf);
}

int convertDataDec(char *buf, int i)
{
	char *cp;
	char tbuf[BUFLEN];
	int j;

	if (strstr(buf, "final") != NULL) {             // final이나오면 전처리기 문장으로 변환
		cp = strchr(buf, '=');
		j = 0;

		do {
			cp -= 1;
		} while (*cp == ' ');

		while (*(cp - 1) != ' ') {
			cp -= 1;
		}

		while (*cp != ' ' && *cp != '=') {
			tbuf[j++] = *cp;
			cp += 1;
		}		

		cp = strchr(buf, '=');
		tbuf[j++] = ' ';

		do {
			cp += 1;
		} while (*cp == ' ');

		while (*cp != ';') {
			tbuf[j++] = *cp;
			cp += 1;
		}
		tbuf[j] = 0;

		strcpy(defineMap[NumOfDef++], tbuf);
		memset(buf, 0, BUFLEN);

		return 1;
	}

	switch (i) {
		case 7 :                                        // static은 static을 제외하고 재귀처리
			for (j = 0; j < DATA_MAX; j++) {
				if (strncmp(buf + 7, DataType[j], strlen(DataType[j])) == 0) {
					return convertDataDec(buf + 7, j);
				}
			}
			break;
		case 10 :                                       // File 자료형을 char *로
			changeDataType(buf, i, "char *");
			return 0;
			break;
		case 11 :                                       // FileWriter 자료형 int로
			changeDataType(buf, i, "int ");
			return 0;
			break;


	}

	if (strstr(buf, "[]") != NULL) {                    // 배열이 나왔을때처리
		cp = strchr(buf, ']');

		while (!IS_ALPHA(*cp)) {
			cp += 1;
		}

		j = 0;
		while (IS_ALPHA(*cp)) {
			tbuf[j++] = *cp;
			cp += 1;
		}
		tbuf[j] = 0;

		if ((cp = strchr(buf, '=')) != NULL) {
			if (strstr(buf, "new") != NULL) {           // 선언과 동시에 new가 나온다면
				cp = strrchr(buf, '[');                 // 배열 정의 가능

				while (*(cp - 1) != ']') {
					tbuf[j++] = *cp;
					cp += 1;
				}	
				tbuf[j] = 0;

				strcat(tbuf, " = {0};");                // 동시에 초기화
			}
			else {
				strcat(tbuf, "[] ");                    // 또는 선언과 동시에 초기화문장
				strcat(tbuf, cp);
			}

			sprintf(buf, "%s %s", DataType[i], tbuf);
		}
		else {                                          // 두가지 상황이 아니라면
			sprintf(buf, "%s *%s;", DataType[i], tbuf); // 포인터로 선언
		}
	}

	if (i == 6) {                                       // String은 char *로 변환
		strcpy(tbuf, buf + 7);
		strcpy(buf, "char *");
		strcat(buf, tbuf);
	}

	return 0;
}

void writeLine(FILE *cFp, char *buf, int cb)
{
	insertTab(cFp, cb);
	fprintf(cFp, "%s\n", buf);
}

int extraction(char *tbuf, char *buf, char eoc, int t)
{
	int i;
	i = 1;

	while (buf[i] != 0) {
		if (buf[i] == eoc) {                    // 처리의 종료문자가 나올때까지 추출
			if (eoc != '*') {
				break;
			}
			else if (buf[i + 1] == '/') {
				break;
			}
		}

		tbuf[i - 1] = buf[i];
		buf[i] = -1 * t;
		i++;
	}

	tbuf[i - 1] = 0;

	return i;
}

void addUsedFunc(int fnum)
{
	int i;

	for (i = 0; i < NumOfUsed; i++) {           // 사용된 함수가 이미 사용된 함수 목록에 있으면
		if (usedFunc[i] == fnum) {
			break;                              // 통과
		}
	}

	if (i == NumOfUsed) {                       // 아니라면 추가
		usedFunc[NumOfUsed++] = fnum;
	}
}

void changeDataType(char *buf, int i, const char *ch)
{
	char *cp = buf + strlen(DataType[i]) + 1;
	char tbuf[BUFLEN];

	sprintf(tbuf, "%s%s", ch, cp);
	strcpy(buf, tbuf);
}

void IOException(char *buf, int cb)
{
	char tbuf[BUFLEN];
	int i , j;
	char *cp;

	j = 0;
	cp = strrchr(buf, '=') - 1;
	while (*cp == ' ') {
		cp--;
	}
	while (IS_VAR(*(cp - 1))) {
		cp--;
	}
	while (IS_VAR(*cp)) {
		tbuf[j++] = *cp++;
	}
	tbuf[j] = 0;

	strcat(buf, "\n");
	for (i = 0; i < cb; i++) {
		strcat(buf, "\t");
	}
	sprintf(buf + strlen(buf), "if (%s < 0) {\n", tbuf);
	for (i = 0; i < cb + 1; i++) {
		strcat(buf, "\t");
	}
	strcat(buf, "fprintf(stderr, \"open error\\n\");\n");
	for (i = 0; i < cb + 1; i++) {
		strcat(buf, "\t");
	}
	strcat(buf, "exit(1);\n");
	for (i = 0; i < cb; i++) {
		strcat(buf, "\t");
	}
	strcat(buf, "}");

	addUsedFunc(FUNC_EXIT);
}

void varParsing(char *buf, char *var, int j)
{
	char *cp;

	memset(var, 0, VARLEN);

	cp = buf + j - 1;
	while (!IS_VAR(*cp)) {
		cp--;
	}

	while (IS_VAR(*(cp - 1))) {
		cp--;
	}
	j = 0;
	while (IS_VAR(*cp)) {
		var[j++] = *(cp++);
	}
}

int strInsert(char *buf, const char *in, int mode)
{
	char tbuf[VARLEN];

	strcpy(tbuf, buf);

	switch (mode) {
		case 1 :
			sprintf(buf, "%c->%s", in[0], tbuf);
			break;
		case 2 :
			if (tbuf[0] == ')') {
				sprintf(buf, "%s *%c%s", in, in[0], tbuf);
			}
			else {
				sprintf(buf, "%s *%c, %s", in, in[0], tbuf);
			}
			break;
		case 3 :
			sprintf(buf, "%s new%s", in, tbuf);
			break;
		default :
			sprintf(buf, "%s%s", in, tbuf);
	}

	return strlen(buf) - strlen(tbuf);
}

void insertFile(int fd, char *buf)
{
	off_t off;
	int len;
	char c;

	len = strlen(buf);

	off = lseek(fd, -1, SEEK_END);
	while (lseek(fd, off, SEEK_SET) >= 0) {
		read(fd, &c, 1);
		buf[len] = c;
		write(fd, buf, len + 1);
		off--;
	}
}

void optionProcessing(void)
{
	char buf[BUFLEN], tbuf[BUFLEN];
	FILE *javaFp, *cFp;
	int javaFd, cFd;
	int i, line;

	if (optFlag[OPT_J]) {
		javaFp = fopen(filename, "r");

		printf("\n%s\n", filename);
		line = 1;
		while (fgets(buf, BUFLEN - 1, javaFp) != NULL) {
			printf("%d\t%s", line++, buf);
		}

		fclose(javaFp);
	}
	if (optFlag[OPT_C]) {
		for (i = 0; i < NumOfUsedH; i++) {
			cFp = fopen(userHeaderTable[i], "r");

			printf("\n%s\n", userHeaderTable[i]);
			line = 1;
			while (fgets(buf, BUFLEN - 1, cFp) != NULL) {
				printf("%d\t%s", line++, buf);
			}

			fclose(cFp);
		}
		for (i = 0; i < NumOfcFile; i++) {
			sprintf(tbuf, "%s.c", cFile[i]);
			cFp = fopen(tbuf, "r");

			printf("\n%s\n", tbuf);
			line = 1;
			while (fgets(buf, BUFLEN - 1, cFp) != NULL) {
				printf("%d\t%s", line++, buf);
			}

			printf("\n");
			fclose(cFp);
		}
	}
	if (optFlag[OPT_F]) {
		off_t jsiz, csiz;

		javaFd = open(filename, O_RDONLY);
		jsiz = lseek(javaFd, 0, SEEK_END);

		printf("\n%s file size is %lld bytes\n", filename, jsiz);

		for (i = 0; i < NumOfUsedH; i++) {
			cFd = open(userHeaderTable[i], O_RDONLY);
			csiz = lseek(cFd, 0, SEEK_END);

			printf("%s file size is %lld bytes\n", userHeaderTable[i], csiz);
		}
		for (i = 0; i < NumOfcFile; i++) {
			sprintf(tbuf, "%s.c", cFile[i]);
			cFd = open(tbuf, O_RDONLY);
			csiz = lseek(cFd, 0, SEEK_END);

			printf("%s file size is %lld bytes\n", tbuf, csiz);
		}
	}
	if (optFlag[OPT_L]) {
		javaFp = fopen(filename, "r");

		line = 1;
		while (fgets(buf, BUFLEN - 1, javaFp) != NULL) {
			line++;
		}
		fclose(javaFp);
		printf("\n%s line number is %d lines\n", filename, line);

		for (i = 0; i < NumOfUsedH; i++) {
			cFp = fopen(userHeaderTable[i], "r");

			line = 1;
			while (fgets(buf, BUFLEN - 1, cFp) != NULL) {
				line++;
			}

			printf("%s line number is %d lines\n", userHeaderTable[i], line);
			fclose(cFp);
		}
		for (i = 0; i < NumOfcFile; i++) {
			sprintf(tbuf, "%s.c", cFile[i]);
			cFp = fopen(tbuf, "r");

			line = 1;
			while (fgets(buf, BUFLEN - 1, cFp) != NULL) {
				line++;
			}

			printf("%s line number is %d lines\n", tbuf, line);
			fclose(cFp);
		}
	}
}

void makeMakefile(void)
{
	FILE *mFp;
	char buf[BUFLEN] = {0};
	int i = 0, len;

	while (IS_VAR(filename[i])) {
		buf[i] = filename[i];
		i++;
	}
	len = strlen(buf);
	strcat(buf, "_Makefile");

	if ((mFp = fopen(buf, "w")) == NULL) {
		fprintf(stderr, "open error for %s\n", buf);
		exit(1);
	}

	buf[len] = 0;

	fprintf(mFp, "%s: ", buf);
	for (i = 0; i < NumOfcFile; i++) {
		fprintf(mFp, "%s.o ", cFile[i]);
	}
	fprintf(mFp, "\n\tgcc ");
	for (i = 0; i < NumOfcFile; i++) {
		fprintf(mFp, "%s.o ", cFile[i]);
	}
	fprintf(mFp, "-o %s", buf);

	for (i = 0; i < NumOfcFile; i++) {
		fprintf(mFp, "\n%s.o : %s.c\n", cFile[i], cFile[i]);
		fprintf(mFp, "\tgcc %s.c -c\n", cFile[i]);
	}

	fflush(mFp);
	fclose(mFp);
}
