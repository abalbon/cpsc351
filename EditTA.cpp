#include <pthread.h>		//Create POSIX threads.
#include <time.h>			//Wait for a random time.
#include <unistd.h>			//Thread calls sleep for specified number of seconds.
#include <semaphore.h>		//To create semaphores
#include <stdlib.h>			
#include <stdio.h>			//Input Output

pthread_t* Students;		//N threads running as Students.
pthread_t TA;				//Separate Thread for TA.

int ChairsCount = 0;
int CurrentIndex = 0;

//Declaration of Semaphores and Mutex Lock.
//Semaphores used:
//A semaphore to signal and wait TA's sleep.
sem_t taSleep;
//An array of 3 semaphores to signal and wait chair to wait for the TA.
sem_t hallChair[3];
//A semaphore to signal and wait for TA's next student.
sem_t nextStu;

//Mutex Lock used:
//To lock and unlock variable ChairsCount to increment and decrement its value.
pthread_mutex_t openChair;

int number_of_students;

//Declared Functions
void* TA_Activity(void* i);
void* Student_Activity(void* threadID);

int main(int argc, char* argv[])
{
//	int number_of_students;		//a variable taken from the user to create student threads.	Default is 5 student threads.
	int id;
	srand(time(NULL));

	//Initializing Mutex Lock and Semaphores.
	sem_init(&nextStu, 0, 0);
	sem_init(&taSleep, 0, 0);
	pthread_mutex_init(&openChair, NULL);

	for (id = 0; id < 3; ++id) {
		sem_init(&hallChair[id], 0, 0);
	}

	if (argc < 2)
	{
		printf("Number of Students not specified. Using default (5) students.\n");
		number_of_students = 5;
	}
	else
	{
		number_of_students = atoi(argv[1]);
		printf("Number of Students specified. Creating %d threads.\n", number_of_students);
	}

	//Allocate memory for Students
	Students = (pthread_t*)malloc(sizeof(pthread_t) * number_of_students);


	//Creating one TA thread and N Student threads.
	pthread_create(&TA, NULL, TA_Activity, NULL);

	for (id = 0; id < number_of_students; id++) {
		pthread_create(&Students[id], NULL, Student_Activity, (void*) &Students[id]);
	}

	//Waiting for TA thread and N Student threads.
	pthread_join(TA, NULL);
	for (id = 0; id < number_of_students; id++) {
		pthread_join(Students[id], NULL);
	}

	//Free allocated memory
	free(Students);
	return 0;
}

void* TA_Activity(void*)
{
	int chairIndex = CurrentIndex % 3;
	//TA is currently sleeping.
	printf("TA is asleep\n");
	sem_wait(&taSleep);
	
	while (CurrentIndex < number_of_students) {

		//lock
		pthread_mutex_lock(&openChair);
		if (ChairsCount == 0 && CurrentIndex > number_of_students) {
			//if chairs are empty, break the loop
			printf("TA goes back to sleep\n");
			pthread_mutex_unlock(&openChair);
			break;
		}

		while (ChairsCount > 0) {			
			//TA gets next student on chair.
			printf("TA calls for next student\n");
			sem_post(&hallChair[chairIndex]);
			ChairsCount--;
			chairIndex = (CurrentIndex + 1) % 3;
			//unlock
			pthread_mutex_unlock(&openChair);

			//TA is currently helping the student
			printf("TA is currently helping student %d\n", CurrentIndex);
			sleep(5);
			sem_post(&nextStu);
			usleep(10);
		}
	}
	return NULL;
}

void* Student_Activity(void* threadID)
{	
	printf("Student %d Enters room\n", CurrentIndex);
	while (CurrentIndex < number_of_students) {
		//Student tried to sit on a chair.
		//Student  needs help from the TA
		int chairs = ChairsCount;
		if (chairs < 3) {
			int index = (CurrentIndex + ChairsCount) % 3; 
			//If student sits on first empty chair then wake the TA
			if (chairs == 0) {
				printf("Student %d wakes TA\n", CurrentIndex + 1);
				sem_post(&taSleep);
			} else {
				//lock
				pthread_mutex_lock(&openChair);
				ChairsCount++;
				//unlock
				pthread_mutex_unlock(&openChair);
			}
			//Student leaves his/her chair
			sem_wait(&hallChair[index]);
			//Student  is getting help from the TA
			sem_post(&nextStu);
			//Student waits to go next.
			sem_wait(&nextStu);
			//Student left TA room
			printf("Student %d left room\n", CurrentIndex + 1);
			CurrentIndex++;
		}
		else {
			//If student didn't find any chair to sit on.
			//Student will return at another time
			printf("Student %d will return another time since all chairs are occupied\n", CurrentIndex+1);
			sleep(7);
		}
	}
	return NULL;

}
