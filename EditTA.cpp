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
sem_t semTa;
//An array of 3 semaphores to signal and wait chair to wait for the TA.
sem_t semChair[3];
//A semaphore to signal and wait for TA's next student.
sem_t semStu;

//Mutex Lock used:
//To lock and unlock variable ChairsCount to increment and decrement its value.
pthread_mutex_t openChair;


//Declared Functions
void* TA_Activity();
void* Student_Activity(void* threadID);

int main(int argc, char* argv[])
{
	int number_of_students;		//a variable taken from the user to create student threads.	Default is 5 student threads.
	int id;
	srand(time(NULL));

	//Initializing Mutex Lock and Semaphores.
	sem_init(&semStu, 0, 0);
	sem_init(&semTa, 0, 0);
	pthread_mutex_init(&openChair, NULL);

	for (id = 0; id < 3; ++id) {
		sem_init(&semChair[3], 0, 0);
	}

	if (argc < 2)
	{
		printf("Number of Students not specified. Using default (5) students.\n");
		number_of_students = 5;
	}
	else
	{
		printf("Number of Students specified. Creating %d threads.\n", number_of_students);
		number_of_students = atoi(argv[1]);
	}

	//Allocate memory for Students
	Students = (pthread_t*)malloc(sizeof(pthread_t) * number_of_students);


	//Creating one TA thread and N Student threads.
	pthread_create(&TA, NULL, TA_Activity, NULL);

	for (id = 0; id < number_of_students; id++) {
		pthread_create(&Students[id], NULL, Student_Activity, (void*)(long)id);
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

void* TA_Activity()
{
	while (1) {
		//TA is currently sleeping.
		sem_wait(&semTa);
		printf("TA is asleep");

		while (1) {
			//lock
			pthread_mutex_lock(&openChair);
			if (ChairsCount == 0) {
				//if chairs are empty, break the loop
				pthread_mutex_unlock(&openChair);
				break;
			}
			//TA gets next student on chair.
			sem_post(&semStu[CurrentIndex]);
			ChairsCount--;


			//TA gets next student on chair.
			sem_post(&semStu);
			ChairsCount--;
			printf("Student left chair. Number of chairs: %d\n", 3 - ChairsCount);
			CurrentIndex = (CurrentIndex + 1) % 3;
			//unlock
			pthread_mutex_unlock(&openChair);

			//TA is currently helping the student
			printf("TA is currently helping the student");
			sleep(5);
			sem_post(&semStu);
			usleep(1000);
		}
	}
}

void* Student_Activity(void* threadID)
{
	//TODO
	int StuTime;

	while (1) {
		//Student  needs help from the TA
		pthread_mutex_lock(&openChair);
		int chairs = ChairsCount;
		pthread_mutex_unlock(&openChair);

		//Student tried to sit on a chair.
		if (chairs < 3) {
			//If student sits on first empty chiar then wake the TA
			if (chairs == 0)
				sem_post(&semTa);

			//lock
			pthread_mutex_lock(&openChair)
				int index = (CurrentIndex + ChairsCount) % 3;
			ChairsCount++;
			//unlock
			pthread_mutex_unlock(&openChair);

			//Student leaves his/her chair
			sem_wait(&semChair[index]);
			//Student  is getting help from the TA
			printf("Student is getting help from the TA\n", (long)threadID);
			//Student waits to go next.
			sem_wait(&semStu);
			//Student left TA room
			printf("Student left room\n");
		}
		else {
			//If student didn't find any chair to sit on.
			//Student will return at another time
			printf("Student will return another time since all chairs are occupied\n", (long)threadID);
		}
	}

}
