#include <pthread.h>		//Create POSIX threads.
#include <time.h>			//Wait for a random time.
#include <unistd.h>			//Thread calls sleep for specified number of seconds.
#include <semaphore.h>		//To create semaphores
#include <stdlib.h>			
#include <stdio.h>			//Input Output

pthread_t *Students;		//N threads running as Students.
pthread_t 	TA;				//Separate Thread for TA.

int ChairsCount = 0;
int CurrentIndex = 0;

/*TODO
 
//Declaration of Semaphores and Mutex Lock.
//Semaphores used:
//A semaphore to signal and wait TA's sleep.
//An array of 3 semaphores to signal, to wait chair, and to wait for the TA.
//A semaphore to signal and wait for TA's next student.
 
 //Mutex Lock used:
//To lock and unlock variable ChairsCount to increment and decrement its value.
 
 //hint: use sem_t and pthread_mutex_t 
 */

sem_t waits[3];
sem_t semStu;
sem_t semTa;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//Declared Functions
void *TA_Activity();
void *Student_Activity(void *threadID);

int main(int argc, char* argv[])
{
	int number_of_students;		//a variable taken from the user to create student threads.	Default is 5 student threads.
	int id;
	srand(time(NULL));

    /*TODO
	//Initializing Mutex Lock and Semaphores.
	
     //hint: use sem_init() and pthread_mutex_init()
     
     */
	 sem_init(&semStu, 0, 1);
	 sem_init(&semTa,0 ,0);
	 pthread_mutex_init(&mutex, NULL);

	if(argc<2)
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
	Students = (pthread_t) malloc(sizeof(pthread_t)*number_of_students);

	pthread_attr_t attr;
	pthread_attr_init(&attr);

    /*TODO
	//Creating one TA thread and N Student threads.
     //hint: use pthread_create
	 */
	pthread_create(&TA, &attr, TA_Activity, NULL);
	for(CurrentIndex = 0; CurrentIndex < number_of_students; CurrentIndex++) {
		pthread_create(&Students, &attr, Student_Activity, NULL);
	}

	/*
	//Waiting for TA thread and N Student threads.
     //hint: use pthread_join
     
     */
	for(CurrentIndex = 0; CurrentIndex < number_of_students; CurrentIndex++) {
		pthread_join(Students, NULL);
	}
	//Free allocated memory
	free(&Students); 
	return 0;
}

void *TA_Activity(void *param)
{
	while(true) {
    // TODO
	//TA is currently sleeping.
	printf("TA is asleep");
    // lock
    pthread_mutex_lock(&mutex);
    //if chairs are empty, break the loop.
	if(ChairsCount = 0) {
		ChairsCount++;
		sem_wait(&semStu);
	}
	//TA gets next student on chair.
     sem_post(&semStu);
	 ChairsCount--;
    //unlock
	pthread_mutex_unlock(&mutex);
	//TA is currently helping the student
	 printf("TA is currently helping the student");
	 sem_wait(&semTa);
     //hint: use sem_wait(); sem_post(); pthread_mutex_lock(); pthread_mutex_unlock()

	pthread_exit(0);

	}
}

void *Student_Activity(void *threadID) 
{
    //TODO
	int *tid;
	tid = (int *)threadID;
     
	//Student  needs help from the TA
	sem_post(&semTa);
	//Student tried to sit on a chair.
	if( ChairsCount < 4) {
		//wake up the TA.
		printf("Wake up the TA");
	
		// lock	
		pthread_mutex_lock(&mutex);
		ChairsCount++;
		
		// unlock
		pthread_mutex_unlock(&mutex);
		//Student leaves his/her chair.
		printf("Student leaves their chair\n");
		//Student  is getting help from the TA
		printf("Student is getting help from the TA\n");
		sem_post(&semTa);
		//Student waits to go next.
		sem_wait(&semStu);
		//Student left TA room
	} else {
		   //If student didn't find any chair to sit on.
    	//Student will return at another time
		printf("Student will return another time since all chairs are occupied\n");
		sem_wait(&semStu);
	}     
     //hint: use sem_wait(); sem_post(); pthread_mutex_lock(); pthread_mutex_unlock()
	pthread_exit(0);		
}
