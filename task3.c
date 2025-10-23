#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<signal.h>
#include<string.h>

int start_processing = 0;
int allow_exit = 0;

void sig_handler_start(int sig) {
    start_processing = 1;  // Signal to start processing
}

void sig_handler_exit(int sig) {
    allow_exit = 1;  // Signal to allow child to exit
}

int main() {
    int size, half;
    int pip1[2], pip2[2], result[2];

    printf("Enter size of array: ");
    if (scanf("%d", &size) != 1 || size <= 0) {
        printf("Invalid size input.\n");
        return 1;
    }
    
    int *arr = (int *)malloc(size * sizeof(int));
    if (arr == NULL) {
        printf("Memory allocation failed.\n");
        return 1;
    }
    
    for (int i = 0; i < size; i++) {
        printf("Enter number for arr[%d]: ", i);
        if (scanf("%d", &arr[i]) != 1) {
            printf("Invalid input.\n");
            free(arr); 
            return 1;
        }
    }

    pipe(pip1);
    pipe(pip2);
    pipe(result);

    half = size / 2;
    int id1 = fork();

    if (id1 == 0) {
        signal(SIGUSR1, sig_handler_start);
        signal(SIGUSR2, sig_handler_exit);

        close(pip1[1]);
        close(pip2[0]);
        close(result[0]);

        int arr_part[half];

        while (!start_processing); // Wait for signal to start processing

        read(pip1[0], arr_part, sizeof(arr_part));
        close(pip1[0]);

        int min = arr_part[0];
        for (int i = 1; i < half; i++) {
            if (arr_part[i] < min) {
                min = arr_part[i];
            }
        }
        write(result[1], &min, sizeof(min));
        close(result[1]);

        while (!allow_exit); // Wait for signal to exit
        exit(0);
    }

    int id2 = fork();

    if (id2 == 0) {
        signal(SIGUSR1, sig_handler_start);
        signal(SIGUSR2, sig_handler_exit);

        close(pip1[0]);
        close(pip1[1]);
        close(pip2[1]);
        close(result[0]);

        int arr_part[half];

        while (!start_processing); // Wait for signal to start processing

        read(pip2[0], arr_part, sizeof(arr_part));
        close(pip2[0]);

        int min = arr_part[0];
        for (int i = 1; i < half; i++) {
            if (arr_part[i] < min) {
                min = arr_part[i];
            }
        }
        write(result[1], &min, sizeof(min));
        close(result[1]);

        while (!allow_exit); // Wait for signal to exit
        exit(0);
    }

    close(pip1[0]);
    close(pip2[0]);
    close(result[1]);

    write(pip1[1], arr, half * sizeof(int));
    close(pip1[1]);

    write(pip2[1], arr + half, half * sizeof(int));
    close(pip2[1]);

    wait(NULL);
    wait(NULL);

    int min1, min2;
    read(result[0], &min1, sizeof(min1));
    read(result[0], &min2, sizeof(min2));
    close(result[0]);

    int smallest = (min1 < min2) ? min1 : min2;
    printf("The smallest number in the array is: %d\n", smallest);

    kill(id1, SIGUSR2);  // Allow child 1 to exit
    kill(id2, SIGUSR2);  // Allow child 2 to exit

    free(arr);

    return 0;
}
