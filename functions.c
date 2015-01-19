//functions.c
#include<stdlib.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#define DEBUG 1
#define BUFFERSIZE 1000000 

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

//write to file
void
toFile(int size, char *string)
{
  //variables
  FILE *f;
  char *mode;
  int i;
  //ops
  mode = "w";
  f = fopen("index.html",mode);
  for(i=0;i<size;i++)
    fprintf(f,"%c",string[i]);
  fclose(f);
}
//interface
void
interface(char *url)
{
  //variables
  int status, sock, conn, stlen, bytes;
  const char *portNumber;
  char *buffer, *request;
  struct addrinfo *hints;
  struct addrinfo *results;
  //ops
  request = "GET / HTTP/1.0\r\n\r\n";
  portNumber = "80";
  stlen = strlen(request);
  buffer = malloc(BUFFERSIZE);
  hints = prep();
  status = getaddrinfo(url,portNumber,hints,&results);
  if(DEBUG) printf("getaddrinfo\n");
  if(status != 0)
  {
    printf("Error: getaddrinfo <%s>\n", gai_strerror(status));
    return;
  }
  else
  {
    if(DEBUG) printf("socket\n");
    sock = socket(results->ai_family,results->ai_socktype,results->ai_protocol);
    if(sock < 0)
    {
      printf("Error: socket connection failue\n");
      return;
    }
    else
    {
      if(DEBUG) printf("connect\n");
      conn = connect(sock,results->ai_addr,results->ai_addrlen);
      if(conn < 0)
      {
	printf("Error: connection failure\n");
	return;
      }
      else
      {
	if(DEBUG) printf("sending request\n");
	bytes = send(sock,request,stlen,0);
	if(bytes < 0)
	{
	  printf("Error: fail send()\n");
	  return;
	}
	else
	{
	  if(DEBUG) printf("waiting for a reply\n");
	  recv(sock,buffer,BUFFERSIZE,0);
	  if(DEBUG) printf("writing to file\n");
	  toFile(BUFFERSIZE,buffer);
	}
      }
    }
  }
  //free memory
  freeaddrinfo(results);
  free(hints);
  free(buffer);
}

