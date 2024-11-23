#include "kernel/types.h"
#include "kernel/stat.h"
#include "user.h"

#define SEM_ID 0

int main(void)
{
  int a,b;
  
  if((a = fork()) && (b = fork()))
  {
    //printf("Inside parent process\n");
    printf("%d\n",a);
    waitpid(a);
    printf("Terminate just one child\n");
  }
  
  else if(a > 0)
  {
    sleep(100);
    printf("Inside next child\n");
  }
  
  else
  {
    printf("Inside first child\n");
    sleep(20);
  }
}
