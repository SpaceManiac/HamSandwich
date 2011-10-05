#include "chat.h"

/* client.c */
/*
#include<sys/socket.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>

main(int argc,char *argv[])
{
   int create_socket;
   int bufsize = 1024;
   char *buffer = malloc(bufsize);
   struct sockaddr_in address;
   
   printf("\x1B[2J");
   if ((create_socket = socket(AF_INET,SOCK_STREAM,0)) > 0)
     printf("The Socket was created\n");
   address.sin_family = AF_INET;
   address.sin_port = htons(15000);
   inet_pton(AF_INET,argv[1],&address.sin_addr);
   
   if (connect(create_socket,(struct sockaddr *)&address,sizeof(address)) == 0)
     printf("The connection was accepted with the server %s...\n",inet_ntoa(address.sin_addr));
   do{
      recv(create_socket,buffer,bufsize,0);
      printf("Message recieved: %s\n",buffer);
      if (strcmp(buffer,"/q")){
	 printf("Message to send: ");
	 gets(buffer);
	 send(cria_socket,buffer,bufsize,0);
      }
   }while (strcmp(buffer,"/q"));
   close(create_socket);
}

*/

/* server.c */
/*
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>

int mySocket;

bool InitSockets(void)
{
   if((mySocket=socket(AF_INET,SOCK_STREAM,0))>0)

     printf("The socket was created\n");
   address.sin_family = AF_INET;
   address.sin_addr.s_addr = INADDR_ANY;
   address.sin_port = htons(15000);
   if (bind(create_socket,(struct sockaddr *)&address,sizeof(address)) == 0)
     printf("Binding Socket\n");
}


int main(void)
{
   int cont,create_socket,new_socket,addrlen;
   int bufsize = 1024;
   char *buffer = malloc(bufsize);
   struct sockaddr_in address;
   
   if ((create_socket = socket(AF_INET,SOCK_STREAM,0)) > 0)
     printf("The socket was created\n");
   address.sin_family = AF_INET;
   address.sin_addr.s_addr = INADDR_ANY;
   address.sin_port = htons(15000);
   if (bind(create_socket,(struct sockaddr *)&address,sizeof(address)) == 0)
     printf("Binding Socket\n");
   listen(create_socket,3);
   addrlen = sizeof(struct sockaddr_in);
   new_socket = accept(create_socket,(struct sockaddr *)&address,&addrlen);
   if (new_socket > 0){
      printf("The Client %s is connected...\n",inet_ntoa(address.sin_addr));
      for(cont=1;cont<5000;cont++)
	printf("\x7");
   }
   do{
      printf("Message to send: ");
      gets(buffer);
      send(new_socket,buffer,bufsize,0);
      recv(new_socket,buffer,bufsize,0);
      printf("Message recieved: %s\n",buffer);
   }while(strcmp(buffer,"/q")); //user ‘q’ to quit
   close(new_socket);
   close(create_socket);
}

*/