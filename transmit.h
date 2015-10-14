/*
All rights reserved by Zoe Liu
File Name       : tranmit.c
Used for        : CNT5410 Assignment #2
Author          : Zoe Liu
Dile Description: This function is used to send specified file to destination
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<gcrypt.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>


                
void filesend(char *outfname ,char *host, char *port) 
{
  struct sockaddr_in cryptaddr;    //initialize encrypt side
  bzero(&cryptaddr, sizeof(cryptaddr));
  cryptaddr.sin_family = AF_INET;
  cryptaddr.sin_addr.s_addr = htons(INADDR_ANY);
  cryptaddr.sin_port = htons(0);
  char buffer[16]; 
    //create a socket
  int crypt_socket = socket(AF_INET, SOCK_STREAM, 0);
  if(crypt_socket < 0)
  {
     printf("Error: Can't create a socket!\n");
     exit(1);
  }
                
  if(bind(crypt_socket, (struct sockaddr*)&cryptaddr, sizeof(cryptaddr)))
  {
     printf("Error: Can't create a bind!\n");
     exit(1);
   }
                
   struct sockaddr_in decaddr;		//receiver IP address
   bzero(&decaddr, sizeof(decaddr));
   decaddr.sin_family = AF_INET;
                
   if(inet_aton(host, &decaddr.sin_addr) == 0)
   {
      printf("Error: IP address have some problems!\n");
      exit(1);
   }
                
   int port1 = atoi(port);
   decaddr.sin_port = htons(port1);
                
//create connection between crypt and decrypt
    if(connect(crypt_socket, (struct sockaddr*)&decaddr, sizeof(decaddr)) < 0)
    {
      printf("Error: Can't connect to %s!\n", host);
      exit(1);
    }
    //send file
    bzero(buffer, sizeof(buffer));
    FILE *fp = fopen(outfname, "r");
    if(fp == NULL)
    {
      printf("Error: No file names %s!\n", outfname);
      exit(1);
    }
    else
    {
      while(!feof(fp))
      {
         fgets(buffer, 1024, fp);
         send(crypt_socket, buffer, 16 , 0);
      }
     }
     fclose(fp);
     close(crypt_socket);
     return 0;
 }

