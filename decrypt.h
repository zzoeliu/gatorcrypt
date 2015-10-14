/*
All rights reserved by Zoe Liu
File Name       : decrypt.c
Used for        : CNT5410 Assignment #2
Author          : Zoe Liu
Dile Description: This function is used to decrypt input file using AES128 in Cipher Block Chaining (CBC) Mode.*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<gcrypt.h>


void decFunc( char* filename , char *key , char* outfname )
{
  gcry_error_t  gcrypt_err;
  gcry_cipher_hd_t gcrypt_cipherhd;
  size_t  index;
  char gcrypt_buffer[128]; 
  size_t gcrypt_keylen = gcry_cipher_get_algo_keylen(GCRY_CIPHER_AES128);
  size_t gcrypt_blklen = gcry_cipher_get_algo_blklen(GCRY_CIPHER_AES128);
  int ininum = 5844;
  char gcrypt_inivec[16] ;
  char gcrypt_cipherbuf[1024];
  FILE *fp, *ofp ;
    gcry_mac_hd_t gcrypt_machd;
  char mac_buffer[128];
  size_t mac_keylen = gcry_mac_get_algo_keylen(GCRY_MAC_HMAC_SHA512);
  size_t mac_blklen = gcry_mac_get_algo_maclen(GCRY_MAC_HMAC_SHA512);
  int ininum = 5844;
  char gcrypt_inivec[16] ;
  char gcrypt_macbuf[64];

  fp = fopen(filename , "r") ;
  ofp = fopen(outfname, "w") ;
//get the last 64 bytes from input file
	fseek(fp, 64L, SEEK_END);	
	fread(gcrypt_macbuffer, 1, 64, fp);//Get the last 64 bytes which is HMAC.
	fseek(fp, 0L, SEEK_SET);


  while( fgets( gcrypt_buffer , 16 , fp )   )
  {
// creates the context handle required for most of the other cipher functions and returns a handle
    gcrypt_err = gcry_cipher_open ( &gcrypt_cipherhd , GCRY_CIPHER_AES128 , GCRY_CIPHER_MODE_CBC , 0 ) ;            
    if ( gcrypt_err )
    {
      printf("gcry_cipher_open failed:  %s/%s\n" , gcry_strsource( gcrypt_err ) , gcry_strerror( gcrypt_err )) ;
      return ;
    }
 //Set the key used for encryption or decryption in the context denoted by the handle 
   gcrypt_err = gcry_cipher_setkey( gcrypt_cipherhd , key , gcrypt_keylen ) ;
    if (gcrypt_err)
    {
      printf("gcry_cipher_setkey failed:  %s/%s\n" , gcry_strsource(gcrypt_err) , gcry_strerror(gcrypt_err)) ;
      gcry_cipher_close( gcrypt_cipherhd );
      return ;
    }
//Set the initialization vector with integer 5844 used for encryption 
//    gcrypt_inivec = intochar( ininum ) ;
   int i,j;
    for(i=1; ininum > 0; i++,ininum/=10) 

        gcrypt_inivec[i] = ininum%10+'0'; //

    for(j=i-1,i=1; j-i>=1; j--,i++) //

    {
        gcrypt_inivec[i] = gcrypt_inivec[i]^gcrypt_inivec[j];
        gcrypt_inivec[j] = gcrypt_inivec[i]^gcrypt_inivec[j];
        gcrypt_inivec[i] = gcrypt_inivec[i]^gcrypt_inivec[j];
    }


    gcrypt_err = gcry_cipher_setiv(gcrypt_cipherhd, gcrypt_inivec, gcrypt_blklen);
    if (gcrypt_err)
    {
      printf("gcry_cipher_setiv failed:  %s/%s\n" , gcry_strsource(gcrypt_err) , gcry_strerror(gcrypt_err));
      gcry_cipher_close( gcrypt_cipherhd );
      return;
    }
//This function is used to encrypt data
    gcrypt_err = gcry_cipher_decrypt( gcrypt_cipherhd , gcrypt_cipherbuf , 16 , gcrypt_buffer , 16 ) ;
    if ( gcrypt_err )
    {
        printf( "gcry_cipher_encrypt failed:  %s/%s\n" , gcry_strsource( gcrypt_err ) , gcry_strerror( gcrypt_err )) ;
        gcry_cipher_close( gcrypt_cipherhd );
        return;
    }
  index=index+16;
  fputs(gcrypt_cipherbuf,ofp);

    gcrypt_err = gcry_mac_open ( &gcrypt_machd , GCRY_MAC_HMAC_SHA512 , 0 , NULL) ;
    if ( gcrypt_err )
    {
      printf("gcry_mac_open failed:  %s/%s\n" , gcry_strsource( gcrypt_err ) , gcry_strerror( gcrypt_err )) ;
      return ;
    }
 //Set the key used for encryption or decryption in the context denoted by the handle 
   gcrypt_err = gcry_mac_setkey( gcrypt_machd , key , mac_keylen ) ;
    if (gcrypt_err)
    {
      printf("gcry_cipher_setkey failed:  %s/%s\n" , gcry_strsource(gcrypt_err) , gcry_strerror(gcrypt_err)) ;
      gcry_cipher_close( gcrypt_machd );
      return ;
    }
    gcrypt_err = gcry_mac_setiv(gcrypt_machd, gcrypt_inivec, mac_blklen);
    if (gcrypt_err)
    {
      printf("gcry_cipher_setiv failed:  %s/%s\n" , gcry_strsource(gcrypt_err) , gcry_strerror(gcrypt_err));
      gcry_cipher_close( gcrypt_machd );
      return;
    }
//This function is used to encrypt data 
    gcrypt_err = gcry_mac_write( gcrypt_machd , gcrypt_cipherbuf , mac_blklen ) ;
    if ( gcrypt_err )
    {
        printf( "gcry_cipher_encrypt failed:  %s/%s\n" , gcry_strsource( gcrypt_err ) , gcry_strerror( gcrypt_err )) ;
	gcry_cipher_close( gcrypt_machd );
        return;
    }


 } 

    gcry_cipher_close( gcrypt_cipherhd ); 
    gcrypt_err =  gcry_mac_verify (gcrypt_machd, gcrypt_macbuf , 64) ;

    if ( gcrypt_err )
    {
        printf( "authentication failed:  %s/%s\n" , gcry_strsource( gcrypt_err ) , gcry_strerror( gcrypt_err )) ;
        gcry_cipher_close( gcrypt_machd );
        exit(62);
    }


// clean up after uses
  gcry_cipher_close( gcrypt_machd );
  
//  free( gcrypt_buffer ) ;
//  free( gcrypt_cipherbuf ) ;
//  free( gcrypt_inivec ) ;
//  free( fp ) ;
//  free(ofp);
    printf( "Successfully encrypted %s to %s. %d bytes written.\n" , filename , outfname ,index) ;


}
