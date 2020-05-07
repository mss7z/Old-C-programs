/************************************************************************
�����^���[�o�g���Ɏ����I�ɕ����Ă����v���O����
adb.exe�Ƀp�X���ʂ��ĂȂ��Ɠ����Ȃ��i�d�l�j
����
2038�N��蓋��
"Borland C++ 5.5.1 for Win32 Copyright (c) 1993, 2000 Borland"
���g�p�̂���

201712XX-201801XX
************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>//����I�w�b�_�[���}�N����`�̐X�̂悤���I

#define VER "6.7.2 " //����ł����̂��Ȃ�

#undef ERROR
#define ERROR 0xfff0
#define SUCCESS 0xffff

#define TARGET_FILE "G:\\ALMtemp\\SS.dat"
#define SS_BACKUP_DIR "G:\\ALMtemp\\SSbackup\\"
#define ADB_EXE "G:\\adb.exe " //���ɃX�y�[�X�����邱�ƁI�I

#define PFSIZE 1843200
#define WIDTH 600
#define HEIGHT 1024

#define ENTER printf("\n");
#define CHECK_NULL_MEM(X) if(X==NULL) program_stop(E_SHORTMEM,NULL);
#define CHECK_NULL_FILE(X,Y,Z) if(X==NULL) program_stop(Y,Z);

#pragma pack(1)//�p�f�B���O�������Ȃ��i�����̂��낤���H�j
//Bitmap��header(Windows Bitmap)
typedef struct {
	unsigned short int bfType;
	unsigned long  int bfSize;
	unsigned short int bfReserved1;
	unsigned short int bfReserved2;
	unsigned long  int bfOffBits;
	
	unsigned long  int bcSize;
	signed   long  int bcWidth;
	signed   long  int bcHeight;
	unsigned short int bcPlanes;
	unsigned short int bcBitCount;
	unsigned long  int biCompression;
	unsigned long  int biSizeImage;
	signed   long  int biXPixPerMeter;
	signed   long  int biYPixPerMeter;
	unsigned long  int biClrUsed;
	unsigned long  int biCirImportant;
}Bitmap_header;
//RGB888
typedef struct {
	unsigned char blue;
	unsigned char green;
	unsigned char red;
//	unsigned char unknown;//32bit�̎��Ɏg�p
}color;
#pragma pack()
typedef struct {
	unsigned short win;
	unsigned short lose;
	unsigned short restart;
	unsigned short playing;
	unsigned short matching;
	unsigned short menu;
	unsigned short home;
}number_times;

void wait(int);
void nprint(unsigned long int,number_times*);
void convert_dec(unsigned long int,char*);
void restart_app();
void program_stop(int,char*);
void mix_adb_command(char*);

void tap(int,int);
void swipe(int,int,int,int);

void mix_number(int,char*);
void mix_str(char*,char*);

int pfile_open(unsigned long int**,char*);
double compare(unsigned long int*,unsigned long int*);

int analysis();
int sb_get(unsigned long int**);
void action(unsigned long int);

int logging_ss(unsigned long int*,int);
void time_convert_hex(time_t,char*);

void bmp_write(unsigned long int*,char*);
void bh_default_write(Bitmap_header*);

enum {
	LOSE,
	PLAYING,
	MATCHING,
	WIN,
	MENU,
	HOME,
	ENDOFLIST,
	UNKNOWN,
	E_ERROR,
	E_SHORTMEM,
	E_FCOPEN,
	E_FCREAD,
	E_FCWRITE,
	E_UNKNOWN,
};
#define MIXFN(X) mix_str(file_name,X);return SUCCESS;
int make_file_name (int nnumber,char *file_name) {
	switch (nnumber) {
		case MENU:		MIXFN("menu.dat");
		case WIN:		MIXFN("win.dat");
		case LOSE:		MIXFN("lose.dat");
		case PLAYING:	MIXFN("playing.dat");
		case MATCHING:	MIXFN("matching.dat");
		case HOME:		MIXFN("home.dat");
		default:	return ERROR;
	}
}


int main () {
	unsigned long int i;
	char command[255]="devices";
	mix_adb_command(command);
	printf("\nAutomatic Lose Machine Ver.%s\nFOR �����^���[�o�g��\nStarting!\n",VER);
	system("mkdir G:\\ALMtemp\\SSbackup\\");
	system(command);
	analysis();
	logging_ss(NULL,UNKNOWN);
	wait(6);
	for(i=0;;i++){
		action(i);
	}
	//system("rmdir G:\\ALMtemp");
	//return 0;
}

//�^�b�v�Ȃǂ̖��߁i�s���j������
//������main�֐�����񐔂��󂯎��
void action (unsigned long int number) {
	static int play_screen=0;
	static int last_answer=0,same_number=0;
	static number_times timesn={0};
	int answer;
	ENTER;
	nprint(number,&timesn);
	answer=analysis();
	if(answer==last_answer) {
		same_number++;
		if(same_number>3) {
			printf("�t���[�Y���m�F�������߁A�A�v���̍ċN�������݂܂�\n");
			logging_ss(NULL,WIN);
			restart_app();
			same_number=0;
			timesn.restart++;
			return;
		}
	}else{
		same_number=0;
	}
	last_answer=answer;
	ENTER;
	switch (answer) {
		case WIN:
			printf("������ʂ�F��!!\n");
			logging_ss(NULL,WIN);
			tap(30,945);
			timesn.win++;
			play_screen=0;
			break;
		case LOSE:
			printf("�s�k��ʂ�F��!!\n");
			logging_ss(NULL,LOSE);
			tap(30,945);
			timesn.lose++;
			play_screen=0;
			break;
		case PLAYING:
			printf("������ʂ�F��!!\n");
			if(play_screen==0){
				tap(1,230);
				play_screen=1;
			}else{
				tap(599,230);
				play_screen=0;
			}
			timesn.playing++;
			break;
		case MATCHING:
			printf("�����J�n��ʂ�F��!!\n");
			timesn.matching++;
			wait(1);
			return;
		case MENU:
			printf("���j���[��ʂ�F��!!\n");
			timesn.menu++;
			tap(300,600);
			break;
		case HOME:
			printf("�z�[����ʂ��m�F!!\n�h�E�^���o�g���̋N�������݂܂�\n");
			timesn.home++;
			tap(300,740);
			wait(10);
			return;
		case UNKNOWN:
			printf("�Ď��s�����܂������A�\������Ă����ʂ����Ȃ̂��F���ł��܂���!!!\n�\�����Ȃ��G���[�ł�\n");
			program_stop(E_UNKNOWN,NULL);
		default:	return;
	}
	wait(2);
	return;
}

//�X�N���[���V���b�g���擾���ĕK�v�ɉ����ĕۑ����܂�
//���������X�N���[���V���b�g�̃|�C���^�A�����������
int logging_ss(unsigned long int *sspointer,int situation) {
	static unsigned long int *logss,*losess;
	static short setor=0;
	static char file_head[255]={0};
	static unsigned short counter=1;
	static time_t losetime,logtime;
	unsigned int i;
	ENTER;
	if(setor==0) {
		printf("�X�N���[���V���b�g�̃o�b�N�A�b�v�̃��������m�ۊJ�n\n");
		logss = (unsigned long int*) malloc(sizeof(unsigned long int)*PFSIZE);
		CHECK_NULL_MEM(logss);
		losess = (unsigned long int*) malloc(sizeof(unsigned long int)*PFSIZE);
		CHECK_NULL_MEM(losess);
		printf("����\n");
		
		time_convert_hex(time(NULL),file_head);
		setor=SUCCESS;
		return SUCCESS;
	}else if(situation==WIN) {
		FILE *fp;
		char file_name[511]=SS_BACKUP_DIR,number_str[31]={0};
		
		printf("�O��̔s�k��ʂ̃X�N���[���V���b�g�̕ۑ����J�n\n");
		
		//�v���O�����̋N�����Ԃ�ǉ�
		mix_str(file_name,file_head);
		//�񐔂�ǉ�
		mix_str(file_name,"_T");
		convert_dec(counter,number_str);
		mix_str(file_name,number_str);
		//�t�@�C���쐬���Ԃ�ǉ�
		mix_str(file_name,"_LT");
		time_convert_hex(losetime,number_str);
		mix_str(file_name,number_str);
		//�g���q��ǉ�
		mix_str(file_name,".bmp");
		
		bmp_write(losess,file_name);
		
		counter++;
	}else if (situation==LOSE){
		printf("�擾�����X�N���[���V���b�g��s�k��ʂƂ��ă}�[�N\n");
		for(i=0;i<PFSIZE;i++) {
			losess[i] = logss[i];
		}
		losetime=logtime;
		return SUCCESS;
	}else if (sspointer!=NULL){
		printf("�X�N���[���V���b�g�̃o�b�N�A�b�v���擾\n");
		for(i=0;i<PFSIZE;i++) {
			logss[i] = sspointer[i];
		}
		logtime=time(NULL);
		return SUCCESS;
	}
	return ERROR;
}

//�ق��̃R���p�C���Ŏg�����Ƃ��l���A������֐�(���Ԑ�p)
//���Ԃ��󂯎���āA������ɂ��ĕԂ��܂�
void time_convert_hex(time_t number,char *strp) {
	unsigned long int work;
	short i;
	strp[0]='0';
	strp[1]='x';
	for(i=(sizeof(time_t)*2)+1;i>1;i--){
		work=(unsigned long int)number&0xF;
		if(0<=work && work<=9)	strp[i]=work+'0';
		else					strp[i]=work-10+'A';
		number=number>>4;
	}
	return;
}

//�������Ŏ󂯎�����X�N�V���̃f�[�^�̏ꏊ�������|�C���^�A�������Ńt�@�C���̖��O���󂯎��
//�������̃|�C���^�̃f�[�^��bmp�ɕϊ����A�������̏ꏊ�ɕۑ�����
void bmp_write(unsigned long int *fbdata,char *file_name) {
	ENTER;
	unsigned long int i,j;
	//unsigned char padding[4]={0};//WIDTH��4�Ŋ���؂�Ȃ��Ƃ��̋l�ߕ��i�p�f�B���O�j
	//�\���̂�color�̓񎟌��z��̓��I�m��
	color **after_conversion=(color**)malloc(sizeof(color*)*HEIGHT);
	CHECK_NULL_MEM(after_conversion);
	for(i=0;i<HEIGHT;i++) {
		after_conversion[i]=(color*)malloc(sizeof(color)*WIDTH);
		CHECK_NULL_MEM(after_conversion[i]);
	}
	Bitmap_header header={0};
	FILE *bmp_fp;
	bh_default_write(&header);
	
	//bmp�ւ̕ϊ��̊j
	for(i=0;i<HEIGHT;i++) {
		unsigned long int line=(HEIGHT-i)*WIDTH;
		for(j=0;j<WIDTH;j++) {
			unsigned long int work=fbdata[line+j];
			after_conversion[i][j].red=work&0xff;
			after_conversion[i][j].green=work>>8&0xff;
			after_conversion[i][j].blue=work>>16&0xff;
			//after_conversion[i][j].unknown=0;//32bit�̎��Ɏg�p
		}
	}
	//��������
	bmp_fp=fopen(file_name,"wb");
	CHECK_NULL_FILE(bmp_fp,E_FCOPEN,file_name);
	fwrite(&header,sizeof(header),1,bmp_fp);
	for(i=0;i<HEIGHT;i++) {
		fwrite(after_conversion[i],sizeof(color),WIDTH,bmp_fp);
		//fwrite(padding,1,WIDTH-((WIDTH/4)*4),bmp_fp);//�v�Z�����M�Ȃ��v����
	}
	fclose(bmp_fp);
	free(after_conversion);
	return;
}

//�����Ŏ󂯎�����\���̂�Bitmap_header�̃|�C���^�̎����ꏊ�Ƀf�t�H���g�̒l����������
void bh_default_write(Bitmap_header *data) {
	data->bfType		=0x4D42;//�G���f�B�A���̂����Ŕ��]���Ă��܂��̂Ŕ��]���ď����Ă܂�
	data->bcSize		=40;
	data->bcWidth		=WIDTH;
	data->bcHeight		=HEIGHT;
	data->bcPlanes		=1;
	data->bcBitCount	=24;//24bit Bitmap
	return;
}

//���̖��̒ʂ�A�v�����ċN�����܂�
void restart_app() {
	ENTER;
	printf("�A�v���̍ċN�������܂�\n");
	swipe(295,0,295,30);//�S��ʂ��炠�̉�ʂ��o��
	tap(350,1000);//�l�p���{�^��
	swipe(100,250,100,10);//�A�v���̏I��
	tap(250,1000);//�z�[���{�^��
	tap(300,740);//�z�[���Ńh�E�^���o�g���̈ʒu
	printf("�A�v���̍ċN�����I�����܂���\n");
	wait(10);
	return;
}

//���݂̏�Ԃ̉�͂�����
//
int analysis() {
	static short setor=0;
	static unsigned long int *PCpointer[ENDOFLIST]={0};
	static char file_name[ENDOFLIST][255]={0};
	int i,j;
	ENTER;
	if(setor==0) {
		printf("\n��r���̃t�@�C����ǂݍ��݊J�n\n");
		for(i=0;i<ENDOFLIST;i++) {
			make_file_name(i,file_name[i]);
			pfile_open(&PCpointer[i],file_name[i]);
		}
		printf("�ǂݍ��݊���\n");
		setor=SUCCESS;
		return SUCCESS;
	}else{
		short counter=1;
		unsigned long int *Tpointer;//�擾�����X�N���[���V���b�g�̃f�[�^�������Ă���̈�ւ̃|�C���^
		short check=0;
		printf("���͊J�n\n");
		while (1){
			double compared_data;
			double max=0;
			
			//�X�N���[���V���b�g�擾
			sb_get(&Tpointer)==ERROR;
			//��r
			printf("\n��r��\t\t�ގ���\n");
			for(i=0;i<ENDOFLIST;i++) {
				printf("\"%s\"\t",file_name[i]);
				if(max>80) {
					printf("���ł�80%%�ȏ�̃f�[�^����̂��߃L�����Z��\n");
					continue;
				}
				compared_data = compare(PCpointer[i],Tpointer);
				printf("%05.2lf%%\n",compared_data);
				if(compared_data>max) {
					max=compared_data;
					check=i;
				}
			}
			if(max<40.0) {
				printf("\n�ő�l��40.00%%������%05.2lf%%�̂��ߍēx���͂��܂�\n\n",max);
				counter++;
				if(counter>3)return UNKNOWN;
			}else{
				break;
			}
		}
		printf("���͌���:\"%s\"\n\n",file_name[check]);
		logging_ss(Tpointer,UNKNOWN);
		free(Tpointer);
		return check;
	}
}

//�X�N���[���V���b�g�i�X�N���[���o�b�t�@�j���擾���󂯂Ƃ����|�C���^�̒���sb�̃f�[�^�̃|�C���^������
//�����̓|�C���^�̃|�C���^
//�擾�����X�N���[���V���b�g�̃f�[�^������|�C���^  �ւ̃|�C���^(�ڂ�����analysis�֐���ǂ��)
int sb_get (unsigned long int **Tpointerp) {
	ENTER;
	printf("�X�N���[���V���b�g�擾�J�n\n");
	char command[255]="pull /dev/graphics/fb0 ";
	mix_adb_command(command);
	swipe(295,0,295,30);
	mix_str(command,TARGET_FILE);
	system(command);
	if(pfile_open(Tpointerp,TARGET_FILE)==ERROR) {
		printf("�擾���s\n");
		program_stop(E_ERROR,NULL);
	}
	printf("�X�N���[���V���b�g�擾����\n");
	return SUCCESS;
}

//�t�@�C�����J���w�肳�ꂽ�|�C���^�������Ă���|�C���^�Ƀ��������m�ۂ��A�f�[�^�𒆂ɓ����
//�������͕ۑ��ꏊ���w�肷��|�C���^  �ւ̃|�C���^  ���������m�ۂ��Ă��̃A�h���X����ꂽ�|�C���^�ւ̃|�C���^
//�������̓t�@�C���̖��O(���̂܂�fopen��fread�ɓ�����)
int pfile_open (unsigned long int **hpointer,char *file_name) {
	FILE *fp;
	ENTER;
	printf("�������̊m�ۂ��J�n\n");
	*hpointer = (unsigned long int*) malloc(sizeof(unsigned long int)*PFSIZE);
	CHECK_NULL_MEM(hpointer);
	printf("����\n�t�@�C���̓W�J���J�n\n");
	fp = fopen(file_name,"rb");
	CHECK_NULL_FILE(fp,E_FCOPEN,file_name);
	fread(*hpointer,sizeof(unsigned long int),PFSIZE,fp);
	fclose(fp);
	printf("\"%s\"��W�J����\n",file_name);
	return SUCCESS;
}

//��̔z��̔�r������
//�����͓�̔z��̃|�C���^
//�߂�l�͉��p�[�Z���g������������
double compare(unsigned long int *compareA,unsigned long int *compareB) {
	unsigned int i,check=0;
	for(i=0;i<PFSIZE;i++) {
		if(compareA[i]==compareB[i]) {
			check++;
		}
	}
	return ((double)check/i)*100.0;
}

//�������œn���ꂽ������̌��ɁA�������œn���ꂽ�����������
//�C���ς�(�������̖��O��file_name�ɂȂ��Ă�̂͂��Ƃ��ƃt�@�C���̖��O����邽�߂̊֐�����������
//        (�܂������ĂȂ������ł��A���݂܂��񋖂��Ă����������ł����܂�����i���ł�����Ƃ͌����ĂȂ��j
void mix_str (char *mdata,char *ddata) {
	int i,j;
	for(i=0;i<255;i++) {
		if(mdata[i]=='\0') {
			for(j=0;i+j<255;j++) {
				if(ddata[j]=='\0') return;
				mdata[i+j]=ddata[j];
			}
		}
	}
	return;
}

//�����ő҂����ꂽ�b���҂�
//"Windows.h"�K�v
void wait(int wait_time) {
	ENTER;
	printf("\n%d�b�ҋ@���Ă��܂�...\n",wait_time);
	Sleep(wait_time*1000);
}

//�v���O�����̏�Ԃ�\��
//�����ŉ�����s�����������
//"stdlib.h","time.h"�K�v
void nprint(unsigned long int number,number_times *tsn){
	time_t timenow=time(NULL);
	static time_t secondstart=timenow;
	static unsigned long int hour=0,minute=0,second=0;

	second=timenow-secondstart;
	if(second>=60){
		secondstart=timenow;
		minute++;
		second-=60;
		if(minute>=60){
			hour++;
			minute=0;
		}
	}
	
	system("cls");
	printf("Automatic Lose Machine Ver.%s\nFOR �����^���[�o�g��\n",VER);
	printf("================================ LOG ================================\n");

	printf("%10d���\n%02d����%02d��%02d�b�o��\n",number,hour,minute,second);
	printf("   \t ����\t �s�k\t ����\t�����O\t�ċN��\tMENU\tHOME\n");
	printf("(��)\t%5d\t%5d\t%5d\t%6d\t%6d\t%4d\t%4d\n\n",
			tsn->win,tsn->lose,tsn->playing,tsn->matching,tsn->restart,tsn->menu,tsn->home);
	//printf("����%4d��\t�s�k%4d��\t����%4d��\t�ċN��%4d��\n",tsn->win,tsn->lose,tsn->restart);
	//printf("�����J�n%2d��\t���j���[%2d��\t�z�[��%2d��\n\n",tsn->matching,tsn->menu,tsn->home);
	return;
}

//x���W��y���W��^����Ƃ������^�b�v����adb�����s
//"stdlib.h"�K�v
void tap(int x,int y){
	char command[255]="shell input tap ";
	mix_adb_command(command);
	ENTER;
	printf("x=%d,y=%d���^�b�v���܂�\n",x,y);
	mix_number(x,command);
	mix_number(y,command);
	system(command);
	return;
}

//�ŏ���x���W��y���W�A�Ō��x���W��y���W��^����Ƃ������X���C�v����adb�����s
void swipe(int fx,int fy,int tx,int ty) {//��ō\���̂ɂ���?
	char command[255]="shell input swipe ";
	mix_adb_command(command);
	ENTER;
	printf("x=%d,y=%d����x=%d,y=%d�ɃX���C�v���܂�\n",fx,fy,tx,ty);
	mix_number(fx,command);
	mix_number(fy,command);
	mix_number(tx,command);
	mix_number(ty,command);
	system(command);
	return;
}
	

//������̍Ō�ɐ��l��ǉ�
//�������Œǉ����������l�A�������Ő��l��ǉ�������������̃A�h���X��^����
void mix_number(int number,char *command) {
	char charactern[5]={0};
	int i,j;
	convert_dec((unsigned long int)number,charactern);
	for(i=0;i<255;i++){
		if(command[i]!='\0')continue;
		for(j=0;j<5&&i+j<255;j++) {
			if(charactern[j]=='\0'){
				command[i+j]=' ';
				return;
			}
			command[i+j]=charactern[j];
		}
	}
	return;
}

//���̐����l�𕶎���ɕϊ�
//�������ŕϊ����������A�������ŕϊ�������镶����̃A�h���X��^����
void convert_dec(unsigned long int number,char *output){
	char syori[5]={0};
	int sagyo,i,check;
	for(i=0;i<5;i++) {
		sagyo=number;
		sagyo/=10;
		sagyo*=10;
		syori[i]=number-sagyo;
		number/=10;
		if(number==0) {
			check=i;//syori��ł̍Ō�̂����̔ԍ�
			break;
		}
	}
	for(i=check;i>=0;i--) {
		output[check-i]=syori[i]+'0';
	}
	return;
}

//���̖��̒ʂ�Ƃ݂Ă̒ʂ�v���O�������ُ��~�����܂�
void program_stop(int situation,char *file_name) {
	printf("\n\n\nERROR:");
	if(file_name!=NULL) printf("�t�@�C��\"%s\"��",file_name);
	switch(situation) {
		case E_FCOPEN:		printf("�J���܂���\n");break;
		case E_FCREAD:		printf("�ǂݍ��݂ł��܂���\n");break;
		case E_FCWRITE:		printf("�������݂ł��܂���\n");break;
		case E_SHORTMEM:	printf("SYSTEM���[���ȃ������[�s���ł�\n");break;
		case E_UNKNOWN:		printf("���������Ő[���ȃG���[���������܂���\n");break;
		case E_ERROR:		break;
		default:			printf("�s���ȃG���[(0x%04X)���������܂���\n",situation);break;
	}
	printf("10�b��Ƀv���O�������I�����܂�\nERROR_CODE:0x%04X\n",situation);
	wait(10);
	exit(1);
}

//�󂯎�����������ADB_EXE�̌��ɒǉ�����
void mix_adb_command(char *adbcom) {
	unsigned short i;
	char command[255]=ADB_EXE;
	mix_str(command,adbcom);
	for(i=0;i<255;i++) {//�Ȃ񂩃��ȃR�[�h
		adbcom[i]=command[i];
		if(command[i]=='\0')return;
	}
	return;
}