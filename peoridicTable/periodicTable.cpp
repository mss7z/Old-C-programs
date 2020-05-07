#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STRSIZE 63
#define NOT_FOUND -1

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
	{1,{"h","水素","hydrogen"},"1.00794",1,1},
	{2,{"he","ヘリウム","helium"},"4.002602",1,18},
	{3,{"li","リチウム","lithium"},"6.941",2,1},
	{4,{"be","ベリリウム","beryllium"},"9.012182",2,2},
	{5,{"b","ホウ素","boron"},"10.811",2,13},
	{6,{"c","炭素","carbon"},"12.0107",2,14},
	{7,{"n","窒素","nitrogen"},"14.0067",2,15},
	{8,{"o","酸素","oxygen"},"15.9994",2,16},
	{9,{"f","フッ素","fluorine"},"18.9984032",2,17},
	{10,{"ne","ネオン","neon"},"20.1797",2,18},
	{11,{"na","ナトリウム","sodium"},"22.98976928",3,1},
	{12,{"mg","マグネシウム","magnesium"},"24.305",3,2},
	{13,{"al","アルミニウム","aluminium"},"26.9815386",3,13},
	{14,{"si","ケイ素","silicon"},"28.0855",3,14},
	{15,{"p","リン","phosphorus"},"30.973762",3,15},
	{16,{"s","硫黄","sulfur"},"32.065",3,16},
	{17,{"cl","塩素","chlorine"},"35.453",3,17},
	{18,{"ar","アルゴン","argon"},"39.948",3,18},
	{19,{"k","カリウム","potassium"},"39.0983",4,1},
	{20,{"ca","カルシウム","calcium"},"40.078",4,2},
	{26,{"fe","鉄","iron"},"55.845",4,8},
	{29,{"cu","銅","copper"},"63.546",4,11},
	{30,{"zn","亜鉛","zinc"},"65.38",4,12},
};
//　印刷する量を減らすためにtableのデータの数を大幅に減らしました。

const int tableSize=sizeof(table)/sizeof(table[0]);

int getStr(char[],const int);
void convStrSmall(char[]);

int serchElementStr(const char[]);
int serchElementNum(const int);
int serchElement(char[]);

void dispElement(const int);
void dispStrCapitalize(const char[]);

int main(){
	char input[STRSIZE];
	int findNum;
	printf("元素を検索します\n -q で終了\n");
	while(true){
		printf("調べたい元素の原子番号、原子記号、もしくは名前を入力してください\n>");
		getStr(input,STRSIZE);
		if(strcmp(input,"-q")==0) break;
		findNum=serchElement(input);
		if(findNum==NOT_FOUND){
			printf("見つかりませんでした\n");
		}else{
			dispElement(findNum);
			printf("\n");
		}
	}
	printf("終了します\n");
	return 0;
}

void dispElement(const int n){
	printf("原子番号: %d\n",table[n].number);
	printf("原子記号: ");
	dispStrCapitalize(table[n].str[SYMBOL]);
	printf("\n日本語名: %s\n",table[n].str[NAME_JA]);
	printf(" 英語名 : ");
	dispStrCapitalize(table[n].str[NAME_EN]);
	printf("\n 原子量 : %s\n",table[n].weight);
	printf("  周期  : %d\n",table[n].periodic);
	printf("   族   : %d\n",table[n].group);
	return;
}
//最初の文字が小文字の場合大文字にして表示する
void dispStrCapitalize(const char str[]){
	if('a'<=str[0] && str[0]<='z'){
		printf("%c",str[0]-'a'+'A');
	}else{
		printf("%c",str[0]);
	}
	printf("%s",str+1);
	return;
}

int serchElement(char str[]){
	int elementNum=-1;
	if('0'<=str[0] && str[0]<='9'){//最初の文字が数字だったらその原子番号を探す
		elementNum=strtol(str,NULL,10);
	}
	if(elementNum!=-1){
		return serchElementNum(elementNum);
	}else{
		convStrSmall(str);
		return serchElementStr(str);
	}
}

int serchElementNum(const int num){
	int i;
	for(i=0;i<tableSize;i++){
		if(num==table[i].number){
			return i;
		}
	}
	return NOT_FOUND;
}

int serchElementStr(const char str[]){
	int i,j;
	for(i=0;i<tableSize;i++){
		for(j=0;j<STR_NUM;j++){
			if(strcmp(str,table[i].str[j])==0){
				return i;
			}
		}
	}
	return NOT_FOUND;
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
		//shiftJisのマルチバイト文字だった時は処理を飛ばす
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