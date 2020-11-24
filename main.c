/*
    Proyecto Final - DNA Sequence
    Alvaro Santana A01196914
    Jesus Lozano A01194162
    Sergio Diosdado A00516971
*/
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/stat.h>
#include "daemonize.c"

int main(int argc, char **argv)
{
    daemonize();
    while (1)
    {
        
    }
}