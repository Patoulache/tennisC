#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <bits/types/siginfo_t.h>
int tube [2];
int i = 0;


int zoneEnvoi()
{
  int res = (rand() % 8) + 1;
  if ( ( res == 1 ) || ( res == 2 ) || ( res == 7 ) || ( res == 8 ))
  {
    return -1;
  }
  else
  {
    return res;
  }
}

int zoneReception()
{
  return ((rand() % 4) + 3);
}

void traiterSignal (int signalRecu, siginfo_t* info, void* pasUtileIci)
{

  switch ( signalRecu )
  {
  case SIGUSR1 : printf ("Le joueur2 receptionne le signal SIGUSR1 envoye par le processus %d\n", info->si_pid);

    unsigned long taille = 0;
    int envoi = zoneEnvoi();
    int reception = zoneReception();
    int aToi = -1;
    
    // On lit sizeof(int) octets depuis le tube : c'est la taille du message qui sera lu

    read ( tube[0] , &taille , sizeof (unsigned long) );
    
    printf ("Le joueur2 lit %lu octets depuis le tube (desc=%d) : c'est la taille (%lu) du message\n", sizeof(int), tube[0], taille);

    // On alloue la memoire pour stocker le message

    int* zone = (int*) malloc ( taille );

    if ( zone )
    {
      printf("la zone de reception du joueur2 est : %d\n", reception);
      printf ("Le joueur2 alloue %lu octets\n", taille);

      // On lit le message proprement dit

      read ( tube[0] , zone , taille );
      
      printf ("Le joueur2 lit %lu octets depuis le tube : c'est le message \"%d\"\n", taille, *zone);
      if (*zone == -1)
      {
        unsigned long tailleEnvoi = sizeof (envoi);
        printf("la zone d'envoie du joueur2 est : %d\n", envoi);
        write ( tube[1], &tailleEnvoi, sizeof (unsigned long));
        write ( tube[1], &envoi, tailleEnvoi);
      }
      else if (*zone == reception)
      {
        unsigned long tailleEnvoi = sizeof (envoi);
        printf("la zone d'envoie du joueur2 est : %d\n", envoi);
        write ( tube[1], &tailleEnvoi, sizeof (unsigned long));
        write ( tube[1], &envoi, tailleEnvoi);
      }
      else
      {
        unsigned long tailleEnvoi = sizeof (envoi);
        printf("la zone d'envoie du joueur2 est : %d\n", envoi);
        write ( tube[1], &tailleEnvoi, sizeof (unsigned long));
        write ( tube[1], &aToi, tailleEnvoi);
      }
      
      free (zone);

      kill ( info->si_pid , SIGUSR2 );

      printf ("Le joueur2 envoie SIGUSR2 vers le joueur1 %d\n", info->si_pid);
    }
    else
    {
      perror ("Erreur d'allocation de mémoire\n");
      exit (-3);

      printf ("Le joueur2 ecrit %lu octets dans le tube : c'est le message \"%d\"\n", taille, *zone);                  
    }

    break;

  case SIGUSR2 : printf ("Le joueur1 receptionne le signal SIGUSR2 envoye par le processus %d\n", info->si_pid);

    unsigned long taille2 = 0;
    int envoi2 = zoneEnvoi();
    int reception2 = zoneReception();
    int aToi2 = -1;
    // On lit sizeof(int) octets depuis le tube : c'est la taille du message qui sera lu

    read ( tube[0] , &taille2 , sizeof (unsigned long) );
    
    printf ("Le joueur1 lit %lu octets depuis le tube (desc=%d) : c'est la taille (%lu) du message\n", sizeof(int), tube[0], taille2);

    // On alloue la memoire pour stocker le message

    int* zone2 = (int*) malloc ( taille2 );

    if ( zone2 )
    {
      printf("la zone de reception du joueur1 est : %d\n", reception2);
      printf ("Le joueur1 alloue %lu octets\n", taille2);

      // On lit le message proprement dit

      read ( tube[0] , zone2 , taille2 );
      
      printf ("Le joueur1 lit %lu octets depuis le tube : c'est le message \"%d\"\n", taille2, *zone2);
      if (*zone2 == -1)
      {
        i++;
        unsigned long tailleEnvoi2 = sizeof (envoi2);
        printf("la zone d'envoie du joueur1 est : %d\n", envoi2);
        write ( tube[1], &tailleEnvoi2, sizeof (unsigned long));
        write ( tube[1], &envoi, tailleEnvoi2);
      }
      else if (*zone2 == reception2)
      {
        unsigned long tailleEnvoi2 = sizeof (envoi2);
        printf("la zone d'envoie du joueur1 est : %d\n", envoi2);
        write ( tube[1], &tailleEnvoi2, sizeof (unsigned long));
        write ( tube[1], &envoi2, tailleEnvoi2);
      }
      else
      {
        i++;
        unsigned long tailleEnvoi2 = sizeof (envoi2);
        printf("la zone d'envoie du joueur1 est : %d\n", envoi2);
        write ( tube[1], &tailleEnvoi2, sizeof (unsigned long));
        write ( tube[1], &aToi2, tailleEnvoi2);
      }
      
      free (zone2);

      kill ( info->si_pid , SIGUSR1 );

      printf ("Le joueur1 envoie SIGUSR1 vers le joueur2 %d\n", info->si_pid);
    }
    else
    {
      perror ("Erreur d'allocation de mémoire\n");
      exit (-3);

      printf ("Le joueur1 ecrit %lu octets dans le tube : c'est le message \"%d\"\n", taille2, *zone2);                  
    }

    break;

  case SIGTERM : 
    printf("Beau Match\n");
    exit (0);

  }
}

int main()
{ 
  /***************************************************/
  /* Création de la structure pour gérer les signaux */
  /***************************************************/

  struct sigaction prepaSignal;
                   prepaSignal.sa_sigaction=&traiterSignal;
                   prepaSignal.sa_flags=SA_SIGINFO | SA_RESTART;  // Ne pas oublier l'instruction wait du père

  sigemptyset(&prepaSignal.sa_mask);

  /****************************/
  /* Création du tube anonyme */
  /****************************/

  if ( pipe (tube) != 0 )
  {
    perror ("Erreur dans la création du tube\n");
    exit (-1);
  }

  printf ("Descripteur de lecture %d \n", tube[0]);
  printf ("Descripteur d'écriture %d \n", tube[1]);
  
  int pid = fork();

  srand (time(NULL));
  while (i < 10)
  {
    switch (pid)
    {
    case -1 :
      perror ("Erreur dans la creation du processus fils\n");
      exit(-2);
    
    case 0 :
      printf ("Joueur2 : PID=%d\n", getpid());

      sigaction (SIGUSR1, &prepaSignal, NULL);
      sigaction (SIGTERM, &prepaSignal, NULL);

      while (i < 10)
      {
        pause();
      }
    default :
      printf ("Joueur1 : PID=%d\n", getpid());

      sigaction (SIGUSR2, &prepaSignal, NULL);

      sleep (1);
      int envoi0 = zoneEnvoi();
      unsigned long taille = sizeof (envoi0);

      // if (i == 0)
      // {
      // }
        write ( tube[1], &taille, sizeof (unsigned long));
        if (envoi0 == -1)
        {
          printf("Joueur1 rate son tir\n");
          i++;
          write ( tube[1], &envoi0, taille);
        }
        else
        {
          printf("Joueur1 envoie la balle en %d\n", envoi0);
          write ( tube[1], &envoi0, taille);
        }
        kill ( pid , SIGUSR1 );

      while(i < 10)
      {
        pause();
      }
    }

  }
  kill ( pid , SIGTERM );

  sleep(1);
  printf("Match fini, bye bye!\n");
  return 0;
}