/*
All rights reserved by Zoe Liu
File Name       : auth.c
Used for        : CNT5410 Assignment #2
Author          : Zoe Liu
Dile Description: This function is used to do authentication and verify for the encrypt and decrypt process using HMAC with SHA-512 as hash function.
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<gcrypt.h>


void verify( char *outfname, char *key )
{
  gcry_error_t  gcrypt_err;
  gcry_mac_hd_t gcrypt_machd;
  char mac_buffer[128]; 
  char mac_cypherbuf[128];
  size_t mac_keylen = gcry_mac_get_algo_keylen(GCRY_MAC_HMAC_SHA512);
  size_t mac_blklen = gcry_mac_get_algo_maclen(GCRY_MAC_HMAC_SHA512);
  int ininum = 5844;
  char mac_inivec[16] ;
  char mac_cypher[1024];
  char mac_cipherbuf[1024];
  FILE *ofp ;
  ofp = fopen(outfname, "w") ;
// creates the context handle required for most of the other cipher functions and returns a handle
  gcrypt_err = gcry_mac_open ( &gcrypt_machd , GCRY_MAC_HMAC_SHA512 , 0 ) ;            
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
      return ;
    }
//Set the initialization vector with integer 5844 used for encryption 
//    gcrypt_inivec = intochar( ininum ) ;
   int i,j;
    for(i=1; ininum > 0; i++,ininum/=10) 

        gcrypt_inivec[i] = ininum%10+'0'; //

    for(j=i-1,i=1; j-i>=1; j--,i++) //

    {
        mac_inivec[i] = mac_inivec[i]^mac_inivec[j];
        mac_inivec[j] = mac_inivec[i]^mac_inivec[j];
        mac_inivec[i] = mac_inivec[i]^mac_inivec[j];
    }


    gcrypt_err = gcry_mac_setiv(gcrypt_machd, mac_inivec, mac_blklen);
    if (gcrypt_err)
    {
      printf("gcry_cipher_setiv failed:  %s/%s\n" , gcry_strsource(gcrypt_err) , gcry_strerror(gcrypt_err));
      return;
    }
//This function is used to encrypt data 
    gcrypt_err =  gcry_mac_verify (gcrypt_machd, gcrypt_macbuf , 64) ;
 
    gcry_cipher_close( gcrypt_machd );
    return gcrypt_err;    
}
