/*
All rights reserved by Zoe Liu
File Name       : gatordec.c
Used for        : CNT5410 Assignment #2
Author          : Zoe Liu
Dile Description: This file is to receive encrypted file from gatorcrypt and decrypt it using generatd keys
*/

#include<stdio.h>
#include<stdlib.h>
#include<gcrypt.h>
#include"excpt.h"
//#include"keygen.h"
#include"decrypt.h"
//#include"transmit.h"

int main( int argc , char *argv[] )
{

//arguments for input
  char *fileName = NULL ;
  char *option = NULL ;
  char *port = NULL ;
  char *outfname =NULL ;
  int  cFile = 0;
  char kg_salt[5] = "NaCl" ;
  char kg_pwd[10];
  long kg_ita = 4096 ;
  char key[1024] ;
//  char *gcrypt_text = NULL ;
//  char gcrypt_cyphertext[1024];
//  char *auth_cypher = NULL ;
  gpg_error_t kg_err ;

// This function is designed to read arguments of the main program
//  readAug( argc , argv , fileName , option1 , dest , option2 );

  if( argc < 2 )
    {
      printf( "You should put at least 1 parameter. Please check. \n" ) ;
      printf( "Usage : gatorcrypt <input file> [-d < IP-addr:port >][-l]. \n" ) ;
      exit(1);
    }
  else if( argc > 4 )
    {
      printf( "You can input at most 3 parameters. Please check. \n" ) ;
      printf( "Usage : gatorcrypt <input file> [-d < IP-addr:port >][-l]. \n" ) ;
      exit(1);
    }


  option=argv[2]; port=argv[3];
  fileName=argv[1];
  outfname=malloc(strlen(argv[1])-3);
//  strcpy(fileName,argv[1]-2);
  int i = 0;
  while(i< strlen(argv[1])-3)
  {
    outfname[i]=argv[1][i];
    i++;
  }
  printf("%s,%s\n",fileName,outfname);
//  strcpy(oufname,argv[1],argv[1]-2);
//This function is used to check if output file exist.  
  cFile = checkFile( outfname ) ;
  if( cFile )
    {
	printf("File %s already exists. Please check.\n" , outfname ) ;
	exit(62) ;
    }
//This function is used to receive file
  if(option=="-d")
  {
     receive( fileName , port) ;
  }

//The following part is going to read password and generate key using PBKDF2.
  printf( "Please enter your password to generate key...\nPassword : " ) ;
  gets( kg_pwd );
//  key = keyGenerate( kg_pwd , kg_salt , kg_ita  ) ;
//The following part is used to calculate length of password and salt.
  size_t pwdlen=0;
  while(kg_pwd[pwdlen]!='\0') pwdlen++;
//  printf("%d,%d\n",pwdlen,sizeof(kg_salt));
  kg_err=gcry_kdf_derive( kg_pwd , pwdlen ,GCRY_KDF_PBKDF2 , GCRY_MD_SHA512 , kg_salt , sizeof(kg_salt) , kg_ita , 512 ,key) ;  
  if( kg_err )
  {
    printf( "Key generation failed for %s\n" , gpg_strerror( kg_err ) ) ;
    exit(1);
  }

  printf("key is %x\n", key);
  if( verify(fileName,key))
  {
    return 62;
  }
//This function is used to encrypt input using AES128
  decFunc( fileName, key , outfname ) ;
//This function is used to do authentication using HMAC


//Free pointer
//  free( fileName ) ;
//  free( option ) ;
//  free( dest ) ;
//  free( outfname ) ;
//  free( gcrypt_cyphertext ) ;
//  free( auth_cypher) ;
 // free( gcrypt_text) ;
  free(outfname);
}
