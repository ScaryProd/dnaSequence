#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

struct data{
    int index;
    int size;
};

char* sample[50];
struct data sampleindex[50];
double charsmaped = 0;
char DNA[369] = "ACAAGATGCCATTGTCCCCCGGCCTCCTGCTGCTGCTGCTCTCCGGGGCCACGGCCACCGCTGCCCTGCCCCTGGAGGGTGGCCCCACCGGCCGAGACAGCGAGCATATGCAGGAAGCGGCAGGAATAAGGAAAAGCAGCCTCCTGACTTTCCTCGCTTGGTGGTTTGAGTGGACCTCCCAGGCCAGTGCCGGGCCCCTCATAGGAGAGGAAGCTCGGGAGGTGGCCAGGCGGCAGGAAGGCGCACCCCCCCAGCAATCCGCGCGCCGGGACAGAATGCCCTGCAGGAACTTCTTCTGGAAGACCTTCTCCTCCTGCAAATAAAACCTCACCCATGAATGCTCACGCAAGTTTAATTACAGACCTGAA";

 int isSubstring(char *stringGrande, char *substring) {
    int i = 0;
    int j = 0;
    for (i = strlen(stringGrande) - strlen(substring); i >= 0; i--) {
        int encontro = 1;
        for (j = 0; j < strlen(substring); j++) {
            if (stringGrande[i + j] != substring[j]) {
                encontro = 0; 
                break;
            }
        }
        if (encontro == 1) {
            return i;
        }
    }
    return -1;
 } 

 void *myFun(void *x)
{
    int tid;
    tid = *((int *) x);
    //printf("\n%s", sample[tid]);
    int index = isSubstring(DNA, sample[tid]);
    //printf("\n%d", index);
    // if(index != -1){
    //     printf(" - %d", index + strlen(sample[tid]));
    // }
    sampleindex[tid].index = index;
    sampleindex[tid].size = strlen(sample[tid]);
     return NULL;
}

int compare (const void * a, const void * b) {
    const struct data *da = a, *db = b;
    return da->index < db->index ? -1 : da->index > db->index;
}


int main(int argc, char const *argv[])
{
    int mapeado = 0;
    int nomapeado = 0;
    int i=0;
    char linea[369];
    int dnasize = strlen(DNA);
    FILE *file;
    file = fopen("texto.seq", "r");
    while(fgets(linea, sizeof(linea), file)!=NULL) {
        linea[strlen(linea) - 1] = '\0';
        sample[i] = strdup(linea); 
        i++;
    }
    fclose(file);
    pthread_t threads[i + 1];
    int thread_args[i + 1];
    int rc;

    for(int j = 0; j < i; j++){
        thread_args[j] = j;
        printf("spawning thread %d\n", j);
        rc = pthread_create(&threads[j], NULL, myFun, (void *) &thread_args[j]);
    }
    for (int j=0; j < i; j++) {
        rc = pthread_join(threads[j], NULL);
    }

    for(int j=0; j < i; j++){
        if(sampleindex[j].index != -1){
            printf("\n%s a partir del caracter %d",  sample[j], sampleindex[j]);
            mapeado++;
        }else{
            printf("\n%s no se encontro",  sample[j]);
            nomapeado++;
        }
    }

    qsort(sampleindex, i, sizeof(*sampleindex), compare);
    for(int j = 0; j < i; j++){
        if(sampleindex[j].index == -1){
            continue;
        }
        if(j + 1 == i){
            charsmaped += sampleindex[j].size;
        }else if(sampleindex[j].index + sampleindex[j].size >= sampleindex[j + 1].index){
            charsmaped += sampleindex[j].size;
            charsmaped -= sampleindex[j].index + sampleindex[j].size - sampleindex[j + 1].index;
            charsmaped -= 1;
        }else{
            charsmaped += sampleindex[j].size;
        }
    }

    charsmaped = charsmaped / dnasize * 100;
    printf("\n");
    printf("\nEl archivo cubre el %d%% del genoma de referencia", (int) floor(charsmaped));
    printf("\n%d secuencias mapeadas", mapeado);
    printf("\n%d secuencias no mapeadas", nomapeado);

    return 0;
}
