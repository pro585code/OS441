 //2nd pretest
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <unistd.h>

#define number_o_threads 4  // actuall threads you want run
#define number_o_objects 6  // thread struct objects to be made
#define BoardX           5  // how many units in the x direction the graph is
#define BoardY           5  // same as above but y

// what you pass to thread
typedef struct thread{

        char character_name[30];
	char symbol;
        int  thread_num;
	int  position_xy[2];
	int  condition;
        pthread_t thread_id;

}thread_data;

// global scope
struct mutex_shared{

        int BOARD[BoardY][BoardX];
        int FINISH;
	int WINNER;
	int FLAG[2];
	int CONDITION;
	char CHARACTER_WITH_CARROT[30];

} SHARED_DATA;

pthread_mutex_t object_mutex;

void magic(thread_data *objects);
void printGrid();
void init_data(thread_data *objects);
void place(thread_data *objects, int number);
int  getRandom(int rangeLow, int rangeHigh, struct timeval time);

void magic(thread_data *objects){

	pthread_mutex_lock(&object_mutex);

		// handles Shared_DATA inside lock
		if(SHARED_DATA.CONDITION == objects->condition){

			SHARED_DATA.CONDITION = objects->thread_num;
			SHARED_DATA.BOARD[objects->position_xy[1]][objects->position_xy[0]] = '-';
			place(objects, objects->thread_num);
			SHARED_DATA.BOARD[objects->position_xy[1]][objects->position_xy[0]] = objects->symbol;

		}

	pthread_mutex_unlock(&object_mutex);

	printf("%s moved to x= %d y= %d \n", objects->character_name, objects->position_xy[0], objects->position_xy[1]);
}

//keep
void *API(void *thread){

        thread_data *objects = (thread_data *)thread;

        // do things here
	while(SHARED_DATA.WINNER != 1){

		magic(objects);
		sleep(2);

		printGrid();

	}

	printf("%s wins", SHARED_DATA.CHARACTER_WITH_CARROT);

        pthread_exit(NULL);

}

//keep
void printGrid(){

	for(int i=0; i<BoardY; i++){

		for(int j=0; j<BoardX; j++){

			printf("%c", SHARED_DATA.BOARD[i][j]);
		}
		printf("\n");
	}
}

// keep
int getRandom(int rangeLow, int rangeHigh, struct timeval time){

	gettimeofday(&time, NULL);
        srandom((unsigned int) time.tv_usec);

        double myRand = rand()/(1.0 + RAND_MAX);
        int range = rangeHigh - rangeLow + 1;
        int myRand_scaled = (myRand * range) + rangeLow;
        return myRand_scaled;
}

//keep
void place(thread_data *objects, int number){

	struct timeval time;
	int randx = getRandom(0,4,time);
	int randy = getRandom(0,4,time);
	int match = 0;

	while(1){
		for(int i=0; i<number_o_objects; i++){

			// debug: printf("number = %d, i= %d \n", number, i);
			if(number != i){
				if(randx == objects[i].position_xy[0] && randy == objects[i].position_xy[1]){

					match = 1;
				}
			}
		}

		if(match == 1){
			randx = getRandom(0,4,time);
			randy = getRandom(0,4,time);

			match = 0;
		}else{

			objects[number].position_xy[0] = randx;
			objects[number].position_xy[1] = randy;

			return;
		}
	}
}

void init_data(thread_data *objects){

	struct timeval time;

        //where we handle all info before I spawn threads

        for(int i=0; i<5; i++){

		for(int j=0; j<5; j++){

			SHARED_DATA.BOARD[i][j] = '-';
		}
	}
	// double check the grid
	printGrid();

	SHARED_DATA.CONDITION = 3;

	//define all your objects
	objects[0].symbol      = 'B';
	objects[0].thread_num  = 0;
	objects[0].condition   = 3;
	strcpy(objects[0].character_name, "Bunny");

	objects[1].symbol      = 'D';
	objects[1].thread_num  = 1;
	objects[1].condition   = 0;
	strcpy(objects[1].character_name, "Taz D");

	objects[2].symbol     = 'T';
	objects[2].thread_num = 2;
	objects[2].condition  = 1;
	strcpy(objects[2].character_name, "Tweety");

	objects[3].symbol     = 'M';
	objects[3].thread_num = 3;
	objects[3].condition  = 2;
	strcpy(objects[3].character_name, "Marvin");

	objects[4].symbol     = 'F';
	objects[4].thread_num = 4;
	objects[4].condition  = 9999;
	strcpy(objects[4].character_name, "Mountain");

	objects[5].symbol     = 'C';
	objects[5].thread_num = 5;
	objects[5].condition  = 9999;
	strcpy(objects[5].character_name, "Carrot");

	for(int i=0; i<number_o_objects; i++){

		//srandom((unsigned int) time.tv_usec);
		place(objects, objects[i].thread_num);
		printf("name: %s, symbol: %c, thread_num: %d\n", objects[i].character_name, objects[i].symbol, objects[i].thread_num);
		printf("position 0x = %d position 1y = %d\n", objects[i].position_xy[0], objects[i].position_xy[1]);
		printf("-----------------------------------------------\n");
	}

	// put all object symbols on board
	for(int i=0; i<6; i++){

		//1 for y 0 for x
		SHARED_DATA.BOARD[objects[i].position_xy[1]][objects[i].position_xy[0]] = objects[i].symbol;

	}


	printGrid();

}


int main(int argc, char *argv[]){

	thread_data thread[number_o_objects];

        init_data(thread);

//        pthread_mutex_init(&object_mutex, NULL);

//	for(int i=0; i<number_o_threads; i++){

//                thread[i].thread_num = i;
//                pthread_create(&(thread[i].thread_id), NULL, API,(void *)(&thread[i]));
//        }

//	for(int i=0; i<number_o_threads; i++){
//		pthread_join(thread[i].thread_id, NULL);
//	}
	//pthread_mutex_destroy(&object_mutex);
	return 0;
}

