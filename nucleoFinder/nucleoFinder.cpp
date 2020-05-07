/*
パソコンに接続されているNUCLEOの一覧を取得し、
一台のNUCLEOを選択するプログラム
呼び出し元のbatファイルと変数をやり取りするために標準エラー出力を多用する脳筋プログラム
E系 2年までの授業内容 + stdarg + enum が分かれば読めると思う
*/

#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

#define VERSION "0.6 ☆べーた☆"
#define NUM_OF_ALPHABET 26	//aからzまでのアルファベットの数

/*
例(とあるNUCLEO_F303REの場合)
code→074502216719E433585D724B
board code→0745

nucleoの種類の判別に使うのはcodeの最初の4文字のboard codeのみ
（注意：公式が「code」「board code」という語句を使っているわけではありません）
*/

#define CODE_LEN 255 		//codeの長さ(かなり余裕を持って255)
#define READ_BUFF_LEN 1024 	//fgetsで読み込む一行のサイズ
#define BLIST_LEN 255 		//nucleoList.txtから読み込めるボードの種類の数
#define BOARD_CODE_LEN 5 	//board codeの長さ
#define BOARD_NAME_LEN 255 	//「NUCLEO_F303RE」などの名前の長さ

//#define ECHO printf("e$");
//#define PLT printf("e$[NclF] ");

typedef struct{
	char bcode[BOARD_CODE_LEN];
	char name[BOARD_NAME_LEN];
}aBoardList;

aBoardList blist[BLIST_LEN]={0};
int blistLen=0;

typedef struct{
	char drive;
	char code[CODE_LEN];
	char board[BOARD_NAME_LEN];
}aNowList;

aNowList nlist[NUM_OF_ALPHABET]={0};
int nlistLen=0;

FILE *msg=stdout;

void findDrive(char[]);
int getCode(char,char[]);
int getBoard(char[],char[]);
char checkDriveChar(char);
char inputBoardDrive();
void printTarget(char);
void readNowList();
void printNowList();
void printHelp();
int isBoardDrive(char);
void printm(const char*, ...);
void printmNoName(const char*, ...);

//sscanfの文字数制限とかのマジックナンバーをいじりやすいようにこの関数だけ最初に書いた
void readBoardList(){
	char *boardList;
	char boardListDefo[]="nucleoList.txt";
	boardList=getenv("NUCLEO_LIST_TXT_PATH");
	if(boardList==NULL){
		boardList=boardListDefo;
	}
	
	FILE *fp;

	char buff[READ_BUFF_LEN]="";
	char temp;
	
	fp=fopen(boardList,"r");
	if(fp==NULL){
		printm("ERROR: %s が開けません\n",boardList);
		exit(1);
	}
	while(fgets(buff,READ_BUFF_LEN,fp)!=NULL){
		
		if(buff[READ_BUFF_LEN-2]!='\0'){
			while((temp=fgetc(fp))!=EOF){
				if(temp=='\n')break;
			}
		}
		if(buff[0]=='#'){
			continue;
		}
		if(sscanf(buff,"%4s %254s",blist[blistLen].bcode,blist[blistLen].name)==2 && isspace(buff[4])){
			blistLen++;
		}else{
			printm("WARNING: %s の %d 行目の書式が異常なため、スキップしました\n",boardList,blistLen+1);
		}
	}
	fclose(fp);
	return;
}

void printHelp(){
	printm("\tNucleo Finder Version: %s\n\
	一文字の大文字小文字のアルファベットを引数に書くと、ターゲットのドライブ名として処理します\n\
	環境変数NUCLEO_LIST_TXT_PATHにnucleoList.txtへのパスを設定してください\n\
	オプション一覧\n\
	-h	ヘルプを表示\n\
	-v	バージョンを表示\n\
	-e	batファイルで使うためにメッセージを標準出力の代わりに標準エラー出力で出力\n\
	-l	nucleoの一覧を表示(一台も接続してないときは 残念な感じ(語彙力) に表示\n\
	\n\
	バグ等の問い合わせはTwitter@mss7z\n\
",VERSION);
}

int main(int argc,char *argv[]){
	int i,len;
	char trgDrive;
	
	/*if(setvbuf(stdout, (char*)NULL, _IONBF, 0)!=0){
		printm("FATAL ERROR: stdoutのバッファリング無効化に失敗\n");
		return 1;
	}
	if(setvbuf(stderr, (char*)NULL, _IONBF, 0)!=0){
		printm("FATAL ERROR: stderrのバッファリング無効化に失敗\n");
		return 1;
	}*/

	readBoardList();
	readNowList();
	for(i=1;i<argc;i++){
		len=strlen(argv[i]);
		//ドライブ文字と考えられる一文字の引数があったとき
		if(len==1 && (trgDrive=checkDriveChar(argv[i][0]))!='\0'){
			//そのドライブ文字が使われているか
			if(isBoardDrive(trgDrive)){
				printTarget(trgDrive);
				return 0;
			}else{
				printm("WARNING: 第 %d 引数で指定された %c ドライブはNUCLEOではありません\n",i,trgDrive);
				continue;
			}
		}else if(len==2 && argv[i][0]=='-'){
			switch(argv[i][1]){
			case 'h':
				printHelp();
				return 0;
			case 'v':
				printm("Nucleo Finder Version: %s",VERSION);
				return 0;
			case 'e':
				msg=stderr;
				if(i!=1){
					printm("WARNING: オプション -e は第 %d 引数で指定されているが、第 1 引数で指定を推奨\n",i);
					printm("WARNING: これ以前のメッセージはstderrで出力されていない可能性がある\n");
				}
				continue;
			case 'l':
				printNowList();
				return 0;
			default:
				printm("WARNING: 不明なオプション -%c\n",argv[i][1]);
				printHelp();
				break;
			}
		}else{
			printm("WARNING: 不明な引数 %s\n",argv[i]);
			printHelp();
			continue;
		}
	}
	if(nlistLen==0){
		printm("ERROR: nucleoFinderで検出できるボードが一枚もありません\n");
	}else if(nlistLen==1){
		printm("%c ドライブの %s (認識コード %s )を自動選択しました\n",nlist[0].drive,nlist[0].board,nlist[0].code);
		trgDrive=nlist[0].drive;
		printTarget(trgDrive);
		return 0;
	}else{
		printm("複数のボードが見つかりました\n");
		printm("次の中から書き込み先のドライブ名を選んで選択してください\n");
		printNowList();
		if((trgDrive=inputBoardDrive())!='\0'){
			printTarget(trgDrive);
			return 0;
		}else{
			return 1;
		}
	}
	//printNowList();
	
	return 0;
}

void printm(const char *str, ...){
	va_list args;
	fprintf(msg,"[nclf] ");
	va_start(args,str);
	vfprintf(msg,str,args);
	va_end(args);
	fflush(msg);
}

void printmNoName(const char *str, ...){
	va_list args;
	va_start(args,str);
	vfprintf(msg,str,args);
	va_end(args);
	fflush(msg);
}

void readNowList(){
	char driveList[NUM_OF_ALPHABET+1]="";//null文字のために+1
	int i;
	findDrive(driveList);
	nlistLen=0;
	for(i=0;driveList[i]!='\0' && i<NUM_OF_ALPHABET;i++){
		if(0==getCode(driveList[i],nlist[nlistLen].code)){
			getBoard(nlist[nlistLen].code,nlist[nlistLen].board);
			nlist[nlistLen].drive=driveList[i];
			
			//printf("code: %s board: %s \n",nlist[nlistLen].code,nlist[nlistLen].board);
			nlistLen++;
		}
	}
	return;
}

void printNowList(){
	int i;
	printmNoName("|Drive|          ボード名 |                  識別コード |\n");
	printmNoName("|-----|-------------------|-----------------------------|\n");
	for(i=0;i<nlistLen;i++){
		printmNoName("| %c:\\ | %17s | %27s |\n",nlist[i].drive,nlist[i].board,nlist[i].code);
	}
	return;
}

int isBoardDrive(char drive){
	int i;
	for(i=0;i<nlistLen;i++){
		if(drive==nlist[i].drive){
			return true;
		}
	}
	return false;
}

void printTarget(char drive){
	int i;
	for(i=0;i<nlistLen;i++){
		if(drive==nlist[i].drive){
			printf("DRIVE %c\n",drive);
			printf("BOARD %s\n",nlist[i].board);
			fflush(stdout);
			break;
		}
	}
	return;
}

char inputBoardDrive(){
	const int SIZE=10;
	char input[SIZE]="";
	char ans;
	int i,miss=0;
	printm("ドライブ名を入力してください 例>%c\n",nlist[0].drive-'A'+'a');
	printm("exit と入力すると中断できます\n");
	while(true){
		printmNoName(">");
		fgets(input,SIZE,stdin);
		for(i=0;i<SIZE;i++){
			if(input[i]=='\n')input[i]='\0';
		}
		if(strstr(input,"exit")!=NULL){
			break;
		}
		if(strlen(input)==1){
			ans=checkDriveChar(input[0]);
			if(isBoardDrive(ans)){
				return ans;
			}else{
				printm("選択肢から選んでください\n");
			}
		}else{
			printm("一文字だけ入力してください\n");
		}
		miss++;
		if(miss<3){
		}else if(miss<4){
			printm("大丈夫ですか\n");
		}else if(miss<5){
			printm("大丈夫じゃなさそうですね\n");
		}else if(miss<8){
			printm("WARNING: お使いの脳みそは正常ではありません\n");
		}else{
			printm("FATAL ERROR: 致命的なエラー\n");
			printm("FATAL ERROR: ユーザーの動作が異常です\n");
			printm("FATAL ERROR: この問題はユーザーが小学校からやり直すと解決する可能性があります\n");
			break;
		}
	}
	return '\0';
}

char checkDriveChar(char c){
	if('a'<=c && c<='z'){
		return c-'a'+'A';
	}else if('A'<=c && c<='Z'){
		return c;
	}else{
		return '\0';
	}
}

void findDrive(char list[]){
	DWORD driveBit=GetLogicalDrives();
	char drive;
	int listCount=0;
	for(drive='A';drive<='Z';drive++){
		if(driveBit&0x1){
			list[listCount]=drive;
			listCount++;
		}
		driveBit>>=1;
	}
	return;
}

int getCode(char drive,char code[]){
	char mbedhtm[]="A:\\MBED.HTM";
	mbedhtm[0]=drive;
	
	char search[]="device/?code=";
	int searchCount=0;
	
	enum{
		SEARCH,
		READ_CODE
	};
	int readStatus=0;
	
	int codeCount=0;
	
	FILE *fp;
	char buff[READ_BUFF_LEN]="";
	int i;
	fp=fopen(mbedhtm,"r");
	if(fp==NULL){
		return 1;
	}
	//MBED.HTMを読み込んでcodeを取り出す
	//ネストが深くてすみません
	while(fgets(buff,READ_BUFF_LEN,fp)!=NULL){
		for(i=0;i<READ_BUFF_LEN && buff[i]!='\0';i++){
			switch(readStatus){
			case SEARCH:
				//printf("search:%c buff[%3d]:%c\n",search[searchCount],i,buff[i]);
				if(search[searchCount]==buff[i]){
					searchCount++;
					if(search[searchCount]=='\0'){
						readStatus++;
					}
				}else{
					searchCount=0;
				}
				break;
			case READ_CODE:
				//printf("code:%c\n",buff[i]);
				if(!(codeCount<CODE_LEN) || buff[i]=='\"'){
					fclose(fp);
					return 0;
				}else{
					code[codeCount]=buff[i];
					codeCount++;
				}
				break;
			}
		}
	}
	fclose(fp);
	return 1;
}


int getBoard(char code[],char board[]){
	int i;
	for(i=0;i<blistLen;i++){
		if(strncmp(code,blist[i].bcode,BOARD_CODE_LEN-1)==0){
			strncpy(board,blist[i].name,BOARD_NAME_LEN);
			board[BOARD_NAME_LEN-1]='\0';
			return 0;
		}
	}
	strncpy(board,"UNKNOWN",BOARD_NAME_LEN);
	board[BOARD_NAME_LEN-1]='\0';
	return 1;
}