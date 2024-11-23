#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include <stddef.h>  // To define NULL


char user_buffer[100];

uint64 sys_killall(void)
{
  return killall();
}

uint64 sys_getppid(void)
{
  return getppid();
}

uint64 sys_messageQueueInit(void)
{
  int id;
  argint(0,&id);
   messageQueueInit(id);
   return 0;
}

uint64 sys_waitpid(void)
{
  int id;
  argint(0,&id);
  return waitpid(id);
}

uint64 sys_sendMessage(void)
{
    int msgid;
    argint(0, &msgid);  // Use &msgid to pass the address of msgid

    char msg[30];
    argstr(1, msg, sizeof(msg));  // Use argstr to retrieve a string argument

    int id;
    argint(2, &id);  // Use &id to pass the address of id
    
    

    sendMessage(msgid, msg, id);
    return 0;
}


char* sys_getMessage(void)
{
  int msgid, id;
  
  
  argint(0, &msgid);  
  argint(1, &id);     
  
  
  //char user_buffer[30];
  argstr(2, user_buffer,sizeof(user_buffer));  
  //uint64 p;
  //argaddr(2, &user_buffer);

  getMessage(msgid,id,user_buffer);
  printf("%s \n",user_buffer);
  return user_buffer;
}

uint64 sys_sem_init(void) {
    int semid, value;
    argint(0, &semid);
    argint(1, &value);
    sem_init(&semaphores[semid], value);
    
    return 0;
}

int sys_sem_wait(void) {
    int semid;
    argint(0, &semid);
    sem_wait(&semaphores[semid]);
    return 0;
}

int sys_sem_signal(void) {
    int semid;
    argint(0, &semid);
    sem_signal(&semaphores[semid]);
    return 0;
}

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if(n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
