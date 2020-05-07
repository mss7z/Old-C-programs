/*
kadai5-4 (1) �̒P��e�X�g�̗��K�v���O�����ƃZ�b�g�Ŏg���B
���K�v���O�����Ŏg���P��̃f�[�^�̓������t�@�C�����쐬�ҏW�ł���B
�܂��A�t�@�C�����J���ă������Ƀf�[�^���ڂ����A�V�K�쐬������B
���ɁA��������ҏW����B
�Ō�ɁA�t�@�C���ɏ㏑���ۑ�(�������݊J���Ă���t�@�C�����ɏ�������)���A���O��t���ĕۑ�����B
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
	printf("�P��e�X�g���K�p�̃t�@�C�����쐬����v���O����\n");
	reset();
	menu();
	
	return 0;
}

void menu(){
	const int MODEINPUT_SIZE=3;
	char modeInput[MODEINPUT_SIZE];
	
	while(true){
		printf("\n�������܂��� (���݂̃t�@�C��: %s ",nowFileName);
		if(isEditingFile){
			if(isSaved){
				printf("�ۑ��ς�");
			}else{
				printf("���ۑ�");
			}
		}
		printf(" )\n");
		printf("n.�V�K�쐬\n");
		printf("r.�t�@�C����ǂݍ���\n");
		printf("w.�t�@�C���ɏ㏑��\n");
		printf("a.�t�@�C���ɖ��O��t���ĕۑ�\n");
		printf("c.�ҏW���̃f�[�^��������\n");
		printf("d.�ҏW���̃f�[�^�����ׂĕ\��\n");
		printf("p.�ҏW���̃f�[�^�ɒǋL\n");
		printf("e.�ҏW�ԍ����w�肵�ĕҏW\n");
		printf("q.�I��\n");
		printf(">");
		
		if(getStr(modeInput,MODEINPUT_SIZE)!=1){
			printf("\n�����͈�������͂��Ă�������\n");
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
					printf("�܂��A���O��t���ĕۑ����Ă�������\n");
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
					printf("�I�����܂�\n");
					return;
				}
				break;
			default:
				printf("�s���ȓ��͂ł�\n");
				continue;
		}
	}
}

void editToeic(){
	char inputTemp[STRSIZE]="";
	int editNumber;
	DICT dictTemp;
	if(toeicSize==0){
		printf("�ҏW�ΏۂƂȂ�f�[�^������܂���\n");
		return;
	}
	while(true){
		printf("\n�ҏW�������ҏW�ԍ����w�肵�Ă�������\n");
		while(true){
			if(getStrMsgQuit(inputTemp,STRSIZE,"�ҏW�ԍ� >")==-1){
				return;
			}
			editNumber=strtol(inputTemp,NULL,10);
			if(0<editNumber && editNumber<toeicSize+1){
				editNumber--;
				break;
			}else{
				printf("���̕ҏW�ԍ������݂��Ȃ����A���͂ɖ�肪����܂�\n");
			}
		}
		printf("�ҏW�ԍ� %d �̌��݂̓��e\n",editNumber+1);
		dispDict(editNumber+toeic);
		
		printf("\n�ҏW�ԍ� %d �ɏ㏑��������e����͂��Ă�������\n",editNumber+1);
		if(inputDict(&dictTemp)==INPUT_QUIT){
			printf("���͂𒆎~���܂�\n");
			continue;
		}
		printf("�ȉ��̓��e��ҏW�ԍ� %d �ɏ㏑�����܂�\n",editNumber+1);
		dispDict(&dictTemp);
		if(isUserOK()){
			*(editNumber+toeic)=dictTemp;
			isSaved=false;
		}else{
			printf("�ҏW���e��j�����܂���\n");
		}
	}
}

void inputToeic(){
	if(toeicSize<TOEICSIZE_MAX){
		printf("�f�[�^�ւ̒ǋL���s���܂�\n");
		printf("�I������ɂ� -q ����͂��Ă�������\n");
	}
	while(toeicSize<TOEICSIZE_MAX){
		printf("\n�ҏW�ԍ�: %d \n",toeicSize+1);
		if(inputDict(toeic+toeicSize)==INPUT_QUIT){
			printf("�ҏW�ԍ� %d �ւ̓��͂𒆎~���A�I�����܂�\n",toeicSize+1);
			break;
		}
		isSaved=false;
		toeicSize++;
	}
	if(!(toeicSize<TOEICSIZE_MAX)){
		printf("�f�[�^�̍ő吔( %d�� )�ɒB�������߂���ȏ�ǋL�ł��܂���\n",TOEICSIZE_MAX);
	}
}

int inputDict(DICT *data){
	char inputTemp[STRSIZE]="";
	while(true){
		if(getStrMsgQuit(inputTemp,STRSIZE,"���ԍ� >")==-1){
			return INPUT_QUIT;
		}
		data->number=strtol(inputTemp,NULL,10);
		if(data->number>0){
			break;
		}else{
			printf("���͂ɖ�肪����܂�\n");
		}
	}
	while(true){
		switch(getStrMsgQuit(inputTemp,STRSIZE,"�p�P�� >")){
			case 0:
				printf("���͂��Ă�������\n");
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
		switch(getStrMsgQuit(inputTemp,STRSIZE,"���{�� >")){
			case 0:
				printf("���͂��Ă�������\n");
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
	printf("���ԍ�:%d\n",dataP->number);
	printf("�p�P��  :%s\n",dataP->English);
	printf("���{��  :%s\n",dataP->Japanese);
	return;
}

void dispToeic(){
	DICT *toP=toeic;
	if(toeicSize==0){
		printf("�f�[�^������܂���\n");
	}else{
		printf("�ԍ�  ���ԍ�  �p�P�� \t���{��\n");
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
	printf("�f�[�^�����������܂�\n");
	if(isUserOK()){
		toeicSize=0;
		isSaved=false;
		printf("���������܂���\n");
	}else{
		printf("�L�����Z�����܂���\n");
	}
	return;
}

bool isUserOK(){
	const int INPUT_SIZE=3;
	char input[INPUT_SIZE];
	while(true){
		printf("��낵���ł���(y/n)\n>");
		if(getStr(input,INPUT_SIZE)!=1){
			printf("\n�����͈�������͂��Ă�������\n");
			continue;
		}
		switch(input[0]){
			case 'y':
				return true;
			case 'n':
				return false;
			default:
				printf("�s���ȓ��͂ł�\n");
				continue;
		}
	}
}

bool isFileContinueOK(){
	if(!isSaved){
		printf("�ҏW���̃f�[�^�̏�Ԃ͕ۑ�����Ă��܂���\n���̑���𑱍s����");
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
		printf("�J���t�@�C��������͂��Ă�������\n>");
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
		printf(" %s ���J���܂���\n",fileName);
		return FILE_FAILED;
	}
	while(toP<toeic+TOEICSIZE_MAX){
		ret=fscanf(fp,"%d %s %s",&(inputTemp.number),inputTemp.Japanese,inputTemp.English);
		if(ret==3){
			*toP=inputTemp;
			toP++;
		}else{
			if(ret!=EOF){
				toP--;//fscanf�̕Ԃ�l��EOF�łȂ��ꍇ�t�@�C���̌`���ɖ�肪�������Ƃ������ƂȂ̂�toP�������f�[�^��
						//�������܂�Ă��Ȃ��B�Ȃ̂łP�߂��B
			}
			break;
		}
	}
	fclose(fp);
	toeicSize=toP-toeic;
	if(!(toP<toeic+TOEICSIZE_MAX)){
		printf("�ǂݍ��߂���( %d�� )�ɒB�������ߓǂݍ��݂𒆒f���܂�\n",TOEICSIZE_MAX);
		isStopEm=true;
	}else if(ret!=EOF){
		printf(" %d �s�ڂ̃t�@�C���̌`���ɖ�肪���邽�ߓǂݍ��݂𒆒f���܂�\n",toeicSize+1);
		isStopEm=true;
	}
	if(isStopEm){
		printf("�ǂݍ��݂�r���Œ��f�������߁A���̃t�@�C���ɏ㏑������ƒ��f�����ȍ~�̃f�[�^��������\��������܂�\n");
	}
	printf(" %d ���ǂݍ��݂܂���\n",toeicSize);
	isSaved=true;
	isEditingFile=true;
	return FILE_SUCCESS;
}

void writeNewFile(){
	char fileName[FILENAME_MAX]="";
	while(true){
		printf("�V�K�쐬����t�@�C��������͂��Ă�������\n>");
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
		printf(" %s ���J���܂���\n",fileName);
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