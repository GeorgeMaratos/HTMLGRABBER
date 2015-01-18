//functions.c
#include<stdlib.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>

//interface
void
interface(char *url)
{
  //variables
  const char *portNumber;
  const struct addrinfo hints;
  struct addrinfo *results;
  //ops
  
