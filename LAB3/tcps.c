#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

#define BUF_SIZE 2048

void *handleConnection(void *arg) {
    int connfd = *((int *)arg);
    char rbuf[1024];
    char buffer[100];

    while (1) {
        ssize_t rb = read(connfd, rbuf, sizeof(rbuf));
        if (rb <= 0) {
            break; // Connection closed or error occurred
        }

        rbuf[rb] = '\0';
        printf("Client sent: %s\n", rbuf);

        if (access(rbuf, F_OK) == 0) {
            printf("File exists\n");
            FILE *file = fopen(rbuf, "r");

            if (file != NULL) {
                printf("File opened successfully.\n");

                while (fgets(buffer, sizeof(buffer), file) != NULL) {
                    printf("Read from file: %s", buffer);
                    write(connfd, buffer, strlen(buffer));
                }

                printf("\n");
                fclose(file);
            } else {
                write(connfd, "File not found", 20);
                perror("Error opening file");
            }
        } else {
            write(connfd, "File not found", 20);
        }
    }

    close(connfd);
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <port #> \n", argv[0]);
        exit(0);
    }

    int sockfd, connfd, sin_size;
    char rbuf[1024];
    struct sockaddr_in servAddr, clienAddr;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Failure to setup an endpoint socket");
        exit(1);
    }

    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(atoi(argv[1]));
    servAddr.sin_addr.s_addr = INADDR_ANY;

    if ((bind(sockfd, (struct sockaddr *)&servAddr, sizeof(struct sockaddr))) < 0) {
        perror("Failure to bind server address to the endpoint socket");
        exit(1);
    }

    printf("Server waiting for client at port %d\n", atoi(argv[1]));

    listen(sockfd, 5);
    sin_size = sizeof(struct sockaddr_in);

    while (1) {
        if ((connfd = accept(sockfd, (struct sockaddr *)&clienAddr, (socklen_t *)&sin_size)) < 0) {
            perror("Failure to accept connection to the client");
            exit(1);
        }

        pthread_t tid;
        if (pthread_create(&tid, NULL, handleConnection, &connfd) != 0) {
            perror("Failed to create thread for client connection");
            close(connfd);
        } else {
            pthread_detach(tid);
        }
    }

    close(sockfd);
    return 0;
}

