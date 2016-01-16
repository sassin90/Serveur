#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>

#define SIZE 1024


void init_sockaddr (struct sockaddr_in *name, const char *hostname, uint16_t port)
{
  struct hostent *hostinfo; 
  
  name->sin_family = AF_INET;   /* Adresses IPV4 (Internet) */
  name->sin_port = htons (port); /* On gère le little/big Endian */
  
  hostinfo = gethostbyname (hostname); /* appeler les fonctions de résolution de nom de la libC */
  
  if (hostinfo == NULL) /* Si ne peut transformer le nom de la machine en adresse IP */
    {
      fprintf (stderr, "Unknown host %s.\n", hostname);
      exit (-1);
    }
  name->sin_addr = *(struct in_addr *) hostinfo->h_addr_list[0]; /* Le dernier champs de la structure est garni */
}

char* expld(char* str){
int i=0,l=0,k=0;
for(i=0;i<strlen(str);i++) if(str[i]== '/')k++;
if(k==0) return str;
for(i=0;i<strlen(str);i++){
if(str[i]== '/') l++;
if(l==k) return str+i+1;}
}


int main(int argc, char* argv[])
{
  
  int ds, port,size, fich;
  char nomServeur[SIZE], nomFichier[SIZE],buffer[SIZE];
  struct sockaddr_in name;
  
  if(argc != 4) /* Test Le nombre d'arguments */
    {
      fprintf(stderr,"Usage : ./client nomServeur port nomFichier \n"); /*Message d'erreur si nb d'arguments invalide*/
      exit(0);
    }
  
  strcpy(nomServeur,argv[1]); /*Copie l'argument 1 dans nomServeur*/
  strcpy(nomFichier,argv[3]); /*Et l'argument 3 est le nom du fichier a transmettre*/

  port = atoi(argv[2]); /*L'argument 2 quant à lieu et le numero de port sur lequel le serveur ecoute*/

  if( (fich = open(nomFichier,O_RDONLY)) == -1) /*On ouvre (en binaire) le fichier nomFich en lecture seule*/
    {  						/*Avec un test pour éviter les erreurs*/
      fprintf(stderr, "Fichier inexistant\n");
      exit(1);
    }

  ds = socket(PF_INET, SOCK_STREAM, 0); /*On crée notre socket de de data : ds*/

  if(ds < 0)
  {				/*Avec un test pour éviter les erreurs*/
      perror("Probleme socket : ");
      exit(-1);
  }

  init_sockaddr(&name,nomServeur,port); /*Initialise notre socket internet : name*/

  if (connect(ds, (struct sockaddr*) &name, sizeof(struct sockaddr)) < 0) /*On se connecte à notre serveur*/
    {									  /*Avec un test pour éviter les erreurs*/	
      perror("Problème connect");
      exit(-2);
    }
 					
  if( send(ds,expld(nomFichier), strlen(expld(nomFichier)) + 1,0) <0 )	/*...ecrire dans la socket...*/
    {							/*...et on envoi le nom du fichier*/
      perror("Le nom du fichier n'a pas été envoyé");	/*Avec un test pour éviter les erreurs*/
      exit(-3);
    }
if(recv(ds,buffer,3,0)>0) {
   puts("Transmission en cours...");			

  while((size=read(fich,buffer,SIZE)))  send(ds,buffer,size,0); 	/*Tant qu'on est pas à la fin du fichier...*/
    			}					/*...on lit dans le fichier et on l'envoi dans la socket*/
  puts("Fichier transmis !");
  close(ds);			/*On ferme notre socket*/
  close(fich);			/*Et notre fichier*/

  return 0;
  
}


