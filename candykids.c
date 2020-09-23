#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include "bbuff.h"
#include "stats.h"

_Bool stop_thread = false;

typedef struct{
    int fnum;
    double time_stamp_in_ms;
} candy_t;

double current_time_in_ms(void){
    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    return now.tv_sec * 1000.0 + now.tv_nsec/1000000.0;
}

void *factory(void *arg){
    int fid = *((int*)arg);
    int wait_time;
    while(!stop_thread){
        wait_time = rand()%4;
        // printf("\tFactory %d ships candy & waits %ds\n", fid, wait_time);
        candy_t *candy = malloc(sizeof(candy_t));
        candy->fnum = fid;
        candy->time_stamp_in_ms = current_time_in_ms();
        bbuff_blocking_insert(candy);
        stats_record_produced(fid);
        sleep(wait_time);
    }
    printf("Candy-factory %d done\n", fid);
    return NULL;
}

void *kid(void *arg){
    int wait_time;
    int fid;
    double delay;
    while(true){
        wait_time = rand()%2;
        candy_t *candy = (candy_t *) bbuff_blocking_extract();
        if(candy!=NULL){
            fid = candy->fnum;
            delay = current_time_in_ms() - candy->time_stamp_in_ms;
            stats_record_consumed(fid, delay);
        }
       free(candy);
        sleep(wait_time);
    }
    return NULL;
}


int main(int argc, char* argv[]){
    // 1.  Extract arguments
    if(argc!=4){
        printf("wrong number of arguments\n");
        exit(1);
    }
    int factories=atoi(argv[1]),kids=atoi(argv[2]),seconds=atoi(argv[3]);
    printf("facotries:%d kids:%d seconds:%d",factories,kids,seconds);

    if(factories<0||kids<0||seconds<0){
        printf("can't have negative values\n");
        exit(1);
    }
    // 2.  Initialize modules
    bbuff_init();
    stats_init(factories);

    // 3.  Launch candy-factory threads
    pthread_t *factories_arr = malloc(factories * (sizeof(pthread_t)));
    int *factories_id;
    for (int i=0;i<factories;i++){
        int *factories_id=malloc(sizeof(int));
        *factories_id = i;
        pthread_create(&factories_arr[i], NULL, factory, factories_id);
        free(factories_id);
    }
    // 4.  Launch kid threads
    pthread_t *kids_arr = malloc(kids * (sizeof(pthread_t)));
    for(int i=0;i<kids;i++){
        pthread_create(&kids_arr[i], NULL, kid, NULL);
    }
    // 5.  Wait for requested time
    for(int i=1;i<seconds;i++){
        sleep(1);
        printf("Time:%ds\n",i);
    }
    // 6.  Stop candy-factory threads
    stop_thread=true;
    for(int i=0;i<factories;i++){
        pthread_join(factories_arr[i],NULL);
    }
    // 7.  Wait until no more candy
    while(bbuff_is_empty()!=true){
        printf("waiting for all candy to be consumed\n");
        sleep(1);
    }
    // 8.  Stop kid threads
    for(int i=0;i<kids;i++){
        pthread_cancel(kids_arr[i]);
        pthread_join(kids_arr[i],NULL);
    }
    // 9.  Print statistics
    stats_display();
    // 10. Cleanup any allocated memory
    free(factories_arr);
    free(kids_arr);
    stats_cleanup();
    return 0;
}