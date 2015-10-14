/*
All rights reserved by Zoe Liu
File Name       : receive.c
Used for        : CNT5410 Assignment #2
Author          : Zoe Liu
Dile Description: This function is used to receive file fro client.
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<gcrypt.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/socket.h>


void receive(char *filename, char *port)
{



  int port1 = 0;
  port1 = atoi(port);
           
  struct sockaddr_in decaddr;    //initialize decrypt
  bzero(&decaddr, sizeof(decaddr));
  decaddr.sin_family = AF_INET;
  decaddr.sin_addr.s_addr = htons(INADDR_ANY);
  decaddr.sin_port = htons(port1);
           
  //create a socket
  int decrypt_socket = socket(AF_INET,SOCK_STREAM,0);
  if(decrypt_socket < 0)
  {
      printf("Create Socket Failed!");
      exit(1);
  }
           
  if(bind(decrypt_socket,(struct sockaddr*)&decaddr,sizeof(decaddr)))
  {
       printf("Server Bind Port : %d Failed!", port1);
       exit(1);
  }
           
  //socket listen
  if (listen(decrypt_socket, 10))
  {
      printf("Server Listen Failed!");
      exit(1);
  }
           
  while(1)
   {
               
      struct sockaddr_in cryptaddr;
      socklen_t cryptaddr_length = sizeof(cryptaddr);
               
      //create a communication channel
      int new_socket = accept(decrypt_socket, (struct sockaddr*)&cryptaddr, &cryptaddr_length);
      if (new_socket < 0)
      {
         printf("Server Accept Failed!\n");
         break;
      }       
      
      int BUFFER_SIZE = 16;
      char buffer[BUFFER_SIZE];
      bzero(buffer, sizeof(buffer));

/*      //receive filename
     if (recv(new_socket, buffer, BUFFER_SIZE, 0) < 0)
     {
         printf("Recieve filename failed!\n");
         break;
      }
      else
      {
          printf("buffer=%s", buffer);
      }
     char filename[260];
     bzero(filename, sizeof(filename));
     strncpy(filename, buffer, strlen(buffer));
               
     if (filename != NULL)
     {
                    printf("filename receive sucessful!=");
     }
*/
     int length = 0;
     length = recv(new_socket, buffer, BUFFER_SIZE, 0);
               
     if (length < 0)
     {
       printf("Recieve Data Failed!\n");
       exit(1);
     }
               
               
                FILE *fp = fopen(filename, "r");
               
                if (fp == NULL)     //never have this file
                {
                    int i = 0;
                    //receive data
                   
                    fp = fopen(newfilename, "w");
                   
                   
                   
                    while (strlen(buffer) > 0)
                    {
                        //decrypt
                        printf("buffer%s", buffer);
                        fputs(buffer, fp1);
                        printf("接受中%d", i);
                        bzero(buffer, sizeof(buffer));
                    }
                   
                }
                else
                {
                    printf("file have exist!");
                    exit(33);
                }
               
                fclose(fp1);
                fclose(fp);
         }
}
