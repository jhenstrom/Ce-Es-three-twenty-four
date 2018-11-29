#include <stdio.h>
#include "csapp.h"

/* Recommended max cache and object sizes */
#define MAX_CACHE_SIZE 1049000
#define MAX_OBJECT_SIZE 102400

/* You won't lose style points for including this long line in your code */
static const char *user_agent_hdr = "User-Agent: Mozilla/5.0 (X11; Linux x86_64; rv:10.0.3) Gecko/20120305 Firefox/10.0.3\r\n";
static const char *connection = "Connection: close\r\n";
static const char *prox_connection = "Proxy-Connection: close\r\n";

typedef struct {
  int *buf;
  int n;
  int front;
  int rear;
  sem_t mutex;
  sem_t slots;
  sem_t items;
} sbuf_t;

sbuf_t sbuf;

typedef struct {
  char** buf;
  int n;
  int front;
  int rear;
  sem_t mutex;
  sem_t slots;
  sem_t items;

} logbuf_t;

logbuf_t logbuf;

typedef struct {
  char* request;
  char* response;
} citem_t;

typedef struct {
  citem_t* buf;
  int front;
  int rear;
  sem_t read_mutex;
  sem_t write_mutex;
} cache_t;

void cache_init(cache_t *sp)
{
  sp->buf = Malloc(MAX_CACHE_SIZE);
  sp->front = sp->rear = 0;
  Sem_init(&sp->read_mutex, 0, 1);
  Sem_init(&sp->write_mutex, 0, 1);
}

void cache_deinit(cache_t *sp)
{
  Free(sp->buf);
}

void cache_insert(cache_t *sp, citem_t entry)
{
  P(&sp->write_mutex);
  sp->buf[(++sp->rear)] = entry;
  V(&sp->write_mutex);
}

void cache_remove(cache_t *sp, citem_t entry)
{
  //writing part
  P(&sp->write_mutex);
  //TODO: remove here
  V(&sp->write_mutex);
}

void cache_read(cache_t *sp, char* req)
{
  P(&sp->read_mutex);
  citem_t hit = cache_get(sp, req);
  if (hit != NULL)
  {
    cache_remove(sp, hit);
    cache_insert(sp, hit);
  }
  V(&sp->read_mutex);
}

citem_t cache_get(chache_t *sp, char* req)
{
  //TODO: reading part
  return NULL;
}

void logbuf_init(logbuf_t *sp, int n)
{
  sp->buf = Calloc(n, MAXBUF*sizeof(char*));
  sp->n = n;
  sp->front = sp->rear = 0;
  Sem_init(&sp->mutex, 0, 1);
  Sem_init(&sp->slots, 0, n);
  Sem_init(&sp->items, 0, 0);
}

void logbuf_deinit(logbuf_t *sp)
{
  Free(sp->buf);
}

void logbuf_insert(logbuf_t *sp, char* item)
{
  P(&sp->slots);
  P(&sp->mutex);
  sp->buf[(++sp->rear)%(sp->n)] = item;
  V(&sp->mutex);
  V(&sp->items);
}

char* logbuf_remove(logbuf_t *sp)
{
  char* item;
  P(&sp->items);
  P(&sp->mutex);
  item = sp->buf[(++sp->front)%(sp->n)];
  V(&sp->mutex);
  V(&sp->slots);
  return item;
}

void sbuf_init(sbuf_t *sp, int n)
{
  sp->buf = Calloc(n, sizeof(int));
  sp->n = n;
  sp->front = sp->rear = 0;
  Sem_init(&sp->mutex, 0, 1);
  Sem_init(&sp->slots, 0, n);
  Sem_init(&sp->items, 0, 0);
}

void sbuf_deinit(sbuf_t *sp)
{
  Free(sp->buf);
}

void sbuf_insert(sbuf_t *sp, int item)
{
  P(&sp->slots);
  P(&sp->mutex);
  sp->buf[(++sp->rear)%(sp->n)] = item;
  V(&sp->mutex);
  V(&sp->items);
}

int sbuf_remove(sbuf_t *sp)
{
  int item;
  P(&sp->items);
  P(&sp->mutex);
  item = sp->buf[(++sp->front)%(sp->n)];
  V(&sp->mutex);
  V(&sp->slots);
  return item;
}

int chop(char *str, int n)
{
    int len = strlen(str);
    if (n > len)
        n = len;
    memmove(str, str+n, len - n + 1);
    return(len - n);
}

int is_at_user(char* wire)
{
  char* temp = Malloc(MAXBUF*sizeof(char));
  memcpy(temp, wire, 11);
  if(strstr(temp, "User-Agent:") != NULL)
  {
    Free(temp);
    //printf("found user\n");
    return 0;
  }
  Free(temp);
  return 1;
}

int is_at_host(char* wire)
{
  char* temp = Malloc(MAXBUF*sizeof(char));
  memcpy(temp, wire,5);
  if(strstr(temp, "Host:") != NULL)
  {
    Free(temp);
    //printf("found Host\n");
    return 0;
  }
  Free(temp);
  return 1;
}

int is_at_prox_connection(char* wire)
{
  char* temp = Malloc(MAXBUF*sizeof(char));
  memcpy(temp, wire, 17);
  if(strstr(temp, "Proxy-Connection:") != NULL)
  {
    Free(temp);
    //printf("found prox\n");
    return 0;
  }
  Free(temp);
  return 1;
}

int is_at_connection(char* wire)
{
  char* temp = Malloc(MAXBUF*sizeof(char));
  memcpy(temp, wire, 11);
  if(strstr(temp, "Connection:") != NULL)
  {
    Free(temp);
    return 0;
  }
  Free(temp);
  return 1;
}

int* proccess_top(char* token, char* request, char* host_port, int* offset)
{
  int offset_local = 0;
  char* pntr;
  char* path = Malloc(MAXBUF*sizeof(char));

  char* get_post = strtok_r(token, " ", &pntr);

  char* body = strtok_r(NULL, " ", &pntr);
  chop(body, 7);
  for(int i = 0; i < strlen(body); i++)
  {
    if (body[i] == '/')
    {
      memcpy(host_port, body, i);
      memcpy(path, &body[i], strlen(body) - i);
    }
  }

  memcpy(request, get_post, strlen(get_post));
  //printf("%s\n", request);
  offset_local += strlen(get_post);
  memcpy(request+offset_local, " ", 1);
  //printf("%s\n", request);
  offset_local++;
  memcpy(request+offset_local, path, strlen(path));
  //printf("%s\n", request);
  offset_local += strlen(path);
  memcpy(request+offset_local, " HTTP/1.0\r\n", 11);
  //printf("%s\n", request);
  offset_local += 11;

  offset = &offset_local;
  Free(path);
  return offset;
}

int parse_request(int skt, char* request, char* port, char* host, char* httprequest)
{
  int host_found = -1;
  int* offset = &host_found;
  int connection_found = -1;
  int prox_connection_found = -1;
  int user_found = -1;
  char* host_port = Malloc(MAXBUF*sizeof(char));
  char* skt_request = Malloc(MAXBUF*sizeof(char));
  int req_len;
  if((req_len = read(skt, skt_request, MAXBUF*sizeof(char))) < 0)
  {
    printf("read error\n");
  }

  char* token = Malloc(MAXBUF*sizeof(char*));
  char* savepntr;

  token = strtok_r(skt_request, "\n", &savepntr);
  memcpy(httprequest, token, strlen(token));
  //printf("%s\n", token);
  offset = proccess_top(token, request, host_port, offset);
  //printf("%s\n", request);
  while((token = strtok_r(NULL, "\n", &savepntr)) != NULL)
  {
    //printf("%s\n", token);
    int offset_local = *offset;
    if(is_at_connection(token) == 0)
    {
      memcpy(request+offset_local, connection, strlen(connection));
      offset_local += strlen(connection);
      offset = &offset_local;
      connection_found = 0;
    }
    else if (is_at_user(token) == 0)
    {
      memcpy(request+offset_local, user_agent_hdr, strlen(user_agent_hdr));
      offset_local += strlen(user_agent_hdr);
      offset = &offset_local;
      user_found = 0;
    }
    else if(is_at_prox_connection(token) == 0)
    {
      memcpy(request+offset_local, prox_connection, strlen(prox_connection));
      offset_local += strlen(prox_connection);
      offset = &offset_local;
      prox_connection_found = 0;
    }
    else if (is_at_host(token) == 0)
    {
      //printf("is at Host\n");
      memcpy(request+offset_local, token, strlen(token));
      offset_local += strlen(token);
      memcpy(request+offset_local, "\n", 1);
      offset_local+= 1;
      offset = &offset_local;
      host_found = 0;
    }
    else
    {
      memcpy(request+offset_local, token, strlen(token));
      offset_local += strlen(token);
      memcpy(request+offset_local, "\n", 1);
      offset_local+=1;
      offset = &offset_local;
    }
  }
  *offset = *offset - 2;
  if(host_found != 0)
  {
  //  printf("host not found\n");
    memcpy(request+*offset, "Host: ", 6);
    *offset += 6;
    memcpy(request+*offset, host_port, strlen(host_port));
    *offset += strlen(host_port);
    memcpy(request+*offset, "\r\n", 2);
    *offset += 2;
  }
  if(prox_connection_found != 0)
  {
    //printf("prox_conect not found\n");
    memcpy(request+*offset, prox_connection, strlen(prox_connection));
    *offset += strlen(prox_connection);
  }
  if(connection_found != 0)
  {
    //printf("connect not found\n");
    memcpy(request+*offset, connection, strlen(connection));
    *offset += strlen(connection);
  }
  if(user_found != 0)
  {
    //printf("user not found\n");
    memcpy(request+*offset, user_agent_hdr, strlen(user_agent_hdr));
    *offset += strlen(user_agent_hdr);
  }
  memcpy(request+*offset, "\r\n", 2);
  *offset += 2;
  // FILE* fd = Fopen("request.txt", "w");
  // fprintf(fd, "%s\n", request);
  // Fclose(fd);

  token = strtok_r(host_port, ":", &savepntr);
  memcpy(host, token, strlen(token));
  token = strtok_r(NULL, ":", &savepntr);
  memcpy(port, token, strlen(token));
  Free(host_port);
  Free(skt_request);
  return 1;
}

void *logging_thread(void *vargp)
{
  Pthread_detach(pthread_self());
  FILE *f;
  f = fopen("log.txt", "w");
  fclose(f);
  while(1)
  {
    char* message = logbuf_remove(&logbuf);
    f = fopen("log.txt", "a");
    fprintf(f, "%s\n", message);
    fclose(f);
  }
}

void *handle_connection(void *vargp)
{
  Pthread_detach(pthread_self());
  while(1)
  {
    int connfd = sbuf_remove(&sbuf);
    char* request = Malloc(MAXBUF*sizeof(char));
    char* response = Malloc(MAX_OBJECT_SIZE*sizeof(char));
    char* port = Malloc(MAXBUF*sizeof(char));
    char* host = Malloc(MAXBUF*sizeof(char));
    char* httprequest = Malloc(MAXBUF*sizeof(char));
    Free(vargp);
    //echo(connfd);
    //TODO: get URL
    //TODO: check cache for URL (enclose in semaphores for write)
    //TODO: if hit, move cache entry to front of list and return it's body
    //else do the normal stuff
    parse_request(connfd, request, port, host, httprequest);
    int skt_web = Open_clientfd(host, port);
    int sent;
    if((sent = send(skt_web, request, strlen(request), 0)) < 0)
    {
        printf("send to skt_web error\n");
    }
    int resp_len = 0;
    int offset_local = 0;
    while(1)
    {
      if((resp_len = recv(skt_web, response+offset_local, MAX_OBJECT_SIZE, 0)) <= 0)
      {
        break;
      }
      else
      {
        offset_local += resp_len;
      }
    }

    //TODO: cache response, if chache gets to big, dump oldest object

    if((sent = send(connfd, response, offset_local, 0)) < 0)
    {
      printf("send to connfd error\n");
    }
    char message[MAXBUF];
    snprintf(message, sizeof(message), "Request [%s] successful.", httprequest);
    logbuf_insert(&logbuf, message);
    Close(connfd);
    sleep(1);
    Free(httprequest);
    Free(request);
    Free(response);
    Free(port);
    Free(host);
  }
}

int main(int argc, char* argv[])
{
  int listenfd;
  int *connfdp;
  socklen_t clientlen;
  struct sockaddr_storage* clientaddr;
  pthread_t tid;
  pthread_t tid2;
  listenfd = Open_listenfd(argv[1]);
  //Bind
  //Listen
  //TODO: create thread pool
  //TODO: launch logging thread
  logbuf_init(&logbuf, MAXBUF);
  Pthread_create(&tid2, NULL, logging_thread, NULL);
  sbuf_init(&sbuf, MAXBUF);
  for(int i=0; i < 5; i++)
  {
    Pthread_create(&tid, NULL, handle_connection, NULL);
  }
  while(1)
  {
    clientlen = sizeof(struct sockaddr_storage);
    connfdp = Malloc(sizeof(int));
    *connfdp = Accept(listenfd, (SA *) &clientaddr, &clientlen);
    //TODO: launch trhead from pool instead of following line
    sbuf_insert(&sbuf, *connfdp);
    //Pthread_create(&tid, NULL, handle_connection, connfdp);
    //TODO: add long entry to logging queue
  }
  return 0;
}
