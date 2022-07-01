#include <sys/ipc.h>  // for the inter process communication 
#include <sys/shm.h>  // for allowing creating of shared memory 
#include <pthread.h>  // for allowing threading 
#include <stdio.h>    
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "shared_mem.h"


// This gets the message from the shared memory
void *get(void* sargp){

  struct shm_data* shmmData = (struct shm_data*) sargp;
  do{

    pthread_mutex_lock(&(shmmData->s2c.mutex));
    printf("This is get\n");
    while(shmmData->s2c.front == shmmData->s2c.tail){
      // full.wait(&lock)
      printf("get the waiting function\n");
      pthread_cond_wait(&(shmmData->s2c.empty), &(shmmData->s2c.mutex));
    }
    // item = buff[front % MAX]
    printf("Message recieved Output >>> ");  
    puts(shmmData->s2c.messages[shmmData->s2c.front]);  
    shmmData->s2c.front++;

    pthread_cond_signal(&(shmmData->s2c.full));
    printf("This is the full signal for s2c\n");

    pthread_mutex_unlock(&(shmmData->s2c.mutex));
    printf("Here get is unlocking s2c\n");

  }while(1);

}

// sending 

void* put(void* sargp){

      struct shm_data* shmmData = (struct shm_data*) sargp;
    do{
      // putting an item into the buffer 
      // lock.acquire()
      pthread_mutex_lock(&(shmmData->c2s.mutex));
      printf("In put method\n");
      while(shmmData->c2s.tail != shmmData->c2s.front){ //////////////////////////////
        
        // full.wait(&lock)
        printf("Put function is waiting\n");
        pthread_cond_wait(&(shmmData->c2s.full), &(shmmData->c2s.mutex));

      }

      // puts items into the c2s messages
      // strcpy (destination, )
      printf("client input >>> ");
      fgets(shmmData->c2s.messages[shmmData->c2s.tail], MAX_MSG_LEN, stdin);
      puts(shmmData->c2s.messages[shmmData->c2s.tail]);

      // tail ++
      shmmData->c2s.tail++;

      // empty.signal()
      pthread_cond_signal(&(shmmData->c2s.empty));
      printf("the empty signal\n");
      pthread_mutex_unlock(&(shmmData->c2s.mutex));
      printf("this is the unlock signal for client to server \n");

    }while(1);

}

int main (int argc, char* argv[]){

  int entryKey;// key for shared mem
  int shMemId; // shared memory Identifier

  //creating a pointer to type shm_data   
  struct shm_data *shmmData;

  entryKey = atoi(argv[1]); // typecasted

  shMemId = shmget(entryKey,sizeof((struct shm_data*)shmmData),0666|IPC_CREAT);

  //check if segment created 
  if (shMemId == -1){
     printf("creation of mem segment did not work\n");
     return 1;
  }

  ///// get a pointer to the mem segment aftter you attach to it ///////
  shmmData = (struct shm_data*)shmat(shMemId,(void*)0, 0);   

  if(shmmData == (void*) -1 )
    printf("Shared memory attachment did not work");

   // create 2 threads 
  pthread_t reader;  // this thread is for reading 
  pthread_t sender;  // this thread is for sending 
  printf("thread1 and 2 created\n");

  //struct message_buffer* tempBuffer;

  //.messages = &(shmmData->s2c);

  // THIS RECIEVES A MESSAGE FROM SERVER -> CLIENT
 int threader1 = pthread_create(&reader, NULL, get, &shmmData->s2c.messages);

  // THIS SENDS A MESSAGE FROM CLIENT -> SERVER
  int threader2 = pthread_create(&sender, NULL, put, &shmmData->s.messages);

 int threader3 = pthread_join(sender, NULL);
 int threader4 = pthread_join(reader, NULL);

}






