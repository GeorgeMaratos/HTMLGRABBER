//functions.c
#include<stdlib.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#define DEBUG 0
#define BUFFERSIZE 1000000 

typedef struct url
{
  int size;
  char *first, *second, *fileName;
}Url;

char *
getFilename(char *string)
{
  //variables
  int i, j, stringSize, hasFilename, slashNumber, sentinel;
  char *name;
  //ops
  j = 0;
  sentinel = 0;
  slashNumber = 0;
  hasFilename = 0;
  stringSize = strlen(string);
  name = malloc(stringSize);
  for(i=0;i<stringSize;i++)
  {
    if(string[i] == '.') hasFilename++;
    else if(string[i] == '/') slashNumber++;
  }
  if(DEBUG) printf("filename: %d  slashNumber: %d\n", hasFilename, slashNumber);
  i = 0;
  if(hasFilename)
  {
    while(i < stringSize)
    {
      if(string[i] == '/') sentinel++;
      if(sentinel >= slashNumber && string[i] != '/') name[j++] = string[i];
      i++;
    }
    if(DEBUG) printf("File Name: %s\n", name);
  }
  //return
  return name;
}

Url *
format(char *string)
{
  //variables
  int i, size, index;
  Url *ret;
  char *f, *def, *fname;
  //ops
  if(DEBUG) printf("Begin format\n");
  f = malloc(sizeof(char) * 11);
  f = "/index.html";
  def = malloc(sizeof(char) * 10);
  def = "index.html";
  ret = malloc(sizeof(Url));
  size = strlen(string);
  if(size < 7)
  {
    printf("Error: url is not valid\n");
    return NULL;
  }
  ret->first = malloc(sizeof(char) * size);
  ret->second = malloc(sizeof(char) * size); 
  for(i=7;i<size;i++)
  {
    if(string[i] != '/')
	ret->first[i - 7] = string[i];
    else break;
  }
  index = i;
  while( i < size )
    ret->second[i - index] = string[i++];
  if(DEBUG) printf("parsed url: %s||%s\n", ret->first, ret->second);
  if(DEBUG) printf("Size of second: %d\n", strlen(ret->second));
  if(strlen(ret->second) < 1)
  {
    ret->second = f;
    ret->fileName = def;
  }
  else 
  {
    fname = getFilename(ret->second);
    if(strlen(fname) > 1) ret->fileName = fname;
    else ret->fileName = def;
  }
  //return
  return ret;
}
      
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

//printIp IGNORE, used for debugging
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
    inet_ntop(ptr->ai_family, addr, ipstr, sizeof(ipstr));
    printf("%s: %s\n", ipver, ipstr);
  }
}

//write to file
void
toFile(Url *info, int size, char *string)
{
  //variables
  FILE *f;
  char *mode;
  int i;
  //ops
  mode = "w";
  if(DEBUG) printf("Filename from toFile: %s\n", info->fileName);
  f = fopen(info->fileName, mode);
  for(i=0;i<size;i++)
    fprintf(f, "%c", string[i]);
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
  Url *parsedUrl;
  //ops
  request = malloc(sizeof(char) * 200);
  parsedUrl = format(url);
  sprintf(request,"GET %s HTTP/1.0\r\nHost: %s\r\n\r\n", parsedUrl->second, parsedUrl->first);
  if(DEBUG) printf("request: %s\n", request);
  portNumber = "80";
  stlen = strlen(request);
  buffer = malloc(BUFFERSIZE);
  hints = prep();
  status = getaddrinfo(parsedUrl->first, portNumber, hints, &results);
  if(DEBUG) printf("getaddrinfo\n");
  if(status != 0)
  {
    printf("Error: getaddrinfo <%s>\n", gai_strerror(status));
    return;
  }
  else
  {
    if(DEBUG) printf("socket\n");
    sock = socket(results->ai_family, results->ai_socktype, results->ai_protocol);
    if(sock < 0)
    {
      printf("Error: socket connection failue\n");
      return;
    }
    else
    {
      if(DEBUG) printf("connect\n");
      conn = connect(sock, results->ai_addr, results->ai_addrlen);
      if(conn < 0)
      {
	printf("Error: connection failure\n");
	return;
      }
      else
      {
	if(DEBUG) printf("sending request\n");
	bytes = send(sock, request, stlen, 0);
	if(bytes < 0)
	{
	  printf("Error: fail send()\n");
	  return;
	}
	else
	{
	  if(DEBUG) printf("waiting for a reply\n");
	  recv(sock, buffer, BUFFERSIZE, 0);
	  if(DEBUG) printf("writing to file\n");
	  if(DEBUG) printf("second: %s\n", parsedUrl->second);
	  toFile(parsedUrl, BUFFERSIZE, buffer);
	}
      }
    }
  }
  //free memory
  freeaddrinfo(results);
  free(hints);
  free(buffer);
  free(parsedUrl);
  free(request);
}

