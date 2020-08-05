
#define PROMPT "20152385>"
#define BACKUP_NAME "backup"
#define LOG_NAME "log.txt"

#define ROOT_HOME 3			// mac기준 root에서 home까지 / 3개
#define B_BACKUP 1
#define B_RECOVER 2

#define PATH_MAX 256
#define BUF_SIZ 1024
#define BACKUP_MAX 101

#define OPT_NUM 5
#define OPT_M 0
#define OPT_N 1
#define OPT_T 2
#define OPT_D 3
#define OPT_A 4
#define OPT_TABLE "mntda"

#define LOG_FORMAT "[%06lld %06lld]\t%s"

typedef struct _node {
	char filename[PATH_MAX];		// 백업할 파일 이름 
	int period;						// 백업 주기
	int opt[5];						// 받은 옵션
	char optarg[5][PATH_MAX];		// 옵션 인자
	char last[PATH_MAX];			// 마지막으로 백업된 파일 이름
	struct _node *prev;
	struct _node *next;
} node;

typedef struct _dList {
	node *head;				// list의 맨 앞 노드의 앞 노드 항상 비어있음
	node *cur;				// 현재 가르키고 있는 노드
	node *tail;				// list의 맨 마지막 노드 
	int size;
} dList;					// 따라서 헤드는 항상 비어있음

int initializing(int argc, char *argv[]);		// 시작시 초기화, usage를 처리할 함수
void prompt(void);						// prompt출력과 명령을 전달할 함수
long long getNowTime();					// 현재 시간을 long long형태로 리턴하는 함수
void getAbsolPath(char *fname);			// 파일명에 따라 절대 경로를 얻을 함수
char *getFileName(char *path, char *fname);		// 경로에서 파일명만 잘라낼 함수
long long getBackupPath(char *fname);	// fname을 백업경로로 바꿔줄 함수
void comAdd(char *com);					// add 명령
void comRemove(char *com);				// remove 명령
void comCompare(char *com);				// compare 명령
void comRecover(char *com);				// recover 명령
void comList();							// list 명령
int copyFile(char *path, char *newPath);		// 파일 복사
int backupFile(char *path, char *newPath, int mode);	// 주어진 파일에 대해 backup 실행
void optionD(const char *dname, char *com);	// add에 -d옵션 처리
void catFile(const char *path);
void *thread_backup(void *arg);
