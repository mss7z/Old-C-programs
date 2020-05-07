/*
kadai4-2�Ő��삵���P��e�X�g�̗��K�v���O�������t�@�C������P��f�[�^��ǂݍ��ނ悤�ɕύX�����B
�P��f�[�^�́A���ԍ��A���{��A�p�P��̏��ɋL�^����Ă��āAtab��X�y�[�X�ŋ�؂�B
�����ɁA���̓_�����ǂ����B
������@�́A�قƂ�ǕύX����Ă��Ȃ����A�t�@�C���𑀍삷�邽�߂̕������ǉ����ꂽ�B
�܂��A���O�t�@�C���ɂǂ̉p�P�ꂪ�������s���������������L�^����悤�ɂ����B
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
	printf("�P��e�X�g���K�p�v���O����\n\n");
	if((logFileP=fopen(LOG_FILENAME,"a"))==NULL){
		printf("�G���[�I���O�t�@�C�� %s ���J���܂���\n",LOG_FILENAME);
		exit(EXIT_FAILURE);
	}
	writeLogTime();
	fprintf(logFileP,"Program start.\n");
	printf("�܂��A�V�����t�@�C����ǂݍ���ł�������\n");
	readNewFile();
	if(nowFileName[0]=='\0'){
		printf("�I�����܂�\n");
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
		printf("\n�������܂��� (���݂̃t�@�C��: %s )\n",nowFileName);
		printf("a.���X�g���ŏ�����ɒ���\n");
		printf("b.���X�g���V���b�t��\n");
		printf("c.���X�g�����ׂĕ\��\n");
		printf("t.���X�g�̏��ɖ����o��\n");
		printf("n.�V�����t�@�C�����J��\n");
		printf("r.���ݓǂݍ���ł���t�@�C�����ēǂݍ��݂��čX�V����\n");
		printf("q.�I��\n");
		printf(">");
		
		if(getStr(modeInput,MODEINPUT_SIZE)!=1){
			printf("\n�����͈�������͂��Ă�������\n");
			continue;
		}
		
		printf("\n");
		switch(modeInput[0]){
			case 'a':
				makeListFromFirst();
				printf("���X�g���ŏ�����ɂ��܂���\n");
				break;
			case 'b':
				makeListShuffle();
				printf("���X�g���V���b�t�����܂���\n");
				break;
			case 'c':
				printf("���X�g��\�����܂�\n");
				dispList();
				break;
			case 't':
				printf("���X�g�̏��ɖ����o���܂�\n");
				questionS();
				break;
			case 'n':
				printf("�V���Ƀt�@�C�����J���܂�\n");
				readNewFile();
				break;
			case 'r':
				printf(" %s ���ēǂݍ��݂��܂�\n",nowFileName);
				readFile(nowFileName);
				break;
			case 'q':
				printf("�I�����܂�\n");
				return;
			default:
				printf("�s���ȓ��͂ł�\n");
				continue;
		}
	}
}

void readNewFile(){
	char fileName[FILENAME_MAX]="";
	while(true){
		printf("�J���t�@�C��������͂��Ă�������\n>");
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
		printf(" %s ���J���܂���\n",fileName);
		return FILEREAD_FAILED;
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
	if(toeicSize==0){
		if(ret==EOF){
			printf("��̃t�@�C���ł�\n");
		}else{
			printf("�ǂݍ��߂܂���\n");
		}
		return FILEREAD_FAILED;
	} 
	if(!(toP<toeic+TOEICSIZE_MAX)){
		printf("�ǂݍ��߂���(%d��)�ɒB�������ߓǂݍ��݂𒆒f���܂�\n",TOEICSIZE_MAX);
	}else if(ret!=EOF){
		printf(" %d �s�ڂ̃t�@�C���̌`���ɖ�肪���邽�ߓǂݍ��݂𒆒f���܂�\n",toeicSize+1);
	}
	printf(" %d ���ǂݍ��݂܂���\n",toeicSize);
	makeListFromFirst();
	return FILEREAD_SUCCESS;
}

void dispList(){
	DICT **listPP=list;
	printf("���ԍ�  �p�P�� \t���{��\n");
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
	//list�̃����_���̓����������
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
	fprintf(logFileP,"�J�n �t�@�C���� %s \n",nowFileName);
	printf("-q�œr���I��\n");
	printf("-s�ł��̉p�P��̍ŏ��̈ꕶ����\��\n");
	while(listPP<toeicSize+list){
		printf("\n�i��: %d/%d\n",val+1,toeicSize);
		writeLogTime();
		fprintf(logFileP,"%d %s %s ->",(*listPP)->number,(*listPP)->English,(*listPP)->Japanese);
		switch(question(*listPP)){
			case CORRECT:
				correctVal++;
				val++;
				fprintf(logFileP,"����\n");
				break;
			case INCORRECT:
				val++;
				fprintf(logFileP,"�s����\n");
				break;
			case EXIT:
				fprintf(logFileP,"�r���I��\n");
				goto EXITLOOP;
			default:
				break;
		}
		listPP++;
	}
	EXITLOOP:;
	printf("\n��������: %d\n",val);
	if(val==0) val=1;//�[�����Z�h�~
	percentage=((double)correctVal/(double)val)*100.0;
	printf("�������@�@: %.2lf%\n",percentage);
	writeLogTime();
	fprintf(logFileP,"�I�� �������� %d ������ %.2lf%\n",val,percentage);
	return;
}
int question(const DICT *dataP){
	char input[STRSIZE]={'\0'};
	printf("���ԍ�: %d\n",dataP->number);
	printf("�u%s�v�̉p�P���\n",dataP->Japanese);
	while(true){
		printf(">");
		getStr(input,STRSIZE);
		if(input[0]=='-'){
			switch(input[1]){
				case 'q':
					printf("�r���ł����I�����܂�\n");
					return EXIT;
				case 's':
					printf("�ŏ��̈ꕶ���� %c �ł�\n",dataP->English[0]);
					break;
				default:
					printf("�s���ȓ��͂ł�\n");
					break;
			}
		}else if(strcmp(input,dataP->English)==0){
			printf("�����ł�\n");
			return CORRECT;
		}else{
			printf("�s�����ł�\n");
			printf("������������ %s �ł�\n",dataP->English);
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