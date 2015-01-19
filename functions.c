//functions.c
#include<stdlib.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#define DEBUG 1

//prep
struct addrinfo *
prep()
{
  //variables
  struct addrinfo *h;
  //ops
  h = malloc(sizeof(struct addrinfo));
  memset(h,0,sizeof(struct addrinfo));
  h->ai_family = AF_UNSPEC;
  h->ai_socktype = SOCK_STREAM;
  h->ai_flags = AI_PASSIVE;
  //return
  return h;
}

//printIp
void
printIp(struct addrinfo *r)
{
  //variables
  struct addrinfo *ptr;
  struct sockaddr_in *ipv4;
  struct sockaddr_in6 *ipv6;
  void *addr;
  char *ipver;
  char ipstr[INET6_ADDRSTRLEN];
  //ops
  for(ptr=r;ptr!=NULL;ptr=ptr->ai_next)
  {
    if(ptr->ai_family == AF_INET)
    {
      ipv4 = (struct sockaddr_in *)ptr->ai_addr;
      addr = &(ipv4->sin_addr);
      ipver = "IPV4";
    }
    else
    {
      ipv6 = (struct sockaddr_in6 *)ptr->ai_addr;
      addr = &(ipv6->sin6_addr);
      ipver = "IPV6";
    }
    inet_ntop(ptr->ai_family,addr,ipstr,sizeof(ipstr));
    printf("%s: %s\n", ipver, ipstr);
  }
}

//interface
void
interface(char *url)
{
  //variables
  int status, sock, conn;
  const char *portNumber;
  struct addrinfo *hints;
  struct addrinfo *results;
  //ops
  portNumber = "80";
  hints = prep();
  status = getaddrinfo(url,portNumber,hints,&results);
  if(DEBUG) printf("getaddrinfo\n");
  if(DEBUG && status != 0)
  {
    printf("Error: getaddrinfo <%s>\n", gai_strerror(status));
    return;
  }
  else
  {
    if(DEBUG) printf("socket\n");
    sock = socket(results->ai_family,results->ai_socktype,results->ai_protocol);
    if(DEBUG && sock < 0)
    {
      printf("Error: socket connection failue\n");
      return;
    }
    else
    {
      if(DEBUG) printf("connect\n");
      conn = connect(sock,results->ai_addr,results->ai_addrlen);
      if(DEBUG && conn < 0)
      {
	printf("Error: connection failure\n");
	return;
      }
      else if(DEBUG) printf("all sucessful\n");
    }
  }
  //free memory
  freeaddrinfo(results);
  free(hints);
}

