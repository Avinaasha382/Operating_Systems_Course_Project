#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <stdbool.h>


#define MAX_FILES 10
#define MAX_THREADS 10

typedef struct 
{
    char filename[50];
    sem_t rw_mutex;
    sem_t mutex;
    int reader_count;
} FileResource;

FileResource files[MAX_FILES];

// Process threads below default Processes of each type are 10.

pthread_t readers[MAX_FILES][MAX_THREADS];
pthread_t writers[MAX_FILES][MAX_THREADS];
pthread_t deleters[MAX_FILES][MAX_THREADS];
pthread_t renamers[MAX_FILES][MAX_THREADS];

// Pointers for each process -> Points towards the File undergoing the Process 
int rptr[MAX_FILES] = { -1 };
int wptr[MAX_FILES] = { -1 };
int dptr[MAX_FILES] = { -1 };
int reptr[MAX_FILES] = { -1 };

void *renamer(void *arg) 
{
    int file_index = *(int *)arg;
    FileResource *file = &files[file_index];

    
    sem_wait(&file->rw_mutex);

    char new_filename[50];
    snprintf(new_filename, sizeof(new_filename), "renamed_file%d.txt", file_index + 1);


    if (rename(file->filename, new_filename) == 0) 
    {
        printf("Renamer %ld has renamed the file %s to %s.\n", pthread_self(), file->filename, new_filename);
        snprintf(file->filename, sizeof(file->filename), "%s", new_filename); 
    } 
    else 
    {
        perror("Error renaming file");
    }


    sem_post(&file->rw_mutex);

    return NULL;
}

void *deleter(void *arg)
{
    int file_index = *(int *)arg;
    FileResource *file = &files[file_index];

    
    sem_wait(&file->rw_mutex);

    char command[256];
    sprintf(command, "python3 main.py file%d.txt 3", file_index+1);
    //system(command);
    FILE *pipe = popen(command, "r");
    if (pipe == NULL) 
    {
        perror("popen failed");
        return NULL;
    }



    char buffer[128];
    memset(buffer,0,128);

    while (fgets(buffer, sizeof(buffer), pipe) != NULL) 
    {
        printf("Output from Python: %s", buffer);
        buffer[strcspn(buffer, "\n")] = '\0';
    }

    pclose(pipe);


    if(strcmp(buffer,"y")==0)
    {

        if (unlink(file->filename) == 0) 
        {
            printf("Deleter %ld has deleted the file %s.\n", pthread_self(), file->filename);
        }    
        else 
        {
            perror("Error deleting file");
        }
    } 
    
    sem_post(&file->rw_mutex);
    sem_post(&file->mutex);

    return NULL;
}
void *reader(void *arg) 
{
    int file_index = *(int *)arg;
    FileResource *file = &files[file_index];

    sem_wait(&file->mutex);
    file->reader_count++;
    if (file->reader_count == 1) 
    {
        sem_wait(&file->rw_mutex);
    }
    sem_post(&file->mutex);

    int fd = open(file->filename, O_RDONLY);
    if (fd == -1) {
        perror("Error opening file for reading");
        return NULL;
    }

    printf("Reader %ld is reading the file %s:\n", pthread_self(), file->filename);
    //char buffer[256];
    //ssize_t bytes;
    //while ((bytes = read(fd, buffer, sizeof(buffer) - 1)) > 0) {
    ///    buffer[bytes] = '\0';
        //printf("%s", buffer);
    //}
    printf("\n");

    //system("python3 main.py");

    char command[256];
    sprintf(command, "python3 main.py file%d.txt 2", file_index+1);
    system(command);

    close(fd);

    sem_wait(&file->mutex);
    file->reader_count--;
    if (file->reader_count == 0) 
    {
        sem_post(&file->rw_mutex);
    }
    sem_post(&file->mutex);

    return NULL;
}


void *writer(void *arg) 
{
    int file_index = *(int *)arg;
    FileResource *file = &files[file_index];

    sem_wait(&file->rw_mutex);

    int fd = open(file->filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1) 
    {
        perror("Error opening file for writing");
        sem_post(&file->rw_mutex);
        return NULL;
    }

    printf("Writer %ld is writing to the file %s.\n", pthread_self(), file->filename);

    char command[256];
    sprintf(command, "python3 main.py file%d.txt 1", file_index+1);
    system(command);

 
    close(fd);

    sem_post(&file->rw_mutex);

    return NULL;
}

int main() 
{

    for (int i = 0; i < MAX_FILES; i++) 
    {
        snprintf(files[i].filename, sizeof(files[i].filename), "file%d.txt", i + 1);
        sem_init(&files[i].rw_mutex, 0, 1);
        sem_init(&files[i].mutex, 0, 1);
        files[i].reader_count = 0;
    }

    int file_index;
    bool file_manager = true;
    int choice;

    while (file_manager) 
    {
        printf("Enter the file index (0 to %d) and choice (0: Reader, 1: Writer, 2: Exit):\n", MAX_FILES - 1);
        scanf("%d %d", &file_index, &choice);

        if (file_index < 0 || file_index >= MAX_FILES) 
        {
            printf("Invalid file index.\n");
            continue;
        }

        switch (choice) 
        {
            case 0:
                pthread_create(&readers[file_index][++rptr[file_index]], NULL, reader, &file_index);
                break;
            case 1:
                pthread_create(&writers[file_index][++wptr[file_index]], NULL, writer, &file_index);
                break;
            case 2:
                pthread_create(&deleters[file_index][++dptr[file_index]], NULL, deleter, &file_index);
                break;
            case 3:
                pthread_create(&renamers[file_index][++reptr[file_index]], NULL, renamer, &file_index);
                break;
            case 4:
                file_manager = false;
                break;
            default:
                printf("Invalid choice.\n");
        }
    }

    for (int i = 0; i < MAX_FILES; i++) 
    {
        for (int j = 0; j <= rptr[i]; j++) 
        {
            pthread_join(readers[i][j], NULL);
        }
        for (int j = 0; j <= wptr[i]; j++) 
        {
            pthread_join(writers[i][j], NULL);
        }
        for (int j = 0; j <= wptr[i]; j++) 
        {
            pthread_join(deleters[i][j], NULL);
        }
    }

    for (int i = 0; i < MAX_FILES; i++) 
    {
        sem_destroy(&files[i].rw_mutex);
        sem_destroy(&files[i].mutex);
    }

    return 0;
}
