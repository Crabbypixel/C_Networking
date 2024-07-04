#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>

void error(const char* err_msg)
{
    fprintf(stderr, "Error: %s", err_msg);
    exit(1);
}

int open_socket()
{
    /* This function creates a TCP listener socket and returns it */

    int s = socket(PF_INET, SOCK_STREAM, 0);
    if(s == -1)
        error("Cannot open socket");
    return s;
}

void connect_socket(const char* ip_address, int socket, int port)
{
    /*
    * This function connects a port to the newly opened socket
    * Checks whether if the port is open or usable and prints
    * an appropriate message otherwise.
    */
   
    struct sockaddr_in si;
    si.sin_family = PF_INET;
    si.sin_addr.s_addr = inet_addr(ip_address);
    si.sin_port = htons(port);
    if(connect(socket, (struct sockaddr*)&si, sizeof(si)) == -1) {
        close(socket);
        error("Cannot connect to the server, server closed or network errors.");
    }
}

int send_message(int socket, char* msg)
{
    /*
    * This function sends the given message to the
    * client and returns a value (-1 if failed).
    */

    int result = send(socket, msg, strlen(msg)+1, 0);
    if(result == -1)
        fprintf(stderr, "%s: %d\n", "Error talking to the server", strerror(errno));
    // printf("Entered string: %s\n", msg);
    return result;
}

int parse_message(int socket, char* buf, int len) {
    /*
    * This function below reads data sent from the client.
    * Uses recv() library function to read data.
    * recv():
    * - The characters are not terminated with '\0'
    * - String always terminates with '\r\n'
    * - Returns number of characters parsed, -1 if an error occurs
    *   0 if client closed the connection
    */
    
    int parse_value = recv(socket, buf, len, 0);
    if(parse_value == -1) {
        close(socket);
        error("Failed to parse the string");
    }

    return parse_value;
}