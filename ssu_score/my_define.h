// OPTION DEFINE
#define SET_OPTION "e:thpc"
#define NUM_OPTION 5
#define OPT_H 0
#define OPT_T 1
#define OPT_E 2
#define OPT_C 3
#define OPT_P 4

// MAX SIZE
#define PATH_SIZE 300
#define FILE_NAME 30
#define KEY_MAX 14

//SCORE DEFINE
#define COM_WRN 0.1
#define COM_ERR 0

//NUM of STD, ANS
#define STD_MAX 101
#define PRO_MAX 101

//DIRECTORY
#define P_DIR ".."

//A to a
#define UPtoLO 32 

// TIME LIMIT
#define TIME_LIMIT "5000000"

//SPECIAL CHARACTER
#define IS_SPECIAL(c) (((c) < 'A' || (c) > 'Z') && ((c) < 'a' || (c) > 'z') && ((c) < '0' || (c) > '9') && (c) != '_')
#define OPER_EQ 128

//STRUCT
typedef struct sss{
	char *stdId;
	float grade[PRO_MAX];
} student;                  // 학생 구조체

typedef struct ppp{
	char *proId;
	float score;
} problem;                  // 문제 구조체

typedef struct tree{
	int data;
	struct tree *left;
	struct tree *right;
} tree;                     // 트리 구조체

// NAME OF FILE
const char score_table[] = "score_table.csv";
const char score_save[] = "score.csv";
const char psKiller[] = "killer";

// TOKEN 
const char oper[] = "+-*/%!==&|^";    // 연산자배열
const char keyword[][20] = {        // 키워드 배열
	"char",
	"int",
	"short",
	"long",
	"float",
	"double",
	"const",
	"restrict",
	"singed",
	"unsigend",
	"extern",
	"static",
	"struct",
	"void",
};
char prOper[500];

char proList[PRO_MAX][FILE_NAME];       // 받은 파일 목록
char stdList[STD_MAX][FILE_NAME];       // 받은 학생 목록
char stdDir[PATH_SIZE];                 // 학생 폴더 이름
char ansDir[PATH_SIZE];                 // 정답 폴더 이름

// USING EXTERN
char opt[NUM_OPTION][6];                // 옵션을 저장하는 배열
char opt_arg[100][FILE_NAME];           // 옵션에 필요한 인자들을 저장하는 배열
student std[STD_MAX];                   // 학생 구조체 배열
problem pro[PRO_MAX];                   // 문제 구조체 배열
int num_problem;                        // 문제 개수를 저장하는 변수
int start_prog;                         // 프로그램 문제가 시작하는 번호를 알려주는 변수
int num_std;                            // 학생 수를 저장하는 변수
int	result_table;                       // 결과 테이블을 불러올때 사용하는 파일디스크립터 변수

// PRINT FUNCTION
void print_help(void)
{
	printf("Usage : ssu_score <STUDENTDIR> <TRUEDIR> [OPTION]\n" 
			"Option :\n"
			"-e <DIRNAME>\tprint error on 'DIRNAME/ID/qname_error.txt' file\n"
			"-t <QNAMES>\tcompile QNAME.c with -lpthread option\n"
			"-h\t\tprint usage\n"
			"-p\t\tprint student's score and total average\n"
			"-c <IDS>\tprint ID's score\n");
}	

// OPTION -C
void print_score()                                      // -c 옵션을 처리할 함수
{
	char buf[BUFSIZ * 3];
	int len;

	if ((result_table = open(score_save, O_RDONLY)) < 0) {
		fprintf(stderr, "open error for %s\n", score_save);
		exit(1);
	}

	char *p;

	len = read(result_table, buf, BUFSIZ * 3 - 1);
	for (int j = 0; j < opt[OPT_C][0]; j++) {
		p = strstr(buf, opt_arg[opt[OPT_C][j]]);       // 학생의 학번을 찾아서
		while (*p != '\n') {                           // 개행이 나올때까지 뒤로간다음
			p += 1;
		}
		while (*p != ',') {                             // 다시 ,이 나올때까지 앞으로 와서 합계점수의 맨 앞부분을 가르키는 주소값을 저장
			p -= 1;
		}
		printf("%s's score : %.1f\n", opt_arg[opt[OPT_C][j + 1]], atof(p + 1)); // 학번과 합계점수 출력
	}

	close(result_table);
}

// TOKEN FUNCTION
void tokInfill(char tok[][101])					// 받은 tokken을 지우면서 뒤에있는 tokken을 끌어옴 
{
	int i;

	for (i = 0; tok[i + 1][0] != 0; i++) {
		strcpy(tok[i], tok[i + 1]);
	}
	tok[i][0] = 0;
}

void tokSetting(char tok[][101])                // 짤려 있는 토큰을 보고서에 쓴 상태와 같이 셋팅 하는 함수
{
	int i;
	int check;
	int preLen;

	for (i = 0; tok[i][0] != 0; i++) {
		check = 0;
		if (i > 0) {
			preLen = strlen(tok[i - 1]);
		}

		if (tok[i][0] == '*' && tok[i][1] != '=') {
			if (i == 0) {
				for (int j = 0; tok[i][j] == '*'; j++) {
					strcat(tok[i], tok[i + 1]);
					tokInfill(&tok[i + 1]);
				}
			}
			else {
				for (int j = 0; j < 12; j++) {
					if (strcmp(tok[i - 1], keyword[j]) == 0) {
						if (IS_SPECIAL(tok[i + 1][0]) && tok[i + 1][0] != '*') {
							strcat(tok[i - 1], tok[i]);
							tokInfill(&tok[i]);
						}
						else {
							for (int k = 0; tok[i][k] == '*'; k++) {
								strcat(tok[i], tok[i + 1]);
								tokInfill(&tok[i + 1]);
							}
						}

						check = 1;
						break;
					}
				}

				if (check == 0) {
					if (IS_SPECIAL(tok[i - 1][preLen - 1])) {
						for (int k = 0; tok[i][k] == '*'; k++) {
							strcat(tok[i], tok[i + 1]);
							tokInfill(&tok[i + 1]);
						}
					}
				}

			}
		}
		else if ((tok[i][0] == '+' || tok[i][0] == '-') && (tok[i][0] == tok[i][1])) {
			if (i == 0 || IS_SPECIAL(tok[i - 1][0])) {
				strcat(tok[i], tok[i + 1]);
				tokInfill(&tok[i + 1]);
			}
			else {
				strcat(tok[i - 1], tok[i]);
				tokInfill(&tok[i]);
			}
		}
		else if (tok[i][0] == '[') {
			check = 1;
			while (check) {
				if (tok[i + 1][0] == ']') {
					check = 0;
				}
				strcat(tok[i], tok[i + 1]);
				tokInfill(&tok[i + 1]);
			}

			if (i > 0) {
				strcat(tok[i - 1], tok[i]);
				tokInfill(&tok[i]);
			}
		}
		else if (tok[i][0] == '(') {
			if (i > 0 && IS_SPECIAL(tok[i - 1][preLen - 1]) == 0) {
				strcat(tok[i - 1], tok[i]);
				tokInfill(&tok[i]);

				if (strstr(tok[i - 1], "sizeof") != NULL) {
					while (tok[i][0] != ')') {
						strcat(tok[i - 1], tok[i]);
						tokInfill(&tok[i]);
					}
					strcat(tok[i - 1], tok[i]);
					tokInfill(&tok[i]);
				}

				i--;
			}
			if (tok[i + 1][0] == ')') {
				strcat(tok[i], tok[i + 1]);
				tokInfill(&tok[i + 1]);
			}
			else {
				for (int j = 0; j < KEY_MAX; j++) {
					if (strstr(tok[i + 1], keyword[j]) != NULL) {
						while (tok[i][check] != ')') {
							strcat(tok[i], tok[i + 1]);
							tokInfill(&tok[i + 1]);
							check = strlen(tok[i]) - 1;
						}
						break;
					}
				}
			}
		}
		else if (tok[i][0] == '-') {
			if (i == 0 || IS_SPECIAL(tok[i - 1][preLen - 1])) {
				strcat(tok[i], tok[i + 1]);
				tokInfill(&tok[i + 1]);
			}
			else if (tok[i + 1][0] == '>') {
				strcat(tok[i], tok[i + 1]);
				tokInfill(&tok[i + 1]);
			}
		}
		else if (strchr(oper, tok[i][0]) != NULL && tok[i + 1][0] == '=') {
			strcat(tok[i], tok[i + 1]);
			tokInfill(&tok[i + 1]);
		}
		else if (tok[i][0] == '&') {
			if (i == 0 || (tok[i][1] == 0 && IS_SPECIAL(tok[i - 1][preLen - 1]))) {
				strcat(tok[i], tok[i + 1]);
				tokInfill(&tok[i + 1]);
			}
		}
		else if (tok[i][0] == '!' && tok[i][1] == 0) {
			strcat(tok[i], tok[i + 1]);
			tokInfill(&tok[i + 1]);
		}
		else if ((tok[i][0] == '=' || tok[i][0] == '|' || tok[i][0] == '&') && tok[i][0] == tok[i + 1][0]) {
			strcat(tok[i], tok[i + 1]);
			tokInfill(&tok[i + 1]);
		}
		else if (tok[i][0] == '~') {
			strcat(tok[i], tok[i + 1]);
			tokInfill(&tok[i + 1]);
		}
	}
}
void priorityOper() {                 // 연산자의 우선순위를 배열에 저장 하는 함수
	char arr[100];

	prOper['('] = 1;
	prOper['-' + OPER_EQ] = 2;
	prOper['*'] = 3;
	prOper['/'] = 3;
	prOper['%'] = 3;
	prOper['+'] = 4;
	prOper['-'] = 4;
	prOper['>' + OPER_EQ] = 5;
	prOper['<' + OPER_EQ] = 5;
	prOper['<'] = 6;
	prOper['>'] = 6;
	prOper['=' + OPER_EQ] = 7;
	prOper['!' + OPER_EQ] = 7;
	prOper['&'] = 8;
	prOper['^'] = 9;
	prOper['|'] = 10;
	prOper['&' + OPER_EQ] = 11;
	prOper['|' + OPER_EQ] = 12;
	prOper['='] = 14;
	prOper[','] = 15;
}
void swapTLR(tree *cur)                 // 트리의 왼쪽 오른쪽을 바꾸는 함수
{
	tree *tmp = cur->left;
	cur->left = cur->right;
	cur->right = cur->left;
}

// DECLARATION FUNCTION
void get_start_info(void);              // 프로그램이 시작할때 필요한 정보들을 저장하는 함수
void check_score_table(void);           // 스코어 테이블이 있는지 확인하고 없다면 만들어줄 함수
void make_killer(void);                 // 5초이상 동작하는 프로그램에 대해 프로그램 종료를 위해 필요한 프로그램을 작성하는 함수
int option_handling(int, char *[]);     // 옵션을 처리해줄 함수
void compile_answer(int, int);          // 정답파일들을 컴파일 해줄 함수
int score_program(const int, const int);            // 프로그램문제에서 나온 결과값을 정답 결과값과 비교하는 함수
float grade_student_program(int);                   // 한 학생에 대해 모든 프로그램문제를 채점해주는 함수
int token_seperator(const int, const int, char [BUFSIZ][101], char [20][BUFSIZ][101]);  // 토큰을 분리하는 함수 (이 함수 에서 tokSetting()을 부른다)
void freeTree(tree *);                  // 만들어 놓은 트리의 동적할당을 free()하는 함수
int makeTree(tree **, char[][101]);     // 트리를 만드는 함수 (이 함수 안에서 tokDfs()를 부른다)
void tokDfs(char [][101], tree *, const int, const int);    // 트리를 연산자 기준으로 DFS형식으로 저장하는 함수
int scoringDFS(tree *, tree *, char[][101], char[][101]);   // DFS 스타일로 학생의 답과 정답을 비교하는 함수
int score_void(const int, const int);   // 한 학생에 대해 모든 공백문제를 채점할 함수
void score_student();                   // 모든 학생에 대해 채점하는 함수
