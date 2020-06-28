#include <stdio.h>
#include <string.h>
#include <stdlib.h> // For malloc
#include <pthread.h> // For pthread

#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "list.h"




#define MAX_NUM_CHARS 100 // Deifing the maximum size of a message
pthread_cond_t Buffer_Not_Full=PTHREAD_COND_INITIALIZER; // Creating condition variables
pthread_cond_t Buffer_Not_Empty=PTHREAD_COND_INITIALIZER;
pthread_mutex_t mVar=PTHREAD_MUTEX_INITIALIZER;

#define MAX_LEN 1024
#define PORT 22110





void *keyboardInput(void *senderList) {
	List *sl = senderList;

	while (1)
	{
		pthread_mutex_lock(&mVar); // Locking mutext
		if (sl->size > 0) {
			pthread_cond_wait(&Buffer_Not_Full,&mVar); // Do a wait on the condition that we have no more room for a message
		}
		
		printf("Enter a value: "); // Getting message from the user
		char *ptr = (char*) malloc(MAX_NUM_CHARS * sizeof(char)); // Dynamically allocating an array of char for message
		scanf("%s", ptr); // Getting user input

		List_append(sl, ptr); // Putting user input on the list

		pthread_mutex_unlock(&mVar); // Unlocking mutext

		pthread_cond_signal(&Buffer_Not_Empty);  // Signaling incase the consumer did a wait earlier, it is now ready

	}

	return NULL;
}

void *sender(void *receiverList) {
	List *rl = receiverList;
	while (1) {

		pthread_mutex_lock(&mVar);
		if (rl->size < 1)
		{
			pthread_cond_wait(&Buffer_Not_Empty,&mVar); // Doing a wait since there are no messages
		}
		

		printf(">> %s\n", (char *) List_first(rl)); // Printing the message on the terminal

		Node *temp = List_curr(rl);
		
		List_remove(receiverList); // Removing the first item on the list

		free(temp); // Freeing the dynamically allocated char array

		pthread_mutex_unlock(&mVar); // Unlocking mutex variable
        pthread_cond_signal(&Buffer_Not_Full); // Signalling incase the producer is waiting for space


	}
	
	return NULL;
}

void *display(void *receiverList) {

	return NULL;
}

void *receiver(void *receiverList) {

	return NULL;
}

int main (int argc, char *argv[]) {
	// // Testing for command line arguements
	// if (argc > 1) {
	// 	for (int i = 1; i < argc; i++)
	// 	{
	// 		printf("%s\t", argv[i]);
		
		
	// 	} 
	// } else {
	// 	printf("Incorrect Number of arguements \n");
	// 	return 0;
	// }

	// connect using: netcat -u 127.0.0.1 22110

	// PROGRAM INITIALIZATION
	List *senderList, *receiverList; // Defining the sendlist (messages to send) and the receiverList (messages received)
	senderList  = List_create(); // Initializing the list to contain the elements to send
	receiverList = List_create(); // Initializing the list to contain the elements received, and are waiting to display on the screen

	// pthread_t producerThread, consumerThread;
	// pthread_create(&producerThread, NULL, keyboardInput, senderList);
	// sender(senderList);

	// NOTE: SocketTaken from Dr. Brian Fraser CMPT 300 Workshop on June 26 2020
	struct sockaddr_in sin;
	memset(&sin, 0 ,sizeof(sin));
	sin.sin_family = AF_INET; // Connection may be from network
	sin.sin_addr.s_addr = htonl(INADDR_ANY); // Host to network long
	sin.sin_port = htons(PORT); // Host to network short

	int socketDescriptor = socket(PF_INET, SOCK_DGRAM, 0); // Creating the socket for UDP

	bind(socketDescriptor, (struct sockaddr*) &sin, sizeof(sin));

	while (1)
	{
		printf("Hello");
		struct sockaddr_in sinRemote;
		unsigned int sinLen = sizeof(sinRemote);
		char messageRx[MAX_LEN];
		int bytesRx = recvfrom(socketDescriptor, messageRx, MAX_LEN, 0, (struct sockaddr*) &sinRemote, &sinLen);
		int terminateIdx = (bytesRx < MAX_LEN) ? bytesRx: MAX_LEN - 1;
		messageRx[terminateIdx] = 0;
		printf("Message Received (%d bytes): \n\n'%s'\n", bytesRx, messageRx);

	}
	








return 0;
}