#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include "ssu_runtime.h"
#include "my_define.h"

int main(int argc, char *argv[])
{
	int o;

	gettimeofday(&begin_t, NULL);

	if (argc < 2) {
		fprintf(stderr, "require argument\n");
		exit(1);
	}

	if (strcmp(argv[1], "-c") == 0) {
		int check = 0;
		for (int i = 2; i < argc && i < 7; i++) {
			if (strcmp(argv[i], "-h") == 0) {
				print_help();
				check = -1;	
				break;
			}
			strcat(opt_arg[i], argv[i]);
			opt[OPT_C][i - 1] = i;
			opt[OPT_C][0]++;
		}

		if (check == 0) {
			print_score();
		}

		gettimeofday(&end_t, NULL);
		ssu_runtime(&begin_t, &end_t);

		exit(0);
	}

	if ((o = option_handling(argc, argv)) != 0) {
		if (opt[OPT_H][0]) {
			print_help();
		}
		else if (o == -1) {
			fprintf(stderr, "confirm usage for option with -h option\n");
		}

		gettimeofday(&end_t, NULL);
		ssu_runtime(&begin_t, &end_t);

		exit(0);
	}

	get_start_info();

	check_score_table();

	score_student();

	if (opt[OPT_C][0]) {
		print_score();
	}

	system("rm killer");
	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}

void get_start_info(void)				// 문제 번호 및 학생들의 학번으  전역 변수 배열에 저장 시킬 함수
{
	char com_ls[PATH_SIZE]; 
	char buf[BUFSIZ], buf2[BUFSIZ];	
	int tmpOut, tmpErr;
	int fd;
	int len, line, of;
	int j, tmp;

	printf("%c", 0);
	fprintf(stderr, "%c", 0);

	tmpOut = dup(1);			// stdout의 디스크립터를 임시 변수로 복사
	tmpErr = dup(2);

	if ((fd = open("temp", O_RDWR | O_CREAT | O_TRUNC, 0644)) < 0) {
		fprintf(stderr, "first temp creat fail\n");
		exit(1);
	}

	dup2(fd, 1);				// stdout에 fd를 복사 system명령어를 파일에 받기 위함

	sprintf(com_ls, "ls %s", ansDir);	// 폴더 이름 저장
	system(com_ls);				// com_ls 명령어를 실행
	lseek(fd, 0, SEEK_SET);

	line = 0, j = 0;
	while ((len = read(fd, buf, BUFSIZ - 1)) > 0) {
		for (int i = 0; i < len; i++) {
			if (buf[i] == '\n') {				// 개행이 나올 때
				proList[line++][j] = 0;		// 맨뒤에 NULL를 넣고 라인을 증가
				j = 0;							// j를 초기화

				if (buf[i + 1] < '0' || buf[i + 1] > '9') {		// 파일명이 숫자로 시작하지 않는다면 (즉, 문제폴더가 아니라면) 반복문 탈출
					break;
				}
				continue;
			}
			proList[line][j++] = buf[i];
		}
	}

	for (int i = 1; i < line; i++) {			// 폴더 이름 정렬
		int tmp = atoi(proList[i]);
		for (int k = i - 1; tmp < atoi(proList[k]) && k >= 0; k--) {
			strcpy(buf, proList[k]);
			strcpy(proList[k], proList[k + 1]);
			strcpy(proList[k + 1], buf);
		}
	}

	for (int i = 0; proList[i][0] != 0; i++) {
		num_problem = i;
		pro[i].proId = proList[i];

		if (start_prog == 0) {
			sprintf(buf2, "%s/%s/%s.c", ansDir, proList[i], proList[i]);
			if (access(buf2, F_OK) == 0) {	
				start_prog = i;
			}
		}
	}

	num_problem++;
	compile_answer(start_prog, num_problem);

	dup2(tmpOut, 1);
	close(fd);

	if ((fd = open("temp", O_RDWR | O_CREAT | O_TRUNC, 0644)) < 0) {
		fprintf(stderr, "second temp not creat\n");
		exit(1);
	}

	dup2(fd, 1);

	sprintf(com_ls, "ls %s", stdDir);			// 학생들의 학번폴더들의 이름을 저장
	system(com_ls);
	lseek(fd, 0, SEEK_SET);

	line = 0, j = 0;
	while ((len = read(fd, buf, BUFSIZ - 1)) > 0) {
		for (int i = 0; i < len; i++) {
			if (buf[i] == '\n') {				// 개행이 나올 때
				stdList[line][j] = 0;		// 맨뒤에 NULL을 넣고 라인을 증가
				std[line].stdId = stdList[line];

				line++;
				j = 0;							// j를 초기화
				continue;
			}
			stdList[line][j++] = buf[i]; 
		}
	}
	num_std = line;
	close(fd);

	dup2(tmpOut, 1);
	close(tmpOut);
	system("rm temp");

	make_killer();

	if (opt[OPT_E][0]) {
		mkdir(opt_arg[opt[OPT_E][1]], 0777);
		chdir(opt_arg[opt[OPT_E][1]]);
		usleep(500000);

		for (int i = 0; i < num_std; i++) {
			mkdir(std[i].stdId, 0777);
		}
		chdir(P_DIR);
	}
}

void check_score_table(void)
{
	int tableDes;
	char filePath[PATH_SIZE];
	char buf[BUFSIZ];
	char *input = (char *)malloc(sizeof(char) * 5);
	int sel;
	int i, c, len;

	sprintf(filePath, "%s/%s", ansDir, score_table); 

	if ((tableDes = open(filePath, O_WRONLY | O_CREAT | O_EXCL, 0644)) < 0) {		// 중복을 확인하고 파일을 open()
		if ((tableDes = open(filePath, O_RDONLY)) < 0) {		//파일이 존재한다면 파일을 읽기로 open()
			fprintf(stderr, "open error for %s\n", score_table);
			exit(1);
		}
		// 이미 존재하는 점수 테이블에 따라 선언된 전역 변수 배열에 문제의 점수를 입력
		c = 0, i = 0;
		while ((len = read(tableDes, buf, BUFSIZ - 1)) > 0) {
			buf[len] = 0;

			input = strtok(buf, "\n,");
			do {
				c++;
				if (c % 2 == 0) {
					pro[i++].score = atof(input);
				}
			} while ((input = strtok(NULL, "\n,")) != NULL);
		}

		close(tableDes);
	}
	else {
		printf("score_table.csv file doesn't exist in %s\n"
				"1. input blank question and program question's score. ex) 0.5, 1\n"
				"2. input all question's score. ex) Input value of 1-1 : 0.1\n"
				"selet type >> ", ansDir);

		do {
			scanf("%d", &sel);

			switch (sel) {
				case 1 :
					printf("input value of blank question : ");
					scanf("%s", input);
					for (i = 0; i < start_prog; i++) {
						pro[i].score = atof(input);

						sprintf(buf, "%s,%s\n", proList[i], input);
						len = strlen(buf);
						write(tableDes, buf, len);
					}

					printf("input value of program question : ");
					scanf("%s", input); 
					for (i = start_prog; i < num_problem; i++) {
						pro[i].score = atof(input);

						sprintf(buf, "%s,%s\n", proList[i], input);
						len = strlen(buf);
						write(tableDes, buf, len);
					}
					break;
				case 2:
					for (i = 0; i < num_problem; i++) {
						printf("Input of %s : ", proList[i]);
						scanf("%s", input);

						pro[i].score = atof(input);

						sprintf(buf, "%s,%s\n", proList[i], input);
						len = strlen(buf);
						write(tableDes, buf, len);
					}
					break;
				default:
					printf("wrong number\nselect type >> ");
			}
		} while (sel != 1 && sel != 2);

		free(input);
		close(tableDes);
	}

	if ((result_table = open(score_save, O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0) {
		fprintf(stderr, "open error for %s\n", score_save);
		exit(1);
	}
	else {
		for (i = 0; i < start_prog; i++) {
			write(result_table, ",", 1);
			write(result_table, pro[i].proId, strlen(pro[i].proId));
			write(result_table, ".txt", 4);
		}
		for (i = start_prog; i < num_problem; i++) {
			write(result_table, ",", 1);
			write(result_table, pro[i].proId, strlen(pro[i].proId));
			write(result_table, ".c", 2);
		}

		write(result_table, "\n", 1);
	}
	close(result_table);
}

void make_killer(void)			// 프로세스 킬러프로그램을 생성
{
	int killFd;
	int len;

	if (access(psKiller, F_OK) == 0) {
		return;
	}

	if ((killFd = open("timeover.c", O_WRONLY | O_CREAT | O_TRUNC, 0600)) < 0) {
		fprintf(stderr, "killer open error\n");
		exit(1);
	}

	char buf[] = {
		"#include <unistd.h>\n"
			"#include <stdlib.h>\n"
			"#include <sys/time.h>\n\n"
			"int main(void)\n"
			"{\n"
			"	usleep(" TIME_LIMIT ");\n"
			"	system(\"pkill .stdexe\");\n"
			"	exit(0);\n"
			"}\n"
	};

	len = strlen(buf);

	write(killFd, buf, len);

	close(killFd);

	system("gcc timeover.c -o killer");

	system("rm timeover.c");
}

int option_handling(int ar, char *arg[])
{
	char buf[BUFSIZ];
	int optNum = 1, i, j;
	int c = 0;
	int ret = 0;

	if (arg[1][0] != '-') {
		strcpy(stdDir, arg[1]);
		strcpy(ansDir, arg[2]);
	}

	for (i = 1; i < ar; i++) {
		if (arg[i][0] == '-') {
			switch (arg[i][1]) {
				case 'h' :
					opt[OPT_H][0] = 1;
					ret = 1;

					break;

				case 't' :
					if (i >= ar - 1 || arg[i + 1][0] == '-') {
						ret = -1;
						break;
					}
					for (j = 1; j <= 5 && i + j < ar; j++) {
						if (arg[i + j][0] == '-') {
							break;
						}

						opt[OPT_T][j] = optNum;
						strcpy(opt_arg[optNum], arg[i + j]);
						optNum++;
					}

					opt[OPT_T][0] = j - 1;
					break;

				case 'e' :
					opt[OPT_E][0] = 1;
					if (i >= ar - 1 || arg[i + 1][0] == '-') {
						ret = -1;
					}
					opt[OPT_E][1] = optNum;
					strcpy(opt_arg[optNum], arg[i + 1]);
					optNum++;
					break;

				case 'c' :
					if (i >= ar - 1 || arg[i + 1][0] == '-' || arg[i + 1][0] == 0) {
						ret = -1;
					}
					for (j = 1; j <= 5 && i + j < ar; j++) {
						opt[OPT_C][j] = optNum;
						strcpy(opt_arg[optNum], arg[i + j]);
						optNum++;
					}

					opt[OPT_C][0] = j - 1;
					break;

				case 'p' :
					opt[OPT_P][0] = 1;
					break;
			}
		}
	}

	return ret;
}

void compile_answer(int start, int end)
{
	char filePath[PATH_SIZE];
	char buf[BUFSIZ];
	char buf2[BUFSIZ];
	int fd, errFd;
	int tmpOut, tmpErr;
	char c, *cp;
	int off, len;

	tmpOut = dup(1);
	tmpErr = dup(2);

	sprintf(filePath, "%s", ansDir);
	chdir(filePath);
	usleep(200000);

	for (int i = start; i < end; i++) {
		sprintf(filePath, "%s", proList[i]);
		chdir(filePath);
		usleep(200000);

		sprintf(buf, "%s.stdout", proList[i]);
		if ((fd = open(buf, O_WRONLY | O_CREAT | O_EXCL, 0644)) < 0) {
			chdir("..");
			usleep(200000);
			continue;
		}

		sprintf(buf2, "%s_error.txt", proList[i]);
		errFd = open(buf2, O_RDWR | O_CREAT | O_TRUNC, 0644);

		dup2(errFd, 2);

		sprintf(buf, "gcc %s.c -o %s.exe", proList[i], proList[i]);
		system(buf);

		if (lseek(errFd, 0, SEEK_END) <= 1) {
			close(errFd);
			sprintf(buf, "rm %s", buf2);
			system(buf);
		}
		else {
			do {
				off = lseek(errFd, -2, SEEK_CUR);
				read(errFd, &c, 1);
			} while (off > 0 && c != '\n');

			read(errFd, buf, 100);

			if ((cp = strstr(buf, "error")) != NULL) {
			}
			if ((cp = strstr(buf, "warning")) != NULL) {
			}
			close(errFd);
		}

		dup2(fd, 1);

		sprintf(buf, "./%s.exe", proList[i]);
		system(buf);

		chdir(P_DIR);
		usleep(200000);
		close(fd);
	}

	dup2(tmpOut, 1);
	dup2(tmpErr, 2);
	close(tmpOut);
	close(tmpErr);

	chdir(P_DIR);
	usleep(200000);
}

int score_program(const int stdDes, const int ansDes)
{
	char stdBuf[BUFSIZ];
	char ansBuf[BUFSIZ];
	int ret = 0;
	int stdLen = 0, ansLen = 0;
	int i = 0, j = 0, check;

	lseek(stdDes, 0, SEEK_SET);
	lseek(ansDes, 0, SEEK_SET);

	while (1) {
		if (stdLen == 0) {
			stdLen = read(stdDes, stdBuf, BUFSIZ - 1);
			stdBuf[stdLen] = 0;
			i = 0;
		}
		if (ansLen == 0) {
			ansLen = read(ansDes, ansBuf, BUFSIZ - 1);
			ansBuf[ansLen] = 0;
			j = 0;
		}

		if (stdLen == 0 && ansLen == 0) {
			ret = 1;
		}
		else if (stdLen == 0 || ansLen == 0) {
			ret = -1;
			if (stdLen == 0) {
				while (ansBuf[j] == ' ' || ansBuf[j] == '\n') {
					j++;				//남아 있는 문자열이 띄어쓰기나 개행이라면 답이 될 가능성이 있음
					if (j == ansLen) {
						continue;
					}
				}
				if (ansBuf[j] == 0) {
					ret = 1;
				}
			}
			else {
				while (stdBuf[i] == ' ' || ansBuf[i] == '\n') {
					i++;				// 남아 있는 문자열이 띄어쓰기나 개행이라면 답이 될 가능성이 있음
					if (i == stdLen) {
						continue;
					}
				}
				if (stdBuf[i] == 0) {
					ret = 1;
				}
			}}

		check = 1;
		while (check) {
			while (stdBuf[i] == ' ') {					// 공백 제거
				i++;
			}
			if (stdBuf[i] >= 'A' && stdBuf[i] <= 'Z') {
				stdBuf[i] += UPtoLO;				// 대문자를 소문자로
			}
			while (ansBuf[j] == ' ') {					// 공백 제거
				j++;
			}
			if (ansBuf[j] >= 'A' && ansBuf[j] <= 'Z') {
				ansBuf[j] += UPtoLO;				// 대문자를 소문자로
			}

			if(stdBuf[i] != ansBuf[j]) {
				ret = -1;
				break;
			}

			i++, j++;

			if (stdBuf[i] == 0) {
				stdLen = 0;
				check = 0;
			}
			if (ansBuf[i] == 0) {
				ansLen = 0;
				check = 0;
			}
		}

		if (ret == 1 || ret == -1) {
			break;
		}
	}

	return ret;
}

float grade_student_program(int stdNum)
{
	char stdPath[PATH_SIZE];
	char ansPath[PATH_SIZE];
	char buf[BUFSIZ], buf2[BUFSIZ];
	int stdFd, errFd, ansFd;
	int tmpOut, tmpErr;
	float score, sumScore = 0;
	int len, off, timeCheck = 0;
	char c, *cp;
	const int progNum = num_problem - start_prog;

	tmpOut = dup(1);
	tmpErr = dup(2);

	sprintf(stdPath, "%s/%s", stdDir, std[stdNum].stdId);

	for (int i = start_prog; i < num_problem; i++) {
		sprintf(buf2, "%s/%s.c", stdPath, pro[i].proId);
		if (access(buf2, F_OK) < 0) {
			continue;
		}

		score = 0;
		sprintf(buf2, "%s/%s_error.txt", stdPath, pro[i].proId);
		if (opt[OPT_E][0]) {
			sprintf(buf2, "./%s/%s/%s_error.txt", opt_arg[opt[OPT_E][1]], std[stdNum].stdId, pro[i].proId);
		}
		errFd = open(buf2, O_RDWR | O_CREAT | O_TRUNC, 0600);

		dup2(errFd, 2);

		sprintf(buf, "gcc %s/%s.c -o %s/%s.stdexe", stdPath, pro[i].proId, stdPath, pro[i].proId);
		if (opt[OPT_T][0]) {
			for (int j = 1; j <= 5; j++) {
				if (strcmp(pro[i].proId, opt_arg[opt[OPT_T][j]]) == 0) {
					strcat(buf, " -lpthread");
					printf("%s\n", buf);
				}
			}
		}
		system(buf);
		dup2(tmpErr, 2);

		sprintf(buf, "rm %s", buf2);

		if ((len = lseek(errFd, 0, SEEK_END)) <= 1) {
			close(errFd);
			system(buf);
		}
		else {
			do {
				off = lseek(errFd, -2, SEEK_CUR);
				read(errFd, &c, 1);
			} while (off > 0 && c != '\n');

			memset(buf2, 0, BUFSIZ);
			len = read(errFd, buf2, 100);

			if ((cp = strstr(buf2, "error")) != NULL) {
				score = COM_ERR;
				close(errFd);
				std[stdNum].grade[i] = 0;

				if (!opt[OPT_E][0]) {
					system(buf);
				}
				continue;
			}
			if ((cp = strstr(buf2, "warning")) != NULL) {
				len = atoi(buf2);
				score -= COM_WRN * len;
			}
			close(errFd);
		}

		sprintf(buf2, "./%s/%s.stdout", stdPath, pro[i].proId);
		if ((stdFd = open(buf2, O_RDWR | O_CREAT | O_TRUNC, 0600)) < 0) {
			fprintf(stderr, "stdFd creat Error for %s\n", buf2);
			exit(1);
		}

		errFd = open("timeover", O_RDWR | O_CREAT | O_TRUNC, 0600);

		dup2(stdFd, 1);
		dup2(errFd, 2);

		sprintf(buf, "%s/%s.stdexe & ./%s", stdPath, pro[i].proId, psKiller);
		system(buf);
		
		if (lseek(errFd, 0, SEEK_END) > 1) {
			timeCheck = 1;
		}
		close(errFd);
		dup2(tmpErr, 2);

		sprintf(buf, "%s/%s/%s.stdout",ansDir, pro[i].proId, pro[i].proId);
		if ((ansFd = open(buf, O_RDONLY)) < 0) {
			fprintf(stderr, "ansFd open Error for %s\n",buf);
			exit(1);
		}

		if (timeCheck == 0 && score_program(stdFd, ansFd)) {
			score += pro[i].score;
		}

		if (score < 0) {
			score = 0;
		}

		std[stdNum].grade[i] = score;
		sumScore += score;

		dup2(tmpOut, 1);
		
		close(stdFd);
		close(ansFd);
	}

	system("rm timeover");
	close(tmpOut);
	close(tmpErr);
	return sumScore;
}

int token_seperator(const int stdFd,const int ansFd, char stdTok[BUFSIZ][101], char ansTok[20][BUFSIZ][101]) 
{
	char stdBuf[BUFSIZ] = {0};
	char ansBuf[BUFSIZ] = {0};
	int ansCheck = 0;
	int stdLen = 0, ansLen = 0;
	int i = 0, j = 0;
	int ti = 0, tj = 0;
	int st = 0, at = 0;
	int c = 1;

	while (c) {
		if (stdLen == 0) {
			stdLen = read(stdFd, stdBuf, BUFSIZ - 1);
			stdBuf[stdLen] = 0;
			i = 0;
		}
		if (ansLen == 0) {
			ansLen = read(ansFd, ansBuf, BUFSIZ - 1);
			ansBuf[ansLen] = 0;
			j = 0;
		}

		if (stdLen < BUFSIZ - 1 && ansLen < BUFSIZ - 1) {
			c = 0;
		}

		while (i < stdLen - 1 || j < ansLen - 1) {
			if (i < stdLen - 1) {
				while(stdBuf[i] == ' ') { i++; }

				if (stdBuf[i] != '"') {
					stdTok[st][ti++] = stdBuf[i++];
				}
				else {
					do {
						stdTok[st][ti++] = stdBuf[i++];
					} while (stdBuf[i] != '"');
					stdTok[st][ti++] = stdBuf[i++];

					stdTok[st][ti] = 0;
					st++;
					ti = 0;
					continue;
				}

				if (IS_SPECIAL(stdBuf[i]) || IS_SPECIAL(stdBuf[i - 1])) { 
					if (stdBuf[i] == '\n') {
						i = stdLen;
						continue;
					}
					stdTok[st][ti] = 0;
					st++;
					ti = 0;

					if ((strchr(oper, stdBuf[i]) != NULL) && (stdBuf[i + 1] == '=' || (stdBuf[i + 1] == stdBuf[i] && (stdBuf[i] != '%' && stdBuf[i] != '/' && stdBuf[i] != '*')))) {
						stdTok[st][ti++] = stdBuf[i++];
						stdTok[st][ti++] = stdBuf[i++];

						if (stdTok[st][1] ==  '<' || stdTok[st][1] == '>') {
							if (stdBuf[i] == '=') {
								stdTok[st][ti++] = stdBuf[i++];
							}
						}

						stdTok[st][ti] = 0;
						st++;
						ti = 0;
					}
					else if (stdBuf[i] == '-' && stdBuf[i + 1] == '>') {
						stdTok[st][ti++] = stdBuf[i++];
						stdTok[st][ti++] = stdBuf[i++];

						stdTok[st][ti] = 0;
						st++;
						ti = 0;
					}
				}
			}

			if (j < ansLen - 1) {
				while(ansBuf[j] == ' ') { j++; }

				if (ansBuf[j] != '"') {
					ansTok[ansCheck][at][tj++] = ansBuf[j++];
				}
				else {
					do {
						ansTok[ansCheck][at][tj++] = ansBuf[j++];
					} while (ansBuf[j] != '"');
					ansTok[ansCheck][at][tj++] = ansBuf[j++];
					at++;
					tj = 0;
					continue;
				}

				if (ansTok[ansCheck][at][0] == ':') {
					ansTok[ansCheck][at][0] = 0;
					at = -1;
					ansCheck++;
				}

				if(IS_SPECIAL(ansBuf[j]) || IS_SPECIAL(ansBuf[j - 1])) {
					ansTok[ansCheck][at][tj] = 0;
					at++;
					tj = 0;

					if ((strchr(oper, ansBuf[j]) != NULL) && (ansBuf[j + 1] == '=' || (ansBuf[j + 1] == ansBuf[j] && (ansBuf[j] != '%' && ansBuf[j] != '/' && ansBuf[j] != '*')))) {
						ansTok[ansCheck][at][tj++] = ansBuf[j++];
						ansTok[ansCheck][at][tj++] = ansBuf[j++];

						if (ansBuf[j] == '=') {
							if (ansTok[ansCheck][at][1] == '<' || ansTok[ansCheck][at][1] == '>') {
								ansTok[ansCheck][at][tj++] = ansBuf[j++];
							}
						}

						ansTok[ansCheck][at][tj] = 0;
						at++;
						tj = 0;
					}
				}	
			}
		}
	}
	if (stdTok[st - 1][0] == '\n') {
		stdTok[--st][0] = 0;
	}
	if (ansTok[ansCheck][at - 1][0] == '\n') {
		ansTok[ansCheck][--at][0] = 0;
	}

	tokSetting(stdTok);
	for (i = 0; i <= ansCheck; i++) {
		tokSetting(ansTok[i]);
	}

	return ansCheck;
}

void tokDfs(char tok[][101], tree *cur, const int fir, const int last)
{
	if (fir >= last) {
		return;
	}
	if (last - fir == 1) {
		cur->data = fir;

		cur->left = NULL;
		cur->right = NULL;
		return;
	}

	int i;
	int maxPr = 0;
	int maxPrInd = -1;
	int tokLen = 0;
	int parCnt = 0;

	for (i = fir; i < last; i++) {
		tokLen = strlen(tok[i]);
		if (tok[i][tokLen - 1] == '(') {
			if (maxPr < prOper['(']) {
				maxPr = prOper['('];
				maxPrInd = i;
			}
			i++, parCnt++;
			while (parCnt > 0) {
				tokLen = strlen(tok[i]);
				if (tok[i][tokLen - 1] == '(') {
					parCnt++;
				}
				if (tok[i][0] == ')') {
					parCnt--;
				}
				i++;
				if (i > last) {
					return;
				}
			}
			i--;
		}
		else if (IS_SPECIAL(tok[i][0]) == 1) {
			if (tok[i][1] == 0 || IS_SPECIAL(tok[i][1]) == 1) {
				if (tok[i][1] == '=') {
					if (tok[i][0] == '<' || tok[i][0] =='>') {
						if (maxPr < prOper['<']) {
							maxPr = prOper['<'];
							maxPrInd = i;
						}
					}
					else if (tok[i][0] == '=' || tok[i][0] == '!') {
						if (maxPr < prOper['=' + OPER_EQ]) {
							maxPr = prOper['=' + OPER_EQ];
							maxPrInd = i;
						}
					}
					else {
						if (maxPr < prOper['=']) {
							maxPr = prOper['='];
							maxPrInd = i;
						}
					}

				}
				else if (tok[i][1] == 0) {
					if (maxPr < prOper[tok[i][0]]) {
						maxPr = prOper[tok[i][0]];
						maxPrInd = i;
					}
				}
				else if (tok[i][0] == tok[i][1]) {
					if (maxPr < prOper[tok[i][0] + OPER_EQ]) {
						maxPr = prOper[tok[i][0] + OPER_EQ];
						maxPrInd = i;
					}
				}
				else if (tok[i][1] == '*') {
					continue;
				}
				else if (tok[i][0] == '-' && tok[i][1] == '>') {
					if (maxPr < prOper['-' + OPER_EQ]) {
						maxPr = prOper['-' + OPER_EQ];
						maxPrInd = i;
					}
				}
			}
		}
	}

	if (maxPr == 1) {
		cur->data = fir;

		cur->left = (tree *)malloc(sizeof(tree));
		cur->right = (tree *)malloc(sizeof(tree));

		tokDfs(tok, cur->left, fir + 1, last - 1);
		tokDfs(tok, cur->right, last - 1, last);
	}
	else if (maxPr == 0) {
		cur->data = fir;

		cur->left = NULL;
		cur->right = (tree *)malloc(sizeof(tree));

		tokDfs(tok, cur->right, fir + 1, last);
	}
	else {
		cur -> data = maxPrInd;

		cur->left = (tree *)malloc(sizeof(tree));
		cur->right = (tree *)malloc(sizeof(tree));

		tokDfs(tok, cur->left, fir, cur->data);
		tokDfs(tok, cur->right, cur->data + 1, last);
	}

	if (tok[cur->data][0] == '>' && tok[cur->data][1] != '>') {
		tok[cur->data][0] = '<';
		swapTLR(cur);
	}
}

void freeTree(tree *cur)
{
	if (cur == NULL) {
		return;
	}

	if (cur->left != NULL) {
		freeTree(cur->left);
	}
	if (cur->right != NULL) {
		freeTree(cur->right);
	}

	cur = NULL;
	free(cur);
}

int makeTree(tree **root, char tok[][101])
{
	int i;
	int l = 0;
	int c = 0;
	int ret = 0;

	while (tok[l][0] != 0) l++;
	if (l <= 0) {
		*root = NULL;
		return 0;
	}
	if (tok[l - 1][0] == ';') {
		ret = 1;
		l--;
	}

	for (i = 0; i < KEY_MAX; i++) {
		if (strstr(tok[0], keyword[i]) != NULL) {
			c = 1;
			break;
		}
	}

	*root = (tree *)malloc(sizeof(tree));

	if (tok[0][0] == '#' || c == 1) {
		tree *node = *root;
		node->data = 0;
		node->left = NULL;
		node->right = NULL;
		for (i = 1; i < l; i++) {
			node->right = (tree *)malloc(sizeof(tree));
			node = node -> right;
			node->data = i;
			node->left = NULL;
			node->right = NULL;
		}

		return ret;
	}	

	tokDfs(tok, *root, 0, l);

	return ret;
}

int scoringDFS(tree *stdTree, tree *ansTree, char stdTok[][101], char ansTok[][101])
{
	int leftCor = 0;
	int rightCor = 0;

	if (stdTree == NULL || ansTree == NULL) {
		return 0;
	}

	if (strcmp(stdTok[stdTree->data], ansTok[ansTree->data]) != 0) {
		return 0;
	}

	if (stdTree->left == NULL && ansTree->left == NULL) {
		leftCor = 1;
	}
	else if (stdTree->left == NULL || ansTree->left == NULL) {
		return 0;
	}
	if (stdTree->right == NULL && ansTree->right == NULL) {
		rightCor = 1;
	}
	else if (stdTree->right == NULL || ansTree->right == NULL) {
		return 0;
	}

	if (stdTree->left != NULL && ansTree->left != NULL) {
		leftCor = scoringDFS(stdTree->left, ansTree->left, stdTok, ansTok);
	}

	if (stdTree->right != NULL && ansTree->right != NULL) {
		rightCor = scoringDFS(stdTree->right, ansTree->right, stdTok, ansTok);
	}

	if (!(leftCor && rightCor) && strstr("+*&&||^!==", stdTok[stdTree->data]) != NULL) {
		leftCor = scoringDFS(stdTree->left, ansTree->right, stdTok, ansTok);
		rightCor = scoringDFS(stdTree->right, ansTree->left, stdTok, ansTok);
	}

	if (leftCor && rightCor) {
		return 1;
	}
	else {
		return 0;
	}
}

int score_void(const int stdDes, const int ansDes)
{
	char stdTok[BUFSIZ][101];
	char ansTok[20][BUFSIZ][101];
	int ret = 0;
	int i, ansCheck;
	int stdSemi = 0;
	int ansSemi = 0;
	tree *stdTree = NULL, *ansTree = NULL;

	memset(stdTok, 0, BUFSIZ * 101);
	memset(ansTok, 0, 20 * BUFSIZ * 101);

	lseek(stdDes, 0, SEEK_SET);
	lseek(ansDes, 0, SEEK_SET);

	ansCheck = token_seperator(stdDes, ansDes, stdTok, ansTok);

	stdSemi = makeTree(&stdTree, stdTok);

	for (i = 0; i <= ansCheck; i++) {
		ansSemi = makeTree(&ansTree, ansTok[i]);
		if (scoringDFS(stdTree, ansTree, stdTok, ansTok[i])) {
			if (stdSemi == ansSemi) {
				ret = 1;
				break;
			}
		}

		freeTree(ansTree);
	}

	freeTree(stdTree);

	return ret;
}

float grade_student_void(int stdNum)
{
	char stdPath[PATH_SIZE];
	char ansPath[PATH_SIZE];
	char buf[BUFSIZ], buf2[BUFSIZ];
	int stdFd, ansFd;
	float score, sumScore = 0;
	int len;

	priorityOper();

	sprintf(stdPath, "%s/%s", stdDir, std[stdNum].stdId);

	for (int i = 0; i < start_prog; i++) {
		sprintf(buf, "%s/%s.txt", stdPath, pro[i].proId);
		if (access(buf, F_OK) < 0) {
			continue;
		}

		score = 0;

		if ((stdFd = open(buf, O_RDONLY)) < 0) {
			fprintf(stderr, "open error %s\n", buf);
			exit(1);
		}

		sprintf(buf, "%s/%s/%s.txt", ansDir, pro[i].proId, pro[i].proId);

		if ((ansFd = open(buf, O_RDONLY)) < 0) {
			fprintf(stderr, "open error %s\n", buf);
			exit(1);
		}
		if (score_void(stdFd, ansFd)) {
			score = pro[i].score;
		}
		sumScore += score;
		std[stdNum].grade[i] = score;

		close(stdFd);
		close(ansFd);
	}

	return sumScore;
}

void score_student()
{
	float score, sumScore = 0;
	int i;
	char buf[BUFSIZ];

	for (i = 0; i < num_std; i++) {
		score = 0;
		score += grade_student_void(i);
		score += grade_student_program(i);

		sumScore += score;

		printf("%s is finished.. ", std[i].stdId);
		if (opt[OPT_P][0]) {
			printf("score : %.1f", score);
		}
		printf("\n");

		if ((result_table = open(score_save, O_RDWR | O_APPEND)) < 0) {
			fprintf(stderr, "open error for %s\n", score_save);
			exit(1);
		}	

		write(result_table, std[i].stdId, strlen(std[i].stdId));
		for (int j = 0; j < num_problem; j++) {
			write(result_table, ",", 1);
			sprintf(buf, "%.1f", std[i].grade[j]);
			write(result_table, buf, strlen(buf));
		}
		sprintf(buf, ",%.1f\n", score);
		write(result_table, buf, strlen(buf));

		close(result_table);
	}

	if (opt[OPT_P][0]) {
		printf("Total average : %.1f\n", sumScore / num_std);
	}

}
