#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>

void error(const char* err_msg)
{
    fprintf(stderr, "Error: %s\n", err_msg);
    exit(1);
}

int open_listener_socket()
{
    /*
    * This function opens a TCP socket (similar to a data stream).
    * Before a server can use a socket to talk to a client program,
    * it goes through 4 stages: Bind, Listen, Accept and Begin
    */

    int listener_socket = socket(AF_INET, SOCK_STREAM, 0);              // Create a TCP socket
    if(listener_socket == -1)
        error("Cannot open socket");
    return listener_socket;
}

void bind_server_socket(int listener_socket, int port)
{
    /*
    * This function connects a port to the newly opened socket
    * Checks whether if the port is open or usable and prints
    * an appropriate message otherwise.
    */

    struct sockaddr_in socket_info;                                     // Store server/client socket information
    socket_info.sin_family = AF_INET;                                   // Set socket name
    socket_info.sin_port = htons(port);                                 // Set socket port
    socket_info.sin_addr.s_addr = INADDR_ANY;                           // Set a random address for the socket

    int reuse_value = 1;                                                // Reuse port if possible
    if(setsockopt(listener_socket, SOL_SOCKET, SO_REUSEADDR, (char*)&reuse_value, sizeof(int)) == -1)
        error("Cannot reuse port.");

    int bind_value = bind(listener_socket, (struct sockaddr*)&socket_info, sizeof(socket_info));
    if(bind_value == -1)                                
        error("Cannot bind the port to socket");
}

int start_listening(int socket, int max_connections)
{
    /*
    * This function makes the socket to listen for
    * any upcoming clients to connect. Returns 0
    * if successful.
    */
   
    int listen_value = listen(socket, max_connections);
    if(listen_value != 0) {                                
        error("Cannot listen to the port");
    }
    return listen_value;
}

int accept_client_connection(int server_socket)
{
    /*
    * This function is called when a socket has a port
    * open and actively listening to clients.
    * It returns a new socket for the client
    * when a client connects to the server.
    */

    struct sockaddr_in client;
    int address_size = sizeof(client);

    int client_socket = accept(server_socket, (struct sockaddr*)&client, &address_size);
    if(client_socket == -1)
        error("Failed to accept client connection");
    
    return client_socket;
}

int parse_message(int client_socket, char* buf, int len) {
    /*
    * This function below reads data sent from the client.
    * Uses recv() library function to read data.
    * recv():
    * - The characters are not terminated with '\0'
    * - String always terminates with '\r\n'
    * - Returns number of characters parsed, -1 if an error occurs
    *   0 if client closed the connection
    */
    
    int parse_value = recv(client_socket, buf, len, 0);
    return parse_value;
}

int send_message(int socket, char* message)
{
    /*
    * This function sends the given message to the
    * client and returns a value (-1 if failed).
    */

    int send_result = send(socket, message, strlen(message)+1, 0);
    if(send_result == -1)
        error("Cannot establish connection to send messages");
    return send_result;
}