#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <time.h>




#define PORT 8080

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

sem_t log_lock;
void log_message(const char *operation, const char *filename) 
{
    struct timeval tv;
    gettimeofday(&tv, NULL);

    sem_wait(&log_lock); 

    FILE *log_file = fopen("log.txt", "a");

    if (log_file == NULL) 
    {
        perror("Error opening logfile");
        sem_post(&log_lock); 
        return;
    }

    fprintf(log_file, "[%ld.%06ld] %s from file: %s\n",tv.tv_sec, tv.tv_usec, operation, filename);
    fclose(log_file);

    sem_post(&log_lock); 
}


void display_file_metadata(const char *filename) 
{
    struct stat file_stat;
    
    if (stat(filename, &file_stat) == -1) 
    {
        perror("Error retrieving file metadata");
        return;
    }
    
    printf("Metadata for file: %s\n", filename);
    printf("Size: %ld bytes\n", file_stat.st_size);
    printf("Permissions: ");
    printf((file_stat.st_mode & S_IRUSR) ? "r" : "-");
    printf((file_stat.st_mode & S_IWUSR) ? "w" : "-");
    printf((file_stat.st_mode & S_IXUSR) ? "x" : "-");
    printf("\n");

    char creation_time[26];
    ctime_r(&file_stat.st_ctime, creation_time);
    creation_time[strcspn(creation_time, "\n")] = '\0';
    printf("Creation time: %s\n", creation_time);
    
}

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

    log_message("Rename", file->filename);
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
    log_message("Delete", file->filename);
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

    printf("\n");

    char command[256];
    sprintf(command, "python3 main.py file%d.txt 2", file_index+1);
    system(command);

    close(fd);
    log_message("Read", file->filename);


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
    log_message("Write", file->filename);

    sem_post(&file->rw_mutex);

    return NULL;
}


int main() 
{


    sem_init(&log_lock, 0, 1); 

    int server_fd;

    if((server_fd = socket(AF_INET,SOCK_STREAM,0))<0)
    {
        perror("ERROR");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in address = {0};
    
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) == -1) 
    {
        perror("Bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 5) == -1) 
    {
        perror("Listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Server is listening on port %d...\n", PORT);


    for (int i = 0; i < MAX_FILES; i++) 
    {
        snprintf(files[i].filename, sizeof(files[i].filename), "file%d.txt", i + 1);
        sem_init(&files[i].rw_mutex, 0, 1);
        sem_init(&files[i].mutex, 0, 1);
        files[i].reader_count = 0;
    }

    int file_manager=1;
    while (file_manager) 
    {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd == -1) 
        {
            perror("Accept failed");
            continue;
        }

        char buffer[256] = {0};
        read(client_fd, buffer, sizeof(buffer));
        printf("Received request: %s\n", buffer);

        int file_index, choice;
        sscanf(buffer, "%d %d", &file_index, &choice);


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
                display_file_metadata(files[file_index].filename);
            case 5:
                file_manager=0;
                break;
            default:
                printf("Invalid choice.\n");
        }
        close(client_fd);
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
    close(server_fd);
    return 0;
}
