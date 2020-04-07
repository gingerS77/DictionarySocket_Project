#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <pthread.h>
#include <vector>
#include <string>
#include <iostream>
using namespace std;

void * cientThread(void *arg)
{
  unsigned long int idThread = pthread_self();
  printf("In client thread having id = %lu\n",idThread);
  char message[1000] = "exodus";
  char buffer[1024];
  int clientSocket;
  struct sockaddr_in serverAddr;
  socklen_t addr_size;

   // Create the socket. 
  clientSocket = socket(PF_INET, SOCK_STREAM, 0);
  printf("socket created client thread # %d\n",clientSocket);
  //Configure settings of the server address
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_port = htons(9999);
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);

  //Connect the socket to the server using the address
  addr_size = sizeof serverAddr;
  int connResp = connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);
  printf("client thread after Connect, response # %d\n",connResp);
  vector<string> testing_strings{"exodus","globus","aggressive","aggresive","evaluate","pandemic","pandmic"};
  for(int i=0; i < testing_strings.size(); ++i){
	  if( send(clientSocket , testing_strings[i].c_str() , testing_strings[i].length() , 0) < 0)
	  {
	       printf("Send failed\n");
	  }  

	  
	  int bytesRecd=0;
	  //Read the message from the server into the buffer
	  if((bytesRecd = recv(clientSocket, buffer, 1024, 0)) < 0)
	  {
	      printf("Receive failed\n");
	  }
	  buffer[bytesRecd] = '\0';
	  //Print the received message
	  printf("Result for %s : Correctness received: %s and bytes recd = %d\n",testing_strings[i].c_str(),buffer,bytesRecd);
  }
  close(clientSocket);
  pthread_exit(NULL);
}

int main()
{
  int i = 0;
  pthread_t tid;
  // while(i < 4)
  // {
    if( pthread_create(&tid, NULL, cientThread, NULL) != 0 )
          printf("Failed to create thread\n");
  //   ++i;
  // }

  
  // i = 0;
  // while(i < 4)
  // {
     pthread_join(tid,NULL);
     printf("Main: Client thread: joined\n");
  //}
  return 0;
}
 
