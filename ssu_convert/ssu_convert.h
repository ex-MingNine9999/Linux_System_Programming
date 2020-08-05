//DEFINE
#define PATH_MAX 300
#define BUFLEN 1024
#define VARLEN 128
#define OBJ_MAX 10
#define FUNC_MAX 20
#define NAME_MAX 50
#define ACMD_NUM 3
#define DATA_MAX 12
#define C_DATA 6
#define KEY_MAX 4
#define DEF_MAX 10
#define FUNC_NEW 6
#define FUNC_EXIT 7
#define KEY_IF 0

#define RET_CLASS 1234567890 
#define RET_KEY 1234567891
#define RET_SKIP 1234567892
#define RET_OPCB 1234567893

#define LINE_END "{};\n"

#define OPTION_DEF "jcpflr"
#define OPT_MAX 6
#define OPT_J 0
#define OPT_C 1
#define OPT_P 2
#define OPT_F 3
#define OPT_L 4
#define OPT_R 5

#define HEADER_TABLE "header"
#define HEADER_MAX 128

#define IS_ALPHA(c) (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z'))
#define IS_NUM(c) ((c) >= '0' && (c) <= '9')
#define IS_VAR(c) (IS_ALPHA(c) || IS_NUM(c) || (c) == '_')
#define UPPER(c) (c) -= 32

const char *AccessModifier[] = {
	"public",
	"private",
	"protected"
};

const char *DataType[] = {
	"int",
	"char",
	"float",
	"long",
	"short",
	"double",
	"String",
	"static",
	"boolean",
	"final",
	"File",
	"FileWriter",
};

const char *KeyWord[] = {
	"if",
	"else",
	"for",
	"while",
};

// DECLARATION FUNCTION
void optionHandling(int , char *[]);				// 받은 옵션을 저장할 함수
void setHeaderTable(void);							// 헤더 테이블에 있는 함수와 헤더를 매칭시켜주는 함수
void setFuncTable(void);							// java의 함수와 c의 함수를 매칭 시켜주는 함수
void initializing(void);							// 시작 초기화 함수
void convert(void);									// java파일을 c파일로 바꾸는 함수
int convertLine(char [BUFLEN], int);			// 한 줄 변환 함수
void insertTab(FILE *, int);						// 한 줄의 입력 앞에 탭을 넣어줄 함수
int convertDataDec(char *, int);					// 변수 선언 처리 함수 (배열처리)
void writeLine(FILE *, char *, int);				// 한 줄을 파일에 쓰는 함수
int extraction(char *, char *, char, int);			// 문자열, 주석 임시 추출 하는 함수
void addUsedFunc(int);								// 사용된 함수의 목록을 만들어 주는 함수
void changeDataType(char *, int, const char *);			//	java의 자료형을 C의 원하는 자료형으로 변환해주는 함수
void IOException(char *, int);						// java의 main함수에 IOException 처리가 있을경우 예외처리 
void varParsing(char *, char *, int);				// 함수앞에 있는 객체의 변수명을 파싱해오는 함수
int strInsert(char *, const char *, int);			// 문자열 중간에 문자열을 삽입하는 함수
void insertFile(int, char *);						// 파일의 맨 앞에 문자열을 삽입하는 함수 (ex. 헤더파일, 전처리문)
void optionProcessing(void);						// 옵션을 처리하는 함수
void makeMakefile(void);							// 메이크파일을 만드는 함수
