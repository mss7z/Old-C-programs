/************************************************************************
動物タワーバトルに自動的に負けてくれるプログラム
adb.exeにパスが通ってないと動かない（仕様）
汚い
2038年問題搭載
"Borland C++ 5.5.1 for Win32 Copyright (c) 1993, 2000 Borland"
を使用のため

201712XX-201801XX
************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <Windows.h>//見ろ！ヘッダーがマクロ定義の森のようだ！

#define VER "6.7.2 " //これでいいのかなぁ

#undef ERROR
#define ERROR 0xfff0
#define SUCCESS 0xffff

#define TARGET_FILE "G:\\ALMtemp\\SS.dat"
#define SS_BACKUP_DIR "G:\\ALMtemp\\SSbackup\\"
#define ADB_EXE "G:\\adb.exe " //後ろにスペースをつけること！！

#define PFSIZE 1843200
#define WIDTH 600
#define HEIGHT 1024

#define ENTER printf("\n");
#define CHECK_NULL_MEM(X) if(X==NULL) program_stop(E_SHORTMEM,NULL);
#define CHECK_NULL_FILE(X,Y,Z) if(X==NULL) program_stop(Y,Z);

#pragma pack(1)//パディングをさせない（いいのだろうか？）
//Bitmapのheader(Windows Bitmap)
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
//	unsigned char unknown;//32bitの時に使用
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
	printf("\nAutomatic Lose Machine Ver.%s\nFOR 動物タワーバトル\nStarting!\n",VER);
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

//タップなどの命令（行動）をする
//引数でmain関数から回数を受け取る
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
			printf("フリーズを確認したため、アプリの再起動を試みます\n");
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
			printf("勝利画面を認識!!\n");
			logging_ss(NULL,WIN);
			tap(30,945);
			timesn.win++;
			play_screen=0;
			break;
		case LOSE:
			printf("敗北画面を認識!!\n");
			logging_ss(NULL,LOSE);
			tap(30,945);
			timesn.lose++;
			play_screen=0;
			break;
		case PLAYING:
			printf("試合画面を認識!!\n");
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
			printf("試合開始画面を認識!!\n");
			timesn.matching++;
			wait(1);
			return;
		case MENU:
			printf("メニュー画面を認識!!\n");
			timesn.menu++;
			tap(300,600);
			break;
		case HOME:
			printf("ホーム画面を確認!!\nドウタワバトルの起動を試みます\n");
			timesn.home++;
			tap(300,740);
			wait(10);
			return;
		case UNKNOWN:
			printf("再試行をしましたが、表示されている画面が何なのか認識できません!!!\n予期しないエラーです\n");
			program_stop(E_UNKNOWN,NULL);
		default:	return;
	}
	wait(2);
	return;
}

//スクリーンショットを取得して必要に応じて保存します
//第一引数がスクリーンショットのポインタ、第二引数が状態
int logging_ss(unsigned long int *sspointer,int situation) {
	static unsigned long int *logss,*losess;
	static short setor=0;
	static char file_head[255]={0};
	static unsigned short counter=1;
	static time_t losetime,logtime;
	unsigned int i;
	ENTER;
	if(setor==0) {
		printf("スクリーンショットのバックアップのメモリを確保開始\n");
		logss = (unsigned long int*) malloc(sizeof(unsigned long int)*PFSIZE);
		CHECK_NULL_MEM(logss);
		losess = (unsigned long int*) malloc(sizeof(unsigned long int)*PFSIZE);
		CHECK_NULL_MEM(losess);
		printf("成功\n");
		
		time_convert_hex(time(NULL),file_head);
		setor=SUCCESS;
		return SUCCESS;
	}else if(situation==WIN) {
		FILE *fp;
		char file_name[511]=SS_BACKUP_DIR,number_str[31]={0};
		
		printf("前回の敗北画面のスクリーンショットの保存を開始\n");
		
		//プログラムの起動時間を追加
		mix_str(file_name,file_head);
		//回数を追加
		mix_str(file_name,"_T");
		convert_dec(counter,number_str);
		mix_str(file_name,number_str);
		//ファイル作成時間を追加
		mix_str(file_name,"_LT");
		time_convert_hex(losetime,number_str);
		mix_str(file_name,number_str);
		//拡張子を追加
		mix_str(file_name,".bmp");
		
		bmp_write(losess,file_name);
		
		counter++;
	}else if (situation==LOSE){
		printf("取得したスクリーンショットを敗北画面としてマーク\n");
		for(i=0;i<PFSIZE;i++) {
			losess[i] = logss[i];
		}
		losetime=logtime;
		return SUCCESS;
	}else if (sspointer!=NULL){
		printf("スクリーンショットのバックアップを取得\n");
		for(i=0;i<PFSIZE;i++) {
			logss[i] = sspointer[i];
		}
		logtime=time(NULL);
		return SUCCESS;
	}
	return ERROR;
}

//ほかのコンパイラで使うことを考え、作った関数(時間専用)
//時間を受け取って、文字列にして返します
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

//第一引数で受け取ったスクショのデータの場所を示すポインタ、第二引数でファイルの名前を受け取り
//第一引数のポインタのデータをbmpに変換し、第二引数の場所に保存する
void bmp_write(unsigned long int *fbdata,char *file_name) {
	ENTER;
	unsigned long int i,j;
	//unsigned char padding[4]={0};//WIDTHが4で割り切れないときの詰め物（パディング）
	//構造体のcolorの二次元配列の動的確保
	color **after_conversion=(color**)malloc(sizeof(color*)*HEIGHT);
	CHECK_NULL_MEM(after_conversion);
	for(i=0;i<HEIGHT;i++) {
		after_conversion[i]=(color*)malloc(sizeof(color)*WIDTH);
		CHECK_NULL_MEM(after_conversion[i]);
	}
	Bitmap_header header={0};
	FILE *bmp_fp;
	bh_default_write(&header);
	
	//bmpへの変換の核
	for(i=0;i<HEIGHT;i++) {
		unsigned long int line=(HEIGHT-i)*WIDTH;
		for(j=0;j<WIDTH;j++) {
			unsigned long int work=fbdata[line+j];
			after_conversion[i][j].red=work&0xff;
			after_conversion[i][j].green=work>>8&0xff;
			after_conversion[i][j].blue=work>>16&0xff;
			//after_conversion[i][j].unknown=0;//32bitの時に使用
		}
	}
	//書き込み
	bmp_fp=fopen(file_name,"wb");
	CHECK_NULL_FILE(bmp_fp,E_FCOPEN,file_name);
	fwrite(&header,sizeof(header),1,bmp_fp);
	for(i=0;i<HEIGHT;i++) {
		fwrite(after_conversion[i],sizeof(color),WIDTH,bmp_fp);
		//fwrite(padding,1,WIDTH-((WIDTH/4)*4),bmp_fp);//計算式自信なし要検証
	}
	fclose(bmp_fp);
	free(after_conversion);
	return;
}

//引数で受け取った構造体のBitmap_headerのポインタの示す場所にデフォルトの値を書き込む
void bh_default_write(Bitmap_header *data) {
	data->bfType		=0x4D42;//エンディアンのせいで反転してしまうので反転して書いてます
	data->bcSize		=40;
	data->bcWidth		=WIDTH;
	data->bcHeight		=HEIGHT;
	data->bcPlanes		=1;
	data->bcBitCount	=24;//24bit Bitmap
	return;
}

//その名の通りアプリを再起動します
void restart_app() {
	ENTER;
	printf("アプリの再起動をします\n");
	swipe(295,0,295,30);//全画面からあの画面を出す
	tap(350,1000);//四角いボタン
	swipe(100,250,100,10);//アプリの終了
	tap(250,1000);//ホームボタン
	tap(300,740);//ホームでドウタワバトルの位置
	printf("アプリの再起動を終了しました\n");
	wait(10);
	return;
}

//現在の状態の解析をする
//
int analysis() {
	static short setor=0;
	static unsigned long int *PCpointer[ENDOFLIST]={0};
	static char file_name[ENDOFLIST][255]={0};
	int i,j;
	ENTER;
	if(setor==0) {
		printf("\n比較元のファイルを読み込み開始\n");
		for(i=0;i<ENDOFLIST;i++) {
			make_file_name(i,file_name[i]);
			pfile_open(&PCpointer[i],file_name[i]);
		}
		printf("読み込み完了\n");
		setor=SUCCESS;
		return SUCCESS;
	}else{
		short counter=1;
		unsigned long int *Tpointer;//取得したスクリーンショットのデータが入っている領域へのポインタ
		short check=0;
		printf("分析開始\n");
		while (1){
			double compared_data;
			double max=0;
			
			//スクリーンショット取得
			sb_get(&Tpointer)==ERROR;
			//比較
			printf("\n比較元\t\t類似率\n");
			for(i=0;i<ENDOFLIST;i++) {
				printf("\"%s\"\t",file_name[i]);
				if(max>80) {
					printf("すでに80%%以上のデータありのためキャンセル\n");
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
				printf("\n最大値が40.00%%未満の%05.2lf%%のため再度分析します\n\n",max);
				counter++;
				if(counter>3)return UNKNOWN;
			}else{
				break;
			}
		}
		printf("分析結果:\"%s\"\n\n",file_name[check]);
		logging_ss(Tpointer,UNKNOWN);
		free(Tpointer);
		return check;
	}
}

//スクリーンショット（スクリーンバッファ）を取得し受けとったポインタの中にsbのデータのポインタを入れる
//引数はポインタのポインタ
//取得したスクリーンショットのデータを入れるポインタ  へのポインタ(詳しくはanalysis関数を読んで)
int sb_get (unsigned long int **Tpointerp) {
	ENTER;
	printf("スクリーンショット取得開始\n");
	char command[255]="pull /dev/graphics/fb0 ";
	mix_adb_command(command);
	swipe(295,0,295,30);
	mix_str(command,TARGET_FILE);
	system(command);
	if(pfile_open(Tpointerp,TARGET_FILE)==ERROR) {
		printf("取得失敗\n");
		program_stop(E_ERROR,NULL);
	}
	printf("スクリーンショット取得成功\n");
	return SUCCESS;
}

//ファイルを開き指定されたポインタが示しているポインタにメモリを確保し、データを中に入れる
//第一引数は保存場所を指定するポインタ  へのポインタ  メモリを確保してそのアドレスを入れたポインタへのポインタ
//第二引数はファイルの名前(そのままfopenやfreadに投げる)
int pfile_open (unsigned long int **hpointer,char *file_name) {
	FILE *fp;
	ENTER;
	printf("メモリの確保を開始\n");
	*hpointer = (unsigned long int*) malloc(sizeof(unsigned long int)*PFSIZE);
	CHECK_NULL_MEM(hpointer);
	printf("成功\nファイルの展開を開始\n");
	fp = fopen(file_name,"rb");
	CHECK_NULL_FILE(fp,E_FCOPEN,file_name);
	fread(*hpointer,sizeof(unsigned long int),PFSIZE,fp);
	fclose(fp);
	printf("\"%s\"を展開成功\n",file_name);
	return SUCCESS;
}

//二つの配列の比較をする
//引数は二つの配列のポインタ
//戻り値は何パーセント同じだったか
double compare(unsigned long int *compareA,unsigned long int *compareB) {
	unsigned int i,check=0;
	for(i=0;i<PFSIZE;i++) {
		if(compareA[i]==compareB[i]) {
			check++;
		}
	}
	return ((double)check/i)*100.0;
}

//第一引数で渡された文字列の後ろに、第二引数で渡された文字列をつける
//修正済み(第一引数の名前がfile_nameになってるのはもともとファイルの名前を作るための関数だったから
//        (まだ直してないだけです、すみません許してください何でもしますから（何でもするとは言ってない）
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

//引数で待たされた秒数待つ
//"Windows.h"必要
void wait(int wait_time) {
	ENTER;
	printf("\n%d秒待機しています...\n",wait_time);
	Sleep(wait_time*1000);
}

//プログラムの状態を表示
//引数で何回実行したかを入力
//"stdlib.h","time.h"必要
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
	printf("Automatic Lose Machine Ver.%s\nFOR 動物タワーバトル\n",VER);
	printf("================================ LOG ================================\n");

	printf("%10d回目\n%02d時間%02d分%02d秒経過\n",number,hour,minute,second);
	printf("   \t 勝利\t 敗北\t 試合\t試合前\t再起動\tMENU\tHOME\n");
	printf("(回)\t%5d\t%5d\t%5d\t%6d\t%6d\t%4d\t%4d\n\n",
			tsn->win,tsn->lose,tsn->playing,tsn->matching,tsn->restart,tsn->menu,tsn->home);
	//printf("勝利%4d回\t敗北%4d回\t試合%4d回\t再起動%4d回\n",tsn->win,tsn->lose,tsn->restart);
	//printf("試合開始%2d回\tメニュー%2d回\tホーム%2d回\n\n",tsn->matching,tsn->menu,tsn->home);
	return;
}

//x座標とy座標を与えるとそこをタップするadbを実行
//"stdlib.h"必要
void tap(int x,int y){
	char command[255]="shell input tap ";
	mix_adb_command(command);
	ENTER;
	printf("x=%d,y=%dをタップします\n",x,y);
	mix_number(x,command);
	mix_number(y,command);
	system(command);
	return;
}

//最初のx座標とy座標、最後のx座標とy座標を与えるとそこをスワイプするadbを実行
void swipe(int fx,int fy,int tx,int ty) {//後で構造体にする?
	char command[255]="shell input swipe ";
	mix_adb_command(command);
	ENTER;
	printf("x=%d,y=%dからx=%d,y=%dにスワイプします\n",fx,fy,tx,ty);
	mix_number(fx,command);
	mix_number(fy,command);
	mix_number(tx,command);
	mix_number(ty,command);
	system(command);
	return;
}
	

//文字列の最後に数値を追加
//第一引数で追加したい数値、第二引数で数値を追加したい文字列のアドレスを与える
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

//正の整数値を文字列に変換
//第一引数で変換したい数、第二引数で変換後を入れる文字列のアドレスを与える
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
			check=i;//syori上での最後のけたの番号
			break;
		}
	}
	for(i=check;i>=0;i--) {
		output[check-i]=syori[i]+'0';
	}
	return;
}

//その名の通りとみての通りプログラムを異常停止させます
void program_stop(int situation,char *file_name) {
	printf("\n\n\nERROR:");
	if(file_name!=NULL) printf("ファイル\"%s\"が",file_name);
	switch(situation) {
		case E_FCOPEN:		printf("開けません\n");break;
		case E_FCREAD:		printf("読み込みできません\n");break;
		case E_FCWRITE:		printf("書き込みできません\n");break;
		case E_SHORTMEM:	printf("SYSTEMが深刻なメモリー不足です\n");break;
		case E_UNKNOWN:		printf("内部処理で深刻なエラーが発生しました\n");break;
		case E_ERROR:		break;
		default:			printf("不明なエラー(0x%04X)が発生しました\n",situation);break;
	}
	printf("10秒後にプログラムを終了します\nERROR_CODE:0x%04X\n",situation);
	wait(10);
	exit(1);
}

//受け取った文字列をADB_EXEの後ろに追加する
void mix_adb_command(char *adbcom) {
	unsigned short i;
	char command[255]=ADB_EXE;
	mix_str(command,adbcom);
	for(i=0;i<255;i++) {//なんかヤなコード
		adbcom[i]=command[i];
		if(command[i]=='\0')return;
	}
	return;
}