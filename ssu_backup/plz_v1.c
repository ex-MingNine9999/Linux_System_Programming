#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>
#include <pthread.h>
#include <time.h>
#include "ssu_runtime.h"
#include "ssu_backup.h"

char backupDIR[PATH_MAX];
char logName[PATH_MAX];
FILE *logFp;
dList *backupList;
int flag_D;
pthread_mutex_t logMutex = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char *argv[])
{
	gettimeofday(&begin_t, NULL);

	initializing(argc, argv);

	prompt();

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}

int initializing(int argc, char *argv[])
{
	char buf[BUF_SIZ];
	int ret = 0;
	struct stat statbuf;

	if (argc > 2) {
		ret = -1;
	}
	else if (argc == 1) {
		strcpy(backupDIR, BACKUP_NAME);
		mkdir(backupDIR, 0777);
	}
	else {
		stat(argv[1], &statbuf);
		if (!S_ISDIR(statbuf.st_mode)) {
			ret = -1;
		}

		if (!(statbuf.st_mode & S_IXUSR)) {
			ret = -1;
		}

		if (ret == 0) {
			sprintf(backupDIR, "%s/%s", argv[1], BACKUP_NAME);
			mkdir(backupDIR, 0777);
		}
	}

	if (ret < 0) {
		fprintf(stderr, "usage : %s <DIRECTORY>\n", argv[0]);
		exit(1);
	}

	sprintf(logName, "%s/%s", backupDIR, LOG_NAME);
	if ((logFp = fopen(logName, "w")) == NULL) {
		fprintf(stderr, "fopen error for %s\n", LOG_NAME);
		exit(1);
	}

	backupList = (dList *)malloc(sizeof(dList));
	list_init(backupList);
	getAbsolPath(backupDIR);

	return ret;
}

void prompt(void)
{
	char com[BUF_SIZ];

	while (1) {
		memset(com, 0, BUF_SIZ);
		flag_D = 0;
		logCntl(L_CNO, NULL);

		printf("%s ", PROMPT);
		fgets(com, BUF_SIZ - 1, stdin);

		com[strlen(com) - 1] = 0;				// 개행 제거

		if (strncmp(com, "add", 3) == 0) {
			comAdd(com + 4);
		}
		else if (strncmp(com, "remove", 6) == 0) {
			comRemove(com + 7);
		}
		else if (strncmp(com, "compare", 7) == 0) {
			comCompare(com + 8);
		}
		else if (strncmp(com, "recover", 7) == 0) {
			comRecover(com + 8);
		}
		else if (strncmp(com, "list", 4) == 0) {
			comList();
		}
		else if (strncmp(com, "exit", 4) == 0) {
			return;
		}
		else if (strncmp(com, "ls", 2) == 0 || strncmp(com, "vi", 2) == 0 || com[0] == 0) {
			system(com);
		}
		else {
			fprintf(stderr, "20152385sh: command not found %s\n", com);
		}
	}

	pthread_mutex_destroy(&logMutex);
}

int logCntl(int mode, char *msg)
{
	pthread_mutex_lock(&logMutex);

	switch (mode) {
		case L_CNO :
			fclose(logFp);
			if ((logFp = fopen(logName, "a+")) == NULL) {
				pthread_mutex_unlock(&logMutex);
				return -1;
			}
			break;
		case L_MSG :
			if(fprintf(logFp, "%s\n", msg) < 0) {
				pthread_mutex_unlock(&logMutex);
				return -1;
			}
			break;
	}

	pthread_mutex_unlock(&logMutex);

	return mode;
}

long long getNowTime()
{
	long long ret;
	time_t ttt;
	struct tm now;
	char buf[BUF_SIZ];

	time(&ttt);
	now = *localtime(&ttt);

	sprintf(buf, "%02d%02d%02d%02d%02d%02d", 
			now.tm_year%100, now.tm_mon + 1, now.tm_mday, now.tm_hour, now.tm_min, now.tm_sec);

	ret = atoll(buf);

	return ret;
}

void getAbsolPath(char *fname)
{
	char temp[PATH_MAX];	
	int i, j;
	int cnt = 0, check = 0;

	getcwd(temp, PATH_MAX);

	if (fname[0] == '.') {
		for (i = 0; strncmp(fname + i, "../", 3) == 0; i += 3) {
			cnt++;
		}
		if (cnt == 0) {
			while (fname[i] != '/') {
				i++;
			}
			i++;

			strcat(temp, "/");
		}
		else {
			for (j = strlen(temp) - 1; j >= 0; j--) {
				if (temp[j] == '/') {
					check++;
				}

				if (check == cnt) {
					break;
				}
			}
			temp[j + 1] = 0;
		}

		strcat(temp, fname + i);
		strcpy(fname, temp);
	}
	else if (fname[0] == '~') {
		for (i = 0; fname[i] != '/'; i++)
			;

		for (j = 0; check < ROOT_HOME; j++) {
			if (temp[j] == '/') {
				check++;
			}
		}
		temp[j] = 0;

		strcat(temp, fname + i + 1);
		strcpy(fname, temp);
	}
	else if (fname[0] == '/') {
		return;
	}
	else {
		strcat(temp, "/");
		strcat(temp, fname);
		strcpy(fname, temp);
		return;
	}
}

char *getFileName(char *path, char *fname)
{
	char *cp;

	if ((cp = strrchr(path, '/')) == NULL) {
		strcpy(fname, path);
	}
	else {
		strcpy(fname, cp + 1);
	}

	return fname;
}

long long getBackupPath(char *fname)
{
	char buf[PATH_MAX];
	long long ret = getNowTime();

	strcpy(buf, fname);
	sprintf(fname, "%s/%s_%lld", backupDIR, buf, ret);

	return ret;
}

void comAdd(char *com)
{
	node tmpNode;
	char buf[BUF_SIZ] = {0};
	char logbuf[BUF_SIZ];
	char *cp = com;
	int checkUsage = 0;
	struct stat statbuf;
	static char chain;
	long long t;

	memset((void *)&tmpNode, 0, sizeof(node));

	while (*cp == ' ') cp++;

	if ((cp = strtok(cp, " ")) == NULL) {
		checkUsage = 1;	
	}
	else {
		while (*cp == ' ') cp++;
		strcpy(buf, cp + strlen(cp) + 1);

		if (access(cp, F_OK) < 0) {
			checkUsage = 1;
		}

		if (checkUsage == 0) {
			stat(cp, &statbuf);

			if (S_ISREG(statbuf.st_mode) || S_ISDIR(statbuf.st_mode)) {
				strcpy(tmpNode.filename, cp);
				getAbsolPath(tmpNode.filename);

				if (list_search(backupList, tmpNode.filename) != NULL) {
					checkUsage = 2;
				}
			}
			else {
				checkUsage = 1;
			}
		}
	}

	if ((cp = strtok(NULL, " ")) == NULL) {
		checkUsage = 1;
	}
	else {
		while (*cp == ' ') cp++;
		if (strchr(cp, '.') != NULL) {
			checkUsage = 3;
		}
		else {
			tmpNode.period = atoi(cp);

			if (tmpNode.period < 5 || tmpNode.period > 10) {
				checkUsage = 4;
			}
		}
	}

	if (checkUsage) {
		switch (checkUsage) {
			case 1 :
				fprintf(stderr, "usage : add <FILENAME> <PERIOD> <OPTIONS>...\n");
				break;
			case 2 :
				fprintf(stderr, "%s is already added the list\n", tmpNode.filename);
				break;
			case 3 :
				fprintf(stderr, "<PERIOD> must be integer\n");
				break;
			case 4 :
				fprintf(stderr, "5 <= <PERIOD> <= 10\n");
				break;
		}

		return;
	}

	while ((cp = strtok(NULL, " \n")) != NULL) {
		while (*cp == ' ') cp++;

		if (cp[0] == '-') {
			switch (cp[1]) {
				case 'm' :
					tmpNode.opt[OPT_M] = 1;
					break;
				case 'n' :
					tmpNode.opt[OPT_N] = 1;
					if ((cp = strtok(NULL, " \n")) == NULL || *cp == '-') {
						fprintf(stderr, "add -n option usage : -n <NUMBER> (1 <= <NUMBER> <= 100)\n");
						return;
					}
					else {
						while (*cp == ' ') cp++;

						if ((strchr(cp, '.') != NULL) && (1 > atoi(cp) || atoi(cp) > 100)) {
							fprintf(stderr, "add -n option usage : -n <NUMBER> (1 <= <NUMBER> <= 100) (<NUMBER> must be integer)\n");
							return;
						}

						strcpy(tmpNode.optarg[OPT_N], cp);
					}
					break;
				case 't' :
					tmpNode.opt[OPT_T] = 1;
					if ((cp = strtok(NULL, " \n")) == NULL || *cp == '-') {
						fprintf(stderr, "add -t option usage : -t <TIME> (60 <= TIME <= 1200)\n");
						return;
					}
					else {
						while (*cp == ' ') cp++;

						if ((strchr(cp, '.') != NULL) && (60 > atoi(cp) || atoi(cp) > 1200)) {
							fprintf(stderr, "add -t option usage : -t <TIME> (60 <= TIME <= 1200) (<TIME> must be integer)\n");
							return;
						}

						strcpy(tmpNode.optarg[OPT_T], cp);
					}
					break;
				case 'd' :
					if (!S_ISDIR(statbuf.st_mode)) {
						fprintf(stderr, "add -d option usage : add <DIRECTORY> -d\n"
								"%s is not directory\n", tmpNode.filename);

						return;	
					}

					tmpNode.opt[OPT_D] = 1;

					if (flag_D == 0) {
						chain++;
						flag_D = 1;
					}
					break;
				default :
					fprintf(stderr, "A %s is unknown option\n", cp);
					return;
			}
		}
		else {
			fprintf(stderr, "A %s is not an option\n", cp);
			return;
		}
	}

	if (S_ISDIR(statbuf.st_mode) && tmpNode.opt[OPT_D] == 0) {
		fprintf(stderr, "directory is needed -d option\n");
		return;
	}

	if (flag_D) {
		tmpNode.optarg[OPT_D][0] = chain;
	}

	list_pushBack(backupList, tmpNode);

	t = getNowTime();

	if (tmpNode.opt[OPT_D] == 1) {
		optionD(tmpNode.filename, buf);
		sprintf(logbuf, LOG_FORMAT " added", t / 1000000, t % 1000000, tmpNode.filename);

		logCntl(L_MSG, logbuf);
		return;
	}

	if (pthread_create(&(backupList->tail->tid), NULL, thread_backup, backupList->tail) != 0) {
		fprintf(stderr, "pthread_create error for %s\n", backupList->tail->filename);
		list_delete(backupList, tmpNode.filename);
		return;
	}

	sprintf(logbuf, LOG_FORMAT " added", t / 1000000, t % 1000000, tmpNode.filename);
	logCntl(L_MSG, logbuf);
}

void comRemove(char *com)
{
	char *cp;
	char buf[PATH_MAX];
	char logbuf[BUF_SIZ];
	int dchain;
	char *dname;
	node *tmpNode;
	pthread_t tid;
	long long t;

	if ((cp = strtok(com, " \n")) == NULL) {
		fprintf(stderr, "remove usage : remove <FILENAME> or <OPTION>\n");
		return;
	}
	else {
		while (*cp == ' ') cp++;

		if (strncmp(cp, "-a", 2) == 0) {
			cp += 2;
			while (*cp == ' ') cp++;

			if (strtok(NULL, " \n") != NULL) {
				fprintf(stderr, "remove -a option usage : remove -a\n");
				return;
			}

			for (backupList->cur = backupList->head->next;
					backupList->cur != backupList->tail->next;
					backupList->cur = backupList->cur->next) {
				if((tid = backupList->cur->tid) > 0) {
					pthread_cancel(tid);
				}

				t = getNowTime();
				sprintf(logbuf, LOG_FORMAT " deleted", t / 1000000, t % 1000000, backupList->cur->filename);
				logCntl(L_MSG, logbuf);
			}

			list_deleteAll(backupList);
		}
		else {
			strcpy(buf, cp);
			getAbsolPath(buf);
			if (list_search(backupList, buf) != NULL) {
				if (backupList->cur->opt[OPT_D]) {
					dchain = backupList->cur->optarg[OPT_D][0];
					dname = backupList->cur->filename;

					tmpNode = backupList->cur->next;
					while (tmpNode != NULL &&
							tmpNode->optarg[OPT_D][0] == dchain &&
							strncmp(dname, tmpNode->filename, strlen(dname)) == 0) {
						comRemove(tmpNode->filename);
						tmpNode = backupList->cur;
					}
					tid = (pthread_t)-1;
				}
				else {
					tid = backupList->cur->tid;
				}
			}

			if (list_delete(backupList, buf) < 0) {
				fprintf(stderr, "Not found %s\n", buf);
				return;
			}
			if (tid > 0) {
				pthread_cancel(tid);
			}

			t = getNowTime();
			sprintf(logbuf, LOG_FORMAT " deleted", t / 1000000, t % 1000000, buf);
			logCntl(L_MSG, logbuf);
		}
	}
}

void comCompare(char *com)
{
	char *cp = com;
	char fname1[PATH_MAX], fname2[PATH_MAX];
	int checkUsage = 0;
	struct stat statbuf1, statbuf2;

	while (*cp == ' ') cp++;

	if ((cp = strtok(cp, " \n")) == NULL) {
		checkUsage = 1;
	}
	else {
		strcpy(fname1, cp);
	}

	if (checkUsage == 0) {
		while (*cp == ' ') cp++;
		if (cp == NULL || (cp = strtok(NULL, " \n")) == NULL) {
			checkUsage = 1;
		}
		else {
			strcpy(fname2, cp);
		}
	}

	if (checkUsage == 0) {
		while (*cp == ' ') cp++;
		if (cp == NULL || (cp = strtok(NULL, " \n")) != NULL) {
			checkUsage = 1;
		}
	}

	if (checkUsage) {
		fprintf(stderr, "usage : compare <FILENAME1> <FILENAME2>\n");
		return;
	}

	if (access(fname1, F_OK) < 0) {
		fprintf(stderr, "%s is not exist\n", fname1);
		checkUsage = 1;
	}
	if (access(fname2, F_OK) < 0) {
		fprintf(stderr, "%s is not exist\n", fname2);
		checkUsage = 1;
	}
	if (checkUsage) {
		return;
	}

	stat(fname1, &statbuf1);
	stat(fname2, &statbuf2);

	if (statbuf1.st_size == statbuf2.st_size &&
			statbuf1.st_mtime == statbuf2.st_mtime) {
		printf("%s and %s are same file. (%s == %s) \n", fname1, fname2, fname1, fname2);
	}
	else {
		printf("%s and %s are different file. (%s != %s) \n", fname1, fname2, fname1, fname2);
		printf("%s's size : %lld\tmtime : %ld\n"
				"%s's size : %lld\tmtime : %ld\n",
				fname1, statbuf1.st_size, statbuf1.st_mtime,
				fname2, statbuf2.st_size, statbuf2.st_mtime);
	}

}

void comRecover(char *com)
{
	char *cp = com;
	char old_f[PATH_MAX], new_f[PATH_MAX] = {0};
	char fname[PATH_MAX];
	int check = 0, i, j, n;
	DIR *dp;
	struct dirent *dentry;
	char fileList[BACKUP_MAX][PATH_MAX] = {0};
	long long t;

	while (*cp == ' ') cp++;

	if ((cp = strtok(cp, " ")) == NULL) {
		fprintf(stderr, "usage : recover <FILENAME> <OPTION>\n");
		return;
	}
	else {
		while (*cp == ' ') cp++;

		strcpy(old_f, cp);
		getAbsolPath(old_f);
	}

	if (access(old_f, F_OK) < 0) {
		fprintf(stderr, "not found file for %s\n", old_f);
		return;
	}
	if (list_search(backupList, old_f) == NULL) {
		fprintf(stderr, "not found in backup list for %s\n", old_f);
		return;
	}

	if ((cp = strtok(NULL, " \n")) != NULL) {
		while (*cp == ' ') cp++;

		if (cp[0] == '-') {
			if (cp[1] == 'n') {
				if ((cp = strtok(NULL, " \n")) == NULL) {
					fprintf(stderr, "recover -n option usage : -n <NEWFILE>\n");

					return;
				}
				else {
					while (*cp == ' ') cp++;

					strcpy(new_f, cp);

					if (access(new_f, F_OK) == 0) {
						fprintf(stderr, "%s is already exist\n", new_f);
						return;
					}
					check = -1;
				}
			}
		}
		else {
			fprintf(stderr, " A %s is not an option\n", cp);
			return;
		}
	}

	if (check == 0) {
		strcpy(new_f, old_f);
	}

	getFileName(old_f, fname);

	if ((dp = opendir(backupDIR)) == NULL) {
		fprintf(stderr, "opendir error for %s\n", backupDIR);
		return;
	}

	n = 0;
	strcpy(fileList[n++], "select nothing (return prompt)");
	while ((dentry = readdir(dp)) != NULL) {
		if (strstr(dentry->d_name, fname) != NULL) {
			strcpy(fileList[n++], dentry->d_name);
		}
	}
	closedir(dp);

	if (n == 1) {
		fprintf(stderr, "do not make backup file yet\n");
		return;
	}

	for (i = 2; i < n; i++) {							// list를 시간순으로 insert sort
		strcpy(fname, fileList[i]);
		t = atoll(fname + strlen(fname) - 12);
		for (j = i - 1; j > 0; j--) {
			if (t >= atoll(fileList[j] + strlen(fileList[j]) - 12)) {
				break;
			}
			strcpy(fileList[j + 1], fileList[j]);
		}
		strcpy(fileList[j + 1], fname);
	}

	for (i = 0; i < n; i++) {
		printf("%3d. %s\n", i, fileList[i]);
	}

	printf("\nselect number : ");
	n = -1;
	scanf("%d", &n);
	while (n < 0 || n >= i) {
		printf("wrong number\nselect number : ");
		scanf("%d", &n);
	}
	getchar();
	if (n == 0) {
		return;
	}

	sprintf(fileList[0], "%s/%s", backupDIR, fileList[n]);
	backupFile(fileList[0], new_f, B_RECOVER, NULL);
	catFile(fileList[0]);
	comRemove(old_f);
}

void comList()
{
	list_print(backupList);
}

int copyFile(char *path, char *newPath)
{
	char buf[BUF_SIZ * 3];
	int orgFd, backFd;
	int len, mode;
	struct stat statbuf;

	stat(path, &statbuf);
	mode = statbuf.st_mode & 0777;

	if ((orgFd = open(path, O_RDONLY)) < 0) {
		fprintf(stderr, "origin open error for %s\n", path);
		return -1;
	}
	if ((backFd = open(newPath, O_WRONLY | O_CREAT | O_TRUNC, mode)) < 0) {
		fprintf(stderr, "backup open error for %s\n", newPath);
		return -1;
	}

	while ((len = read(orgFd, buf, BUF_SIZ * 3 - 1)) > 0) {
		write(backFd, buf, len);
	}

	close(orgFd);
	close(backFd);

	return 0;
}

int backupFile(char *path, char *newPath, int mode, dList *list)
{
	long long t;
	char logbuf[BUF_SIZ];
	node n;

	switch (mode) {
		case B_BACKUP :
			newPath = (char *)calloc(PATH_MAX, sizeof(char));
			memset((void *)&n, 0, sizeof(node));

			list_search(backupList, path);
			t = getBackupPath(getFileName(path, newPath));

			strcpy(n.filename, newPath);
			list_pushBack(list, n);
			break;
		case B_RECOVER:
			t = getNowTime();
			getAbsolPath(newPath);
			break;
		default :
			fprintf(stderr, "unknown mode\n");
			return -1;
	}

	if (copyFile(path, newPath) < 0) {
		fprintf(stderr, "copy error for %s\n", path);
		return -1;
	}

	switch (mode) {
		case B_BACKUP :
			sprintf(logbuf, LOG_FORMAT " generated", t / 1000000, t % 1000000, newPath);
			logCntl(L_MSG, logbuf);

			free(newPath);
			break;
		case B_RECOVER :
			sprintf(logbuf, LOG_FORMAT " recovered\n\t\t->%s", t / 1000000, t % 1000000, path, newPath);
			logCntl(L_MSG, logbuf);
			break;
		default :
			fprintf(stderr, "unknown mode\n");
			return -1;
	}

	return 0;
}

void optionD(const char *dname, char *com)
{
	DIR *dp;
	struct dirent *dentry;
	struct stat statbuf;
	char fname[PATH_MAX];
	char newCom[BUF_SIZ] = {0};
	char *cp;

	if ((cp = strstr(com, "-d")) == NULL) {
		fprintf(stderr, "???\n");
		return;
	}

	if ((dp = opendir(dname)) == NULL) {
		fprintf(stderr, "opendir error for %s\n", dname);
		return;
	}

	while ((dentry = readdir(dp)) != NULL) {
		if (dentry->d_ino == 0
				|| strcmp(dentry->d_name, ".") == 0
				|| strcmp(dentry->d_name, "..") == 0) {
			continue;
		}

		sprintf(fname, "%s/%s", dname, dentry->d_name);
		stat(fname, &statbuf);

		if (S_ISDIR(statbuf.st_mode)) {
			cp[0] = '-';
			cp[1] = 'd';
			sprintf(newCom, "%s %s", fname, com);
		}
		else {
			cp[0] = ' ';
			cp[1] = ' ';
			sprintf(newCom, "%s %s", fname, com);
		}

		comAdd(newCom);
	}

	closedir(dp);
}

void catFile(const char *path)
{
	int fd;
	char buf[BUF_SIZ * 3];
	int len;

	fd = open(path, O_RDONLY);

	printf("\n%s\n", path);
	while ((len = read(fd, buf, BUF_SIZ * 3 - 1)) > 0) {
		write(1, buf, len);
	}
	puts("");

	close(fd);
}

void *thread_backup(void *arg)
{
	node* np = (node *)arg;
	dList *flist = (dList *)malloc(sizeof(dList));
	long long t, st;
	int n;
	struct stat statbuf;
	time_t mt;

	list_init(flist);

	if (np->opt[OPT_T]) {
		st = atoll(np->optarg[OPT_T]);
		st = (st / 60 * 100) + (st % 60);
	}
	if (np->opt[OPT_N]) {
		n = atoi(np->optarg[OPT_N]) - 1;
	}
	if (np->opt[OPT_M]) {
		stat(np->filename, &statbuf);
		mt = statbuf.st_mtime;
	}

	while (1) {
		sleep(np->period);

		if (np->opt[OPT_T] && flist->size > 0) {
			t = getNowTime();

			flist->cur = flist->head->next;
			while (t - atoll(flist->cur->filename + strlen(flist->cur->filename) - 12) >= st) {
				if (remove(flist->cur->filename) < 0) {
					fprintf(stderr, "remove error for %s\n", flist->cur->filename);
				}
				list_delete(flist, flist->cur->filename);

				flist->cur = flist->head->next;
			}
		}
		if (np->opt[OPT_N]) {
			while (flist->size > n) {
				flist->cur = flist->head->next;

				if (remove(flist->cur->filename) < 0) {
					fprintf(stderr, "remove error for %s\n", flist->cur->filename);
				}
				list_delete(flist, flist->cur->filename);
			}
		}
		if (np->opt[OPT_M]) {
			stat(np->filename, &statbuf);
			if (mt == statbuf.st_mtime) {
				continue;
			}
			mt = statbuf.st_mtime;
		}

		if (backupFile(np->filename, NULL, B_BACKUP, flist) < 0) {
			fprintf(stderr, "backupFile error for %s\n", np->filename);
		}
	}

	return NULL;
}
