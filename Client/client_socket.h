int open_socket();
int parse_message(int socket, char* buf, int len);
void connect_socket(const char* ip_address, int socket, int port);
int send_message(int socket, char* msg);
void error(const char* err_msg);