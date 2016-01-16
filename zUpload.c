#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define TAILLE 1024

int main(int argc, char* argv[]){

	char buffer[TAILLE],nomf[20],dir[20]="Fichiers Trans/";
	FILE * ptrf;
	ssize_t taille;
	if (recv(0,nomf,12,0) < 0){
		perror("Reception du nom du fichier échoué!");
		exit(-1);
	}
	send(1,"OK",3,0);
	if (nomf == NULL){
		perror("Le nom de fichier est invalide");
		exit(-1);
	}
	ptrf = fopen(strcat(dir,nomf),"w+");
		while((taille = recv(0,buffer,TAILLE,0)))
			fwrite(buffer,taille,1,ptrf);
  	printf("Réception du fichier avec succée\n");
	fclose(ptrf);
	return 0;
}
