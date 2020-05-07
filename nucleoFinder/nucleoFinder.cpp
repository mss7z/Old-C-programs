/*
�p�\�R���ɐڑ�����Ă���NUCLEO�̈ꗗ���擾���A
����NUCLEO��I������v���O����
�Ăяo������bat�t�@�C���ƕϐ�������肷�邽�߂ɕW���G���[�o�͂𑽗p����]�؃v���O����
E�n 2�N�܂ł̎��Ɠ��e + stdarg + enum ��������Γǂ߂�Ǝv��
*/

#include <stdio.h>
#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <ctype.h>

#define VERSION "0.6 ���ׁ[����"
#define NUM_OF_ALPHABET 26	//a����z�܂ł̃A���t�@�x�b�g�̐�

/*
��(�Ƃ���NUCLEO_F303RE�̏ꍇ)
code��074502216719E433585D724B
board code��0745

nucleo�̎�ނ̔��ʂɎg���̂�code�̍ŏ���4������board code�̂�
�i���ӁF�������ucode�v�uboard code�v�Ƃ��������g���Ă���킯�ł͂���܂���j
*/

#define CODE_LEN 255 		//code�̒���(���Ȃ�]�T��������255)
#define READ_BUFF_LEN 1024 	//fgets�œǂݍ��ވ�s�̃T�C�Y
#define BLIST_LEN 255 		//nucleoList.txt����ǂݍ��߂�{�[�h�̎�ނ̐�
#define BOARD_CODE_LEN 5 	//board code�̒���
#define BOARD_NAME_LEN 255 	//�uNUCLEO_F303RE�v�Ȃǂ̖��O�̒���

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

//sscanf�̕����������Ƃ��̃}�W�b�N�i���o�[��������₷���悤�ɂ��̊֐������ŏ��ɏ�����
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
		printm("ERROR: %s ���J���܂���\n",boardList);
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
			printm("WARNING: %s �� %d �s�ڂ̏������ُ�Ȃ��߁A�X�L�b�v���܂���\n",boardList,blistLen+1);
		}
	}
	fclose(fp);
	return;
}

void printHelp(){
	printm("\tNucleo Finder Version: %s\n\
	�ꕶ���̑啶���������̃A���t�@�x�b�g�������ɏ����ƁA�^�[�Q�b�g�̃h���C�u���Ƃ��ď������܂�\n\
	���ϐ�NUCLEO_LIST_TXT_PATH��nucleoList.txt�ւ̃p�X��ݒ肵�Ă�������\n\
	�I�v�V�����ꗗ\n\
	-h	�w���v��\��\n\
	-v	�o�[�W������\��\n\
	-e	bat�t�@�C���Ŏg�����߂Ƀ��b�Z�[�W��W���o�͂̑���ɕW���G���[�o�͂ŏo��\n\
	-l	nucleo�̈ꗗ��\��(�����ڑ����ĂȂ��Ƃ��� �c�O�Ȋ���(��b��) �ɕ\��\n\
	\n\
	�o�O���̖₢���킹��Twitter@mss7z\n\
",VERSION);
}

int main(int argc,char *argv[]){
	int i,len;
	char trgDrive;
	
	/*if(setvbuf(stdout, (char*)NULL, _IONBF, 0)!=0){
		printm("FATAL ERROR: stdout�̃o�b�t�@�����O�������Ɏ��s\n");
		return 1;
	}
	if(setvbuf(stderr, (char*)NULL, _IONBF, 0)!=0){
		printm("FATAL ERROR: stderr�̃o�b�t�@�����O�������Ɏ��s\n");
		return 1;
	}*/

	readBoardList();
	readNowList();
	for(i=1;i<argc;i++){
		len=strlen(argv[i]);
		//�h���C�u�����ƍl������ꕶ���̈������������Ƃ�
		if(len==1 && (trgDrive=checkDriveChar(argv[i][0]))!='\0'){
			//���̃h���C�u�������g���Ă��邩
			if(isBoardDrive(trgDrive)){
				printTarget(trgDrive);
				return 0;
			}else{
				printm("WARNING: �� %d �����Ŏw�肳�ꂽ %c �h���C�u��NUCLEO�ł͂���܂���\n",i,trgDrive);
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
					printm("WARNING: �I�v�V���� -e �͑� %d �����Ŏw�肳��Ă��邪�A�� 1 �����Ŏw��𐄏�\n",i);
					printm("WARNING: ����ȑO�̃��b�Z�[�W��stderr�ŏo�͂���Ă��Ȃ��\��������\n");
				}
				continue;
			case 'l':
				printNowList();
				return 0;
			default:
				printm("WARNING: �s���ȃI�v�V���� -%c\n",argv[i][1]);
				printHelp();
				break;
			}
		}else{
			printm("WARNING: �s���Ȉ��� %s\n",argv[i]);
			printHelp();
			continue;
		}
	}
	if(nlistLen==0){
		printm("ERROR: nucleoFinder�Ō��o�ł���{�[�h���ꖇ������܂���\n");
	}else if(nlistLen==1){
		printm("%c �h���C�u�� %s (�F���R�[�h %s )�������I�����܂���\n",nlist[0].drive,nlist[0].board,nlist[0].code);
		trgDrive=nlist[0].drive;
		printTarget(trgDrive);
		return 0;
	}else{
		printm("�����̃{�[�h��������܂���\n");
		printm("���̒����珑�����ݐ�̃h���C�u����I��őI�����Ă�������\n");
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
	char driveList[NUM_OF_ALPHABET+1]="";//null�����̂��߂�+1
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
	printmNoName("|Drive|          �{�[�h�� |                  ���ʃR�[�h |\n");
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
	printm("�h���C�u������͂��Ă������� ��>%c\n",nlist[0].drive-'A'+'a');
	printm("exit �Ɠ��͂���ƒ��f�ł��܂�\n");
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
				printm("�I��������I��ł�������\n");
			}
		}else{
			printm("�ꕶ���������͂��Ă�������\n");
		}
		miss++;
		if(miss<3){
		}else if(miss<4){
			printm("���v�ł���\n");
		}else if(miss<5){
			printm("���v����Ȃ������ł���\n");
		}else if(miss<8){
			printm("WARNING: ���g���̔]�݂��͐���ł͂���܂���\n");
		}else{
			printm("FATAL ERROR: �v���I�ȃG���[\n");
			printm("FATAL ERROR: ���[�U�[�̓��삪�ُ�ł�\n");
			printm("FATAL ERROR: ���̖��̓��[�U�[�����w�Z�����蒼���Ɖ�������\��������܂�\n");
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
	//MBED.HTM��ǂݍ����code�����o��
	//�l�X�g���[���Ă��݂܂���
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