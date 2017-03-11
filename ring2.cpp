/* 
 * File:   ring2.cpp
 * Author: Michael
 *
 * Created on October 27, 2013, 5:41 PM
 */

#include <stdlib.h>
#include <errno.h>
#include <strings.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <cstring>

void error(const char *msg) {
    perror(msg);
    exit(0);
}

void client(const char *ip, char *buffer){
    int sockfd, n;
    struct sockaddr_in serv_addr;

    // Get the port number the server will be listening to
    int portNo = 82;
    // Create a SOCKET object called sockfd.
    // Call the socket function and return its value to the m_socket variable.
    // For this application, use the Internet address family, streaming sockets, and
    // the TCP/IP protocol.
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // Check for general error
    if (sockfd < 0) {
        error("ERROR opening socket");
    }

    bzero((char *) &serv_addr, sizeof (serv_addr));

    // Set Internet address family to AF_INET.
    serv_addr.sin_family = AF_INET;
    // Set the server side port number
    serv_addr.sin_port = htons(portNo);
    // bind the server side ip address (the ip address is passed in as a parameter)
    inet_pton(AF_INET, ip, &serv_addr.sin_addr);

    // Establish a connection between the client and server (pass in the socket, server address and length of the address)
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0) {
        error("ERROR connecting");
    }

    //char message[] = "Hello from Ring 2";
    // Send the message out to the server without waiting for reply
    n = send(sockfd, buffer, strlen(buffer), 0);
    // Check for general error
    if (n < 0) {
        error("ERROR writing to socket");
    }
    
    // close the socket
    close(sockfd);
}

char *server() {
    int sockfd, newsockfd, clilen;
    char* buffer = new char[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;

    // Create a SOCKET object called sockfd.
    // Call the socket function and return its value to the m_socket variable.
    // For this application, use the Internet address family, streaming sockets, and
    // the TCP/IP protocol.
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        error("ERROR opening socket");
    }

    bzero((char *) &serv_addr, sizeof (serv_addr));

    // Set the port number the server will be listening to 
    int portnNo = 81;
    // Set Internet address family to AF_INET.
    serv_addr.sin_family = AF_INET;
    // Set the IP address the server is bind to (the ip address of your computer,
    // which will be passed in as an parameter for client side code)
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    // Set the port number to which the socket will be bound to.
    serv_addr.sin_port = htons(portnNo);

    // Call the bind function, passing the created socket and the sockaddr_in structure as parameters.
    // Check for general errors.
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0) {
        error("ERROR on binding");
    }

    // Call the listen function, passing the created socket and the maximum number of allowed
    // connections to accept as parameters. Check for general errors.
    if (listen(sockfd, 10) < 0) {
        perror("listen");
        exit(1);
    }

    clilen = sizeof (cli_addr);

    // accept function takes the first connection request on the queue and creates another socket
    // with the same properties as sockfd
    newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

    // Check for general errors.
    if (newsockfd < 0) {
        error("ERROR on accept");
    }

    // close socket
    close(sockfd);

    // Set the value of buffer to zero
    bzero(buffer, 256);

    // Get the message from the Client. Check for general errors
    n = read(newsockfd, buffer, 255);
    if (n < 0) {
        error("ERROR reading from socket");
    }

    printf("get the message: %s from ring 1\n", buffer);
    // close socket
    close(newsockfd);
    return buffer;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "usage %s IP address\n", argv[1]);
        exit(0);
    }
    char *buffer;
    int i;
    for(i=0; i<10; i++)
    {
        buffer = server();
        usleep(100*1000);//gives the program time to close the address
        client(argv[1], buffer);
    }
    return 0;
}

