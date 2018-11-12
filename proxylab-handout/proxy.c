#include <stdio.h>
#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";

typedef struct
{
  int socket;
} ThreadArgs;

void

void *handle_connection(void *vargp)
{
  char*
  int connfd = *((int *)vargp);
  Pthread_detach(pthread_self());
  Free(vargp);
  echo(connfd);
  /*
  This is where I will handle the connection.
  Create HTML Request and send it, then pass
  the response to the socket connfd before
  closing it.
  */
  Close(connfd);
  return NULL;
}

int main()
{
  int listenfd, connfdp;
  int port = 8080;
  socket_t clientlen;
  struct sockaddr_storage* clientaddr;
  pthread_t tid;
  listenfd = Open_listenfd(port);
  //Bind
  //Listen
  while(1)
  {
    clientlen = sizeof(struct sockaddr_storage);
    connfdp = Malloc(sizeof(int));
    *connfdp = Accept(listenfd, (SA *) &clientaddr, &clientlen);
    struct ThreadArgs* args = (struct ThreadArgs)Malloc(sizeof(struct ThreadArgs));
    args->socket = connfdp;
    Pthread_create(&tid, NULL, handle_connection, connfd);
  }
  printf("%s", user_agent_hdr);
  return 0;
}
