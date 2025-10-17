#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
int Read =0;
int wright=1;
int main(int argumnent, char*argArr[]){
int size =argumnent-1;
int *arr = (int*)malloc(size*sizeof (int));
int *Rarr = (int*)malloc(size*sizeof (int));
for(int i =0;i<size;i++)
arr[i]=atoi(argArr[i+1]);

int pip[2];
int bytesread;
pipe(pip);
if(fork()==0){

    close(pip[Read]);
    for(int i=0;i<size-1;i++)
    for(int j=0;j<size-1;j++){
        if(arr[j]>arr[j+1])
        {
            int temp=arr[j];
            arr[j]=arr[j+1];
            arr[j+1]=temp;
        }

    }
    write(pip[wright],arr,size*sizeof(int));
    close(pip[wright]);
}
else /* parent, reader */
{
close ( pip [wright] ) ; /* close unused end */
bytesread = read (pip [Read], Rarr, size*sizeof(int)) ;
int d;
for(int i=0;i<size;i++){
d=Rarr[i];
printf ("Read %d bytes :%d\n", bytesread, d) ;}
close ( pip [Read] ) ; /* close used end */
}

    return 0;
}
