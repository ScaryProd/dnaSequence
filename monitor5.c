#include <stdio.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h> // library for fcntl function

#define MAX_EVENTS 1024                           /* Maximum number of events to process*/
#define LEN_NAME 16                               /* Assuming that the length of the filename \
                             won't exceed 16 bytes*/
#define EVENT_SIZE (sizeof(struct inotify_event)) /*size of one event*/
#define BUF_LEN (MAX_EVENTS * (EVENT_SIZE + LEN_NAME))
#define PORT 8080
/*buffer to store the data of events*/


int fd, wd;

#define TERMINATION ".seq"

void sig_handler(int sig)
{

    /* Step 5. Remove the watch descriptor and close the inotify instance*/
    inotify_rm_watch(fd, wd);
    close(fd);
    exit(0);
}

int main()
{
    /* Creacion de cosas server */
    int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Forcefully attaching socket to the port 8080
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    valread = read(new_socket, buffer, 1024);
    printf("%s\n", buffer);

    /*Creacion de cosas monitor*/
    char *path_to_be_watched;
    signal(SIGINT, sig_handler);

    path_to_be_watched = "/home/alvaro/7mosemestre/prograAvanzada/dnaSequence/helloworld";

    //Initialize inotify
    fd = inotify_init();

    if (fcntl(fd, F_SETFL, O_NONBLOCK) < 0) // error checking for fcntl
        exit(2);

    //Add watch
    wd = inotify_add_watch(fd, path_to_be_watched, IN_CREATE);

    if (wd == -1)
    {
        printf("Could not watch : %s\n", path_to_be_watched);
    }
    else
    {
        printf("Watching : %s\n", path_to_be_watched);
    }
    
    while (1)
    {

        int i = 0, length;
        char buffer[BUF_LEN];

        /* Step 3. Read buffer*/
        length = read(fd, buffer, BUF_LEN);

        /* Step 4. Process the events which has occurred */
        while (i < length)
        {

            struct inotify_event *event = (struct inotify_event *)&buffer[i];

            if (event->len)
            {
                if (event->mask & IN_CREATE)
                {
                    if (event->mask & IN_ISDIR)
                    {
                        printf("The directory %s was created.\n", event->name);
                    }
                    else
                    {
                        if(strstr(event->name, TERMINATION) != NULL) {
                            printf("The SEQ file %s was created.\n", event->name);
                            send(new_socket, event->name, strlen(event->name), 0);
                            
                        }
                        else{
                            printf("The file %s was created.\n", event->name);
                        }

                    }
                }
            }
            i += EVENT_SIZE + event->len;
        }
    }
}