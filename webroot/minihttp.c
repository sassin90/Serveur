#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>


#define EOL "\r\n" // End-of-line byte sequence
#define EOL_SIZE 2





#define PORT 80 // The port users will be connecting to
#define WEBROOT "./webroot" // The webserver's root directory
void handle_connection(int, struct sockaddr_in *); // Handle web requests
int get_file_size(int); // Returns the filesize of open file descriptor

int send_string(int sockfd, unsigned char *buffer) {
int sent_bytes, bytes_to_send;
bytes_to_send = strlen(buffer);
while(bytes_to_send > 0) {
sent_bytes = send(sockfd, buffer, bytes_to_send, 0);
if(sent_bytes == -1)
return 0; 
bytes_to_send -= sent_bytes;
buffer += sent_bytes;
}
return 1; 
}

int recv_line(int sockfd, unsigned char *dest_buffer) {

unsigned char *ptr;
int eol_matched = 0;
ptr = dest_buffer;
while(recv(sockfd, ptr, 1, 0) == 1) { // Read a single byte.
if(*ptr == EOL[eol_matched]) { // Does this byte match terminator?
eol_matched++;
if(eol_matched == EOL_SIZE) { // If all bytes match terminator,
*(ptr+1-EOL_SIZE) = '\0'; // terminate the string.
return strlen(dest_buffer); // Return bytes received
}
} else {
eol_matched = 0;
}
ptr++; // Increment the pointer to the next byter.
}
return 0; // Didn't find the end-of-line characters.
}


int main(void) {
int sockfd, new_sockfd, yes=1;
struct sockaddr_in host_addr, client_addr; // My address information
socklen_t sin_size;
printf("Accepting web requests on port %d\n", PORT);
if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1)
printf("in socket");
if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
printf("setting socket option SO_REUSEADDR");
host_addr.sin_family = AF_INET; // Host byte order
host_addr.sin_port = htons(PORT); // Short, network byte order
host_addr.sin_addr.s_addr = INADDR_ANY; // Automatically fill with my IP.
memset(&(host_addr.sin_zero), '\0', 8); // Zero the rest of the struct.
if (bind(sockfd, (struct sockaddr *)&host_addr, sizeof(struct sockaddr)) == -1)
printf("binding to socket");
if (listen(sockfd, 20) == -1)
printf("listening on socket");
while(1) { // Accept loop.
sin_size = sizeof(struct sockaddr_in);
new_sockfd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);
if(new_sockfd == -1)
printf("accepting connection");
handle_connection(new_sockfd, &client_addr);
}
return 0;
}


void handle_connection(int sockfd, struct sockaddr_in *client_addr_ptr) {
unsigned char *ptr, request[500], resource[500];
int fd, length;
length = recv_line(sockfd, request);
printf("Got request from %s:%d \"%s\"\n", inet_ntoa(client_addr_ptr->sin_addr),
ntohs(client_addr_ptr->sin_port), request);
ptr = strstr(request, " HTTP/"); // Search for valid-looking request.
if(ptr == NULL) { // Then this isn't valid HTTP.
printf(" NOT HTTP!\n");
} else {
*ptr = 0; // Terminate the buffer at the end of the URL.
ptr = NULL; // Set ptr to NULL (used to flag for an invalid request).
if(strncmp(request, "GET ", 4) == 0) // GET request
ptr = request+4; // ptr is the URL.
if(strncmp(request, "HEAD ", 5) == 0) // HEAD request
ptr = request+5; // ptr is the URL.
if(ptr == NULL) { // Then this is not a recognized request.
printf("\tUNKNOWN REQUEST!\n");
} else { // Valid request, with ptr pointing to the resource name
if (ptr[strlen(ptr) - 1] == '/') // For resources ending with '/',
strcat(ptr, "index.html"); // add 'index.html' to the end.
strcpy(resource, WEBROOT); // Begin resource with web root path
strcat(resource, ptr); // and join it with resource path.
fd = open(resource, O_RDONLY, 0); // Try to open the file.
printf("\tOpening \'%s\'\t", resource);
if(fd == -1) { // If file is not found
printf(" 404 Not Found\n");
send_string(sockfd, "HTTP/1.0 404 NOT FOUND\r\n");
send_string(sockfd, "Server: Tiny webserver\r\n\r\n");
send_string(sockfd, "<html><head><title>404 Not Found</title></head>");
send_string(sockfd, "<body><h1>URL not found</h1></body></html>\r\n");
} else { // Otherwise, serve up the file.
printf(" 200 OK\n");
send_string(sockfd, "HTTP/1.0 200 OK\r\n");
send_string(sockfd, "Server: Tiny webserver\r\n\r\n");
if(ptr == request + 4) { // Then this is a GET request
if( (length = get_file_size(fd)) == -1)
printf("getting resource file size");
if( (ptr = (unsigned char *) malloc(length)) == NULL)
printf("allocating memory for reading resource");
read(fd, ptr, length); // Read the file into memory.
send(sockfd, ptr, length, 0); 
free(ptr); // Free file memory.
}
close(fd); // Close the file.
} // End if block for file found/not found.
} // End if block for valid request.
} // End if block for valid HTTP.
shutdown(sockfd, SHUT_RDWR); // Close the socket gracefully.
}
/* This function accepts an open file descriptor and returns
* the size of the associated file. Returns -1 on failure.
*/
int get_file_size(int fd) {
struct stat stat_struct;
if(fstat(fd, &stat_struct) == -1)
return -1;
return (int) stat_struct.st_size;
}
