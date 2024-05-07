#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>

#define NUM_GUESTS 5
#define NUM_ROOMS 3

sem_t availableRooms; //Sem for rooms available
sem_t checkInSema; //Sem for Mutual Exclusion of reservationist 
sem_t checkOutSema; //Check-Out Process

//checkOutSema

int totalGuests = 0; 
int poolCount = 0;
int restaurantCount = 0;
int fitnessCenterCount = 0;
int businessCenterCount = 0;

int rooms[NUM_ROOMS] = {0}; // Initialize rooms as available (0) / defined

//Function of the Guests behavior and Interation with the Hotel
//Interaction with the Check-In Function
//Sleep and Activity Function
//Check Out 
void* guest(void* arg) {
    int guestIndex = *((int*)arg);
 

    //Enter hotel, 
    printf("Guest %d enters the hotel.\n", guestIndex);
    totalGuests++; // Increment total guests


    sem_wait(&availableRooms); // Wait until at least one room is available // availableRoom - 1
    printf("Guest %d goes to the check-in reservationist.\n", guestIndex);

    sem_wait(&checkInSema); // Wait the check-in Function/ Mutual Exlcusion of Check-In

    int roomAssigned = 0;
    for (int i = 0; i < NUM_ROOMS; ++i) {
        if (rooms[i] == 0) { // Check if room is available
            rooms[i] = guestIndex + 1; // Assign room to guest
            roomAssigned = 1;
            printf("The check-in reservationist greets Guest %d.\n", guestIndex);
            printf("Check-in reservationist assigns room %d to Guest %d.\n", i, guestIndex);
            printf("Guest %d receives Room %d and completes check-in.\n", guestIndex, i);
            break;
        }
    }

    sem_post(&checkInSema); // Release check-in function, Allowing Next Guest to Check-In

    // Random Sleep for Guest
    sleep(rand() % 3 + 1);

    // Randomize the Activity of the Guest
    switch (rand() % 4) {
        case 0:
            printf("Guest %d goes to the pool.\n", guestIndex);
            poolCount++;
            break;
        case 1:
            printf("Guest %d goes to the restaurant.\n", guestIndex);
            restaurantCount++;
            break;
        case 2:
            printf("Guest %d goes to the fitness center.\n", guestIndex);
            fitnessCenterCount++;
            break;
        case 3:
            printf("Guest %d goes to the business center.\n", guestIndex);
            businessCenterCount++;
            break;
    }

    sem_wait(&checkOutSema); // Wait the Check-Out function, only 1 guest allowed to check-out,
    //Guest wait to use CR

    for (int i = 0; i < NUM_ROOMS; ++i) {
        if (rooms[i] == guestIndex + 1) { // Check if guest occupies the room
            printf("Guest %d goes to the check-out reservationist and returns room %d.\n", guestIndex, i);
            printf("The check-out reservationist greets Guest %d and receives the key from room %d.\n", guestIndex, i);
            rooms[i] = 0; // Free the room
            break;
        }
    }

    printf("Guest %d receives the receipt.\n", guestIndex);

    sem_post(&checkOutSema); // Release check-out semaphore
    sem_post(&availableRooms); // Release the room

    return NULL;
}

int main() {
    pthread_t guests[NUM_GUESTS];

    sem_init(&availableRooms, 0, NUM_ROOMS); // Initialize available rooms of Semaphore
    sem_init(&checkInSema, 0, 1); // Initialize check-in Semaphore to allow one guest at a time
    sem_init(&checkOutSema, 0, 1); // Initialize check-out Semaphore to allow one guest at a time

    srand(time(NULL));

    //Create a thread for each guest
    for (int i = 0; i < NUM_GUESTS; ++i) {
        int* guestIndex = (int*)malloc(sizeof(int));
        *guestIndex = i;
        //this swweeps through the guest index, assigning to the thread
        pthread_create(&guests[i], NULL, guest, (void*)guestIndex);
    }

    //sync the completetion of threads
    for (int i = 0; i < NUM_GUESTS; ++i) {
        pthread_join(guests[i], NULL);
    }

    // Print Guest summary
    printf("\nGuest Activity:\n");
    printf("Total Guests: %d\n", totalGuests);
    printf("Pool: %d\n", poolCount);
    printf("Restaurant: %d\n", restaurantCount);
    printf("Fitness Center: %d\n", fitnessCenterCount);
    printf("Business Center: %d\n", businessCenterCount);

    sem_destroy(&availableRooms);
    sem_destroy(&checkInSema);
    sem_destroy(&checkOutSema);

    return 0;
}


//Work Cited For Information:
//https://www.geeksforgeeks.org/use-posix-semaphores-c/
//https://www.geeksforgeeks.org/multithreading-in-c/
//https://www.geeksforgeeks.org/thread-functions-in-c-c/
//https://stackoverflow.com/questions/59234585/pthread-programming-short-example
//https://dev.to/endeavourmonk/semaphore-in-operating-system-with-its-own-implementation-in-c-19ia
