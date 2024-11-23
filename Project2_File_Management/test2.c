#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <stdbool.h>

// Binary semaphores
sem_t rw_mutex; // For exclusive access to the file for writers
sem_t mutex;    // For synchronizing access to reader_count

int reader_count = 0;
const char *filename = "example.txt";

// Reader thread function
void *reader(void *arg) {
    int fd;
    sem_wait(&mutex); // Lock mutex to update reader_count safely
    reader_count++;
    if (reader_count == 1) {
        sem_wait(&rw_mutex); // First reader locks rw_mutex for exclusive file access
    }
    sem_post(&mutex); // Unlock mutex after updating reader_count

    // Read file content
    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file for reading");
        return NULL;
    }

    printf("Reader %ld is reading the file:\n", pthread_self());
    char buffer[256];
    ssize_t bytes;
    while ((bytes = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes] = '\0';
        printf("%s", buffer);
    }
    printf("\n");

   // while(1)
    //{

    //}
    close(fd);

    // Decrease reader count
    sem_wait(&mutex);
    reader_count--;
    if (reader_count == 0) {
        sem_post(&rw_mutex); // Last reader releases rw_mutex
    }
    sem_post(&mutex);

    return NULL;
}

// Writer thread function
void *writer(void *arg) {
    int fd;
    sem_wait(&rw_mutex); // Lock rw_mutex to get exclusive access to the file for writing

    // Open file and write data
    fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1) {
        perror("Error opening file for writing");
        sem_post(&rw_mutex);
        return NULL;
    }

    printf("Writer %ld is writing to the file.\n", pthread_self());
    const char *text = "This is a line written by writer.\n";
    if (write(fd, text, strlen(text)) == -1) {
        perror("Error writing to file");
    } else {
        printf("Data written to file by writer %ld.\n", pthread_self());
    }
    while(1)
    {

    }

    close(fd);

    sem_post(&rw_mutex); // Release exclusive access after writing
    return NULL;
}

int main() {
    // Initialize semaphores
    sem_init(&rw_mutex, 0, 1); // rw_mutex initially unlocked (1)
    sem_init(&mutex, 0, 1);    // mutex initially unlocked (1)
    int rptr=-1;
    int wptr=-1;
    pthread_t readers[10];
    pthread_t writers[10];

    bool file_manager = true;

    int choice;
    while(file_manager)
    {
        printf("Enter the choice:\n");
        scanf("%d",&choice);

        switch(choice)
        {
            case 0:
                pthread_create(&readers[++rptr],NULL,reader,NULL);
                
            break;
            case 1:
                pthread_create(&writers[++wptr],NULL,writer,NULL);
                //pthread_join(writers[wptr],NULL);
            break;
            case 2:
                file_manager=false;
            break;

        }

    }
    for(int i=0;i<=rptr;i++)
    {
        pthread_join(readers[i],NULL);
    }
    for(int i=0;i<=wptr;i++)
    {
        pthread_join(writers[i],NULL);
    }



    // ->pthread_t reader_thread1, reader_thread2, writer_thread;

    // Create threads for readers and writer
    //pthread_create(&reader_thread1, NULL, reader, NULL);
    // pthread_create(&reader_thread2, NULL, reader, NULL);
    //pthread_create(&writer_thread, NULL, writer, NULL);

    // Wait for threads to complete
    //pthread_join(reader_thread1, NULL);
    //pthread_join(reader_thread2, NULL);
    //pthread_join(writer_thread, NULL);

    // Destroy semaphores
    sem_destroy(&rw_mutex);
    sem_destroy(&mutex);

    return 0;
}
