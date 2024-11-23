#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

int main(void)
{
  int childId = fork();
  
  if(childId > 0) 
  {
    printf("Inside parent process\n");
    
    messageQueueInit(9);
    char message[30] = "Hello World";
    
    sleep(50);
    
    sendMessage(9, message, 0);  
    int status;
    
    wait(&status);
  }
  else  // Child process
  {
    sleep(50);
    printf("Inside child process\n");
    
    //printf("Message received from parent is: ");
    
    char buffer[40]; 
    
  
    getMessage(9, 0, buffer);  
    printf("END\n");
  }
}

