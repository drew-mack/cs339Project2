#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void create_recieve_thread(char* port){
  int i;
  pthread_t tid;
  void* ret;

  pthread_create(&tid, NULL, recieve_packet, (void *)port);
  
  pthread_exit(NULL);
}
