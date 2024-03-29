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

/* Constants */
#define BUFFER_SIZE 1000
#define PORT_NUMBER 7800
#define HOST_NAME "172.26.37.44"
#define USER_NAME "dongweiw\n"
#define G_VALUE 15
#define P_VALUE 97
#define COMMAND "openssl sha256 dh.c"

/* Function Prototype */
int mod_func(int g, int exp, int p);



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
    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0)
    {
        perror("ERROR writing to socket");
        exit(0);
    }

    /* Read b value */
    FILE *fp;
    fp = popen(COMMAND, "r");
    char dh_buffer[BUFFER_SIZE];
    fgets(dh_buffer, BUFFER_SIZE, fp);

    /* Get the first two hexadecimal digits */
    char hex_value[BUFFER_SIZE];
    char b_in_hex[3];
    b_in_hex[2] = '\0';
    strcpy(hex_value, strstr(dh_buffer, "= ") + 2);
    strncpy(b_in_hex, hex_value, 2);

    /* Convert first two hexadecimal digits to an integer */
    int b = strtol(b_in_hex, NULL, 16);

    /* Compute g^b mod p */
    int g = G_VALUE;
    int p = P_VALUE;
    int gbmodp = mod_func(g, b, p);
    char gbmodp_str[10];
    sprintf(gbmodp_str, "%d\n", gbmodp);

    /* Send g^b mod p */
    n = write(sockfd, gbmodp_str, strlen(gbmodp_str));
    if (n < 0)
    {
        perror("ERROR writing to socket");
        exit(0);
    }

    n = read(sockfd, buffer, 255);
    if (n < 0)
    {
      perror("ERROR reading from socket");
      exit(EXIT_FAILURE);
    }

    /* Receive value g^a mod p */
    int gamodp = atoi(buffer);
    //printf("Received g^a mod p is %d\n", gamodp);

    /* Calculate g^ab mod p */
    int gbamodp = mod_func(gamodp, b, p);
    char gbamodp_str[10];
    sprintf(gbamodp_str, "%d\n", gbamodp);
    buffer[n] = 0;

    /* Send g^ab mod p */
    n = write(sockfd, gbamodp_str, strlen(gbamodp_str));
    if (n < 0)
    {
      perror("ERROR writing to socket");
      exit(EXIT_FAILURE);
    }

    /* if succeeded */
    n = read(sockfd, buffer, 255);
    if (n < 0)
    {
      perror("ERROR reading from socket");
      exit(EXIT_FAILURE);
    }

    printf("The result from server: %s\n", buffer);

    close(sockfd);

    return 0;
}



// Calculate g ^ exp mod p
int mod_func(int g, int exp, int p) {
	int r;
	int result = 1;

	while (exp > 0) {
		r = exp % 2;

		if (r == 1){
			result = (result * g) % p;
		}
		g = g * g % p;

		exp = exp / 2;
	}
	return result;
}
