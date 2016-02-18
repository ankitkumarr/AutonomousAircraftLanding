#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <semaphore.h>
#include <string.h>

void *createplane(void *args);
void startlanding (int pno, int rno);
typedef struct {
	int pno;
	int arrived;
	pthread_t tid;
	int fcap;
	int emeflag;
	int p;
	int landflag;
}plane;


typedef struct {
	int number;
	int occupied;
}runwayst;

pthread_mutex_t runway;
pthread_mutex_t runway2;
pthread_mutex_t runway3;
pthread_mutex_t planes;
pthread_mutex_t aplanes;
plane allplanes[25];
runwayst runways[3];


int em_count;

int main (int argc, char* argv) {

	int tstatus;
	srand(time(NULL));
	int i;
	for( i = 0; i < 3 ; i++) {
		runways[i].number = i;
		runways[i].occupied = 0;
	}



	for (i = 0; i < 25; i++) {

		allplanes[i].pno = i + 1;
		allplanes[i].fcap = (rand() % (150 - 10)) + 10;
		allplanes[i].landflag = 0;
		allplanes[i].arrived = 0;
		allplanes[i].emeflag = 0;
		
		allplanes[i].p = 0;

	}

	if ( (pthread_mutex_init (&runway, NULL)) != 0) {
		printf("\n Error creating semaphore\n");
		exit(1);
	}
	if ((pthread_mutex_init (&runway2, NULL)) != 0) {
		printf("\n Error creating semaphore\n");
		exit(1);
	}
	if ((pthread_mutex_init (&runway3, NULL)) != 0) {
		printf("\n Error creating semaphore\n");
		exit(1);
	}
	if ((pthread_mutex_init (&planes, NULL)) != 0) {
		printf("\n Error creating semaphore\n");
		exit(1);
	}
	if ((pthread_mutex_init (&aplanes, NULL)) != 0) {
		printf("\n Error creating semaphore\n");
		exit(1);
	}


	for (i = 0 ; i < 25; i ++) {
		tstatus = pthread_create (&allplanes[i].tid, NULL, createplane, (void *) &allplanes[i]);
		if(tstatus != 0) {
			printf("Error while creating a thread\n");
			exit(1);
		}

		sleep((rand()%5));
	}

	for ( i = 0; i < 25; i++) {
		pthread_join(allplanes[i].tid, NULL);
	}

	pthread_mutex_destroy(&runway);
	pthread_mutex_destroy(&runway2);
	pthread_mutex_destroy(&runway3);
	pthread_mutex_destroy(&planes);
	pthread_mutex_destroy(&aplanes);
	return 0;
}


void startlanding (int pno, int rno) {


	allplanes[pno].landflag = 1;
	if(allplanes[pno].emeflag == 1) {
		printf("Plane %d with emergency status is starting to land in runway %d  with fuel %d left \n", pno, rno, allplanes[pno].fcap);
		int ltime = (rand() % 5)+1;

		if(ltime> allplanes[pno].fcap) {
			printf("Plane %d with emergency status cannot land, hence aborting, run for safety\n", pno);
			exit(0);	
		}
		sleep(ltime);
		printf("Plane %d with emergency status has landed on the runway %d with fuel %d remaining\n", pno, rno, allplanes[pno].fcap - ltime);

		int cltime = (rand() % 5) + 1;
		sleep(cltime); 
		printf("Plane %d with emergency status has cleared the runway %d\n", pno, rno);
		pthread_mutex_lock(&planes);
		em_count--;
		pthread_mutex_unlock(&planes);
	}



	else {
		printf("Plane %d is about to land at runway %d to land with fuel %d\n", pno, rno, allplanes[pno].fcap);
		int ltime = rand()%8 + 1;
		if(ltime > allplanes[pno].fcap) {
			printf("Plane %d cannot land, crashing!!!!! run!!", pno);
			exit(0);
		}

		sleep(ltime);
		printf("Plane %d has landed on the runway %d with fuel %d remaining\n", pno, rno, allplanes[pno].fcap - ltime);

		int cltime = rand() % 10 + 3;
		sleep(cltime);
		printf("Plane %d cleared the runway %d\n", pno, rno);
	}



	if(rno == 0) {
		pthread_mutex_lock(&runway);
		runways[rno].occupied = 0;
		pthread_mutex_unlock(&runway);
	}

	if(rno == 1) {
		pthread_mutex_lock(&runway2);
		runways[rno].occupied = 0;
		pthread_mutex_unlock(&runway2);
	}

	if(rno == 2) {
		pthread_mutex_lock(&runway3);
		runways[rno].occupied = 0;
		pthread_mutex_unlock(&runway3);
	}
}






void *createplane(void *args) {

	long planetid;
	int flag = 0;
	int pno = ((plane *)args)->pno ;
	printf("Plane %d has arrived in the airzone with fuel %d\n", pno, allplanes[pno].fcap);
			
	

	allplanes[pno].arrived = 1;


	int b = 0;
	int a = 0;
	plane tempplane;
	tempplane.emeflag = 0;
	tempplane.fcap = 100000;
	tempplane.landflag = 0;
	tempplane.arrived = 1;
	for( a = 0; a < 25; a++ ) {
		if( ((tempplane.fcap > allplanes[a].fcap) && (allplanes[a].landflag==0)) && (allplanes[a].arrived==1) ) {
			tempplane = allplanes[a];
			b = a;
		}

	}
	allplanes[b].p = 1;


	int i;
	int cflag = 0;
	while( allplanes[pno].fcap > 1) {

		for(i=0; i < allplanes[pno].fcap; i++) {
			//priority will be given to planes with emergency
			if((em_count > 0  && allplanes[pno].emeflag == 0) || (allplanes[pno].p!=1)) {
				allplanes[pno].fcap--;
				sleep(1);
			}
			else {		//can land now

				if(runways[0].occupied == 0) 
				{
					pthread_mutex_lock(&runway);
					runways[0].occupied = 1;
					pthread_mutex_unlock(&runway);
					startlanding(pno, 0);
					flag = 1;
					return;

				}

				else if(runways[1].occupied == 0) {

					pthread_mutex_lock(&runway2);
					runways[1].occupied = 1;
					pthread_mutex_unlock(&runway2);
					startlanding(pno, 1);
					flag = 1;
					return;
				}
				else if(runways[2].occupied == 0) {

					pthread_mutex_lock(&runway3);
					runways[2].occupied = 1;
					pthread_mutex_unlock(&runway3);
					startlanding(pno, 2);
					flag = 1;
					return;

				}
				else {
					sleep(1);
					allplanes[pno].fcap--;
				}
			}
if((cflag==0) && (allplanes[pno].fcap < 10)) {
				printf("Plane %d has reached its danger zone with less fuel\n", pno);
				cflag = 1;
			}


		if(rand()%22 == 3) {

		printf("Plane %d is now in an emergency state \n", pno);
		pthread_mutex_lock(&planes);
		em_count++;
		pthread_mutex_unlock(&planes);
		allplanes[pno].p = 1;
		}
		}
	}

	if(flag == 0) {
		printf("Plane %d crashed causing the airport to blast, please run for safety\n", pno);
		exit(0);
	}

}


