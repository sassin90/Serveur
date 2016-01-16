#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <time.h> 

void attends(void);
char* date_actuel(void);

int main (int argc, char ** argv){
	// Vérification de la syntaxe
	if (argc < 3 || argc > 8 ){
		fprintf (stderr, "Aide : %s [-d] port service [port2 service2]  [port3 service3]\n", argv [0]);
		exit (EXIT_FAILURE);
	}

	atexit(attends);
	int i=0;
	// éxecution en arr. plan (daemon)
	if(strcmp(argv[1],"-d")==0) { daemon(1,0); i++;}
	// protocol TCP/IP
	struct protoent* pro = getprotobyname ("tcp");

	// démarage des services
	int pid_svc_1 = fork();
	if( pid_svc_1 == -1) perror ("fork");
	if( pid_svc_1 == 0 ){
		int pid_svc_2 = fork();
		if( pid_svc_2 == -1) perror ("fork");
		if( pid_svc_2 == 0 ){
			// service 1
			printf("Démarage de service %s sur le  port %s...\n",argv[i+2],argv[i+1]);
			int sck, port;
			sck = socket(AF_INET, SOCK_STREAM, pro->p_proto); // Création de la socketa
			if (sck == -1) 	  perror ("socket");
			char * lafin;
			long lport = strtol (argv[i+1], &lafin, 0);      // Convertion du port (chaine de char) vers  long
			if (*lafin == 0){
				if (lport < 0 || lport > 65535)  error ("Numero de port doit être compris entre 0 et 65535");
				port = htons (lport);	
			}
			// Parametrage de socketa
			struct sockaddr_in sck_addr;
			memset (&sck_addr, 0, sizeof (sck_addr));
			sck_addr.sin_family = AF_INET;		// pour TCP/IP
			sck_addr.sin_port = port;
			// association des params à la socket
			if (bind (sck, (struct sockaddr *) &sck_addr, sizeof (sck_addr)) != 0) 	perror ("bind");
			// mettre la socket en état d'écoute			
			if (listen (sck, 128) == -1)	perror ("listen");			
			atexit (attends); // gestion des processus
			while (1){
				struct sockaddr_in sck_addr_clt;
				int longeur = sizeof (sck_addr_clt);
				// connection avec client
				int sck_clt = accept (sck, (struct sockaddr*)&sck_addr_clt, &longeur);
				if (sck_clt == -1)	perror ("accept");
				// on ouvre le fichier log;
				FILE* ptrlog;
				ptrlog = fopen("logs.txt","a+");

				fprintf(ptrlog,"%s\t",date_actuel());
				fprintf(ptrlog,"le client %s",inet_ntoa(sck_addr_clt.sin_addr));
				fprintf(ptrlog,":%d\t",htons(sck_addr_clt.sin_port));
				fprintf(ptrlog,"est connecté sur %ld de service %s\n",lport,argv[i+2]);
				
					
				
				int pid = fork (); // serveur multiclients : novo clt => novo processus
				if (pid == -1) perror ("fork");
			 	if (pid == 0){
					if ((sck_clt != 0 && dup2 (sck_clt, 0) == -1))	error ("dup2");
					if ((sck_clt != 1 && dup2 (sck_clt, 1) == -1))	error ("dup2");
					if ( sck_clt != 0 && sck_clt != 1)	close (sck_clt);
					close (2);
					execve (argv[i+2],NULL, __environ); // exécution de la service 
					perror ("execve");
				}
				close (sck_clt);
				fclose(ptrlog);
			}
		}
		else{ // service 2 
			if(argc >= i+5) {
				printf("Démarage de service %s sur le port %s...\n",argv[i+4],argv[i+3]);
				int sck, port;
				sck = socket(AF_INET, SOCK_STREAM, pro->p_proto);
				if (sck == -1) 	  perror ("socket");
				char * lafin;
				long lport = strtol (argv[i+3], &lafin, 0);
				if (*lafin == 0){
					if (lport < 0 || lport > 65535)  error ("Le numero de port doit être compris entre 0 et 65535");
					port = htons (lport);	
				}
				struct sockaddr_in sck_addr;
				memset (&sck_addr, 0, sizeof (sck_addr));
				sck_addr.sin_family = AF_INET;
				sck_addr.sin_port = port;
				if (bind (sck, (struct sockaddr *) &sck_addr, sizeof (sck_addr)) != 0) 	perror ("bind");
				if (listen (sck, 128) == -1)	perror ("listen");
				atexit (attends);
				while (1){
				  	struct sockaddr_in sck_addr_clt;
					int longeur = sizeof (sck_addr_clt);
					int sck_clt = accept (sck, (struct sockaddr*)&sck_addr_clt, &longeur);
					int pid;
					if (sck_clt == -1)	perror ("accept");
					// on ouvre le fichier log;
					FILE* ptrlog;
					ptrlog = fopen("logs.txt","a+");

					fprintf(ptrlog,"%s\t",date_actuel());
					fprintf(ptrlog,"le client %s",inet_ntoa(sck_addr_clt.sin_addr));
					fprintf(ptrlog,":%d\t",htons(sck_addr_clt.sin_port));
					fprintf(ptrlog,"est connecté sur %ld de service %s\n",lport,argv[i+4]);
				
					
					waitpid (-1, 0, WNOHANG);
					pid = fork ();
					if (pid == -1) perror ("fork");
					if (pid == 0){
						if ((sck_clt != 0 && dup2 (sck_clt, 0) == -1))	error ("dup2");
						if ((sck_clt != 1 && dup2 (sck_clt, 1) == -1))	error ("dup2");
						if (sck_clt != 0 && sck_clt != 1)	close (sck_clt);
						close (2);
						execve (argv[i+4],NULL, __environ);
						perror ("execve");
					}
					close (sck_clt);
					fclose(ptrlog);
				}
			}	
		}	
	}	
	else {// service 3 
		atexit (attends);
		if(argc == i+7) {
			printf("Démarage de service %s sur le port %s...\n",argv[i+6],argv[i+5]);
			int sck, port;
			sck = socket(AF_INET, SOCK_STREAM, pro->p_proto);
			if (sck == -1) 	  perror ("socket");
			char * lafin;
			long lport = strtol (argv[i+5], &lafin, 0);
			if (*lafin == 0){
				if (lport < 0 || lport > 65535)  error ("Le numero de port doit être compris entre 0 et 65535");
				port = htons (lport);	
			}
			struct sockaddr_in sck_addr;
			memset (&sck_addr, 0, sizeof (sck_addr));
			sck_addr.sin_family = AF_INET;
			sck_addr.sin_port = port;
			if (bind (sck, (struct sockaddr *) &sck_addr, sizeof (sck_addr)) != 0) 	perror ("bind");
			if (listen (sck, 128) == -1)	perror ("listen");
			atexit (attends);
			while (1){
			  	struct sockaddr_in sck_addr_clt;
			  	int longeur = sizeof (sck_addr_clt);
			  	int sck_clt = accept (sck, (struct sockaddr*)&sck_addr_clt, &longeur);
			  	if (sck_clt == -1)	perror ("accept");
				// on ouvre le fichier log;
					FILE* ptrlog;
					ptrlog = fopen("logs.txt","a+");

					fprintf(ptrlog,"%s\t",date_actuel());
					fprintf(ptrlog,"le client %s",inet_ntoa(sck_addr_clt.sin_addr));
					fprintf(ptrlog,":%d\t",htons(sck_addr_clt.sin_port));
					fprintf(ptrlog,"est connecté sur %ld de service %s\n",lport,argv[i+4]);
				
			  	atexit (attends);
			  	int pid = fork ();
			  	if (pid == -1) perror ("fork");
			  	if (pid == 0){
		  			if ((sck_clt != 0 && dup2 (sck_clt, 0) == -1))	error ("dup2");
			  		if ((sck_clt != 1 && dup2 (sck_clt, 1) == -1))	error ("dup2");
				  	if ( sck_clt != 0 && sck_clt != 1)	close (sck_clt);
				  	close (2);
				  	execve (argv[i+6], NULL, __environ);
				  	perror ("execve");
				}
				close (sck_clt);
				fclose(ptrlog);
			}
		}
		else if(i==0) while(1); // pour que le ps pére ne s'arrête pas avant ces fils 
	}// end of service  3
	return EXIT_SUCCESS;
}
// les fonctions
void attends(void){ waitpid (-1, 0, WNOHANG); }

char* date_actuel(void){
    char *str = (char*) malloc(19*sizeof(char));
    time_t actuel;
    struct tm * f;
    time ( &actuel );
    f = localtime ( &actuel );
    sprintf(str,"%d/%.2d/%.2d %d:%d:%d", f->tm_mday,1+f->tm_mon, 1900 + f->tm_year, f->tm_hour, f->tm_min, f->tm_sec);
    return str;
}
