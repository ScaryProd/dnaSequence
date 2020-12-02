#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>
#include <string.h>
#include <time.h>

#define SIZE 12000000
#define SIZE_L 20000

#define MAX_LINES_THREAD 100
#define MAX_THREADS 5

struct data
{
    int index;
    int size;
};

char *sample[500];
struct data sampleindex[500];
double charsmaped = 0;
char *DNA;
int num_threads = MAX_THREADS;
int lines_per_thread = MAX_LINES_THREAD;

int compare(const void *a, const void *b)
{
    const struct data *da = a, *db = b;
    return da->index < db->index ? -1 : da->index > db->index;
}

void *myFun(void *x)
{
    int tid;
    tid = *((int *)x);
    printf("Entering thread %d\n", tid);
    for(int idx = tid*100; idx < tid*100+lines_per_thread; idx++){
        char *exists = strstr(DNA, sample[idx]);
        if(exists != NULL) {
            int index = (int)(exists - DNA);
            int size = strlen(sample[idx]);
            sampleindex[idx].index = index;
            sampleindex[idx].size = size;
        } else {
            sampleindex[idx].index = -1;
            sampleindex[idx].size = 0;
        }
    }
    return NULL;
}

int checker(char fileToOpen[1024])
// int main()
{
    // char *fileToOpen = "s_cerevisia.seq";
    FILE *ref;
    ref = fopen("S._cerevisiae_processed.txt", "r");
    
    if (ref)
    {
        DNA = (char *) realloc(DNA, SIZE);
        fgets(DNA, sizeof(char)*SIZE, ref);
    }
    printf("%s\n", fileToOpen);
    
    int mapeado = 0;
    int nomapeado = 0;
    int i = 0;
    char linea[SIZE_L];
    int dnasize = strlen(DNA);

    FILE *file;
    file = fopen(fileToOpen, "r");
    while (fgets(linea, sizeof(linea), file) != NULL)
    {
        linea[strlen(linea) - 1] = '\0';
        linea[strlen(linea) - 1] = '\0';
        sample[i] = strdup(linea);
        i++;
    }
    fclose(file);
    
    clock_t begin = clock();
    int total_lines = i;
    if(i < 100) {
        num_threads = 1;
        lines_per_thread = i;
    } else if (i / 100 > 0 && i / 100 < MAX_THREADS){
        num_threads = i / 100;
        lines_per_thread = i / num_threads;
    }


    pthread_t threads[num_threads];
    int thread_args[num_threads];
    int rc;
    for (int j = 0; j < num_threads; j++)
    {
        thread_args[j] = j;
        printf("spawning thread %d\n", j);
        rc = pthread_create(&threads[j], NULL, myFun, (void *)&thread_args[j]);
    }
    for (int j = 0; j < num_threads; j++)
    {
        rc = pthread_join(threads[j], NULL);
    }
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;

    for (int j = 0; j < total_lines; j++)
    {
        if (sampleindex[j].index != -1)
        {
            printf("\n%s a partir del caracter %d", sample[j], sampleindex[j].index);
            mapeado++;
        }
        else
        {
            printf("\n%s no se encontro", sample[j]);
            nomapeado++;
        }
    }

    qsort(sampleindex, total_lines, sizeof(*sampleindex), compare);
    for (int j = 0; j < total_lines; j++)
    {
        if (sampleindex[j].index == -1)
        {
            continue;
        }
        if (j + 1 == i)
        {
            charsmaped += sampleindex[j].size;
        }
        else if (sampleindex[j].index + sampleindex[j].size >= sampleindex[j + 1].index)
        {
            charsmaped += sampleindex[j].size;
            charsmaped -= sampleindex[j].index + sampleindex[j].size - sampleindex[j + 1].index;
            charsmaped -= 1;
        }
        else
        {
            charsmaped += sampleindex[j].size;
        }
    }
    charsmaped = charsmaped / dnasize * 100;
    printf("\n");
    printf("\nEl archivo cubre el %f del genoma de referencia", charsmaped);
    printf("\n%d secuencias mapeadas", mapeado);
    printf("\n%d secuencias no mapeadas\n", nomapeado);
    printf("Exec time: %f\n", time_spent);
    // free(DNA);

    return 0;
}
