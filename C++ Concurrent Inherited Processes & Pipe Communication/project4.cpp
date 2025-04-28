/*
Author: Zachary Reese
Dates: 9/28/23-10/7/23
Due-Date: 10/7/23 @ 11pm
Course: CSC328
Professor: PROFESSOR_NAME_REDACTED
Assignment: Project 4
Purpose: Project 4 uses C++ to create two pipes for reading and writing to pipes between a Parent and its Child process. Each send/receive of data is outputted with its related information, 
	such that every message sent has a corresponding send and receive message to command line and to the pipe. 
*/
#include <iostream>
#include <cstdlib>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>

using namespace std;

//function prototypes
void receiveData(FILE* readStream, void* data, const char* processHierarchy, size_t dataSize, bool numeric);
void sendData(FILE* writeStream, void* data, const char* processHierarchy, bool numeric);
void pipeMessage(const char* processHierarchy, char* messageType, void* data, bool numeric);
void giveError(char* message);
void closeFD(int streamToClose[2], int readWrite);

//I attempted to use template variables, but they're only dynamically set to a data type once during run-time, then left static. A variant variable requires both the function call 
//and the function header to declare and use variants, which tends to defeat the purpose of using a variant once implemented. Thus, I decided to merge int and string handling for
//pipe data into one function for each respective action.

const int streamSize = 24; //standardized stream read size

int main() {
	srand(getpid()); //uses the parent's process id as a seed to get a random number
	
	//declare pipe file descriptor array (one for in and another for out)
	int pipe_fd_childToParent[2];
	int pipe_fd_parentToChild[2];
	if (pipe(pipe_fd_childToParent) == -1 || pipe(pipe_fd_parentToChild) == -1) {
		giveError(const_cast<char*>("pipe"));
	} //end if
	
	//constant chars to be sent over. They are in variables to save processing with converting to char.
	const char* chrParent = "Parent";
	const char* chrChild = "Child";
	const char* approvedMessage = "Approved";
	
	pid_t child_pid = fork(); //creates child process
	
	switch (child_pid) {
		case -1: { //child error
			giveError(const_cast<char*>("fork"));
			break;
		} //end case -1
		case 0: { //child processing
			FILE* readStream = fdopen(pipe_fd_parentToChild[0], "r"); //C file stream objects
			FILE* writeStream = fdopen(pipe_fd_childToParent[1], "w");
			if (readStream == nullptr || writeStream == nullptr) //check for errors
				giveError(const_cast<char*>("fdopen"));
			closeFD(pipe_fd_parentToChild,1); //close unused file descriptors
			closeFD(pipe_fd_childToParent,0);

			long int randProduct, randNum;
			receiveData(readStream, (void*)&randNum, chrChild, streamSize, true); //read from parent
			receiveData(readStream, (void*)&randProduct, chrChild, streamSize, true);
			
			char* response;
            if ((long int)randNum * (long int)getpid() == randProduct) {
                response = strdup(approvedMessage);
            } else {
                response = strdup("Denied");
            } //end if
			sendData(writeStream, (void*)response, chrChild, false); //send to parent
			
			char* endingMessage = new char[streamSize]; //gets ending message from Parent, "BYE"
			memset(endingMessage, 0x00, streamSize); //initialize data
			receiveData(readStream, (void*)endingMessage, chrChild, streamSize, false); //read from parent
			
			if (strcmp(endingMessage, "BYE")) { //error check to see if BYE was sent back properly.
				free(response);
				delete[] endingMessage;
				closeFD(pipe_fd_parentToChild,0); //close file descriptors
				closeFD(pipe_fd_childToParent,1);
				fclose(readStream);
				fclose(writeStream);
			} //end if
			
			break;
		} //end case 0
		default: //parent processing
			FILE* readStream = fdopen(pipe_fd_childToParent[0], "r");
			FILE* writeStream = fdopen(pipe_fd_parentToChild[1], "w");
			if (readStream == nullptr || writeStream == nullptr)
				giveError(const_cast<char*>("fdopen"));
			closeFD(pipe_fd_childToParent,1); //close unused file descriptors
			closeFD(pipe_fd_parentToChild,0);
			long int randomNumber = rand() %99 + 1; //get random number between 0 and 100 but not 0 or 100 exactly.
			long int product = randomNumber * child_pid;
			int status;
			
			sendData(writeStream, (void*)&randomNumber, chrParent, true); //send to child
			sendData(writeStream, (void*)&product, chrParent, true);
			
			char* childMessage = new char[streamSize]; //gets Approved/Denied status from Child
			memset(childMessage, 0x00, streamSize); //initialize data

			receiveData(readStream, (void*)childMessage, chrParent, streamSize, false); //read from child
			
			if (strncmp(childMessage, approvedMessage,strlen(approvedMessage)) == 0)
				cout << "Parent: Thanks for playing!" << endl;
			else
				cout << "Wrong. Please play again." << endl;

			char* tempBye = new char[streamSize];
			strcpy(tempBye, "BYE");
			
			sendData(writeStream, (void*)tempBye, chrParent, false); //send to child

			delete[] childMessage;
			delete[] tempBye;

			closeFD(pipe_fd_childToParent,0); //close file descriptors
			closeFD(pipe_fd_parentToChild,1);
			fclose(readStream);
			fclose(writeStream);
			
			wait(&status);
	} //end switch
	return 0;
} //end main

/*
Function name: receiveData
Description: Handles data extraction from the pipe for both child and parent processes using C stream objects to read from the pipes. Calls pipeMessage after every read/write for the parent/child.
Parameters: FILE* readStream- C stream object used to read from the pipe. readStream could be set to pipe_fd_childToParent[0] or pip_fd_parentToChild[0].
			void* data- A pointer to an object of unspecified type. The point either points to a long int or a C string (char*) value.
			const char* processHierarchy- A constant C string used to determine whether the current process is the Parent or the Child.
			size_t dataSize- A size_t that holds the byte count of the data to be read in. This can be predicted by the size of long int (8 bytes) and the standardized 24 bytes for C strings (char*).
			bool numeric- A boolean to determine whether the current value to read/write is an integer or a string. If numeric is true, then the current process is processing an integer, and vice versa.
*/
void receiveData(FILE* readStream, void* data, const char* processHierarchy, size_t dataSize, bool numeric) {
	char* getData = new char[streamSize];
	fgets(getData, dataSize, readStream);
	if (numeric){ //int processing
		long int intValue = atol(getData); //convert chars read from stream to long int
		memcpy(data, &intValue, sizeof(intValue)); //transfer long int read from stream to void* type to be sent back to child/parent and to pipeMessage.
	} else { //C string processing
		strncpy(static_cast<char*>(data), getData, streamSize); //transfer chars read from stream to void* type to be sent back to child/parent and to pipeMessage.
	} //end if
	delete[] getData;
	pipeMessage(processHierarchy, const_cast<char*>("received"), data, numeric); //display message read from pipe
} //end receiveData

/*
Function name: sendData
Description: Handles data writing to the pipe for both child and parent processes, using C stream objects to write to the pipes. Calls pipeMessage after every read/write for the parent/child.
Parameters: FILE* writeStream- C stream object used to write to pipe. writeStream could be set to pipe_fd_childToParent[1] or pip_fd_parentToChild[1].
			 void* data- A pointer to an object of unspecified type. The point either points to a long int or a C string (char*) value.
			 const char* processHierarchy- A constant C string used to determine whether the current process is the Parent or the Child.
			 bool numeric- A boolean to determine whether the current value to read/write is an integer or a string. If numeric is true, then the current process is processing an integer, and vice versa.
*/
void sendData(FILE* writeStream, void* data, const char* processHierarchy, bool numeric) {
	if (numeric) //int processing
		fprintf(writeStream, "%ld\n", *(long int*)(data)); //write long integer data to pipe
	else //C string processing
		fprintf(writeStream, "%s\n", static_cast<char*>(data)); //write C string data to pipe
	if (fflush(writeStream) == EOF) //check for fflush errors
		giveError(const_cast<char*>("fdopen"));
	
	pipeMessage(processHierarchy, const_cast<char*>("sending"), data, numeric); //display message sent to pipe
} //end sendData

/*
Function name: pipeMessage
Description: Handles all user output regarding reading from or writing to the pipe from the parent or child processes.
Parameters: const char* processHierarchy- A constant C string used to determine whether the current process is the Parent or the Child.
			char* messageType- A constant C string used to determine whether the current process is the Parent or the Child.
			void* data- A pointer to an object of unspecified type. The point either points to a long int or a C string (char*) value.
			bool numeric- A boolean to determine whether the current value to read/write is an integer or a string. If numeric is true, then the current process is processing an integer, and vice versa.
*/
void pipeMessage(const char* processHierarchy, char* messageType, void* data, bool numeric) { //Receive/Write output function
	if (numeric) {
		cout << processHierarchy << " " << messageType << " to pipe: " << *(int*)data << endl;
	} else
		cout << processHierarchy << " " << messageType << " to pipe: " << static_cast<char*>(data) << endl;
} //end pipeMessage

/*
Function name: giveError
Description: Handles common errors within the program and gives a descriptive explanation to the command line.
Parameters: char* message- A C string message describing the structure/object that caused the error.
*/
void giveError(char* message){
    perror(message);
    exit(2);
} //end giveError

/*
Function name: closeFD
Description: Closing the file descriptor that connects to the C file stream.
Parameters: int streamToClose[2]- File descriptor that connects to pipe for reading/writing to parent or child.
			int readWrite- Will either be a 0 or a 1 to represent the read or write side of the file descriptor connecting to pipe.
*/
void closeFD(int streamToClose[2], int readWrite) {
	if (close(streamToClose[readWrite]) == -1)
		giveError(const_cast<char*>("close file descriptor"));
} //end closeFD