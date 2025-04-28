/*
Author: Zachary Reese
Dates: 9/19/23-9/27/23
Due-Date: 9/27/23 @ 11pm
Course: CSC328
Professor: PROFFESSOR_NAME_REDACTED
Assignment: Project 3
Purpose: Project 3 takes in four commands line arguments, validates the data, and uses multithreading to simultaneously process the accumulation of inputted values within subthreads. This program handles errors and gives feedback to the user.
*/
//make memcheck - valgrind

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

struct ThreadData {
	int num;
	int* nump;
	int threadNum; //1-10 value representing which thread is currently processing
};

void incrementValues(void* arg); //increments all values
void giveError(char* message); //error handling
void giveAtoiError(); //error handling for atoi errors
void printThreadData(char* printMode, struct ThreadData* data2); //prints before and after messaging showing values of accumulators and the current thread.

int x;  //declared globally


int main(int argc, char* argv[])
{
	int threadCount; //Number of threads 1-10 to create.
	
	if (argc < 5) { //print warning message about user supplying too little inputs
		printf("You must supply at least four command line arguments in the form of... \n./project3 <Number of Threads> <initial value for x> <initial value for num> <initial value for nump>\n");
        exit(2);
	} else if (argc > 5) { //print warning message about user supplying too many/ not enough inputs and may have made a mistake
		printf("Only enter 4 command line arguments. If you entered more than 4, only the first 4 will be read.\n");
	} //end if

	threadCount = atoi(argv[1]);
	
    if (threadCount == 0 && argv[1][0] != '0'){ //data validation for porcessCount
        giveAtoiError();
    } else if (threadCount <= 0 || threadCount > 10){ //between 1-10
        printf("Only enter numbers 1-10 for process count\n");
        exit(2);
    } //end if

    //Assign input to x
    x = atoi(argv[2]);

    if (x == 0 && argv[2][0] != '0') //data validation for x
        giveAtoiError();

    struct ThreadData data[threadCount];
    int inputNum;

    //Assign input to num
    inputNum = atoi(argv[3]);

    if (inputNum == 0 && argv[3][0] != '0') //data validation for num
        giveAtoiError();

    int* inputNump = (int*)malloc(sizeof(int));
	if (inputNump == NULL)
		giveError("malloc");
	*(inputNump) = atoi(argv[4]);

    if (*(inputNump) == 0 && argv[4][0] != '0') //data validation for nump
        giveAtoiError();

    for (int i = 0; i < threadCount; i++){ //initialize each element of struct array data
        data[i].num = inputNum;
        data[i].nump = (int*)malloc(sizeof(int));
		if (data[i].nump == NULL)
			giveError("malloc");
        *(data[i].nump) = *(inputNump);
    } //end for

	pthread_t threads[threadCount];
	
    
	//Create threads
	printf("Creating and processing %d threads:\n X Value= %d\n num Value= %d\n nump Value= %d\n", threadCount, x, inputNum, *inputNump);

    free(inputNump);

	for (int i = 0; i < threadCount; i++){
		data[i].threadNum = i;
		if (pthread_create(&threads[i], NULL, (void* (*)(void*))incrementValues, (void*)&data[i]) != 0){
			giveError("pthread_create");
		} //end if
	} //end for

	//Check for end of threads
	for (int i = 0; i < threadCount; i++) {
		if (pthread_join(threads[i], NULL) != 0) {
			giveError("pthread_join");
		} else {
            free(data[i].nump);
		} //end if
	} //end for

	printf("FINISHED PROCESSING\n");
} //end main


/*
Function name: incrementValues
Description: Handles each sub-thread as the starting sub-thread function. Accumulates the three accumulators. Input is taken into a void pointer to allow greater flexibility for dynamic data.
Parameters: void* arg: Is a void pointer to allow for dynamic data usage. In this case, arg takes in a ThreadData struct as a pointer that holds num and nump to be accumulated, as well as threadNum 
	to represent the 1-10 id each thread is given.
*/
void incrementValues(void* arg){
	struct ThreadData* data2 = (struct ThreadData*)arg;
	
	printThreadData("BEFORE",data2);
	x += 50;
	(data2->num)++;
	(*(data2->nump))++;
	printThreadData("AFTER",data2);
    pthread_exit(NULL);
} //end incrementValues

/*
Function name: giveError
Description: Handles common errors within program and gives a descriptive explanation to the command line.
Parameters: char* message- Char pointer used to represent a string message describing the structure that caused the error.
*/
void giveError(char* message){
    perror(message);
    exit(2);
} //end giveError

/*
Function Name: giveAtoiError
Description: Gives an error message and handles the program state for common errors caused by Atoi.
*/
void giveAtoiError(){
    printf("atoi invalid conversion to int\n");
    exit(2);
} //end giveAtoiError

/*
Function name: printThreadData
Description: Prints commonly used BEFORE and AFTER messages for processing incrementValues for each thread.
Parameters: char* printMode- Char pointer used to represent a string representing the two print modes: BEFORE and AFTER.
			struct ThreadData* data2- Struct pointer used to store locally declared accumulation data from main, as well as the 1-10 thread id.
*/
void printThreadData(char* printMode, struct ThreadData* data2){
    printf("%s increment:    Thread number %d with TID of %lu: X Value= %d     num Value= %d   nump Value= %d\n", printMode, data2->threadNum, pthread_self(), x, data2->num, *(data2->nump));
} //end printChildData
