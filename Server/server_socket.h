int start_listening(int socket, int max_connections);
int open_listener_socket();
void bind_server_socket(int listener_socket, int port);
int accept_client_connection(int server_socket);
int parse_message(int client_socket, char* buf, int len);
int send_message(int socket, char* message);
void error(const char* err_msg);