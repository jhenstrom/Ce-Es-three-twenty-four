#include <stdio.h>
#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";
static const char *connection = "Connection: close\r\n";
static const char *prox_connection = "Proxy-Connection: close\r\n";
static const char *http = "HTTP/1.0\r\n";
static const char *endline = "\r\n";

// typedef struct
// {
//   int socket;
// } ThreadArgs;

int advance(char* wire, int index)
{
	int i = index;
	while((int)*(wire+i) != 0x0A)
	{
		i++;
		//printf("index = %d\n", i);
	}
	return i;
}

int is_at_http_head(char* wire, int index)
{
  char* temp = Malloc(MAXBUF*sizeof(char));
  memcpy(temp, &wire[index], 8);
  printf("%d\n", index);
  //printf("%s\n", temp);
  if(strstr(temp, "HTTP/1.1") != NULL)
  {
    Free(temp);
    printf("found http\n");
    return 0;
  }
  Free(temp);
  return 1;
}

int is_at_user(char* wire, int index)
{
  char* temp = Malloc(MAXBUF*sizeof(char));
  memcpy(temp, &wire[index], 11);
  if(strstr(temp, "User-Agent:") != NULL)
  {
    Free(temp);
    printf("found user\n");
    return 0;
  }
  Free(temp);
  return 1;
}

int is_at_prox_connection(char* wire, int index)
{
  char* temp = Malloc(MAXBUF*sizeof(char));
  memcpy(temp, &wire[index], 17);
  if(strstr(temp, "Proxy-Connection:") != NULL)
  {
    Free(temp);
    printf("found prox\n");
    return 0;
  }
  Free(temp);
  return 1;
}

int is_at_connection(char* wire, int index)
{
  char* temp = Malloc(MAXBUF*sizeof(char));
  memcpy(temp, &wire[index], 11);
  if(strstr(temp, "Connection:") != NULL)
  {
    Free(temp);
    return 0;
  }
  Free(temp);
  return 1;
}

//reads information from skt and parses it into a request, returns true if
//request is valid and false otherwise
int parse_request(int skt, char* request)
{
  char* skt_request = Malloc(MAXBUF*sizeof(char));
  int req_len;
  if((req_len = read(skt, skt_request, MAXBUF*sizeof(char))) < 0)
  {
    //error
  }
  int index = 0;
  int offset = 0;
  int connection_type_done = -1;
  int prox_connection_type_done = -1;
  int user_replaced = -1;
  for(index = 0; index < strlen(skt_request); index++)
  {
    if(is_at_http_head(skt_request, index) == 0)
    {
      memcpy(request+offset, http, strlen(http));
      offset += strlen(http) - 1;
      index = advance(skt_request, index);
    }
    else if (is_at_user(skt_request, index) == 0)
    {
      memcpy(request+offset, user_agent_hdr, strlen(user_agent_hdr));
      offset += strlen(user_agent_hdr) - 1;
      user_replaced = 0;
      index = advance(skt_request, index);
    }
    else if (is_at_prox_connection(skt_request, index) == 0)
    {
      memcpy(request+offset, prox_connection, strlen(prox_connection));
      offset += strlen(prox_connection) - 1;
      prox_connection_type_done = 0;
      index = advance(skt_request, index);
    }
    else if (is_at_connection(skt_request, index) == 0)
    {
      memcpy(request+offset, connection, strlen(connection));
      offset += strlen(connection) - 1;
      connection_type_done = 0;
      index = advance(skt_request, index);
    }
    else
    {
      *(request+offset) = *(skt_request+index);
    }
    offset++;
  }
  offset--;
  if(user_replaced != 0)
  {
    memcpy(request+offset, user_agent_hdr, strlen(user_agent_hdr));
    offset += strlen(user_agent_hdr);
  }
  if(prox_connection_type_done != 0)
  {
    memcpy(request+offset, prox_connection, strlen(prox_connection));
    offset += strlen(prox_connection);
  }
  if(connection_type_done != 0)
  {
    memcpy(request+offset, connection, strlen(connection));
    offset += strlen(connection);
  }
  memcpy(request+offset, endline, strlen(endline));
  printf("%s\n", request);
  return 1;
}

void *handle_connection(void *vargp)
{
  char* request = Malloc(MAXBUF*sizeof(char));
  char* response = Malloc(MAXBUF*sizeof(char));

  int connfd = *((int *)vargp);
  Pthread_detach(pthread_self());
  Free(vargp);
  //echo(connfd);
  int port_host = parse_request(connfd, request);
  int skt_web = Open_clientfd("localhost", "15213");
  int sent;
  if((sent = send(skt_web, request, strlen(request), 0)) < 0)
  {
      printf("send to skt_web error\n");
  }
  char* buffer = Malloc(MAXBUF*sizeof(char));
  int resp_len = 0;
  while(1)
  {
    printf("here\n");
    if((resp_len = recv(skt_web, buffer, MAX_OBJECT_SIZE, MSG_PEEK)) < 0)
    {
      break;
    }
    else
    {
      printf("%s\n", buffer);
      strcat(response, buffer);
      break;
    }


  }

  printf("%s\n", response);

  if((sent = send(connfd, response, resp_len, 0)) < 0)
  {
    printf("send to connfd error\n");
  }
  Close(connfd);
  return NULL;
}

int main(int argc, char* argv[])
{
  int listenfd;
  int *connfdp;
  socklen_t clientlen;
  struct sockaddr_storage* clientaddr;
  pthread_t tid;
  listenfd = Open_listenfd(argv[1]);
  //Bind
  //Listen
  while(1)
  {
    clientlen = sizeof(struct sockaddr_storage);
    connfdp = Malloc(sizeof(int));
    *connfdp = Accept(listenfd, (SA *) &clientaddr, &clientlen);
    //struct ThreadArgs* args = (struct ThreadArgs)Malloc(sizeof(struct ThreadArgs));
    //args->socket = connfdp;
    Pthread_create(&tid, NULL, handle_connection, connfdp);
  }
  return 0;
}
