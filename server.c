#define _GNU_SOURCE

#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>

/* includes for threading and semaphores */
#include <pthread.h>
#include <semaphore.h>

#include "message.h"

void receiveMessageFromClient(MESSAGE* msg, int receive);
void *thread_execute(void *argv);

static int q_id; // queue ID for IPC
static int serverPID = 1; // pid is 1 by default

int main(int argc, char** argv)
{
     sem_t semlock;
     pthread_t *tids;
     int i;
     int num_threads = 3;

     if(argc != 2)
     {
	  fprintf(stderr, "How to use:  %s <key>\n", argv[0]);
	  return 1;
     }

     int key = atoi(argv[1]);
     if((q_id = init_q(key)) == -1)
     {
	  perror("Failed to start server Queue");
	  return 1;
     }

     printf("Server PID: %d \n", getpid());
     printf("Server Initialized at Key Number: %d \n", key);
     printf("Initializing %d Threads \n", num_threads);
     printf("%d Clients must connect before server is shut down.\n", num_threads);

     if((tids = (pthread_t *) calloc(num_threads, sizeof(pthread_t) ) ) == NULL)
     {
	  perror("Failed to allocate memory for thread IDs.");
	  return 1;
     }
     /* initialize semaphore with shared threads */
     if(sem_init(&semlock, 0, 1) < 0) /* 0 means shared among all threads, 1 means the initial semaphore value is set to 1. */
     {
	  perror("Failed to initialize Sempahores. \n");
	  return 1;
     }
     /* create threads and run execution function */
     for (i = 0; i < num_threads; i++)
     {
	  if(pthread_create(tids + i, NULL, thread_execute, &semlock)) //directly pass semlock as the args for the execute function
	  {
	       fprintf(stderr, "Failed to create thread : %d \n", i);
	       return 1;
	  }
     }
     /* Start Cleanup: Execution complete, join threads for result */
     for (i = 0; i < num_threads; i++)
     {
	  if(pthread_join(tids[i], NULL))
	  {
	       fprintf(stderr, "Failed to join thread : %d \n", i);
	       return 1;
	  }
     }

     printf("Info: All threads complete. Shutting Down Server ... \n");
     /* Destroy semaphore */
     sem_destroy(&semlock);
     removequeue(q_id);
     return 0;
}

/* function pointer that will send 2 messages and read 3 from the client. */
void *thread_execute(void *argv)
{
     sem_t *lock = (sem_t *) argv;
     printf("(pthread ID %lu) has started. Waiting for client to connect \n", (unsigned long) pthread_self());

     while(1)
     {
	  MESSAGE *msg;
	  msg = (MESSAGE *) malloc(sizeof(MESSAGE) + MAX_SIZE -1 );
	  int receive = 0;
	  while(sem_wait(lock) == -1)
	  {
	       if(errno != EINTR)
	       {
		    perror("Thread failed to lock semaphore");
		    return NULL;
	       }
	  }
	  printf("(pthread ID: %lu) has the lock.\n", (unsigned long) pthread_self());
	  receiveMessageFromClient(msg, receive);

	  msgprintf(q_id, msg->pid, serverPID, "Hello client! This is your response message");

	  receiveMessageFromClient(msg, receive);

	  msgprintf(q_id, msg->pid, serverPID, "Enough messages, server is shutting down");

	  receiveMessageFromClient(msg, receive);

	  if(sem_post(lock) == -1)
	  {
	       perror("Thread failed to unlock semaphore.\n");
	  }
	  free(msg);
	  break; //kill ownself after one client for now
     }
     printf("Server: Waiting for next client... \n");
     return NULL;
}

void receiveMessageFromClient(MESSAGE* msg, int receive)
{
     if((receive = msgrcv(q_id, msg, RECEIVE_SZ, serverPID, 0)) < 0)
     {
	  perror("Message Receive Failed.");
     }
     printf("Received message From client PID: %ld \n", msg->pid);
     printf("\t Message:  %s \n", msg->message_text);
}

// function not used after change in requirements
int init_client_q(int key)
{
     int client_id;
     if((client_id = init_q(key)) == -1)
     {
	  fprintf(stderr, "Failed to initialize client message queue: %d", key);
	  return -1;
     }

     msgprintf(q_id, key, getpid(), "Please go to new client queue.");
     return client_id;
}
