#include <stdio.h>
#include <stdlib.h>

#define WIDTH 600
#define HEIGHT 1024
#define PFSIZE 1843200

#define ENTER printf("\n");

//Bitmap��header(Windows Bitmap)
#pragma pack(1)//�p�f�B���O�������Ȃ��i�����̂��낤���H�j
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

typedef struct {
	unsigned char blue;
	unsigned char green;
	unsigned char red;
//	unsigned char unknown;//32bit�̎��Ɏg�p
}color;
#pragma pack()



void bmp_write(unsigned long int*,char*);
void bh_default_write(Bitmap_header*);
int pfile_open(unsigned long int**,char*);
void mix_str(char*,char*);

int main (int argc,char *argv[]) {
	char default_file_name[]="G:\\tempx\\test.dat";
	char *file_name;
	int i,check;
	unsigned long int *original_data;
	FILE *original_fp;
	color test_color;
	printf("color_size=%d",sizeof(color));
	if(argc>1) {
		file_name = argv[1];
	} else {
		file_name = default_file_name;
	}
	pfile_open(&original_data,file_name);
	//��������
	for(i=0;;i++) {
		if(file_name[i]=='\0') {
			check=i;
			break;
		}
	}
	file_name[check-3]='\0';
	mix_str(file_name,"bmp");
	//�����܂łŊg���q��bmp�ɕύX
	bmp_write(original_data,file_name);
	return 0;
}

void bmp_write(unsigned long int *fbdata,char *file_name) {
	unsigned long int i,j;
	//unsigned char padding[4]={0};//WIDTH��4�Ŋ���؂�Ȃ��Ƃ��̋l�ߕ��i�p�f�B���O�j
	//�\���̂�color�̓񎟌��z��̓��I�m��
	color **after_conversion=(color**)malloc(sizeof(color*)*HEIGHT);
	for(i=0;i<HEIGHT;i++) after_conversion[i]=(color*)malloc(sizeof(color)*WIDTH);
	Bitmap_header header={0};
	FILE *bmp_fp;
	bh_default_write(&header);
	for(i=0;i<HEIGHT;i++) {
		unsigned long int line=(HEIGHT-i)*WIDTH;
		for(j=0;j<WIDTH;j++) {
			unsigned long int work=fbdata[line+j];
			//printf("work=%08X\n",work);
			after_conversion[i][j].red=work&0xff;
			after_conversion[i][j].green=work>>8&0xff;
			after_conversion[i][j].blue=work>>16&0xff;
			//after_conversion[i][j].unknown=0;//32bit�̎��Ɏg�p
		}
	}
	bmp_fp=fopen(file_name,"wb");
	fwrite(&header,sizeof(header),1,bmp_fp);
	for(i=0;i<HEIGHT;i++) {
		fwrite(after_conversion[i],sizeof(color),WIDTH,bmp_fp);
		//fwrite(padding,1,WIDTH-((WIDTH/4)*4),bmp_fp);//�v�Z�����M�Ȃ��v����
	}
	fclose(bmp_fp);
	free(after_conversion);
	return;
}

void bh_default_write(Bitmap_header *data) {
	data->bfType		=0x4D42;//�G���f�B�A���̂����Ŕ��]���Ă��܂��̂Ŕ��]���ď����Ă܂�
	data->bcSize		=40;
	data->bcWidth		=WIDTH;
	data->bcHeight		=HEIGHT;
	data->bcPlanes		=1;
	data->bcBitCount	=24;//24bit Bitmap
	return;
}

//�t�@�C�����J���w�肳�ꂽ�|�C���^�������Ă���|�C���^�Ƀ��������m�ۂ��A�f�[�^�𒆂ɓ����
//�������͕ۑ��ꏊ���w�肷��|�C���^  �ւ̃|�C���^  ���������m�ۂ��Ă��̃A�h���X����ꂽ�|�C���^�ւ̃|�C���^
//�������̓t�@�C���̖��O(���̂܂�fopen��fread�ɓ�����)
int pfile_open (unsigned long int **hpointer,char *file_name) {
	FILE *fp;
	ENTER;
	printf("�������̊m�ۂ��J�n\n");
	*hpointer = (unsigned long int*) malloc(sizeof(unsigned long int)*PFSIZE);
	if(*hpointer==NULL) {
		printf("���s\nSystem���[���ȃ������s��\n");
		exit(1);//-------------------------------------------------------�ύX
	}
	printf("����\n�t�@�C���̓W�J���J�n\n");
	fp = fopen(file_name,"rb");
	if(fp==NULL) {
		printf("\nError:\"%s\"���J���܂���\n",file_name);
		exit(1);//-------------------------------------------------------�ύX
	}
	fread(*hpointer,sizeof(unsigned long int),PFSIZE,fp);
	fclose(fp);
	printf("\"%s\"��W�J����\n",file_name);
	return 0;
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
