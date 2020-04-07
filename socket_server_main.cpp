#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <pthread.h>
#include <iostream>
#include "queue_interface.h"
#include <stdio.h>
#include <set>
#include <string>
#include <fstream>
using namespace std;
#define MAX_CLIENTS 40

Queue<int,MAX_CLIENTS> qClientPool;
set<string> Dictionary {"exempt","evaluate","exodus","pandemic","aggressive","incinerate","lexicon","quarantine",
                        "frontliners","doctors","nurses","hospitals","affected","contract","developed","nations",
                        "puma","wuhan","lockdown","stay","home","wash","regularly","disinfectant","pulmonologist",
                        "daily","surging","rumour","experts","geography","worldwide","covid","corona","covid-19",
                        "kidney","liver","crown","hydrate","hydrated","avoid","mask","hand-sanitizer","gloves" };

struct Word_status{
	char* word;
	bool status;
};
Queue<Word_status,MAX_CLIENTS> qLoggerPool;   // Pool of words along with it's status

void* loggerThreadFunc(void *arg)
{
	ofstream log_file("Logger.txt",ios::out|ios::app);
	if(!log_file)
		cout << "Logger file can't be opened"<< endl;

	while(1)
	{
		Word_status ws = qLoggerPool.Pop();
		cout << "word: "<< ws.word << ":"<<boolalpha <<ws.status<<endl;
		log_file << ws.word << ":" << boolalpha << ws.status<< endl;

	}
}

void* workerThreadFunc(void *arg)
{
	// pull the sd from the queue and verify the correctness of the word , send back to the client
	//int* p_workerThread_Id = static_cast<int*>(arg);
	int workerThread_Id = reinterpret_cast<int>(arg);

	printf("Inside workerthreadFunc newsocket # %d\n",workerThread_Id);
	while(1){
		int socket_client = qClientPool.Pop();

		//send(socket_client , "Welcome client: " , 16, 0);
		char *buf = new char[1024]{}; int bytesRecd =0;
		while((bytesRecd = recv(socket_client,buf,1024,NULL)) > 0){
			//  sending the correctness as 1 for correct and 0 for incorrect
			// need to invoke teh correctness function, willc heck if its available in the set
			// if('\r\n' == buf[strlen(buf) - 1]) {
	  //   		buf[strlen(buf) - 1] = '\0';
	  //   		//buf[strlen(buf) - 2] = '\0';
			// }
	    	    		
			printf("buf = %s and bytesRecd = %d\n",buf,bytesRecd);
			Word_status ws{};
			if(Dictionary.find(buf) != Dictionary.end())
			{
				send(socket_client,"Correct",7,0);

				ws={strdup(buf),true};
				
			}
			else
			{
				send(socket_client,"Incorrect",9,0);
				ws={strdup(buf),false};
			}
			qLoggerPool.Push(ws);
			memset(buf,0,1024);
			// printf("sending to the client : 1\n");
			// char *correctness_val = "1";
			// send(socket_client,correctness_val,1,0);	
		}
		printf("Outside while and before Closing the socket communication with that client :id - %d\n",socket_client);
		delete[]buf;
		close(socket_client);
	}
}

int main()       // Server Dictionary
{
	int serverSocket, newSocket;
  	struct sockaddr_in serverAddr;
  	struct sockaddr_storage serverStorage;
  	socklen_t addr_size;
  	//Create the socket. 
  	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
  	// Configure settings of the server address struct
  	// Address family = Internet 
  	serverAddr.sin_family = AF_INET;
  	//Set port number, using htons function to use proper byte order 
  	serverAddr.sin_port = htons(9999);
  	//Set IP address to localhost 
  	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

  	//Set all bits of the padding field to 0 
  	memset(serverAddr.sin_zero, '\0', sizeof(serverAddr.sin_zero));
  	//Bind the address struct to the socket 
  	bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
  	//Listen on the socket, with 40 max connection requests queued 
  	if(listen(serverSocket,MAX_CLIENTS)==0)
    	printf("Listening\n");
  	else
    	printf("Error\n");
    pthread_t logger_thread;
    pthread_create(&logger_thread, NULL, loggerThreadFunc, NULL);

    pthread_t worker_thread[4]; int worker_id = 1; int i=0;
    for(; i < 4; ++i){
    	if( pthread_create(&worker_thread[i], NULL, workerThreadFunc, (void*)worker_id) != 0 )
        	printf("Failed to create worker thread\n"); 
    	else
    		printf("worker thread succesfully created\n");
	}

	
    while(1)
  	{
      	//Accept call creates a new socket for the incoming connection
      	addr_size = sizeof serverStorage;
      	newSocket = accept(serverSocket, (struct sockaddr *) &serverStorage, &addr_size);
      	qClientPool.Push(newSocket);
      	//for each client request creates a thread and assign the client request to it to process
      	//so the main thread can entertain next request
      	printf("after accept call on newsocket # %d\n",newSocket);
      	
      	
   	}
   	//cout << "Before joining all the 4 threads"<< endl;

   	for(i=0;i <4;++i)
   		pthread_join(worker_thread[i],NULL);
   	pthread_join(logger_thread,NULL);
	return 0;
}