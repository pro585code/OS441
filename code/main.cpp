#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtDebug>
#include <QQuickView>
#include "shakehands.h"
#include <QFile>
#include <QTextStream>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>// for windows:: #include <time.h>
//=========================================================//
// only for windows//
//-----------------//
#ifdef __unix__
# include <unistd.h>
#elif defined _WIN32
# include <windows.h>
#define sleep(x) Sleep(1000 * x)
#endif

#define N 7
#define RUNNING_THREADS 4
#define BUNNY 0
#define DEVIL 1
#define TWEETY 2
#define MARVIN 3
#define MOUNTAIN 4
#define CARROT1 5
#define CARROT2 6
#define BOARDSIZE 5
#define M 3

int globalPause = 0;
FILE *output;
ShakeHands myclass;
int getRandom(int rangeLow, int rangeHigh, struct timeval time);
void setup_time_seed();
void printboard();
void capture_winner(int Final_count);

//=================================================================================================//
// prototypes

//=================================================================================================//
typedef struct thread {
     pthread_t thread_id;
     int       thread_num,id,pos_x,pos_y;
     int copy_COUNT_GLOBAL;
     char name[15];
     int condition,copy_FINISH_LINE;
     int CARROT;
} ThreadData;

//used to get winners to second part of the race
char winner_part1[2][15];

//used to see if marvin has all the carrots
int allcarrots_gone=0;

ThreadData thread[N];

pthread_mutex_t toon_signal_mutex;

struct table{
    struct coordinate {
        char TOON;
        int SPOT_TAKEN;
    } POS[5];
} BOARD[5] =
    {
        {
                .POS = {{ .TOON = '-', .SPOT_TAKEN = 0 },
                            { .TOON = '-', .SPOT_TAKEN = 0},
                            { .TOON = '-', .SPOT_TAKEN = 0 },
                            { .TOON = '-', .SPOT_TAKEN = 0},
                            { .TOON = '-', .SPOT_TAKEN = 0 }
                         }
            },
        {
                .POS = {{ .TOON = '-', .SPOT_TAKEN = 0 },
                        { .TOON = '-', .SPOT_TAKEN = 0 },
                        { .TOON = '-', .SPOT_TAKEN = 0 },
                        { .TOON = '-', .SPOT_TAKEN = 0 },
                        { .TOON = '-', .SPOT_TAKEN = 0 }
                     }
        },
        {
                .POS = {{ .TOON = '-', .SPOT_TAKEN = 0 },
                        { .TOON = '-', .SPOT_TAKEN = 0 },
                        { .TOON = '-', .SPOT_TAKEN = 0 },
                        { .TOON = '-', .SPOT_TAKEN = 0 },
                        { .TOON = '-', .SPOT_TAKEN = 0 }
                    }
        },

        {
              .POS = {{ .TOON = '-', .SPOT_TAKEN = 0 },
                    { .TOON = '-', .SPOT_TAKEN = 0 },
                    { .TOON = '-', .SPOT_TAKEN = 0 },
                    { .TOON = '-', .SPOT_TAKEN = 0 },
                    { .TOON = '-', .SPOT_TAKEN = 0 }
                   }
        },
        {
              .POS = {{ .TOON = '-', .SPOT_TAKEN = 0 },
                        { .TOON = '-', .SPOT_TAKEN = 0 },
                        { .TOON = '-', .SPOT_TAKEN = 0 },
                        { .TOON = '-', .SPOT_TAKEN = 0 },
                        { .TOON = '-', .SPOT_TAKEN = 0 }
                   }
        }
    };

struct mutex_shared{
    int CONDITION_TOON,WHOLE_CYCLE,COUNT_GLOBAL,FROZEN[2],FINISH_LINE;
    char WINNER[15];
    struct DEAD{
        int IS_ALIVE;
        int HAS_CARROT;
    }TOON[RUNNING_THREADS];
}SHARED;

//=================================================================================================//
//Functions and structs used for the second part of the Race
typedef struct threadP2 {
     pthread_t thread_id;
     int       thread_num,id;
     int copy_COUNT_GLOBAL;
     char name[15];
     int condition,position,copy_FINISH_LINE;
} ThreadDataP2;

pthread_mutex_t toon_signal_mutexP2;

struct mutex_SHAREDP2P2{
int CONDITION_TOON,WHOLE_CYCLE,COUNT_GLOBAL,FROZEN[2],FINISH_LINE,TOON_POSITION[2];
char WINNER[15];
}SHAREDP2;

void print_board_horizontal(int toon_pos[]){

    char sender2[2][10];
    char toon_letter[2]={winner_part1[0][0],winner_part1[1][0]};
    for(int column= 0 ; column<10; column++){
        printf("_ ");
        fprintf(output,"_ ");
    }

    for(int agent=0;agent<2;agent++){
        printf("\n");
        fprintf(output,"\n");
        for(int column= 0 ; column<10; column++){
            if(toon_pos[agent]==column){
                printf("%c ",toon_letter[agent]);
                fprintf(output,"%c ",toon_letter[agent]);
                sender2[agent][column] = toon_letter[agent];
            }else{
                printf("_ ");
                fprintf(output,"_ ");
                sender2[agent][column] = '-';
            }

            //output to gui
            myclass.cppReturnAnswer24(sender2[0][0]);
            myclass.cppReturnAnswer25(sender2[0][1]);
            myclass.cppReturnAnswer26(sender2[0][2]);
            myclass.cppReturnAnswer27(sender2[0][3]);
            myclass.cppReturnAnswer28(sender2[0][4]);
            myclass.cppReturnAnswer29(sender2[0][5]);
            myclass.cppReturnAnswer30(sender2[0][6]);
            myclass.cppReturnAnswer31(sender2[0][7]);
            myclass.cppReturnAnswer32(sender2[0][8]);
            myclass.cppReturnAnswer33(sender2[0][9]);
            myclass.cppReturnAnswer34(sender2[1][0]);
            myclass.cppReturnAnswer35(sender2[1][1]);
            myclass.cppReturnAnswer36(sender2[1][2]);
            myclass.cppReturnAnswer37(sender2[1][3]);
            myclass.cppReturnAnswer38(sender2[1][4]);
            myclass.cppReturnAnswer39(sender2[1][5]);
            myclass.cppReturnAnswer40(sender2[1][6]);
            myclass.cppReturnAnswer41(sender2[1][7]);
            myclass.cppReturnAnswer42(sender2[1][8]);
            myclass.cppReturnAnswer43(sender2[1][9]);



        }
    }
}

void toon_signal_P2(ThreadDataP2 *toon){
   struct timeval time;

   while(!globalPause){}
   pthread_mutex_lock(&toon_signal_mutexP2);

        if(SHAREDP2.CONDITION_TOON==toon->condition){
            SHAREDP2.CONDITION_TOON=toon->id;
            printf("\nCOUNT_GLOBAL = %d \t toon->name = %s \t| next CONDITION_TOON = %d\n",
                                 SHAREDP2.COUNT_GLOBAL,toon->name,SHAREDP2.CONDITION_TOON);
            fprintf(output,"\nCOUNT_GLOBAL = %d \t toon->name = %s \t| next CONDITION_TOON = %d\n",
                                 SHAREDP2.COUNT_GLOBAL,toon->name,SHAREDP2.CONDITION_TOON);

            //if(toon->name == "Marvin")
            if(strcmp(toon->name,"Muttley") == 0){
                SHAREDP2.WHOLE_CYCLE++;
                if(SHAREDP2.WHOLE_CYCLE%2 == 0){//even
                    SHAREDP2.FROZEN[0]=getRandom(0,1,time);
                    SHAREDP2.FROZEN[1]=getRandom(0,1,time);
                    printf("%s can shoot, timing is even - FROZEN[%s] = %d  - FROZEN[%s] = %d\n",toon->name,winner_part1[0],SHAREDP2.FROZEN[0],winner_part1[1],SHAREDP2.FROZEN[1]);
                    fprintf(output,"%s can shoot, timing is even - FROZEN[%s] = %d  - FROZEN[%s] = %d\n",toon->name,winner_part1[0],SHAREDP2.FROZEN[0],winner_part1[1],SHAREDP2.FROZEN[1]);
                }
                print_board_horizontal(SHAREDP2.TOON_POSITION);
                //print_board_vertical(TOON_POSITION);
                if(SHAREDP2.FINISH_LINE){
                        toon->copy_FINISH_LINE=SHAREDP2.FINISH_LINE;
                        if(SHAREDP2.TOON_POSITION[0]==SHAREDP2.TOON_POSITION[1]){
                            strcpy(SHAREDP2.WINNER,"IT IS A TIE!");
                        }
                        printf("\n WINNER = %s\n",SHAREDP2.WINNER);

                        fprintf(output,"\n WINNER = %s\n",SHAREDP2.WINNER);


                        printf("\n WINNER = yourmom \n");

                        myclass.cppReturnAnswer46(QVariant(SHAREDP2.WINNER));


                }
            }else{

///////////////skip because we fucked up
                myclass.cppReturnAnswer45(SHAREDP2.FROZEN[toon->id]);

                    if(!SHAREDP2.FROZEN[toon->id]){
                        toon->position++;
                        SHAREDP2.TOON_POSITION[toon->id]=toon->position;
                        printf("%s move position : %d",toon->name,toon->position);
                        fprintf(output,"%s move position : %d",toon->name,toon->position);
                    }else{
                        printf("%s was frozen, don't move position : %d",toon->name,toon->position);
                        fprintf(output,"%s was frozen, don't move position : %d",toon->name,toon->position);
                        SHAREDP2.FROZEN[toon->id]--;
                    }
                    if(toon->position>=9||SHAREDP2.FINISH_LINE==1){
                        if(!SHAREDP2.FINISH_LINE){
                            strcpy(SHAREDP2.WINNER,toon->name);
                        }
                        toon->copy_FINISH_LINE=SHAREDP2.FINISH_LINE=1;
                    }
            }
            SHAREDP2.COUNT_GLOBAL++;
            toon->copy_COUNT_GLOBAL=SHAREDP2.COUNT_GLOBAL;
        }
    pthread_mutex_unlock(&toon_signal_mutexP2);
}

void *run_API_P2(void *thread)
{
   ThreadDataP2 *toon  = (ThreadDataP2*)thread;
   setup_time_seed();

   while(!toon->copy_FINISH_LINE){
        toon_signal_P2(toon);
        sleep(1);
   }

   fclose(output);

   pthread_exit(NULL);
}

void init_data_toon(ThreadDataP2 *toon){
    SHAREDP2.WHOLE_CYCLE=0;
    SHAREDP2.COUNT_GLOBAL=0;
    SHAREDP2.CONDITION_TOON=2;
    SHAREDP2.FINISH_LINE=0;
    SHAREDP2.TOON_POSITION[0]=SHAREDP2.TOON_POSITION[1]=0;
    print_board_horizontal(SHAREDP2.TOON_POSITION);

    //winner 1
    toon[0].condition=2;
    toon[0].id=0;
    toon[0].position=0;
    toon[0].copy_COUNT_GLOBAL=SHAREDP2.COUNT_GLOBAL;
    toon[0].copy_FINISH_LINE=SHAREDP2.FINISH_LINE;
    //assign name from winner array here
    strcpy(toon[0].name, winner_part1[0]);

    //winner 2
    toon[1].id=1;
    toon[1].condition=0;
    toon[1].position=0;
    toon[1].copy_COUNT_GLOBAL=SHAREDP2.COUNT_GLOBAL;
    toon[1].copy_FINISH_LINE=SHAREDP2.FINISH_LINE;
    //assign name from winner array here if applicable
    strcpy(toon[1].name, winner_part1[1]);

    //was marvin is now mutley
    toon[2].id=2;
    toon[2].condition=1;
    toon[2].position=0;
    toon[2].copy_COUNT_GLOBAL=SHAREDP2.COUNT_GLOBAL;
    toon[2].copy_FINISH_LINE=SHAREDP2.FINISH_LINE;
    strcpy(toon[2].name, "Muttley");

}

//=================================================================================================//
//functions used for part 1 of the race
int getRandom(int rangeLow, int rangeHigh, struct timeval time) {
    gettimeofday(&time,NULL);
    srandom((unsigned int) time.tv_usec);
    double myRand = rand()/(1.0 + RAND_MAX);
    int range = rangeHigh - rangeLow + 1;
    int myRand_scaled = (myRand * range) + rangeLow;
    return myRand_scaled;
}

void pick_square(char toon,int process){
    struct timeval time;
    int row,column, not_done=1;
    while(not_done){
        row=getRandom(0,BOARDSIZE-1,time);
        column=getRandom(0,BOARDSIZE-1,time);
        if(!BOARD[row].POS[column].SPOT_TAKEN){
            BOARD[row].POS[column].TOON=toon;
            BOARD[row].POS[column].SPOT_TAKEN=1;
            thread[process].pos_x=row;
            thread[process].pos_y=column;
            not_done=0;
        }
    }
}

void free_old_spot(int process){
    int old_row,old_column;
    old_row=thread[process].pos_x;
    old_column=thread[process].pos_y;
    BOARD[old_row].POS[old_column].SPOT_TAKEN=0;
    BOARD[old_row].POS[old_column].TOON='-';
}

void new_position(int process,int *new_x,int *new_y){
    struct timeval time;
    int moveX,moveY,not_done;
    not_done=1;
    while(not_done){
        moveX=getRandom(0,2,time)-1;
        (*new_x)=thread[process].pos_x+moveX;
        moveY=getRandom(0,2,time)-1;
        (*new_y)=thread[process].pos_y+moveY;

        if(!(*new_x<0 || *new_x>BOARDSIZE-1)&&!(*new_y<0 || *new_y>BOARDSIZE-1)){
            not_done=0;
        }
    }
}

void takeover_place(char toon,int process,int new_x,int new_y){
    free_old_spot(process);
    BOARD[new_x].POS[new_y].TOON=toon;
    BOARD[new_x].POS[new_y].SPOT_TAKEN=1;
    thread[process].pos_x=new_x;
    thread[process].pos_y=new_y;
}

void pick_square_marvin(char toon, int process, char name[]){
    int not_done,new_x,new_y;
    not_done=1;
    while(not_done){
        new_position(process, &new_x, &new_y);

        if(!BOARD[new_x].POS[new_y].SPOT_TAKEN){
            takeover_place(toon, process, new_x, new_y);
            not_done=0;
        }else{
            switch(BOARD[new_x].POS[new_y].TOON){
                case 'C':
                    //only if the marvin does not have a carrot
                    if (!thread[process].CARROT) {
                        thread[process].CARROT = 1;
                        SHARED.TOON[process].HAS_CARROT = thread[process].CARROT;
                        takeover_place(toon, process, new_x, new_y);
                        not_done = 0;
                    }
                    break;
                case 'F':
                    if(thread[process].CARROT){
                        SHARED.FINISH_LINE=1;
                        strcpy(SHARED.WINNER,name);
                        free_old_spot(process);
                        SHARED.TOON[process].IS_ALIVE=0;
                        not_done=0;
                    }
                    break;
                case 'B':
                    SHARED.TOON[BUNNY].IS_ALIVE=0;
                    if(SHARED.TOON[BUNNY].HAS_CARROT){
                        if(thread[process].CARROT==1){
                            allcarrots_gone=1;
                        }
                        thread[process].CARROT=1;
                    }
                    takeover_place(toon, process, new_x, new_y);
                    not_done=0;
                    break;
                case 'D':
                    SHARED.TOON[DEVIL].IS_ALIVE=0;
                    if (SHARED.TOON[DEVIL].HAS_CARROT) {
                        if (thread[process].CARROT == 1) {
                            allcarrots_gone = 1;
                        }
                        thread[process].CARROT=1;
                    }
                    takeover_place(toon, process, new_x, new_y);
                    not_done=0;
                    break;
                case 'T':
                    SHARED.TOON[TWEETY].IS_ALIVE=0;
                    if(SHARED.TOON[TWEETY].HAS_CARROT){
                        if(thread[process].CARROT==1){
                            allcarrots_gone=1;
                        }
                        thread[process].CARROT=1;
                    }
                    takeover_place(toon, process, new_x, new_y);
                    not_done=0;
                    break;
            }
        }
    }
}

void change_mountain(char toon_letter,int process){
    int not_done,new_x,new_y;
    not_done=1;
    while(not_done){
        new_position(process, &new_x, &new_y);

        if(!BOARD[new_x].POS[new_y].SPOT_TAKEN){
            takeover_place(toon_letter, process, new_x, new_y);
            not_done=0;
        }
    }
}

void pick_square_tooneys(char toon_letter,int process,char name[]){
    int not_done,new_x,new_y;
    not_done=1;
    while(not_done){
        new_position(process, &new_x, &new_y);

        if(!BOARD[new_x].POS[new_y].SPOT_TAKEN){
            takeover_place(toon_letter, process, new_x, new_y);
            not_done=0;
        }else{
            switch(BOARD[new_x].POS[new_y].TOON){
                case 'C':
                    //if the toon does not already have a carrot
                    if(!thread[process].CARROT){
                    thread[process].CARROT=1;
                    SHARED.TOON[process].HAS_CARROT=thread[process].CARROT;
                    takeover_place(toon_letter, process, new_x, new_y);
                    not_done=0;
                    }
                    break;
                case 'F':
                    if(thread[process].CARROT){
                        SHARED.FINISH_LINE=1;
                        strcpy(SHARED.WINNER,name);
                        free_old_spot(process);
                        SHARED.TOON[process].IS_ALIVE=0;
                        not_done=0;
                    }
                    break;
            }
        }
    }
}

void walking_marving(ThreadData *toon){
    SHARED.WHOLE_CYCLE++;
    if(SHARED.WHOLE_CYCLE%3 == 0 && !SHARED.FINISH_LINE){
        change_mountain('F',MOUNTAIN);
    }
    pick_square_marvin('M', toon->id, toon->name);
    printboard();
}

void walking_tooneys(ThreadData *toon){
    char toon_letter=toon->name[0];
    pick_square_tooneys(toon_letter, toon->id, toon->name);
    printboard();
}

void toon_signal(ThreadData *toon){
    static int Final_count =0;
   pthread_mutex_lock(&toon_signal_mutex);
        if(SHARED.CONDITION_TOON==toon->condition){
            SHARED.CONDITION_TOON=toon->id;

            if(!SHARED.FINISH_LINE){
                if(strcmp(toon->name,"Marvin") == 0 && SHARED.TOON[toon->id].IS_ALIVE){
                    printf("\t-----------------------------------\n");
                    fprintf(output,"\t-----------------------------------\n");
                    walking_marving(toon);
                }else{
                    if(SHARED.TOON[toon->id].IS_ALIVE){
                        printf("\t-----------------------------------\n");
                        fprintf(output,"\t-----------------------------------\n");
                        walking_tooneys(toon);
                    }
                }
        } else {

            if (Final_count < 1) {
                SHARED.FINISH_LINE = 0;

            } else {

                toon->copy_FINISH_LINE = SHARED.FINISH_LINE = 1;
                globalPause = 1;
                myclass.cppReturnAnswer44(1);
            }
            if (Final_count < 2) {
                printf("\n WINNER = %s\n", SHARED.WINNER);
                fprintf(output,"\n WINNER = %s\n", SHARED.WINNER);
            }
            capture_winner(Final_count);

            Final_count++;
            //if marvin was the only toon alive, so everyone is dead now and and we only have 1 winner that is marvin or if marvin got both of the carrots and won
            if ((!SHARED.TOON[0].IS_ALIVE
                    && !SHARED.TOON[1].IS_ALIVE
                    && !SHARED.TOON[2].IS_ALIVE
                    && !SHARED.TOON[3].IS_ALIVE
                    && strcmp(winner_part1[1],"Marvin")
                    && Final_count == 1)
                    || allcarrots_gone==1) {


                        capture_winner(Final_count);
                        Final_count++;
                        SHARED.FINISH_LINE = 1;
                    }
        }

        SHARED.COUNT_GLOBAL++;
        toon->copy_COUNT_GLOBAL=SHARED.COUNT_GLOBAL;
        }
    pthread_mutex_unlock(&toon_signal_mutex);
}

void setup_time_seed(){
    struct timeval time;
    gettimeofday(&time, NULL);
}

void *run_API(void *thread)
{  ThreadData *toon  = (ThreadData*)thread;
   setup_time_seed();
   while(!toon->copy_FINISH_LINE){
        toon_signal(toon);
        sleep(1);
   }
   pthread_exit(NULL);
}

void printboard(){

    char sender[5][5] = {{'-','-','-','-','-'},{'-','-','-','-','-'},{'-','-','-','-','-'},{'-','-','-','-','-'},{'-','-','-','-','-'}};
    int row,column;
    char cartoon;
    for(row=0;row<BOARDSIZE; row++){
        printf("\t");
        fprintf(output,"\t");
        for(column=0;column<BOARDSIZE;column++){
            switch(BOARD[row].POS[column].TOON){
                case 'B':
                    cartoon=BUNNY;
                    if(thread[cartoon].pos_x==row && thread[cartoon].pos_y==column && thread[cartoon].CARROT){
                        printf("%c(C)\t",BOARD[row].POS[column].TOON);
                        fprintf(output,"%c(C)\t",BOARD[row].POS[column].TOON);
                        sender[row][column]= 'W';

                    }else{
                        printf("%c\t",BOARD[row].POS[column].TOON);
                        fprintf(output,"%c\t",BOARD[row].POS[column].TOON);
                        sender[row][column]= BOARD[row].POS[column].TOON;
                    }
                    break;
                case 'D':
                    cartoon=DEVIL;
                    if(thread[cartoon].pos_x==row && thread[cartoon].pos_y==column && thread[cartoon].CARROT){
                        printf("%c(C)\t",BOARD[row].POS[column].TOON);
                        fprintf(output,"%c(C)\t",BOARD[row].POS[column].TOON);
                        sender[row][column]= 'X';
                    }else{
                        printf("%c\t",BOARD[row].POS[column].TOON);
                        fprintf(output,"%c\t",BOARD[row].POS[column].TOON);
                        sender[row][column]= BOARD[row].POS[column].TOON;
                    }
                    break;
                case 'T':
                    cartoon=TWEETY;
                    if(thread[cartoon].pos_x==row && thread[cartoon].pos_y==column && thread[cartoon].CARROT){
                        printf("%c(C)\t",BOARD[row].POS[column].TOON);
                        fprintf(output,"%c(C)\t",BOARD[row].POS[column].TOON);
                        sender[row][column]= 'Z';
                    }else{
                        printf("%c\t",BOARD[row].POS[column].TOON);
                        fprintf(output,"%c\t",BOARD[row].POS[column].TOON);
                        sender[row][column]= BOARD[row].POS[column].TOON;
                    }
                    break;
                case 'M':
                    cartoon=MARVIN;
                    if(thread[cartoon].pos_x==row && thread[cartoon].pos_y==column && thread[cartoon].CARROT){
                        printf("%c(C)\t",BOARD[row].POS[column].TOON);
                        fprintf(output,"%c(C)\t",BOARD[row].POS[column].TOON);
                        sender[row][column]= 'Y';
                    }else{
                        printf("%c\t",BOARD[row].POS[column].TOON);
                        fprintf(output,"%c\t",BOARD[row].POS[column].TOON);
                        sender[row][column]= BOARD[row].POS[column].TOON;
                    }
                    break;

                default:
                    printf("%c\t",BOARD[row].POS[column].TOON);
                    fprintf(output,"%c\t",BOARD[row].POS[column].TOON);
                    sender[row][column]= BOARD[row].POS[column].TOON;
                }
                myclass.cppReturnAnswer(QVariant(sender[0][0]));
                myclass.cppReturnAnswer0(QVariant(sender[0][1]));
                myclass.cppReturnAnswer1(QVariant(sender[0][2]));
                myclass.cppReturnAnswer2(QVariant(sender[0][3]));
                myclass.cppReturnAnswer3(QVariant(sender[0][4]));

                myclass.cppReturnAnswer4(QVariant(sender[1][0]));
                myclass.cppReturnAnswer5(QVariant(sender[1][1]));
                myclass.cppReturnAnswer6(QVariant(sender[1][2]));
                myclass.cppReturnAnswer7(QVariant(sender[1][3]));
                myclass.cppReturnAnswer8(QVariant(sender[1][4]));

                myclass.cppReturnAnswer9(QVariant(sender[2][0]));
                myclass.cppReturnAnswer10(QVariant(sender[2][1]));
                myclass.cppReturnAnswer11(QVariant(sender[2][2]));
                myclass.cppReturnAnswer12(QVariant(sender[2][3]));
                myclass.cppReturnAnswer13(QVariant(sender[2][4]));

                myclass.cppReturnAnswer14(QVariant(sender[3][0]));
                myclass.cppReturnAnswer15(QVariant(sender[3][1]));
                myclass.cppReturnAnswer16(QVariant(sender[3][2]));
                myclass.cppReturnAnswer17(QVariant(sender[3][3]));
                myclass.cppReturnAnswer18(QVariant(sender[3][4]));

                myclass.cppReturnAnswer19(QVariant(sender[4][0]));
                myclass.cppReturnAnswer20(QVariant(sender[4][1]));
                myclass.cppReturnAnswer21(QVariant(sender[4][2]));
                myclass.cppReturnAnswer22(QVariant(sender[4][3]));
                myclass.cppReturnAnswer23(QVariant(sender[4][4]));

        }

        printf("\n");
        fprintf(output,"\n");
    }
}

void create_objects(void){
    pick_square('B',BUNNY);
    pick_square('D',DEVIL);
    pick_square('T',TWEETY);
    pick_square('M',MARVIN);
    pick_square('F',MOUNTAIN);
    pick_square('C',CARROT1);
    pick_square('C',CARROT2);
}

void init_data(ThreadData *toon){
    int i;
    SHARED.WHOLE_CYCLE=0;
    SHARED.COUNT_GLOBAL=0;
    SHARED.CONDITION_TOON=3;
    SHARED.FINISH_LINE=0;
    for(i=0;i<RUNNING_THREADS;i++){
        SHARED.TOON[i].IS_ALIVE=1;
        SHARED.TOON[i].HAS_CARROT=0;
    }
    create_objects();
    printf("\t-----------------------------------\n");
    fprintf(output,"\t-----------------------------------\n");
    printboard();

    toon[0].id=0;
    toon[0].condition=3;
    toon[0].CARROT=0;
    toon[0].copy_COUNT_GLOBAL=SHARED.COUNT_GLOBAL;
    toon[0].copy_FINISH_LINE=SHARED.FINISH_LINE;
    strcpy(toon[0].name, "Bunny");

    toon[1].id=1;
    toon[1].condition=0;
    toon[1].CARROT=0;
    toon[1].copy_COUNT_GLOBAL=SHARED.COUNT_GLOBAL;
    toon[1].copy_FINISH_LINE=SHARED.FINISH_LINE;
    strcpy(toon[1].name, "Devil");

    toon[2].id=2;
    toon[2].condition=1;
    toon[2].CARROT=0;
    toon[2].copy_COUNT_GLOBAL=SHARED.COUNT_GLOBAL;
    toon[2].copy_FINISH_LINE=SHARED.FINISH_LINE;
    strcpy(toon[2].name, "Tweety");

    toon[3].id=3;
    toon[3].condition=2;
    toon[3].CARROT=0;
    toon[3].copy_COUNT_GLOBAL=SHARED.COUNT_GLOBAL;
    toon[3].copy_FINISH_LINE=SHARED.FINISH_LINE;
    strcpy(toon[3].name, "Marvin");
}

//function for getting the winner from part 1 to part 2
void capture_winner(int Final_count){
    //printf("Getting Winner Name\n");
    strcpy(winner_part1[Final_count],SHARED.WINNER);
}

//============================================================================================================================================//
int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    QObject *topLevel = engine.rootObjects().at(0);
    QQuickWindow *window = qobject_cast<QQuickWindow *>(topLevel);

    //Connect from QML to C++
    QObject::connect(window, SIGNAL(qmlRequireData(QString)),
                     &myclass, SLOT(cppGetRequest(QString)));
    //Connect from C++ to QML
    QObject::connect(&myclass, SIGNAL(cppReturnAnswer(QVariant)),
                     window, SLOT(qmlUpdate0(QVariant)));

    QObject::connect(&myclass, SIGNAL(cppReturnAnswer0(QVariant)),
                     window, SLOT(qmlUpdate1(QVariant)));

    QObject::connect(&myclass, SIGNAL(cppReturnAnswer1(QVariant)),
                     window, SLOT(qmlUpdate2(QVariant)));

    QObject::connect(&myclass, SIGNAL(cppReturnAnswer2(QVariant)),
                     window, SLOT(qmlUpdate3(QVariant)));

    QObject::connect(&myclass, SIGNAL(cppReturnAnswer3(QVariant)),
                     window, SLOT(qmlUpdate4(QVariant)));

    QObject::connect(&myclass, SIGNAL(cppReturnAnswer4(QVariant)),
                     window, SLOT(qmlUpdate5(QVariant)));

    QObject::connect(&myclass, SIGNAL(cppReturnAnswer5(QVariant)),
                     window, SLOT(qmlUpdate6(QVariant)));

    QObject::connect(&myclass, SIGNAL(cppReturnAnswer6(QVariant)),
                     window, SLOT(qmlUpdate7(QVariant)));

    QObject::connect(&myclass, SIGNAL(cppReturnAnswer7(QVariant)),
                     window, SLOT(qmlUpdate8(QVariant)));

    QObject::connect(&myclass, SIGNAL(cppReturnAnswer8(QVariant)),
                     window, SLOT(qmlUpdate9(QVariant)));

    QObject::connect(&myclass, SIGNAL(cppReturnAnswer9(QVariant)),
                     window, SLOT(qmlUpdate10(QVariant)));

    QObject::connect(&myclass, SIGNAL(cppReturnAnswer10(QVariant)),
                     window, SLOT(qmlUpdate11(QVariant)));

    QObject::connect(&myclass, SIGNAL(cppReturnAnswer11(QVariant)),
                     window, SLOT(qmlUpdate12(QVariant)));

    QObject::connect(&myclass, SIGNAL(cppReturnAnswer12(QVariant)),
                     window, SLOT(qmlUpdate13(QVariant)));

    QObject::connect(&myclass, SIGNAL(cppReturnAnswer13(QVariant)),
                     window, SLOT(qmlUpdate14(QVariant)));

    QObject::connect(&myclass, SIGNAL(cppReturnAnswer14(QVariant)),
                     window, SLOT(qmlUpdate15(QVariant)));

    QObject::connect(&myclass, SIGNAL(cppReturnAnswer15(QVariant)),
                     window, SLOT(qmlUpdate16(QVariant)));

    QObject::connect(&myclass, SIGNAL(cppReturnAnswer16(QVariant)),
                     window, SLOT(qmlUpdate17(QVariant)));

    QObject::connect(&myclass, SIGNAL(cppReturnAnswer17(QVariant)),
                     window, SLOT(qmlUpdate18(QVariant)));

    QObject::connect(&myclass, SIGNAL(cppReturnAnswer18(QVariant)),
                     window, SLOT(qmlUpdate19(QVariant)));

    QObject::connect(&myclass, SIGNAL(cppReturnAnswer19(QVariant)),
                     window, SLOT(qmlUpdate20(QVariant)));

    QObject::connect(&myclass, SIGNAL(cppReturnAnswer20(QVariant)),
                     window, SLOT(qmlUpdate21(QVariant)));

    QObject::connect(&myclass, SIGNAL(cppReturnAnswer21(QVariant)),
                     window, SLOT(qmlUpdate22(QVariant)));

    QObject::connect(&myclass, SIGNAL(cppReturnAnswer22(QVariant)),
                     window, SLOT(qmlUpdate23(QVariant)));

    QObject::connect(&myclass, SIGNAL(cppReturnAnswer23(QVariant)),
                     window, SLOT(qmlUpdate24(QVariant)));

    //connections for 2nd part
    QObject::connect(&myclass, SIGNAL(cppReturnAnswer24(QVariant)),
                     window, SLOT(qmlUpdate25(QVariant)));

    QObject::connect(&myclass, SIGNAL(cppReturnAnswer25(QVariant)),
                     window, SLOT(qmlUpdate26(QVariant)));

    QObject::connect(&myclass, SIGNAL(cppReturnAnswer26(QVariant)),
                     window, SLOT(qmlUpdate27(QVariant)));

    QObject::connect(&myclass, SIGNAL(cppReturnAnswer27(QVariant)),
                     window, SLOT(qmlUpdate28(QVariant)));

    QObject::connect(&myclass, SIGNAL(cppReturnAnswer28(QVariant)),
                     window, SLOT(qmlUpdate29(QVariant)));

    QObject::connect(&myclass, SIGNAL(cppReturnAnswer29(QVariant)),
                     window, SLOT(qmlUpdate30(QVariant)));

    QObject::connect(&myclass, SIGNAL(cppReturnAnswer30(QVariant)),
                     window, SLOT(qmlUpdate31(QVariant)));

    QObject::connect(&myclass, SIGNAL(cppReturnAnswer31(QVariant)),
                     window, SLOT(qmlUpdate32(QVariant)));

    QObject::connect(&myclass, SIGNAL(cppReturnAnswer32(QVariant)),
                     window, SLOT(qmlUpdate33(QVariant)));

    QObject::connect(&myclass, SIGNAL(cppReturnAnswer33(QVariant)),
                     window, SLOT(qmlUpdate34(QVariant)));

    QObject::connect(&myclass, SIGNAL(cppReturnAnswer34(QVariant)),
                     window, SLOT(qmlUpdate35(QVariant)));

    QObject::connect(&myclass, SIGNAL(cppReturnAnswer35(QVariant)),
                     window, SLOT(qmlUpdate36(QVariant)));

    QObject::connect(&myclass, SIGNAL(cppReturnAnswer36(QVariant)),
                     window, SLOT(qmlUpdate37(QVariant)));

    QObject::connect(&myclass, SIGNAL(cppReturnAnswer37(QVariant)),
                     window, SLOT(qmlUpdate38(QVariant)));

    QObject::connect(&myclass, SIGNAL(cppReturnAnswer38(QVariant)),
                     window, SLOT(qmlUpdate39(QVariant)));

    QObject::connect(&myclass, SIGNAL(cppReturnAnswer39(QVariant)),
                     window, SLOT(qmlUpdate40(QVariant)));

    QObject::connect(&myclass, SIGNAL(cppReturnAnswer40(QVariant)),
                     window, SLOT(qmlUpdate41(QVariant)));

    QObject::connect(&myclass, SIGNAL(cppReturnAnswer41(QVariant)),
                     window, SLOT(qmlUpdate42(QVariant)));

    QObject::connect(&myclass, SIGNAL(cppReturnAnswer42(QVariant)),
                     window, SLOT(qmlUpdate43(QVariant)));

    QObject::connect(&myclass, SIGNAL(cppReturnAnswer43(QVariant)),
                         window, SLOT(qmlUpdate44(QVariant)));

    // does nothing
    QObject::connect(&myclass, SIGNAL(cppReturnAnswer44(QVariant)),
                         window, SLOT(qmlUpdate45(QVariant)));

    //for shooter
    QObject::connect(&myclass, SIGNAL(cppReturnAnswer45(QVariant)),
                         window, SLOT(qmlUpdate46(QVariant)));

    //for text
    QObject::connect(&myclass, SIGNAL(cppReturnAnswer46(QVariant)),
                         window, SLOT(qmlUpdate47(QVariant)));



    output = fopen("/tmp/output.txt", "w+");


    int i;

      init_data(thread);
      pthread_mutex_init(&toon_signal_mutex, NULL);

          for(i=0; i<RUNNING_THREADS; i++){

             thread[i].thread_num=i;
             pthread_create(&(thread[i].thread_id), NULL, run_API, (void *)(&thread[i]));
          }

          //for (i = 0; i < N; i++){
          //  pthread_join(thread[i].thread_id, NULL);
          //}
          //pthread_mutex_destroy(&toon_signal_mutex);



      //printf("%s and %s made it to the mountain",winner_part1[0], winner_part1[1]);

      //begin the second part of the mountain race here
         ThreadDataP2 thread[M];
         init_data_toon(thread);

         pthread_mutex_init(&toon_signal_mutexP2, NULL);
         for(i=0; i<M; i++)
         {
            thread[i].thread_num=i;
            pthread_create(&(thread[i].thread_id), NULL, run_API_P2, (void *)(&thread[i]));
         }

         //for (i = 0; i < M; i++){
         //     pthread_join(thread[i].thread_id, NULL);
         //}
         //pthread_mutex_destroy(&toon_signal_mutexP2);

    return app.exec();
}
