/*
kadai4-2で制作した単語テストの練習プログラムをファイルから単語データを読み込むように変更した。
単語データは、問題番号、日本語、英単語の順に記録されていて、tabやスペースで区切る。
同時に、他の点も改良した。
操作方法は、ほとんど変更されていないが、ファイルを操作するための部分が追加された。
また、ログファイルにどの英単語が正解し不正解だったかを記録するようにした。
*/

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#define STRSIZE 63
#define TOEICSIZE_MAX 100
#define LOG_FILENAME "toeicLog.txt"

enum{
	INCORRECT,
	CORRECT,
	EXIT,
	FILEREAD_SUCCESS,
	FILEREAD_FAILED,
};

typedef struct dict{
	int number;
	char Japanese[STRSIZE];
	char English[STRSIZE];
}DICT;

DICT toeic[TOEICSIZE_MAX];
DICT *list[TOEICSIZE_MAX]={NULL};
int toeicSize=0;
char nowFileName[FILENAME_MAX]="";
FILE *logFileP=NULL;

void menu();
int getStr(char[],const int);
void makeListFromFirst();
void makeListShuffle();
void dispList();
void questionS();
int question(const DICT*);
void readNewFile();
int readFile(const char[]);
void writeLogTime();

int main(){
	printf("単語テスト練習用プログラム\n\n");
	if((logFileP=fopen(LOG_FILENAME,"a"))==NULL){
		printf("エラー！ログファイル %s が開けません\n",LOG_FILENAME);
		exit(EXIT_FAILURE);
	}
	writeLogTime();
	fprintf(logFileP,"Program start.\n");
	printf("まず、新しいファイルを読み込んでください\n");
	readNewFile();
	if(nowFileName[0]=='\0'){
		printf("終了します\n");
		return EXIT_SUCCESS;
	}
	menu();
	writeLogTime();
	fprintf(logFileP,"Program stop.\n");
	fclose(logFileP);
	return EXIT_SUCCESS;
}

void menu(){
	const int MODEINPUT_SIZE=3;
	char modeInput[MODEINPUT_SIZE];
	
	while(true){
		printf("\n何をしますか (現在のファイル: %s )\n",nowFileName);
		printf("a.リストを最初からに直す\n");
		printf("b.リストをシャッフル\n");
		printf("c.リストをすべて表示\n");
		printf("t.リストの順に問題を出す\n");
		printf("n.新しいファイルを開く\n");
		printf("r.現在読み込んでいるファイルを再読み込みして更新する\n");
		printf("q.終了\n");
		printf(">");
		
		if(getStr(modeInput,MODEINPUT_SIZE)!=1){
			printf("\n文字は一つだけ入力してください\n");
			continue;
		}
		
		printf("\n");
		switch(modeInput[0]){
			case 'a':
				makeListFromFirst();
				printf("リストを最初からにしました\n");
				break;
			case 'b':
				makeListShuffle();
				printf("リストをシャッフルしました\n");
				break;
			case 'c':
				printf("リストを表示します\n");
				dispList();
				break;
			case 't':
				printf("リストの順に問題を出します\n");
				questionS();
				break;
			case 'n':
				printf("新たにファイルを開きます\n");
				readNewFile();
				break;
			case 'r':
				printf(" %s を再読み込みします\n",nowFileName);
				readFile(nowFileName);
				break;
			case 'q':
				printf("終了します\n");
				return;
			default:
				printf("不明な入力です\n");
				continue;
		}
	}
}

void readNewFile(){
	char fileName[FILENAME_MAX]="";
	while(true){
		printf("開くファイル名を入力してください\n>");
		getStr(fileName,FILENAME_MAX);
		if( strcmp(fileName,"-q") == 0){
			return;
		}
		if( readFile(fileName)==FILEREAD_SUCCESS){
			strcpy(nowFileName,fileName);
			break;
		}
	}
	return;
}

int readFile(const char fileName[]){
	FILE *fp;
	DICT *toP=toeic;
	DICT inputTemp;
	int ret;
	if( (fp=fopen(fileName,"r")) == NULL ){
		printf(" %s が開けません\n",fileName);
		return FILEREAD_FAILED;
	}
	while(toP<toeic+TOEICSIZE_MAX){
		ret=fscanf(fp,"%d %s %s",&(inputTemp.number),inputTemp.Japanese,inputTemp.English);
		if(ret==3){
			*toP=inputTemp;
			toP++;
		}else{
			if(ret!=EOF){
				toP--;//fscanfの返り値がEOFでない場合ファイルの形式に問題があったということなのでtoPがさすデータは
						//書き込まれていない。なので１つ戻す。
			}
			break;
		}
	}
	fclose(fp);
	toeicSize=toP-toeic;
	if(toeicSize==0){
		if(ret==EOF){
			printf("空のファイルです\n");
		}else{
			printf("読み込めません\n");
		}
		return FILEREAD_FAILED;
	} 
	if(!(toP<toeic+TOEICSIZE_MAX)){
		printf("読み込める上限(%d件)に達したため読み込みを中断します\n",TOEICSIZE_MAX);
	}else if(ret!=EOF){
		printf(" %d 行目のファイルの形式に問題があるため読み込みを中断します\n",toeicSize+1);
	}
	printf(" %d 件読み込みました\n",toeicSize);
	makeListFromFirst();
	return FILEREAD_SUCCESS;
}

void dispList(){
	DICT **listPP=list;
	printf("問題番号  英単語 \t日本語\n");
	while(listPP<toeicSize+list){
		printf("%8d  %s \t%s\n",(*listPP)->number,(*listPP)->English,(*listPP)->Japanese);
		listPP++;
	}
	return;
}
void makeListFromFirst(){
	int i;
	for(i=0;i<toeicSize;i++){
		list[i]=toeic+i;
	}
	return;
}
void makeListShuffle(){
	int i,dataA,dataB;
	DICT *temp;
	srand((unsigned int)time(NULL));
	//listのランダムの二つを交換する
	for(i=0;i<toeicSize;i++){
		dataA=rand()%toeicSize;
		dataB=rand()%toeicSize;
		temp=list[dataA];
		list[dataA]=list[dataB];
		list[dataB]=temp;
	}
	return;
}

int getStr(char str[],const int strSize){
	int i;
	fgets(str,strSize,stdin);
	for(i=0;i<strSize-1;i++){
		if(str[i]=='\n'){
			str[i]='\0';
			return i;
		}
	}
	while(getchar()!='\n');
	return strSize-1;
}

void questionS(){
	int i,correctVal=0,val=0;
	double percentage;
	
	DICT **listPP=list;
	
	writeLogTime();
	fprintf(logFileP,"開始 ファイル名 %s \n",nowFileName);
	printf("-qで途中終了\n");
	printf("-sでその英単語の最初の一文字を表示\n");
	while(listPP<toeicSize+list){
		printf("\n進捗: %d/%d\n",val+1,toeicSize);
		writeLogTime();
		fprintf(logFileP,"%d %s %s ->",(*listPP)->number,(*listPP)->English,(*listPP)->Japanese);
		switch(question(*listPP)){
			case CORRECT:
				correctVal++;
				val++;
				fprintf(logFileP,"正解\n");
				break;
			case INCORRECT:
				val++;
				fprintf(logFileP,"不正解\n");
				break;
			case EXIT:
				fprintf(logFileP,"途中終了\n");
				goto EXITLOOP;
			default:
				break;
		}
		listPP++;
	}
	EXITLOOP:;
	printf("\n解いた回数: %d\n",val);
	if(val==0) val=1;//ゼロ除算防止
	percentage=((double)correctVal/(double)val)*100.0;
	printf("正答率　　: %.2lf%\n",percentage);
	writeLogTime();
	fprintf(logFileP,"終了 解いた回数 %d 正答率 %.2lf%\n",val,percentage);
	return;
}
int question(const DICT *dataP){
	char input[STRSIZE]={'\0'};
	printf("問題番号: %d\n",dataP->number);
	printf("「%s」の英単語は\n",dataP->Japanese);
	while(true){
		printf(">");
		getStr(input,STRSIZE);
		if(input[0]=='-'){
			switch(input[1]){
				case 'q':
					printf("途中ですが終了します\n");
					return EXIT;
				case 's':
					printf("最初の一文字は %c です\n",dataP->English[0]);
					break;
				default:
					printf("不明な入力です\n");
					break;
			}
		}else if(strcmp(input,dataP->English)==0){
			printf("正解です\n");
			return CORRECT;
		}else{
			printf("不正解です\n");
			printf("正しい答えは %s です\n",dataP->English);
			return INCORRECT;
		}
	}
}

void writeLogTime(){
	time_t timeData=time(NULL);
	struct tm *jt=localtime(&timeData);
	fprintf(logFileP,"%d/%02d/%02d ",jt->tm_year+1900,jt->tm_mon+1,jt->tm_mday);
	fprintf(logFileP,"%02d:%02d:%02d :",jt->tm_hour,jt->tm_min,jt->tm_sec);
	return;
}