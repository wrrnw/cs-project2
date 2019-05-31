/* The skelon code retrived from lab-5 client.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <math.h>

#define BUFFER_SIZE 10000
#define PORT_NUMBER 7800
#define HOST_NAME "172.26.36.44"
#define USER_NAME "dongweiw\n"
#define G_VALUE 9
#define P_VALUE 23 //Just pick a small prime number for practice purpose

int main(int argc, char ** argv)
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent * server;

    char buffer[BUFFER_SIZE];

    portno = PORT_NUMBER;


    /* Translate host name into peer's IP address ;
     * This is name translation service by the operating system
     */
    server = gethostbyname(HOST_NAME);

    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    /* Building data structures for socket */
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy(server->h_addr_list[0], (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);


    /* Create TCP socket -- active open
     * Preliminary steps: Setup: creation of active open socket
     */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
    {
        perror("ERROR opening socket");
        exit(0);
    }

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR connecting");
        exit(0);
    }



    /* Do processing */
    /* Send username */
    strcpy(buffer, USER_NAME);
    printf("The buffer currently is: %s\n", buffer);

    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0)
    {
        perror("ERROR writing to socket");
        exit(0);
    }



    bzero(buffer, 256);

    n = read(sockfd, buffer, 255);

    if (n < 0)
    {
        perror("ERROR reading from socket");
        exit(0);
    }

    printf("%s\n", buffer);

    return 0;
}
