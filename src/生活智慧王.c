#include <stdio.h>
#include <stdlib.h> 
#include <string.h>
#include <windows.h>
#include <mmsystem.h>
#include <memory.h>
#pragma comment(lib,"Winmm.lib")

#define HLEN 14
#define WLEN 18
#define block_h 4
#define block_w 4 

HANDLE hand;

int BlockO[block_h][block_w] = {
	{0, 0, 0, 0},
	{0, 1, 1, 0},
	{0, 1, 1, 0},
	{0, 0, 0, 0}
};

int BlockJ[block_h][block_w] = {
	{0, 0, 0, 0},
	{0, 1, 0, 0},
	{0, 1, 1, 1},
	{0, 0, 0, 0}
};

int BlockL[block_h][block_w] = {
	{0, 0, 0, 0},
	{0, 0, 1, 0},
	{1, 1, 1, 0},
	{0, 0, 0, 0}
};

int BlockI[block_h][block_w] = {
	{0, 0, 0, 0},
	{1, 1, 1, 1},
	{0, 0, 0, 0},
	{0, 0, 0, 0}
};

int BlockZ[block_h][block_w] = {
	{0, 0, 0, 0},
	{1, 1, 0, 0},
	{0, 1, 1, 0},
	{0, 0, 0, 0}
};

int BlockS[block_h][block_w] = {
	{0, 0, 0, 0},
	{0, 1, 1, 0},
	{1, 1, 0, 0},
	{0, 0, 0, 0}
};

int BlockT[block_h][block_w] = {
	{0, 0, 0, 0},
	{0, 0, 1, 0},
	{0, 1, 1, 1},
	{0, 0, 0, 0}
};

int surface[HLEN][WLEN] = {0};

int cur_x = 0, cur_y = 0;

int block_cur[block_h][block_w] = {0};

enum blockType{
	O,
	J,
	L,
	I,
	S,
	Z,
	T
};

int block_type_num = 7;
int tm = 800;
UINT_PTR timerId;
int score = 0;
int isEnd = 0;

int Day_Select, date[10];
char input[1000][1000];

struct Class{
	int day;
	int session;
	char name[15];
}class[71];

void setCursorVisable(int v);
void gotoXY(int x, int y);
void printXY(char *str, int x, int y);
void printSurface();
void printBlock(int block[][block_w], int x, int y);
void eraseBlock(int block[][block_w], int x, int y);
void forSleep(int max);
void keyControl();
int isCollision(int x, int y);
int get_block_x_fill(int row);
int get_block_y_fill(int col);
void get_block_left_right_margin(int *left_margin, int* right_margin);
void get_block_top_bottom_margin(int *top_margin, int* bottom_margin);
void copyBlock(int block_s[][block_w], int block_d[][block_w]);
void rotateBlock();
void make_new_block();
static void set_timer(int t);
static void close_timer();
void move_block_down();
void landBlock();
int is_line_fill(int h);
void clear_one_line(int h);
void checkLine();
int printScore();

void ArrayClear();
void DateChoose();
int DayInput_Rule( int y, int m, int d );
int DayMinus_7( int year1, int year2, int month1, int month2, int day1, int day2 );
int DayMinus_30( int year1, int year2, int month1, int month2, int day1, int day2 );
void Date_1();
void Date_7();
void Date_30();
int DataCheck_Day( int year, int month, int day );
int SleepCount( int x );
int CheckRecord( int day );
void SleepRate_1( int x );
void SleepRate7a30(int num);
void DataInput();

void timetable(){
	int i = 0,j = 0,day,session,count = 2;
	char name[15] = {'\0'},select[1];
	for(i = 0;i < 72;i++){
		memset(class[i].name,'\0',sizeof(class[i].name));
	}
	FILE *fp;
    fp = fopen("Timetable.txt", "w");
	printf("��J�Ҫ��T�I\n�P����J1~5�H�~���ƧY�������I\n\n");
	while(1){
		printf("�п�J�P���X(1~5): ");
		scanf("%d",&day);
		if(day < 1 || day > 5)break;
		printf("�п�J�`��(1~14): ");
		scanf("%d",&session);
		if(session < 1 || session > 14){
			printf("�п�J�ŦX�W�w���ȡI\n");
			continue;
		}
		if(strlen(class[(session - 1) * 5 + day].name) == 0){
			printf("�п�J�ҵ{�W��: ");
			scanf("%s",&name);
			strcpy(class[(session - 1) * 5 + day].name,name);
		}
		else{
			printf("�Ӯɶ��w�g���ҵ{�A�O�_�n���?(Y/N): ");
			scanf("%s",&select);
			if(select[0] == 'y' || select[0] == 'Y'){
				printf("�п�J�ҵ{�W��: ");
				scanf("%s",&name);
				strcpy(class[(session - 1) * 5 + day].name,name);
			}
			else{
				continue;
			} 
		}
	}
	printf("          Mon                 Tue                 Wed                 Thu                 Fri\n");
	printf("\n��1�`     ");
	fprintf(fp,"          Mon                 Tue                 Wed                 Thu                 Fri\n");
	fprintf(fp,"\n��1�`     ");
	for(i = 1;i <= 70;i++){
		printf("%-20s",class[i].name);
		fprintf(fp,"%-20s",class[i].name);
		if(i % 5 == 0 && i != 70 && i <= 40){
			printf("\n______________________________________________________________________________________________________________\n\n��%d�`     ",count);
			fprintf(fp,"\n______________________________________________________________________________________________________________\n\n��%d�`     ",count++);
		}
		else if(i % 5 == 0 && i != 70 && i >= 45){
			printf("\n______________________________________________________________________________________________________________\n\n��%d�`    ",count);
			fprintf(fp,"\n______________________________________________________________________________________________________________\n\n��%d�`    ",count++);
		}
	}
	printf("\n______________________________________________________________________________________________________________\n\n");
	fclose(fp); 
}

//�p�⥭�|�~ 
int leapyear(int year){
	if(year % 400 == 0 || year % 4 == 0 && year % 100 != 0){
		return 1;
	}
}

//�p���몺�@�����P���X 
int GetWeekday(int year, int month, int day){
	day = 1;
	return	(year + (year / 4) + (year / 400) - (year / 100) + ((month + day) - 1)) % 7;
}

//�C�L��� 
int Calendar(){
	int year,month,day;
	int weekday, i, count = 0;
	int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	int ldays[] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	
	while(1){
		printf("�п�J�~���(�H�Ů���})(��J0 0 0�H����): ");
		scanf("%d %d %d",&year,&month,&day);
		if(year == 0 || month == 0 || day == 0)break;
		printf("SUN MON TUE WED THU FRI SAT\n");
		weekday = GetWeekday(year, month, day);
		for(i = 0;i < weekday;i++){
			printf("%4s"," ");
		}
		// �|�~ 
		if(leapyear(year) == 1){
			for(i = 1,count = weekday;i <= ldays[month - 1];i++){
				printf("%-4d",i);
				count++;
				if(count % 7 == 0){
					printf("\n");
				}
			}
			printf("\n");
		}
		// ���~ 
		else{
			for(i = 1,count = weekday;i <= days[month - 1];i++){
				printf("%-4d",i);
				count++;
				if(count % 7 == 0){
					printf("\n");
				}
			}
			printf("\n");
		}
	}
}

void CleanArr(char a[1000]){
	int i, j;
	for(i = 0; a[i] != '\0'; i++){
		a[i] = ' ';
	}
}

int Record(){
	int year;
	char name[30], sleep[10], wake[10], select[10], event[100], time[10];
	char work[30], date[10];
	int i,n,mode;
	
	printf("[1]��J��� [2]��ܬ���: ");
	scanf("%d",&mode);
	
	if(mode == 1){
		FILE *a;
    	a = fopen("Record.txt", "a");
	
    	if(NULL==a){
    		printf("open failure");
		}
		else{
			while(1){
				system("cls");
				printf("��J���(ex:19990101): ");
				scanf("%s",date);
				fprintf(a, "%c%c%c%c�~%c%c��%c%c�� ", date[0], date[1], date[2], date[3], date[4], date[5], date[6], date[7]);
				
				system("cls");
				printf("��J�e���ı�ɶ�(XX:XX): ");
				scanf("%s", sleep);
				fprintf(a, "Sleep Time:%s ", sleep);
				
				system("cls");
					printf("��J�_�ɮɶ�(XX:XX): ");
				scanf("%s", wake);
				fprintf(a, "Wake Time:%s ", wake);
			
				system("cls");
				printf("��JBreakfast�ɶ�(XX:XX): ");
				scanf("%s", time);
				fprintf(a, "Breakfast:%s ", time);
				CleanArr(time);
				
				system("cls");
				printf("��JLunch�ɶ�(XX:XX): ");
				scanf("%s", time);
				fprintf(a, "Lunch:%s ", time);
				CleanArr(time);
				
				system("cls");
				printf("��JDinner�ɶ�(XX:XX): ");
				scanf("%s", time);
				fprintf(a, "Dinner:%s ", time);
				CleanArr(time);
			
					system("cls");
				printf("�O�_��J��{(Y/N): ");
				scanf("%s", select);
			
				if(select[0] == 'Y' || select[0] == 'y'){
					while(1){
						system("cls");
						printf("��J�ƥ�(��J00����): ");
						scanf("%s", event);
						if(event[0] == '0' && event[1] == '0'){
							break;
						}
						fprintf(a, "%s ", event);
						CleanArr(event);
						
						printf("��J�ɶ�(XX:XX): ");
						scanf("%s", time);
						fprintf(a, "%s ", time);
						CleanArr(time);
					}
				}
				fprintf(a, "\n"); 
				system("cls");
				printf("�O�_�~���J��{(Y/N): ");
				scanf("%s", select);
				if(select[0] == 'y' || select[0] == 'Y'){
					continue;
				}
				else{
					break;
				}
			}
		fprintf(a,"-------------------------------------------------------\n");
		}
    fclose(a);
	}
	else if(mode == 2){
		FILE *fp;
		char str[10000]={0};
		int b, i;
		printf("�w��J����� : \n");
		fp=fopen("Record.txt","r");
		if(NULL ==fp){
			printf("open failure\n");
		    return 1;
		}
		else{
			for(b=0;b<10000;b++){
				fscanf(fp,"%c",&str[b]);
		}
		}
	
		for(i = 0; str[i] != '\0'; i++){
			printf("%c", str[i]);
		
			if(str[i - 12] == 'D' && str[i - 11] == 'i' && str[i - 10] == 'n'){
				printf("\n");
			}
		
			if(str[i + 5] == '/' && str[i + 8] == '/' ){
				printf("----------------------------------------------------------------------------------\n");
			}
		}
		fclose(fp);
	}
}

void alarm(){
	SYSTEMTIME time;
	SYSTEMTIME clock;
	GetLocalTime(&time);
	printf("�ثe�ɶ��G%2d:%2d:%2d\n",time.wHour,time.wMinute,time.wSecond);
	printf("�]�w�ɶ��G\n");
	printf("�ɡG");
	scanf("%2d",&clock.wHour);
	printf("���G");
	scanf("%2d",&clock.wMinute);
	while(1){
		Sleep(1000);
		GetLocalTime(&time);
		if((time.wHour==clock.wHour)&&(time.wMinute==clock.wMinute)){
			printf("\a\a\a�ɶ���!!!");
			break;
		}
	}
	system("cls");
	PlaySound("Alarm_Clock.wav",NULL,SND_FILENAME | SND_ASYNC | SND_LOOP);
	g:
	hand = GetStdHandle(STD_OUTPUT_HANDLE);
	setCursorVisable(0);
//	srand(time(NULL));
	printSurface();
	make_new_block();
	printBlock(block_cur, cur_x, cur_y);
	set_timer(tm);
	keyControl();
	gotoXY(0, HLEN + 1);
	printScore();
	printf("\nGAME OVER\n");
	PlaySound(NULL,NULL,SND_FILENAME); 
	system("cls");
	//system("PAUSE");
	//PlaySoundA( NULL, NULL, SND_FILENAME|SND_ASYNC );}
}

void setCursorVisable(int v){
	CONSOLE_CURSOR_INFO cursor_info = {100, v};
	SetConsoleCursorInfo(hand, &cursor_info);
}

void gotoXY(int x, int y){
	COORD loc;
	loc.X = x;
	loc.Y = y;
	SetConsoleCursorPosition(hand, loc);
}

void printXY(char *str, int x, int y){
	gotoXY(x, y);
	printf("%s", str);
}

void printSurface(){
	int x, y;
	for(x = 0; x < WLEN; x++){
		for(y = 0; y < HLEN; y++){
			int row = y;
			int col = x;
			if(surface[row][col] == 0){
				printXY(" ", x, y);
			}
			else{
				printXY("O", x, y);
			}
		} 
	}
	x = WLEN;
	for(y = 0; y < HLEN; y++){
		printXY("|", x, y);
	}
	y = HLEN;
	for(x = 0; x < WLEN; x++){
		printXY("-", x, y);
	}
	printXY("+", WLEN, HLEN);
}

void printBlock(int block[][block_w], int x, int y){
	int w, h;
	for(w = 0; w < block_w; w++){
		for(h = 0; h < block_h; h++){
			if(block[h][w] == 1){
				printXY("O", w + x , h + y);
			}
		}
	}
}

void eraseBlock(int block[][block_w], int x, int y){
	int w, h;
	for(w = 0; w < block_w; w++){
		for(h = 0; h < block_h; h++){
			if(block[h][w] == 1){
				printXY(" ", w + x , h + y);
			}
		}
	}
}

void forSleep(int max){
	int i;
	for(i = 0; i < max;i++){
	}
}

void keyControl(){
	int ch = 0;
	MSG msg;
	while(1){
		if(PeekMessage(&msg, NULL, WM_TIMER, WM_TIMER, PM_REMOVE) != 0){
			move_block_down();
		}
		printScore();
		if(isEnd == 1){
			break;
		}
		if(kbhit() != 0){
			ch = getch();
			switch(ch){
				case 72:
					eraseBlock(block_cur, cur_x, cur_y);
					rotateBlock();
					if(isCollision(cur_x, cur_y) == 1){
						rotateBlock();
						rotateBlock();
						rotateBlock();
					}
					printBlock(block_cur, cur_x, cur_y);
					break;
				case 80:
					if(isCollision(cur_x, cur_y + 1) == 0){
						eraseBlock(block_cur, cur_x, cur_y);
						cur_y++;
						printBlock(block_cur, cur_x, cur_y);
					
					}
					break;
				case 75:
					if(isCollision(cur_x - 1, cur_y) == 0){
						eraseBlock(block_cur, cur_x, cur_y);
						cur_x--;
						printBlock(block_cur, cur_x, cur_y);
					}
					break;
				case 77:
					if(isCollision(cur_x + 1, cur_y) == 0){
						eraseBlock(block_cur, cur_x, cur_y);
						cur_x++;
						printBlock(block_cur, cur_x, cur_y);
					}
					break;
				case 32:
					while(isCollision(cur_x, cur_y + 1) == 0){
						eraseBlock(block_cur, cur_x, cur_y);
						cur_y++;
						printBlock(block_cur, cur_x, cur_y);
						score++;
					}
					landBlock();
					break;
				case 113:
					return;
					break;
				default:
					break;
			}
		}
	}
}

int isCollision(int x, int y){
	int leftMargin = 0, rightMargin = 0, topMargin = 0, bottomMargin = 0;
	get_block_left_right_margin(&leftMargin, &rightMargin);
	get_block_top_bottom_margin(&topMargin, &bottomMargin);
	if(x < 0 - leftMargin || y < 0 - topMargin || x + block_w > WLEN + rightMargin || y + block_h > HLEN + bottomMargin){
		return 1;
	}
	int w, h;
	for(w = 0; w < block_w; w++){
		for(h = 0; h < block_h; h++){
			if(block_cur[h][w] == 1){
				if(surface[y+h][x+w] == 1){
					return 1;
				}
			}
		}
	}
	return 0;
}

int get_block_x_fill(int row){
	int is_fill = 0;
	int col;
	for(col = 0; col < block_w; col++){
		if(block_cur[row][col] == 1){
			is_fill = 1;
		}
	}
	return is_fill;
}

int get_block_y_fill(int col){
	int is_fill = 0;
	int row;
	for(row = 0; row < block_w; row++){
		if(block_cur[row][col] == 1){
			is_fill = 1;
		}
	}
	return is_fill;
}

void get_block_left_right_margin(int *left_margin, int* right_margin){
	int is_fill = 0;
	int i;
	for(i = 0; i < block_h / 2; i++){
		is_fill = get_block_y_fill(i);
		if(is_fill == 0){
			*left_margin += 1;
		}
	}
	for(i = block_h - 1; i >= block_h / 2; i--){
		is_fill = get_block_y_fill(i);
		if(is_fill == 0){
			*right_margin += 1;
		}
	}
}

void get_block_top_bottom_margin(int *top_margin, int* bottom_margin){
	int is_fill = 0;
	int i;
	for(i = 0; i < block_w / 2; i++){
		is_fill = get_block_x_fill(i);
		if(is_fill == 0){
			*top_margin += 1;
		}
	}
	for(i = block_w - 1; i >= block_w / 2; i--){
		is_fill = get_block_x_fill(i);
		if(is_fill == 0){
			*bottom_margin += 1;
		}
	}
}

void copyBlock(int block_s[][block_w], int block_d[][block_w]){
	int w, h;
	for(w = 0; w < block_w; w++){
		for(h = 0; h < block_h; h++){
			block_d[h][w] = block_s[h][w];
		} 
	}
}

void rotateBlock(){
	int tmp[block_h][block_w] = {0};
	copyBlock(block_cur, tmp); 
	int w, h;
	for(w = 0; w < block_w; w++){
		for(h = 0; h < block_h; h++){
			block_cur[h][w] = tmp[block_w - 1 - w][h];
		}
	}
}

void make_new_block(){
	enum blockType type = (int)(rand() % block_type_num);
	switch(type){
		case O:
			copyBlock(BlockO, block_cur);
			break;
		case J:
			copyBlock(BlockJ, block_cur);
			break;
		case L:
			copyBlock(BlockL, block_cur);
			break;
		case I:
			copyBlock(BlockI, block_cur);
			break;
		case S:
			copyBlock(BlockS, block_cur);
			break;
		case Z:
			copyBlock(BlockZ, block_cur);
			break;
		case T:
			copyBlock(BlockT, block_cur);
			break;
		default:
			break;
	}
	cur_x = (WLEN - block_h) / 2;
	cur_y = 0;
}

static void set_timer(int t){
	KillTimer(NULL, timerId);
	timerId = SetTimer(NULL, 0, t, NULL);
}

static void close_timer(){
	KillTimer(NULL, timerId);
}

void move_block_down(){
	if(isCollision(cur_x, cur_y + 1) == 0){
		eraseBlock(block_cur, cur_x, cur_y);
		cur_y++;
		printBlock(block_cur, cur_x, cur_y);
	}
	else{
		landBlock();
		checkLine();
		make_new_block();
		if(score >= 100){
			close_timer();
			isEnd = 1;
		}
		if(isCollision(cur_x, cur_y + 1) == 1){
			close_timer();
			isEnd = 1;
			
		}
		printBlock(block_cur, cur_x, cur_y);
	}
}

void landBlock(){
	int w, h;
	for(w = 0; w < block_w; w++){
		for(h = 0; h < block_h; h++){
			if(block_cur[h][w] == 1){
				surface[cur_y+h][cur_x+w] = 1;
			}
		}
	}
}

int is_line_fill(int h){
	int w;
	for(w = 0; w < WLEN; w++){
		if(surface[h][w] == 0){
			return 0;
		}
	}
	return 1;
}

void clear_one_line(int h){
	int w, row;
	for(row = h; row > 1; row--){
		for(w = 0; w < WLEN; w++){
			surface[row][w] = surface[row-1][w];
		}
	}
	for(w = 0; w < WLEN; w++){
		surface[0][w] = 0;
	}
}

void checkLine(){
	int totalLine = 0;
	int h;
	for(h = cur_y; h < HLEN; h++){
		if(is_line_fill(h) == 1){
			clear_one_line(h);
			printSurface();
			totalLine++;
		}
	}
	if(totalLine == 1){
		score += 100;
	}
	else if(totalLine == 2){
		score += 200;
	}
	else if(totalLine == 3){
		score += 300;
	}
	else if(totalLine == 4){
		score += 800;
	}
}

int printScore(){
	gotoXY(WLEN + 3, 0);
	printf("Score: %d", score);
	return score;
} 

void ArrayClear(){
	int i, j;
	for( i = 0; i < 1000; i++ ){
		for( j = 0; j < 1000; j++ ){
			input[i][j] = '\0';
		}
	}
}

void Advise(){
	char YoN[1];
	DataInput();
	while(1){
   		DateChoose();
   		printf("�O�_�n�~��ϥ�?(Y/N): ");
   		scanf("%s", YoN);
   		system("cls");
   		if(YoN[0] == 'n' || YoN[0] == 'N'){
   			break;
		}
	}
}

/*--------------------------
-----��ܭn�d�ߪ��Ѽ�-------
--------------------------*/
void DateChoose(){
	int iplace;
    printf("��J���˵��Ѽ�(1�B7�B30): ");
    scanf("%d", &Day_Select);

    if(Day_Select == 1){
        Date_1();
        iplace = DataCheck_Day( date[0], date[1], date[2]);
        if(iplace != 13579){
        	SleepRate_1(iplace);
		}
    }
    else if(Day_Select == 7){
        Date_7();
        SleepRate7a30(7);
    }
    else if(Day_Select == 30){
        Date_30();
        SleepRate7a30(30);
    }
    else{
        printf( "��J���~�I\n�п�J1�B7��30\n\n" );
        DateChoose();
    }
}

/*----------------------
------��J�W�h----------
----------------------*/
int DayInput_Rule( int y, int m, int d ){
    //printf("%d/%d/%d\n", y,m,d);
    if( m > 12 || m < 1 ){
        printf("���~�I\n���������1~12\n\n");
        return 0;
    }
    if( y % 4 != 0 || ( y % 100 == 0 && y % 400 != 0 )){ //���~2��
        if( m == 2 ){
            if( d < 1 || d > 28 ){
                printf( "���~�I\n��J��������1~28\n\n" );
                return 0;
            }
        }
    }
    if((m == 1) || (m == 3) || (m == 5) || (m == 7) || (m == 8) || (m == 10) || (m == 12)){ //�j��
        if( d < 1 || d > 31 ){
        	printf("2\n");
            printf( "���~�I\n��J��������1~31\n\n" );
            return 0;
        }
    }
    if((m == 4) || (m == 6) || (m == 9) || (m == 11)){
        if( d < 1 || d > 30 ){
            printf( "���~�I\n��J��������1~30\n\n" );
            return 0;
        }
    }
    if((y % 4 == 0 &&  y % 100 != 0 ) || y % 400 == 0){
        if( m == 2 ){
            if( d < 1 || d > 29 ){
                printf( "���~�I\n��J��������1~29\n\n" );
                return 0;
            }
        }
    }
    else{
    	return 1;
	}
}

/*------------------------
-----�P�_�O�_�t7��--------
------------------------*/
int DayMinus_7( int year1, int year2, int month1, int month2, int day1, int day2 ){
    if( year1 == year2 && month1 == month2 && day1 + 6 != day2 ){
        return 1;
    }
    else if( year1 == year2 && month1 != month2 ){
        if( month1 == 1 || month1 == 3 || month1 == 5 || month1 == 7 || month1 == 8 || month1 == 10 || month1 == 12 ){
            if( 31 - day1 + day2 != 7 ){
                return 1;
            }
        }
        else if( month1 != 1 || month1 != 3 || month1 != 5 || month1 != 7 || month1 != 8 || month1 != 10 || month1 != 12 || month1 != 2){
            if( 30 - day1 + day2 != 7 ){
                return 1;
            }
        }
        else if( year1 % 4 != 0 || ( year1 % 100 == 0 && year1 % 400 != 0 )){ //���~
            if( month1 == 2 ){
                if( 28 - day1 + day2 != 7){
                    return 1;
                }
            }
        }
        else{
            if( month1 == 2 ){
                if( 29 - day1 + day2 != 7 ){
                    return 1;
                }
            }
        }
    }
    else if( year1 < year2 ){
        if( 31 - day1 + day2 != 7 ){
            return 1;
        }
    }
}

/*------------------------
-----�P�_�O�_�t30��--------
------------------------*/
int DayMinus_30( int year1, int year2, int month1, int month2, int day1, int day2 ){
    if( year1 == year2 && month1 == month2 && day1 + 29 != day2 ){
        return 1;
    }
    if( year1 == year2 && month1 != month2 ){
        if( month1 == 1 || month1 == 3 || month1 == 5 || month1 == 7 || month1 == 8 || month1 == 10 || month1 == 12 ){
            if( 31 - day1 + day2 != 30 ){
                return 1;
            }
        }
        if( month1 != 1 || month1 != 3 || month1 != 5 || month1 != 7 || month1 != 8 || month1 != 10 || month1 != 12 || month1 != 2){
            if( 30 - day1 + day2 != 30 ){
                return 1;
            }
        }
        if( year1 % 4 != 0 || ( year1 % 100 == 0 && year1 % 400 != 0 )){ //���~
            if( month1 == 2 ){
                if( 28 - day1 + day2 != 30 ){
                    return 1;
                }
            }
        }
        else{
            if( month1 == 2 ){
                if( 29 - day1 + day2 != 30 ){
                    return 1;
                }
            }
        }
    }
    if( year1 < year2 ){
        if( 31 - day1 + day2 != 30 ){
            return 1;
        }
    }
}


void Date_1(){
    printf("��J���(xxxx/xx/xx �p��10���ݸ�0): ");
    scanf("%d/%d/%d", &date[0], &date[1], &date[2]);
    if(DayInput_Rule( date[0], date[1], date[2] ) == 0){
        Date_1();
    }
    printf( "\n" );
}

void Date_7(){
    printf( "��J�_�l���(xxxx/xx/xx �p��10���ݸ�0): " );
    scanf( "%d/%d/%d", &date[0], &date[1], &date[2] );
    if( DayInput_Rule( date[0], date[1], date[2] ) == 0 ){
        Date_7();
    }
    printf( "��J�������(xxxx/xx/xx �p��10���ݸ�0): " );
    scanf( "%d/%d/%d", &date[3], &date[4], &date[5] );
    if( DayInput_Rule( date[3], date[4], date[5] ) == 0 ){
        Date_7();
    }
    if(( date[0] > date[3] ) || ( date[0] == date[3] && date[1] > date[4] ) || ( date[0] == date[3] && date[1] == date[4] && date[2] > date[5] )){
        printf( "���~�I\n�_�l������p�󵲧����\n\n" );
        Date_7();
    }
    if( DayMinus_7( date[0], date[3], date[1], date[4], date[2], date[5] ) == 1 ){
        printf( "���~�I\n��J�`�M����7��\n\n" );
        Date_7();
    }
    
}

void Date_30(){
    printf( "��J�_�l���(xxxx/xx/xx �p��10���ݸ�0): " );
    scanf( "%d/%d/%d", &date[0], &date[1], &date[2] );
    if( DayInput_Rule( date[0], date[1], date[2] ) == 0 ){
        Date_30();
    }
    printf( "��J�������(xxxx/xx/xx �p��10���ݸ�0): " );
    scanf( "%d/%d/%d", &date[3], &date[4], &date[5] );
    if( DayInput_Rule( date[3], date[4], date[5] ) == 0 ){
        Date_30();
    }
    if(( date[0] > date[3] ) || ( date[0] == date[3] && date[1] > date[4] ) || ( date[0] == date[3] && date[1] == date[4] && date[2] > date[5] )){
        printf("���~�I\n�_�l������p�󵲧����\n\n");
        Date_30();
    }
    if( DayMinus_30( date[0], date[3], date[1], date[4], date[2], date[5] ) == 1 ){
        printf( "���~�I\n��J�`�M����30��\n\n" );
        Date_30();
    }
}

int DataCheck_Day( int year, int month, int day ){ //�P�_��J�O�_�P��Ʈw�ۦP
	int i, j;
    for(i = 0;; i++){
    	if(i > 1000){
    		printf("��ƿ��~�ο�\n");
			return 13579; 
		}
        for(j = 0; j < 10; j++){
            if( (input[i][0] - '0') == year / 1000 && (input[i][1] - '0') == year % 1000 / 100 && (input[i][2] - '0') == year % 100 / 10 && (input[i][3] - '0') == year % 10 ){  //�P�_�~��
                if( month < 10 ){ //��J����p��10
                    if( input[i][5] == '0' && input[i][6] - '0' == month){
                        if( day < 10 ){ //��J��p��10
                            if( input[i][9] - '0' == day && input[i][8] == '0'){
                                return i;
                                break;
                            }
                        }
                        else{ // ��J��j��10
                            if( (input[i][8] - '0') == day / 10 && (input[i][9] - '0') == day % 10 ){
                                return i;
                                break;
                            }
                        }
                    }
                }
                else{ // ��J��j��10
                    if( (input[i][5] - '0') == month / 10 && (input[i][6] - '0') == month % 10){
                        if(day < 10){ //��J��p��10
                            if( input[i][9] - '0' == day && input[i][8] == '0'){
                                return i;
                                break;
                            }
                        }
                        else{ //��J��j��10
                            if( input[i][8] - '0' == day / 10 && input[i][9] - '0' == day % 10 ){
                                return i;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
}


int SleepCount(int x){  //10 11 : 13 14 
	int W_count = 0, S_count = 0, i, j;
	int sleep, wake, S_hr, S_min, W_hr, W_min;
	sleep = 0;
	S_hr = (input[x+1][10] - '0') * 10 + (input[x+1][11] - '0');
	S_min = (input[x+1][13] - '0') * 10 + (input[x+1][13] - '0');
	W_hr = (input[x+2][9] - '0') * 10 + (input[x+2][10] - '0');
	W_min = (input[x+2][12] - '0') * 10 + (input[x+2][13] - '0');
	//printf("%d:%d    %d:%d\n", S_hr, S_min, W_hr, W_min);
	if(S_hr > W_hr && S_min <= W_min){
		//printf("1.\n");
		sleep = (24 - S_hr + W_hr) * 60 + W_min - S_min;
	}
	else if(S_hr > W_hr && S_min > W_min){
		//printf("2.\n");
		sleep = (24 - S_hr + W_hr - 1) * 60 + 60 - S_min;
	}
	else if(S_hr < W_hr && S_min <= W_min){
		//printf("3.\n");
		sleep = (W_hr - S_hr) * 60 + W_min - S_min;
	}
	else if(S_hr < W_hr && S_min > W_min){
		//printf("4.\n");
		sleep = (W_hr - S_hr - 1) * 60 + 60 - S_min;
	}
	return sleep;	
}

void SleepRate_1( int x ){
	int time, hour;
	
	hour = (input[x+1][10] - '0') * 10 + (input[x+1][11] - '0');
	time = SleepCount(x);
	//printf("SleepTime = %d", time);
	if(hour <= 22 && hour >20){
		printf("�ܰ��d��\n");
	}
	else if(hour <= 24 && hour > 22){
		printf("��\n");
	}
	else if(hour <= 2 && hour > 0){
		printf("���I�Χa\n");
	}
	else if(hour <= 4 && hour > 2){
		printf("�ӱߺΤF�A�鶴�餣�n\n");
	}
	else if(hour <= 6 && hour > 4){
		printf("�w�g�n���W�F�A�O�n�X�I��\n");
	}
	else if(hour > 6 && hour <= 10){
		printf("���w\n");
	}
	if(time >= 600){
		printf("���I�ΤӦh�F��\n");
	}
	else if(time < 600 && time >= 480){
		printf("�ίv�D�`�R��\n");
	}
	else if(time < 480 && time >= 360){
		printf("�����i�H�h�Τ@�I\n");
	}
	else if(time < 360 && time >=240){
		printf("�O�o�h�`�N�ίv�ɶ������\n");
	}
	else if(time < 240 && time >= 120){
		printf("�ίv�鶴�鰷�d�O�ܭ��n����A���U�n����~����n���ͬ�\n");
	}
	else if(time < 120){
		printf("�w���a\n");
	}
	printf("\n");
}

void SleepRate7a30(int num){
	int i, y, m, d, hr, SleepSum, days;
	hr = 0;
	SleepSum = 0;
	days = num;
	y = date[0];
	m = date[1];
	d = date[2];
	
	while(num --){
		i = DataCheck_Day(y, m, d);
		//printf("i = %d\n", i);
		if(i == 13579){
			printf("�o�Ϳ��~�I\n��ƿ򥢩Τ�����\n\n");
			break;
		}
		//printf("%d/%d/%d\n", y,m,d);
		hr += (input[i+1][10] - '0') * 10 + (input[i+1][11] - '0');
		//printf("%d\n%d\n", (input[i+1][10] - '0') * 10 + (input[i+1][11] - '0'), hr) ;
		SleepSum += SleepCount(i);
		//�p��j�� 
		if(m == 12){
			if(d == 31){
				y += 1;
				m = 1;
				d = 1;
			}
			else{
				d++;
			}
		}
		else if(m == 1 || m == 3 || m == 5 || m == 7 || m == 8 || m == 10){
			if(d == 31){
				m += 1;
				d = 1;
			}
			else{
				d++;
			}
		}
		else if(m == 4 || m == 6 || m == 9 || m == 11){
			if(d == 30){
				m += 1;
				d = 1;
			}
			else{
				d++;
			}
		}
		else if(y % 4 != 0 || ( y % 100 == 0 && y % 400 != 0) && m == 2){ // ���~ 
			if(d == 28){
				m += 1;
				d = 1;
			}
			else{
				d++;
			}
		}
		else if((y % 4 == 0 &&  y % 100 != 0 ) || y % 400 == 0 && m == 2){ //�|�~ 
			if(d == 29){
				m += 1;
				d = 1;
			}
			else{
				d++;
			}
		}
	}
	if(i != 13579){
		SleepSum /= days;
		hr /= days;
	
		if(hr <= 22 && hr >20){
			printf("�ܰ��d��\n");
		}
		else if(hr <= 24 && hr > 22){
			printf("��\n");
		}
		else if(hr <= 2 && hr > 0){
			printf("���I�Χa\n");
		}
		else if(hr <= 4 && hr > 2){
			printf("�ӱߺΤF�A�鶴�餣�n\n");
		}
		else if(hr <= 6 && hr > 4){
			printf("�w�g�n���W�F�A�O�n�X�I��\n");
		}
		else if(hr > 6 && hr <= 10){
			printf("���w\n");
		}
		if(SleepSum >= 600){
			printf("���I�ΤӦh�F��\n");
		}
		else if(SleepSum < 600 && SleepSum >= 480){
			printf("�ίv�D�`�R��\n");
		}
		else if(SleepSum < 480 && SleepSum >= 360){
			printf("�����i�H�h�Τ@�I\n");
		}
		else if(SleepSum < 360 && SleepSum >=240){
			printf("�O�o�h�`�N�ίv�ɶ������\n");
		}
		else if(SleepSum < 240 && SleepSum >= 120){
			printf("�ίv�鶴�鰷�d�O�ܭ��n����A���U�n����~����n���ͬ�\n");
		}
		else if(SleepSum < 120){
			printf("�w���a\n");
		}
		printf("\n");
	}
	
}

void DataInput(){
    FILE *fp_read = fopen( "Record.txt", "r" );
    int i = 0, j;

    while( fscanf( fp_read, "%s", input[i] ) != EOF ){
        //printf( "%s\n", input[i] );
        i++;
    }
    fclose( fp_read );
}

int main(){
	int menu;
	while(1){
		printf("[1]�x�� [2]�Ҫ� [3]��� [4]�ϥΪ̬��� [5]�ϥΪ̫�ĳ: ");
		scanf("%d",&menu);
		switch(menu){
			case 1:
				alarm();
				break;
			case 2:
				timetable();
				break;
			case 3:
				Calendar();
				break;
			case 4:
				Record();
				break;
			case 5:
				Advise();
				break;
		}
	}
	return 0;
}
