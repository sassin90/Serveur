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

#define EOL "\r\n"
#define EOL_SIZE 2
#define PORT 80
#define WEBROOT "./webroot"

int taille_fichier(int);
int envoyer_chaine(int, char*);
int recv_ligne(int, char*);

int main(void) {
    char *ptr, req[500], resource[500];
    int fd, langeur;
    langeur = recv_ligne(0, req);
    ptr = strstr(req, " HTTP/");
    if(ptr == NULL) perror("HTTP");
    else {
    *ptr = 0;
    ptr = NULL;
    if(strncmp(req, "GET ", 4) == 0)
    ptr = req+4;
    if(strncmp(req, "HEAD ", 5) == 0)
    ptr = req+5;
    if(ptr == NULL) printf("\trequete inconnue !\n");
    else {
    if (ptr[strlen(ptr) - 1] == '/')
    strcat(ptr, "index.html");
    strcpy(resource, WEBROOT);
    strcat(resource, ptr);
    fd = open(resource, O_RDONLY, 0);
    if(fd == -1) {
    envoyer_chaine(1, "HTTP/1.0 404 NOT FOUND\r\n");
    envoyer_chaine(1, "Server: Zorgos webserver\r\n\r\n");
    envoyer_chaine(1, "<html><head><title>404 Not Found</title></head>");
    envoyer_chaine(1, "<body><h1>URL not found</h1></body></html>\r\n");
    }
    else {
    envoyer_chaine(1, "HTTP/1.0 200 OK\r\n");
    envoyer_chaine(1, "Server: Zorgos webserver\r\n\r\n");
    if(ptr == req + 4) {
    langeur = taille_fichier(fd);
    ptr = (char *) malloc(langeur);
    read(fd, ptr, langeur);
    send(1, ptr, langeur, 0);
    free(ptr);
    }
    close(fd);
    }
    }
    }

    return 0;
}

// fonctions
int envoyer_chaine(int sockfd, char *buffer) {
    int sent_bytes, bytes_to_send;
    bytes_to_send = strlen(buffer);
    while(bytes_to_send > 0) {
        sent_bytes = send(sockfd, buffer, bytes_to_send, 0);
        if(sent_bytes == -1) return 0;
        bytes_to_send -= sent_bytes;
        buffer += sent_bytes;
    }
    return 1;
}

int recv_ligne(int sockfd, char *dest_buffer) {
    char *ptr;
    int eol_matched = 0;
    ptr = dest_buffer;
    while(recv(sockfd, ptr, 1, 0) == 1) {
        if(*ptr == EOL[eol_matched]) {
            eol_matched++;
            if(eol_matched == EOL_SIZE) {
                *(ptr+1-EOL_SIZE) = '\0';
                return strlen(dest_buffer);
            }
        }
        else eol_matched = 0;
        ptr++;
    }
    return 0;
}
int taille_fichier(int fd) {
    struct stat stat_struct;
    if(fstat(fd, &stat_struct) == -1)
        return -1;
    return (int) stat_struct.st_size;
}
