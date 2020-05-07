#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STRSIZE 63

enum{
	SYMBOL,
	NAME_JA,
	NAME_EN,
	STR_NUM,
};

typedef struct element{
	int number;
	char str[STR_NUM][STRSIZE];
	char weight[STRSIZE];
	int periodic;
	int group;
}ELEMENT;

ELEMENT table[]={
	{1,{"h","���f","hydrogen"},"1.00794",1,1},
	{2,{"he","�w���E��","helium"},"4.002602",1,18},
	{3,{"li","���`�E��","lithium"},"6.941",2,1},
	{4,{"be","�x�����E��","beryllium"},"9.012182",2,2},
	{5,{"b","�z�E�f","boron"},"10.811",2,13},
	{6,{"c","�Y�f","carbon"},"12.0107",2,14},
	{7,{"n","���f","nitrogen"},"14.0067",2,15},
	{8,{"o","�_�f","oxygen"},"15.9994",2,16},
	{9,{"f","�t�b�f","fluorine"},"18.9984032",2,17},
	{10,{"ne","�l�I��","neon"},"20.1797",2,18},
	{11,{"na","�i�g���E��","sodium"},"22.98976928",3,1},
	{12,{"mg","�}�O�l�V�E��","magnesium"},"24.305",3,2},
	{13,{"al","�A���~�j�E��","aluminium"},"26.9815386",3,13},
	{14,{"si","�P�C�f","silicon"},"28.0855",3,14},
	{15,{"p","����","phosphorus"},"30.973762",3,15},
	{16,{"s","����","sulfur"},"32.065",3,16},
	{17,{"cl","���f","chlorine"},"35.453",3,17},
	{18,{"ar","�A���S��","argon"},"39.948",3,18},
	{19,{"k","�J���E��","potassium"},"39.0983",4,1},
	{20,{"ca","�J���V�E��","calcium"},"40.078",4,2},
	{26,{"fe","�S","iron"},"55.845",4,8},
	{29,{"cu","��","copper"},"63.546",4,11},
	{30,{"zn","����","zinc"},"65.38",4,12},
};
//�@�������ʂ����炷���߂�table�̃f�[�^�̐���啝�Ɍ��炵�܂����B

const int tableSize=sizeof(table)/sizeof(table[0]);

int getStr(char[],const int);
void convStrSmall(char[]);

ELEMENT* serchElementStr(const char[]);
ELEMENT* serchElementNum(const int);
ELEMENT* serchElement(char[]);

void dispElement(ELEMENT*);
void dispStrCapitalize(const char[]);

int main(){
	char input[STRSIZE];
	ELEMENT *findP;
	printf("���f���������܂�\n -q �ŏI��\n");
	while(true){
		printf("���ׂ������f�̌��q�ԍ��A���q�L���A�������͖��O����͂��Ă�������\n>");
		getStr(input,STRSIZE);
		if(strcmp(input,"-q")==0) break;
		findP=serchElement(input);
		if(findP==NULL){
			printf("������܂���ł���\n");
		}else{
			dispElement(findP);
			printf("\n");
		}
	}
	printf("�I�����܂�\n");
	return 0;
}

void dispElement(ELEMENT *p){
	printf("���q�ԍ�: %d\n",p->number);
	printf("���q�L��: ");
	dispStrCapitalize(p->str[SYMBOL]);
	printf("\n���{�ꖼ: %s\n",p->str[NAME_JA]);
	printf(" �p�ꖼ : ");
	dispStrCapitalize(p->str[NAME_EN]);
	printf("\n ���q�� : %s\n",p->weight);
	printf("  ����  : %d\n",p->periodic);
	printf("   ��   : %d\n",p->group);
	return;
}
//�ŏ��̕������������̏ꍇ�啶���ɂ��ĕ\������
void dispStrCapitalize(const char str[]){
	if('a'<=str[0] && str[0]<='z'){
		printf("%c",str[0]-'a'+'A');
	}else{
		printf("%c",str[0]);
	}
	printf("%s",str+1);
	return;
}

ELEMENT* serchElement(char str[]){
	int elementNum=-1;
	if('0'<=str[0] && str[0]<='9'){//�ŏ��̕����������������炻�̌��q�ԍ���T��
		elementNum=strtol(str,NULL,10);
	}
	if(elementNum!=-1){
		return serchElementNum(elementNum);
	}else{
		convStrSmall(str);
		return serchElementStr(str);
	}
}

ELEMENT* serchElementNum(const int num){
	ELEMENT *p=table;
	while(p<table+tableSize){
		if(num==p->number){
			return p;
		}
		p++;
	}
	return NULL;
}

ELEMENT* serchElementStr(const char str[]){
	ELEMENT *tableP=table;
	int j;
	char (*strP)[STRSIZE];
	while(tableP<table+tableSize){
		strP = (tableP->str);
		while(strP<(tableP->str)+STR_NUM){
			if(strcmp(str,*strP)==0){
				return tableP;
			}
			strP++;
		}
		tableP++;
	}
	return NULL;
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

void convStrSmall(char str[]){
	char *strP=str;
	unsigned char temp;
	while(*strP!='\0'){
		temp=(unsigned char)(*strP);
		//shiftJis�̃}���`�o�C�g�������������͏������΂�
		if(((0x80<=temp) && (temp<=0xa0)) || ((0xe0<=temp) && (temp<=0xff))){
			strP+=2;
			continue;
		}
		if('A'<=*strP && *strP<='Z'){
			*strP=*strP-'A'+'a';
		}
		strP++;
	}
	return;
}