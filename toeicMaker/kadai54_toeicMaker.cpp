/*
kadai5-4 (1) の単語テストの練習プログラムとセットで使う。
練習プログラムで使う単語のデータの入ったファイルを作成編集できる。
まず、ファイルを開いてメモリにデータを移すか、新規作成をする。
次に、何かしら編集する。
最後に、ファイルに上書き保存(同じ現在開いているファイル名に書き込む)か、名前を付けて保存する。
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define STRSIZE 63
#define TOEICSIZE_MAX 100


enum{
	FILE_SUCCESS,
	FILE_FAILED,
	INPUT_SUCCESS,
	INPUT_QUIT,
};

typedef struct dict{
	int number;
	char Japanese[STRSIZE];
	char English[STRSIZE];
}DICT;

bool isSaved;
bool isEditingFile;
DICT toeic[TOEICSIZE_MAX];
int toeicSize;
char nowFileName[FILENAME_MAX];

void menu();
int getStr(char[],const int);
int getStrMsgQuit(char[],const int,const char[]);
void reset();
void readNewFile();
int readFile(char[]);
void writeNewFile();
int writeFile(char[]);
void clearToeic();
bool isUserOK();
bool isFileContinueOK();
void dispDict(DICT *);
void dispToeic();
int inputDict(DICT *);
void inputToeic();
void editToeic();

int main(){
	printf("単語テスト練習用のファイルを作成するプログラム\n");
	reset();
	menu();
	
	return 0;
}

void menu(){
	const int MODEINPUT_SIZE=3;
	char modeInput[MODEINPUT_SIZE];
	
	while(true){
		printf("\n何をしますか (現在のファイル: %s ",nowFileName);
		if(isEditingFile){
			if(isSaved){
				printf("保存済み");
			}else{
				printf("未保存");
			}
		}
		printf(" )\n");
		printf("n.新規作成\n");
		printf("r.ファイルを読み込む\n");
		printf("w.ファイルに上書き\n");
		printf("a.ファイルに名前を付けて保存\n");
		printf("c.編集中のデータを初期化\n");
		printf("d.編集中のデータをすべて表示\n");
		printf("p.編集中のデータに追記\n");
		printf("e.編集番号を指定して編集\n");
		printf("q.終了\n");
		printf(">");
		
		if(getStr(modeInput,MODEINPUT_SIZE)!=1){
			printf("\n文字は一つだけ入力してください\n");
			continue;
		}
		
		printf("\n");
		switch(modeInput[0]){
			case 'n':
				if(isFileContinueOK()){
					reset();
				}
				break;
			case 'c':
				if(isFileContinueOK()){
					clearToeic();
				}
				break;
			case 'r':
				if(isFileContinueOK()){
					readNewFile();
				}
				break;
			case 'w':
				if(isEditingFile){
					writeFile(nowFileName);
				}else{
					printf("まず、名前を付けて保存してください\n");
					writeNewFile();
				}
				break;
			case 'a':
				writeNewFile();
				break;
			case 'd':
				dispToeic();
				break;
			case 'p':
				inputToeic();
				break;
			case 'e':
				editToeic();
				break;
			case 'q':
				if(isFileContinueOK()){
					printf("終了します\n");
					return;
				}
				break;
			default:
				printf("不明な入力です\n");
				continue;
		}
	}
}

void editToeic(){
	char inputTemp[STRSIZE]="";
	int editNumber;
	DICT dictTemp;
	if(toeicSize==0){
		printf("編集対象となるデータがありません\n");
		return;
	}
	while(true){
		printf("\n編集したい編集番号を指定してください\n");
		while(true){
			if(getStrMsgQuit(inputTemp,STRSIZE,"編集番号 >")==-1){
				return;
			}
			editNumber=strtol(inputTemp,NULL,10);
			if(0<editNumber && editNumber<toeicSize+1){
				editNumber--;
				break;
			}else{
				printf("その編集番号が存在しないか、入力に問題があります\n");
			}
		}
		printf("編集番号 %d の現在の内容\n",editNumber+1);
		dispDict(editNumber+toeic);
		
		printf("\n編集番号 %d に上書きする内容を入力してください\n",editNumber+1);
		if(inputDict(&dictTemp)==INPUT_QUIT){
			printf("入力を中止します\n");
			continue;
		}
		printf("以下の内容を編集番号 %d に上書きします\n",editNumber+1);
		dispDict(&dictTemp);
		if(isUserOK()){
			*(editNumber+toeic)=dictTemp;
			isSaved=false;
		}else{
			printf("編集内容を破棄しました\n");
		}
	}
}

void inputToeic(){
	if(toeicSize<TOEICSIZE_MAX){
		printf("データへの追記を行います\n");
		printf("終了するには -q を入力してください\n");
	}
	while(toeicSize<TOEICSIZE_MAX){
		printf("\n編集番号: %d \n",toeicSize+1);
		if(inputDict(toeic+toeicSize)==INPUT_QUIT){
			printf("編集番号 %d への入力を中止し、終了します\n",toeicSize+1);
			break;
		}
		isSaved=false;
		toeicSize++;
	}
	if(!(toeicSize<TOEICSIZE_MAX)){
		printf("データの最大数( %d件 )に達したためこれ以上追記できません\n",TOEICSIZE_MAX);
	}
}

int inputDict(DICT *data){
	char inputTemp[STRSIZE]="";
	while(true){
		if(getStrMsgQuit(inputTemp,STRSIZE,"問題番号 >")==-1){
			return INPUT_QUIT;
		}
		data->number=strtol(inputTemp,NULL,10);
		if(data->number>0){
			break;
		}else{
			printf("入力に問題があります\n");
		}
	}
	while(true){
		switch(getStrMsgQuit(inputTemp,STRSIZE,"英単語 >")){
			case 0:
				printf("入力してください\n");
				continue;
			case -1:
				return INPUT_QUIT;
			default:
				strcpy(data->English,inputTemp);
				break;
		}
		break;
	}
	while(true){
		switch(getStrMsgQuit(inputTemp,STRSIZE,"日本語 >")){
			case 0:
				printf("入力してください\n");
				continue;
			case -1:
				return INPUT_QUIT;
			default:
				strcpy(data->Japanese,inputTemp);
				break;
		}
		break;
	}
	return INPUT_SUCCESS;
}

void dispDict(DICT *dataP){
	printf("問題番号:%d\n",dataP->number);
	printf("英単語  :%s\n",dataP->English);
	printf("日本語  :%s\n",dataP->Japanese);
	return;
}

void dispToeic(){
	DICT *toP=toeic;
	if(toeicSize==0){
		printf("データがありません\n");
	}else{
		printf("番号  問題番号  英単語 \t日本語\n");
		while(toP<toeicSize+toeic){
			printf("%4d  %8d  %s \t%s\n",toP-toeic+1,toP->number,toP->English,toP->Japanese);
			toP++;
		}
	}
	return;
}

void reset(){
	toeicSize=0;
	strcpy(nowFileName,"NEW_DATA");
	isSaved=true;
	isEditingFile=false;
}

void clearToeic(){
	printf("データを初期化します\n");
	if(isUserOK()){
		toeicSize=0;
		isSaved=false;
		printf("初期化しました\n");
	}else{
		printf("キャンセルしました\n");
	}
	return;
}

bool isUserOK(){
	const int INPUT_SIZE=3;
	char input[INPUT_SIZE];
	while(true){
		printf("よろしいですか(y/n)\n>");
		if(getStr(input,INPUT_SIZE)!=1){
			printf("\n文字は一つだけ入力してください\n");
			continue;
		}
		switch(input[0]){
			case 'y':
				return true;
			case 'n':
				return false;
			default:
				printf("不明な入力です\n");
				continue;
		}
	}
}

bool isFileContinueOK(){
	if(!isSaved){
		printf("編集中のデータの状態は保存されていません\nこの操作を続行して");
		if(isUserOK()){
			return true;
		}else{
			return false;
		}
	}
	return true;
}

void readNewFile(){
	char fileName[FILENAME_MAX]="";
	while(true){
		printf("開くファイル名を入力してください\n>");
		getStr(fileName,FILENAME_MAX);
		if( strcmp(fileName,"-q") == 0){
			return;
		}
		if( readFile(fileName)==FILE_SUCCESS){
			strcpy(nowFileName,fileName);
			break;
		}
	}
	return;
}

int readFile(char fileName[]){
	FILE *fp;
	DICT *toP=toeic;
	DICT inputTemp;
	int ret;
	bool isStopEm=false;
	if( (fp=fopen(fileName,"r")) == NULL ){
		printf(" %s が開けません\n",fileName);
		return FILE_FAILED;
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
	if(!(toP<toeic+TOEICSIZE_MAX)){
		printf("読み込める上限( %d件 )に達したため読み込みを中断します\n",TOEICSIZE_MAX);
		isStopEm=true;
	}else if(ret!=EOF){
		printf(" %d 行目のファイルの形式に問題があるため読み込みを中断します\n",toeicSize+1);
		isStopEm=true;
	}
	if(isStopEm){
		printf("読み込みを途中で中断したため、このファイルに上書きすると中断した以降のデータが失われる可能性があります\n");
	}
	printf(" %d 件読み込みました\n",toeicSize);
	isSaved=true;
	isEditingFile=true;
	return FILE_SUCCESS;
}

void writeNewFile(){
	char fileName[FILENAME_MAX]="";
	while(true){
		printf("新規作成するファイル名を入力してください\n>");
		getStr(fileName,FILENAME_MAX);
		if( strcmp(fileName,"-q") == 0){
			return;
		}
		if( writeFile(fileName)==FILE_SUCCESS){
			strcpy(nowFileName,fileName);
			break;
		}
	}
	return;
}

int writeFile(char fileName[]){
	FILE *fp;
	DICT *toP=toeic;
	isSaved=true;
	if( (fp=fopen(fileName,"w")) == NULL ){
		printf(" %s が開けません\n",fileName);
		return FILE_FAILED;
	}
	while(toP<toeic+toeicSize){
		fprintf(fp,"%d\t%s\t%s\n",toP->number,toP->Japanese,toP->English);
		toP++;
	}
	fclose(fp);
	isEditingFile=true;
	return FILE_SUCCESS;
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

int getStrMsgQuit(char str[],const int strSize,const char msg[]){
	int leng;
	printf(msg);
	leng=getStr(str,strSize);
	if(strcmp(str,"-q")==0){
		return -1;
	}else{
		return leng;
	}
}