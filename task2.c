#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define READ_END 0
#define WRITE_END 1

int main() {
    int N;

    printf("Enter the number of elements: ");
    scanf("%d", &N);

    if (N < 2) {
        printf("Need at least 2 elements.\n");
        return 1;
    }

    int *arr = malloc(N * sizeof(int));
    int half = N / 2;
    int other_half = N - half;

    printf("Enter %d numbers:\n", N);
    for (int i = 0; i < N; i++) {
        scanf("%d", &arr[i]);
    }

    int pipe_p_c1[2], pipe_c1_p[2];
    int pipe_p_c2[2], pipe_c2_p[2];

    if (pipe(pipe_p_c1) == -1 || pipe(pipe_c1_p) == -1 ||
        pipe(pipe_p_c2) == -1 || pipe(pipe_c2_p) == -1) {
        perror("pipe");
        free(arr);
        return 1;
    }

    int pid1 = fork();
    if (pid1 == 0) {
        // Child 1
        close(pipe_p_c1[WRITE_END]);
        close(pipe_c1_p[READ_END]);

        close(pipe_p_c2[READ_END]);
        close(pipe_p_c2[WRITE_END]);
        close(pipe_c2_p[READ_END]);
        close(pipe_c2_p[WRITE_END]);

        int count;
        read(pipe_p_c1[READ_END], &count, sizeof(int));
        int *data = malloc(count * sizeof(int));
        read(pipe_p_c1[READ_END], data, count * sizeof(int));
        close(pipe_p_c1[READ_END]);

        int min = data[0];
        for (int i = 1; i < count; i++) {
            if (data[i] < min) min = data[i];
        }
        free(data);

        write(pipe_c1_p[WRITE_END], &min, sizeof(int));
        close(pipe_c1_p[WRITE_END]);

        exit(0);
    }

    int pid2 = fork();
    if (pid2 == 0) {
        // Child 2
        close(pipe_p_c2[WRITE_END]);
        close(pipe_c2_p[READ_END]);

        close(pipe_p_c1[READ_END]);
        close(pipe_p_c1[WRITE_END]);
        close(pipe_c1_p[READ_END]);
        close(pipe_c1_p[WRITE_END]);

        int count;
        read(pipe_p_c2[READ_END], &count, sizeof(int));
        int *data = malloc(count * sizeof(int));
        read(pipe_p_c2[READ_END], data, count * sizeof(int));
        close(pipe_p_c2[READ_END]);

        int min = data[0];
        for (int i = 1; i < count; i++) {
            if (data[i] < min) min = data[i];
        }
        free(data);

        write(pipe_c2_p[WRITE_END], &min, sizeof(int));
        close(pipe_c2_p[WRITE_END]);

        exit(0);
    }

    // Parent
    close(pipe_p_c1[READ_END]);
    close(pipe_c1_p[WRITE_END]);
    close(pipe_p_c2[READ_END]);
    close(pipe_c2_p[WRITE_END]);

    write(pipe_p_c1[WRITE_END], &half, sizeof(int));
    write(pipe_p_c1[WRITE_END], arr, half * sizeof(int));
    close(pipe_p_c1[WRITE_END]);

    write(pipe_p_c2[WRITE_END], &other_half, sizeof(int));
    write(pipe_p_c2[WRITE_END], arr + half, other_half * sizeof(int));
    close(pipe_p_c2[WRITE_END]);

    int min1, min2;
    read(pipe_c1_p[READ_END], &min1, sizeof(int));
    close(pipe_c1_p[READ_END]);
    read(pipe_c2_p[READ_END], &min2, sizeof(int));
    close(pipe_c2_p[READ_END]);

    free(arr);

    int overall_min = (min1 < min2) ? min1 : min2;
    printf("Smallest number is: %d\n", overall_min);

    return 0;
}
