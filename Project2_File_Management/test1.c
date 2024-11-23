#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

// Function prototypes
void *read_file(void *filename);
void *write_file(void *filename);

// Mutex for coordinating read and write threads
pthread_mutex_t write_mutex = PTHREAD_MUTEX_INITIALIZER;

int main() {
    char filename[] = "example.txt";
    char choice;

    while (1) {
        
        pthread_t thread;

                
        
        printf("Choose an option:\n");
        printf("r - Read from file\n");
        printf("w - Write to file\n");
        printf("q - Quit\n");
        printf("Enter your choice: ");
        scanf(" %c", &choice);

        
        
        
        
        if (choice == 'r') {
            // Create a read thread
            if (pthread_create(&thread, NULL, read_file, (void *)filename) != 0) 
            {
                pthread_join(thread,NULL);
                perror("Failed to create read thread");
            }
            pthread_detach(thread); // Auto-release resources when the thread finishes
        } else if (choice == 'w') 
        {
            // Create a write thread
            if (pthread_create(&thread, NULL, write_file, (void *)filename) != 0) 
            {
                pthread_join(thread,NULL);
                perror("Failed to create write thread");
            }
            pthread_detach(thread); // Auto-release resources when the thread finishes
        } else if (choice == 'q') {
            break; // Exit the loop
        } else {
            printf("Invalid choice. Please try again.\n");
        }
    }

    return 0;
}

// Thread function for reading the file
void *read_file(void *filename) {
    int fd = open((char *)filename, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file for reading");
        return NULL;
    }

    struct flock lock;
    lock.l_type = F_RDLCK;  // Shared read lock
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;         // Lock the whole file

    // Apply read lock
    if (fcntl(fd, F_SETLK, &lock) == -1) {
        perror("Error locking file for reading");
        close(fd);
        return NULL;
    }

    // Reading the file
    char buffer[256];
    ssize_t bytes;
    printf("Reading from file:\n");
    while ((bytes = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
        buffer[bytes] = '\0';
        printf("%s", buffer);
    }
    printf("\n");

    // Unlock and close
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);

    return NULL;
}

// Thread function for writing to the file
void *write_file(void *filename) {
    // Ensure exclusive access for writing using a mutex
    pthread_mutex_lock(&write_mutex);

    int fd = open((char *)filename, O_RDWR);
    if (fd == -1) {
        perror("Error opening file for writing");
        pthread_mutex_unlock(&write_mutex);
        return NULL;
    }

    struct flock lock;
    lock.l_type = F_WRLCK;  // Exclusive write lock
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len = 0;         // Lock the whole file

    // Apply write lock
    if (fcntl(fd, F_SETLK, &lock) == -1) {
        perror("Error locking file for writing");
        close(fd);
        pthread_mutex_unlock(&write_mutex);
        return NULL;
    }

    // Writing to the file
    printf("Enter text to write to the file: ");
    char input[256];
    scanf(" %[^\n]", input);  // Read a line of input

    if (write(fd, input, strlen(input)) == -1) {
        perror("Error writing to file");
    } else {
        printf("Data written to file.\n");
    }

    // Unlock and close
    lock.l_type = F_UNLCK;
    fcntl(fd, F_SETLK, &lock);
    close(fd);

    pthread_mutex_unlock(&write_mutex);

    return NULL;
}
