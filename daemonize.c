#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
static void daemonize()
{
    pid_t process_id = 0;
    pid_t sid = 0;
    //change file creation mask
    umask(0);
    //clone process
    process_id = fork();
    
    if (process_id < 0)
    {
        printf("Failed to fork\n");
        
        exit(1);
    }
    // Kill parent
    if (process_id > 0)
    {
        //print pid
        printf("Pid: %d \n", process_id);
        
        exit(0);
    }
    //Initialize new working session 
    sid = setsid();
    if (sid < 0)
    {
        
        exit(1);
    }
    //move to root directory
    chdir("/");
    //close parent stdin stdout stderr
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
}