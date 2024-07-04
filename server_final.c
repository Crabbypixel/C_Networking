/*
 * Project - Server Program to send and receive messages
 * Socket Programming using threads
 * 
 * Date: 21-12-2023
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <pthread.h>
#include "server_socket.h"

#define EXIT_VALUE 4                                                            // Default exit value for functions
#define MAX_LINKS 2                                                             // Maximum clients connecting to the server

void* deal_with_clients(void* param);                                           // Thread function to comm. with the client

typedef struct {                                                                // Client info to be passed to the thread
    int client_socket;
    int* shared_mem;
} client_info;

int main(int argc, char** argv)
{
    int port;                                                                   // Internet port for server client communications
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <port>\nStarting on default port\n", argv[0]);
        port = 32140;
    } else {
        port = atoi(argv[1]);
        if(port < 30000 || port > 40000) {
            fprintf(stderr, "Enter a port number between 30000 to 40000\nStarting on default port\n");
            port = 32140;
        }
    }

    int server_socket;                                                          // Server socket
    int con = 0;                                                                // Connections to the server
    int* shared_mem = (int*)malloc(sizeof(int));                                // Memory shared flag among all running threads
    *shared_mem = 0;                                                            // to stop when it is equal to EXIT_VALUE

    server_socket = open_listener_socket();                                     // Open listener for server socket
    bind_server_socket(server_socket, port);                                    // Bind socket to port

    start_listening(server_socket, MAX_LINKS);                                  // Start listening to clients
    printf("Server open on port: %d\nWaiting for connections...\n", port);

    pthread_t threads[MAX_LINKS];

    while(1) {
        int client_socket = accept_client_connection(server_socket);            // Accept connection from client
        
        con++;
        client_info clients[MAX_LINKS];                                         // Create struct array to store client info
        clients[con].client_socket = client_socket;
        clients[con].shared_mem = shared_mem;

        // Create thread and send client info
        if(pthread_create(&threads[con], NULL, deal_with_clients, (void*)&clients[con])) {
            error("Cannot create thread!");
            close(client_socket);
            close(server_socket);
            continue;
        }

        if(*shared_mem == EXIT_VALUE) {                                         // Exit if memory value is equal to EXIT_VALUE    
            close(client_socket);
            puts("\nClosing server...");
            free(shared_mem);
            exit(0);
        }

        /*// debug code
        printf("Con value: %d\n", con);
        if(con == MAX_LINKS-1) {
            con = 0;
            pthread_cancel(threads[con]);
        }
        */
    }
}

void* deal_with_clients(void* param)
{
    client_info *client = (client_info*)param;                                  // Create a structure pointer
    int client_socket = client->client_socket;                                  // Store client socket value from given parameter

    while(1) {
        char buf[32];                                                           // Data to send to the client

        int parse_value = parse_message(client_socket, buf, sizeof(buf));       // Get data from client
        if(parse_value == -1) {
            fprintf(stderr, "Error: Failed to parse the string, error code: %d\n", strerror(errno));
            return NULL;
        }

        printf("Buffered Data (len: %d, size: %d, socket: %d): %s", strlen(buf), sizeof(buf), client_socket, buf);

        // If data received is "stop", send "stop" to the client to close the client and server
        if(strcmp(buf, "stop\n") == 0) {
            send_message(client_socket, "stop");
            *(client->shared_mem) = EXIT_VALUE;
            break;
        } else {
            send_message(client_socket, "sent successfully");
        }
    }
    return NULL;
}