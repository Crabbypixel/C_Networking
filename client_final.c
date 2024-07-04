/*
 * Project - Client Program to send and receive messages
 * Socket Programming using threads
 * 
 * Date: 21-12-2023
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "client_socket.h"

int main(int argc, char** argv)
{
    if(argc != 3) {
        fprintf(stderr, "Usage: %s <hostname> <port>\n", argv[0]);
        return 0;
    }

    const char* ip_addr = argv[1];
    int port = atoi(argv[2]);
    
    char buf[32];                                                           // Data buffer
    char response[32];                                                      // To store response from server       

    int socket = open_socket();                                             // Socket to communicate with the server
    connect_socket(ip_addr, socket, port);                                  // Connect socket to port     
    
    while(1) {
        printf("Enter: ");
        fgets(buf, 32, stdin);

        send_message(socket, buf);                                          // Send data to server
        parse_message(socket, response, sizeof(response));                  // Get response

        if(!(strcmp(response, "stop")) || !(strcmp(buf, "close\n"))) {      // If message sent is "stop", then close the client
            puts("Stopped\n");
            connect_socket(ip_addr, open_socket(), port);                  // To refresh socket stream
            close(socket);
            exit(0);
        }
    }

    close(socket);                                                          // Close the socket
    return 0;
}