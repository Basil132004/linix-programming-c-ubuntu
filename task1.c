#include<stdio.h>
#include<sys/wait.h>
#include<unistd.h>

int main(int argc, char *argv[])
{
int id=fork();
if(id==0)
{
    printf("child process started PID= %d, parent pid %d \n",getpid(),getppid());
printf("child running....\n");
        sleep(3);
        printf("child finished.\n");

}
else {

 printf("pareent process PID = %d \n",getpid());
        
            if(wait(NULL)==-1)
           printf("error still waiting for child..\n");
        else
        printf("parent detected child termination.\n");
          
    
}
    return 0;
}