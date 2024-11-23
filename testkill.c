#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

int main(void)
{
  int status;
  int parentId = fork();
  if(parentId > 0) //parent
  {
    printf("Inside main parent with pid %d and parent pid %d\n",getpid(),getppid());
    wait(&status);
  }
  else
  {
    int a,b;
    if((a = fork()) && (b = fork()))
    {
      sleep(10);
      printf("Inside parent with pid %d and parent pid %d \n",getpid(),getppid());
      sleep(100);
      int count = killall();
      printf("Number of descendants terminated %d\n",count);
      
    }
    else
    {
      if(a) //secondChild
      {
        sleep(30);
        printf("Second Child Process with pid %d and parent pid %d\n",getpid(),getppid());
        sleep(100);
      }
      
      else
      {
        sleep(50);
        printf("Third Child Process with pid %d and parent pid %d \n",getpid(),getppid());
        
        int grandChildId = fork();
        if(grandChildId > 0)
        {
          sleep(100);
        }
        
        else
        {
          printf("Inside grandchild with pid %d and parent pid %d\n",getpid(),getppid());
          
          int greatGrandChildId = fork();
          if(greatGrandChildId > 0) 
          {
            sleep(100);
          }
          else
          {
            printf("Great Grand child process with pid %d and parent pid %d \n",getpid(),getppid());
            sleep(200);
          }
        }
      }
      
    }
  }
}
