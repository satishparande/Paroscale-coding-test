#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>


#define NUM_THREADS 4
// this is the size of the hash table
// the size of the hash table should be a prime number
// to reduce the number of collisions
#define HASH_SIZE 100003

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

// this is for maintaning the list hash collisions
// in the hash table
// each node contains a value and a pointer to the next node
// in the list
typedef struct Node 
{
    int value;
    struct Node* next;
} Node;

// hash table to hold unique numbers
Node* hash_table[HASH_SIZE];

// this is argruments for the thread
// each thread will be given a start and end value
// to process
typedef struct 
{    
    long start;
    long end;
    const char* filename;
} ThreadArg;

unsigned int hash(int key) 
{
    return (unsigned int)key % HASH_SIZE;
}

// function to check if a number is present in the hash table
// this function will traverse the linked list
// and check if the number is present
int isPresent(Node* head, int val) 
{
    while (head) {
        if (head->value == val) return 1;
        head = head->next;
    }
    return 0;
}

// function to insert a number into the hash table
// this function will first check if the number is present
// if it is not present, it will insert the number
// at the head of the linked list
// this function is thread-safe
void insertUnique(int num) 
{
    unsigned int h = hash(num);
    pthread_mutex_lock(&mtx);

    if (!isPresent(hash_table[h], num)) 
    {
        Node* newNode = (Node*)malloc(sizeof(Node));
        newNode->value = num;
        newNode->next = hash_table[h];
        hash_table[h] = newNode;
    }

    pthread_mutex_unlock(&mtx);
}

// function to be executed by each thread
// each thread will read a portion of the file
// and process the numbers in that portion
void* processData(void* arg) 
{
    ThreadArg* t = (ThreadArg*)arg;
    FILE* fp = fopen(t->filename, "r");
    if (!fp) 
    {
        perror("fopen");
        return NULL;
    }

    fseek(fp, t->start, SEEK_SET);
    long pos = ftell(fp);
    char buffer[1024];

    // Read the file until the end or until the specified end position
    // and process the numbers
    // using the strtok function to split the line into tokens
    // and then convert the tokens to integers
    // and insert them into the hash table
    while (pos < t->end && fgets(buffer, sizeof(buffer), fp)) 
    {
        char* token = strtok(buffer, " \t\n");
        while (token) 
        {
            int num = atoi(token);
            insertUnique(num);
            token = strtok(NULL, " \t\n");
        }
        // Get the current position in the file
        // and check if it is less than the end position
        pos = ftell(fp);
    }

    fclose(fp);
    return NULL;
}


int main() {
    const char* filename = "numbers.txt";
    FILE* fp = fopen(filename, "r");
    if (!fp) 
    {
        perror("file open failed");
        return 1;
    }

    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fclose(fp);

    // Init hash table
    for (int i = 0; i < HASH_SIZE; ++i) 
    {
        hash_table[i] = NULL;
    }

    pthread_t threads[NUM_THREADS];
    ThreadArg args[NUM_THREADS];
    long chunk = size / NUM_THREADS;

    for (int i = 0; i < NUM_THREADS; ++i) 
    {
        args[i].start = i * chunk;
        //args[i].end = (i == NUM_THREADS - 1) ? size : (i + 1) * chunk;
        args[i].end = (i + 1) * chunk;
        args[i].filename = filename;
        pthread_create(&threads[i], NULL, processData, &args[i]);
    }

    for (int i = 0; i < NUM_THREADS; ++i)
        pthread_join(threads[i], NULL);

    printf("Unique numbers:\n");
    int totalNums=0;
    for (int i = 0; i < HASH_SIZE; ++i) 
    {
        Node* current = hash_table[i];
        while (current) {
            printf("%d\n", current->value);
            Node* temp = current;
            current = current->next;
            free(temp);
            totalNums++;
        }
    }
    
    printf("Total unique numbers: %d\n", totalNums);

    // Cleanup
    pthread_mutex_destroy(&mtx);

    return 0;
}

// Compile with: gcc -o main main.c -lpthread
// Run with: ./main
// Make sure to create a file named "numbers.txt" with random numbers
// to test the program
// The file should contain random numbers separated by spaces or new lines
// The program will read the file, process the numbers in parallel using threads,
// and store the unique numbers in a hash table
// The program will then print the unique numbers and the total count of unique numbers
// The program uses a hash table to store unique numbers
// The hash table is implemented using an array of linked lists
// Each thread will read a portion of the file and process the numbers in that portion
// The program uses mutexes to ensure that the hash table is accessed safely by multiple threads
