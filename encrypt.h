/*
All rights reserved by Zoe Liu
File Name       : encrypt.c
Used for        : CNT5410 Assignment #2
Author          : Zoe Liu
Dile Description: This function is used to encrypt input file using AES128 in Cipher Block Chaining (CBC) Mode.*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<gcrypt.h>


void encryptFunc( char* fName , char *key , char* oName )
{
	char *readBuf;
	char *cyBuf;
	char *macBuf;
	gcry_error_t  gcryErr;
	gcry_cipher_hd_t cypHd;
	gcry_mac_hd_t macHd;
	char iniVec = {‘5’,’8’,’4’,’4’};
	size_t fileSize;
	size_t bufSize;
  	FILE *fp, *ofp ;
  	size_t macKeylen = gcry_mac_get_algo_keylen(GCRY_MAC_HMAC_SHA512);
 	size_t macBlklen = gcry_mac_get_algo_maclen(GCRY_MAC_HMAC_SHA512);
  	size_t cypKeylen = gcry_cipher_get_algo_keylen(GCRY_CIPHER_AES128);
	size_t cypBlklen = gcry_cipher_get_algo_blklen(GCRY_CIPHER_AES128);
 	
	fp = fopen (fName,”r”);
	ofp = fopen(oName, "w") ;

        if (fp == NULL)
	{
		printf(“failed to open %s\n”, fName);
		exit(1);
	}
        else
        {
             	fseek (fp, 0, SEEK_END);   //move file pointer to the end
             	fileSize = ftell (fp); ///求出当前文件指针距离文件开始的字节数
             	fclose (fp);
        }
	if(fileSize%16)
	{
		bufSize = (fileSize/16+1)*16;
	}
	else 
	{
		bufSize = fileSize
	}
	//Read input file and put it into readBuf
	readBuf = (char*)malloc(bufSize*sizeof(char));
	cyBuf = (char*)malloc(1024*sizeof(char));
	macBuf = (char*)malloc(64*sizeof(char));

	fgets(readBuf,fileSize,fp);
	int i=fileSize;
	while(i<bufSize)
	{//append 0 to the readBuf so that the size of readBuf could be divided by 16
		readBuf[i-1] = ‘0’;
		i = i+1;
	}
 
	// creates the context handle required for most of the other cipher functions and returns a handle
    	gcryErr = gcry_cipher_open (&cypHd,GCRY_CIPHER_AES128,GCRY_CIPHER_MODE_CBC,0);            
    	if ( gcryErr )
    	{
      		printf("gcry_cipher_open failed:  %s/%s\n" , gcry_strsource( gcryErr ) , gcry_strerror( gcryErr ));
      		return ;
    	}
 	//Set the key used for encryption or decryption in the context denoted by the handle 
   	gcryErr = gcry_cipher_setkey(cypHd,key,cypKeylen);
    	if (gcryErr)
    	{
		printf("gcry_cipher_setkey failed:  %s/%s\n" , gcry_strsource(gcryErr) , gcry_strerror(gcryErr));
		gcry_cipher_close(cypHd);
      		return ;
    	}
	//Set the initialization vector with integer 5844 used for encryption 
	gcryErr = gcry_cipher_setiv(cypHd,iniVec, cypBlklen);
    	if (gcryErr)
    	{
      		printf("gcry_cipher_setiv failed:  %s/%s\n" , gcry_strsource(gcryErr) , gcry_strerror(gcryErr));
		gcry_cipher_close(cypHd);
      		return;
    	}
	//This function is used to encrypt data 
    	gcryErr = gcry_cipher_encrypt(cypHd , cyBuf, sizeof(cyBuf) ,readBuf, bufSize) ;
    	if ( gcryErr )
    	{
        	printf( "gcry_cipher_encrypt failed:  %s/%s\n" , gcry_strsource( gcryErr ) , gcry_strerror( gcryErr )) ;
		gcry_cipher_close(cypHd);
        	return;
    	}
  	fputs(cyBuf,ofp);
	gcry_cipher_close(cypHd);
	printf( "Successfully encrypted %s to %s. %d bytes written." ,fName,oName,bufSize) ;


	//Do authentication and append to cypher text
    	gcryErr = gcry_mac_open ( & macHd , GCRY_MAC_HMAC_SHA512 , 0 , NULL) ;
    	if(gcryErr)
    	{
      		printf("gcry_mac_open failed:  %s/%s\n" , gcry_strsource( gcryErr ) , gcry_strerror( gcryErr )) ;
		gcry_cipher_close(macHd);
      		return ;
   	}
 	//Set the key used for hmac in the context denoted by the handle 
	gcryErr = gcry_mac_setkey(macHd,key,macKeylen) ;
    	if (gcryErr)
    	{
      		printf("gcry_mac_setkey failed:  %s/%s\n" , gcry_strsource(gcryErr) , gcry_strerror(gcryErr)) ;
		gcry_cipher_close(macHd);
      		return ;
    	}
    	gcryErr = gcry_mac_setiv(macHd, iniVec, macBlklen);
    	if (gcryErr)
    	{
      		printf("gcry_mac_setiv failed:  %s/%s\n" , gcry_strsource(gcryErr) , gcry_strerror(gcryErr));
		gcry_cipher_close(macHd);
      		return;
    	}
	//This function is used to update HMAC data 
    	gcryErr = gcry_mac_write( macHd , cyBuf , sizeof(cyBuf)) ;
    	if ( gcryErr )
    	{
        	printf( "gcry_mac_write failed:  %s/%s\n" , gcry_strsource( gcryErr ) , gcry_strerror( gcryErr )) ;
		gcry_cipher_close(macHd);
        	return;
    	}

	gcryErr = gcry_mac_read(macHd , macBuf , sizeof(macBuf));
       	if ( gcryErr )
   	{
       		printf( "gcry_mac_read failed:  %s/%s\n" , gcry_strsource( gcryErr ) , gcry_strerror( gcryErr )) ;
		gcry_cipher_close(macHd);
       		return;
   	}
	fputs(macBuf,ofp);


	// clean up after uses
  	gcry_cipher_close( macHd );
	printf(“Authenticated 64HMAC has been appended.\n”);
	fclose(fp);
	fclose(ofp);
	free(fp);
	free(ofp);
	free(macBuf);
	free(readBuf);
	free(cyBuf);

}
