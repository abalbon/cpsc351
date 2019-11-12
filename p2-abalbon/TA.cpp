#include <pthread.h>		//Create POSIX threads.
#include <time.h>			//Wait for a random time.
#include <unistd.h>			//Thread calls sleep for specified number of seconds.
#include <semaphore.h>		//To create semaphores
#include <stdlib.h>
#include <stdio.h>			//Input Output

pthread_t *Students;		//N threads running as Students.
pthread_t TA;				//Separate Thread for TA.

int ChairsCount = 0;
int CurrentIndex = 0;
int lastStudent;

const int NUM_CHAIRS = 3;

//TODO

//Declaration of Semaphores and Mutex Lock.
//Semaphores used:
//A semaphore to signal and wait TA's sleep.
sem_t taSleep;
sem_t taOutChairs;
sem_t taInChair;
//An array of 3 semaphores to signal and wait chair to wait for the TA.
//A semaphore to signal and wait for TA's next student.
//Mutex Lock used:
//To lock and unlock variable ChairsCount to increment and decrement its value.
pthread_mutex_t chairMutex;
//hint: use sem_t and pthread_mutex_t

//Declared Functions
void *TA_Activity(void *i);
void *Student_Activity(void *threadID);

int main(int argc, char* argv[])
{
	int number_of_students;		//a variable taken from the user to create student threads.	Default is 5 student threads.
	int id;
	srand(time(NULL));

    //TODO
	//Initializing Mutex Lock and Semaphores.
     //hint: use sem_init() and pthread_mutex_init()
	pthread_mutex_init(&chairMutex, NULL);

	sem_init(&taSleep, 0, 0);

	sem_init(&taOutChairs, 0, 3);

	sem_init(&taInChair, 0, 1);

	if(argc<2)
	{
		printf("Number of Students not specified. Using default (5) students.\n");
		number_of_students = 5;
	}
	else
	{
		number_of_students = atoi(argv[1]);
		printf("Number of Students specified. Creating %d threads.\n", number_of_students);

	}

	lastStudent = number_of_students - 1;

	//Allocate memory for Students
	Students = (pthread_t*) malloc(sizeof(pthread_t)*number_of_students);

    //TODO
	//Creating one TA thread and N Student threads.
     //hint: use pthread_create
	pthread_create(&TA, NULL, &TA_Activity, NULL);

	for(int j = 0; j < number_of_students; j++) {
		pthread_create(&Students[j], NULL, Student_Activity, (void*) (long)j);
	}

	//Waiting for TA thread and N Student threads.
     //hint: use pthread_join

	pthread_join(TA, NULL);
	for(int i = 0; i < number_of_students; ++i) {
		 pthread_join(Students[i], NULL);
	 }

	//Free allocated memory
	free(Students);
	return 0;
}

void* TA_Activity(void *i)
{
    // TODO
	//TA is currently sleeping.
	printf("TA is currently asleep\n");
	sleep(2);
	sem_wait(&taSleep);
	printf("Student %d woke up TA\n", CurrentIndex);

	while(true) {
	    // lock
		pthread_mutex_lock(&chairMutex);
	    //if chairs are empty, break the loop.
		if(ChairsCount == 0) {
			pthread_mutex_unlock(&chairMutex);
		}
		
		//TA gets next student on chair.
		ChairsCount--;
		//unlock
		pthread_mutex_unlock(&chairMutex);
				
		sem_post(&taOutChairs);
		//TA is currently helping the student
		sem_post(&taInChair);

		 //hint: use sem_wait(); sem_post(); pthread_mutex_lock(); pthread_mutex_unlock()
	}
}

void* Student_Activity(void *threadID)
{
	   //TODO
	//Student  needs help from the TA
	printf("Student %d enters room\n", (long)threadID);
	
	while(true) {
		//Student tried to sit on a chair.
		if (ChairsCount < 3) {
			if(ChairsCount == 0){
				//wake up the TA.
				sem_post(&taSleep);
	   		}
			//Student leaves his/her chair.
			sem_wait(&taOutChairs);
			printf("Student %d waits in outside chair\n",(long) threadID);

			// lock
			pthread_mutex_lock(&chairMutex);
			ChairsCount++;
			// unlock
			pthread_mutex_unlock(&chairMutex);

			sleep(2);
			printf("Student %d left chair\n", (long) threadID);
			//Student  is getting help from the TA
			printf("TA is helping student\n");

			//Student waits to go next.
			sem_wait(&taInChair);
			//Student left TA room
			printf("Student %d has left\n", (long) threadID);
			break;

	    } else {
	    //If student didn't find any chair to sit on.
	    //Student will return at another time
			printf("Student %d will return later\n", (long) threadID);
			sleep(10);
		}
	}
	if((long) threadID == lastStudent) {
		sleep(5);
		printf("TA goes back to sleep, no more students\n");
		sem_wait(&taSleep);
	}

     //hint: use sem_wait(); sem_post(); pthread_mutex_lock(); pthread_mutex_unlock()
}
