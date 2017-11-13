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

#include "message.h"

int main(int argc, char** argv) // receive 3 messages and wait for 2
{
     int server_qid;
     int client_pid = getpid(); /* our own pid */
     int key;
     MESSAGE *msg;
     msg = (MESSAGE*) malloc(sizeof(MESSAGE) + MAX_SIZE - 1);

     if(argc != 2)
     {
	  fprintf(stderr, "How to use:  %s <key>\n", argv[0]);
	  return 1;
     }
     
     key = atoi(argv[1]);
     if((server_qid = init_q(key)) == -1)
     {
	  perror("Failed to get Server Message Queue");
	  return 1;
     }

     printf("Client PID: %d \n", client_pid);
     printf("Server ID: %d connected \n", server_qid);
     
     msgprintf(server_qid, 1, client_pid, "This a message from the client"); 
     printf("Waiting response from the server. \n");

     int receive;
     if((receive = msgrcv(server_qid, msg, RECEIVE_SZ, client_pid, 0)) < 0)
     {
	  perror("Failed to receive message.");
	  return 1;
     }
     
     printf("Message received from server:\n\t%s\n", msg->message_text);

     while(1)
     {
       printf("Sending reply \n");
       msgprintf(server_qid, 1, client_pid, "Send me something!");
       if((receive = msgrcv(server_qid, msg, RECEIVE_SZ, client_pid , 0)) < 0)
	  {
	       perror("Message Receive Failed.");
	       return 1;
	  }
	  printf("Received Message from Server:\n\t%s\n", msg->message_text);
	  
	  msgprintf(server_qid, 1, client_pid, "Shutting down. ");
	  break; 
     }
     printf("Client is shutting down.\n");
     return 0;
}
