#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include "stats.h"


typedef struct {
  int fnum, cmade, ceaten;
  double mindelay, maxdelay, avgdelay, totdelay;
}stats;

stats* stats_arr;
int num_factories = 0;

void stats_init(int num_producers){
	num_factories = num_producers;
	printf("num_producers:%d\n",num_producers);
	stats_arr = malloc(sizeof(stats) * num_producers);
	for(int i = 0; i< num_producers; i++){
		stats_arr[i].fnum = i;
		stats_arr[i].cmade = -1;
		stats_arr[i].ceaten = -1;
		stats_arr[i].mindelay = INT_MAX;
		stats_arr[i].maxdelay = 0;
		stats_arr[i].avgdelay = 0;
		stats_arr[i].totdelay = 0;
	}
}

void stats_cleanup(void)
{
	if (stats_arr == NULL) return;
	free(stats_arr);
}

void stats_record_produced(int factory_number){
	stats_arr[factory_number].cmade ++;
}

void stats_record_consumed(int factory_number, double delay_in_ms){
	if(stats_arr[factory_number].totdelay == 0){
		stats_arr[factory_number].cmade = 0;
		stats_arr[factory_number].ceaten = 0;
	}

	stats_arr[factory_number].ceaten ++;
	if(delay_in_ms < stats_arr[factory_number].mindelay) {
		stats_arr[factory_number].mindelay = delay_in_ms;
	}
	if(delay_in_ms > stats_arr[factory_number].maxdelay) {
		stats_arr[factory_number].maxdelay = delay_in_ms;
	}
	stats_arr[factory_number].totdelay += delay_in_ms;
    stats_arr[factory_number].avgdelay = stats_arr[factory_number].totdelay/stats_arr[factory_number].ceaten;

}


void stats_display(void){
	printf("Statistics:\n");
	printf("Factory#    #Made    #Eaten    Min_Delay[ms]    Avg Delay[ms]    Max Delay[ms]\n");
	for(int i = 0; i < num_factories; i++){
		if(stats_arr[i].cmade!=stats_arr[i].ceaten){
			printf("ERROR: Mismatch between number made and eaten.\n");
		}
		else{
			printf("%5d%9d%12d%17f%17f%17f\n", 
				stats_arr[i].fnum,
				stats_arr[i].cmade,
				stats_arr[i].ceaten,
				stats_arr[i].mindelay,
				stats_arr[i].avgdelay,
				stats_arr[i].maxdelay);
		}
	}
}