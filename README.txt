README for CSCI 4061 Lab 2

Project contributors (names) : 
	Marvin Vilaysack (vilay006)
	March Phonphiboun (phonp002)

# Overview of the lab

In this lab, we have implemented a message queue system that will respond to a client and server process communicating through a key that was passed through a message. These message structures are passed between processes for Inter Process Communication (IPC). This is used to encusre each thread reads each queue when no other threads are reading it at the same time.   


# Build and executing the client and the server

Step 1. 
	Please run 'make' inside the directory with all the files. This will build all the executables needed to run the server and client.
Step 2. 
	You need to start the server by running it along with a key for the queue. 
	To do this, you will need to enter in a key as an argument when running the server executable (XXXX will be the key):
	Type:
	./server XXXX
	For example, if you wanted 3924 as the key for the queue, you would type:
	./server 3924
Step 3. You will need to start the client with the same key for the queue.
	Type: 
	./client XXXX
	For example, with running the server in Step 2, I would need to type (NOTE: key MUST be identital to the one used to run the server):
	./client 3924
Step 4.
	The client process will send 3 messages, receive 2 from the server and then end its process/shutdown.
Step 5. 
	Repeat Step 3 a few more times to create more clients for other threads in the IPC. Inside every thread, there should exist a client, the server will shut down and delete the queue. The default number of threads is 3 (three clients must be initiated before the server will shut down. This can be changed in server.c by changing numOfThreads )


# Any errors or mistaks?

No errors have been found as of yet. No queue should exist with any messages when server is ran. The lab should be ran on a linux system as we use System V message queues. 

# Designing the IPC-Semaphore lab: 

We designed & implememnted the IPC portion of the lab first. We made it sure a single threaded server and client process can communicate within each other, as well as implemented the use of threads with semaphores. 

We have implemented Interprocess communication using System V message for both the client and server processes. We simplfied the message structure by only having one main queue to send messages from the client. 


Inside our message structure, we created a structure where servers will read messages with a message type, and clients will read messages with their message type that is the same to their PID. This avoids the need to send and receive messages from both sides.

Semaphores are used to decide which thread can communicate with the client process, before releasing the semaphore flag and passing the 'lock' to someone else.

We have decided to use unnamed POSIX semaphores instead of the older SystemV semaphores <sys/sem.h>. 

# Resources
We have found the slides to be very resourceful for us as well as man pages, and online resources such as stackoverflow for solving issues.


# Outputs
We have scripted some sample outputs of the lab and how it can properly be used in
serverscript.txt and clientscript.txt