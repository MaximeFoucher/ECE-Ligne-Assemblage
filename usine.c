#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <values.h>
#include <stdbool.h>
#include "usine.h"
#include "exclusion.h"
#include "precedences.h"


int Verification(Usine* usine){
    //permet de verifier si les fichiers txt sont tous en accords

    //verification pour exclusions.txt
    // si un sommet n'existe pas dans operations.txt alors explique à l'interlocuteur et coupe le code
    FILE * exclusions = fopen("exclusions.txt","r");
    if (!exclusions)
    {
        printf("Erreur de lecture du fichier exclusions.txt lors de la verification\n");
        //verifie la bonne ouverture
        fflush(stdout);
        exit(-1);
    }
    int num1, num2, compteurindiqueligne=1, fin=0;

    while (fscanf(exclusions, "%d %d", &num1, &num2) != EOF) {
        if(num1>usine->ordre){
            printf("probleme a la ligne %d du fichier exclusions.txt ou le premier numero n'est pas indique dans operations.txt\n", compteurindiqueligne);
            fin=1;
        }
        if(num2>usine->ordre){
            printf("probleme a la ligne %d du fichier exclusions.txt ou le deuxieme numero n'est pas indique dans operations.txt\n", compteurindiqueligne);
            fin=1;
        }
        compteurindiqueligne++;
    }

    //verification pour precedences.txt
    // si un sommet n'existe pas dans precedences.txt alors explique à l'interlocuteur et coupe le code
    FILE * precedences = fopen("precedences.txt","r");
    if (!precedences)
    {
        printf("Erreur de lecture du fichier precedences.txt lors de la verification\n");
        fflush(stdout);
        exit(-1);
    }
    int operation1, operation2;
    compteurindiqueligne=1;

    while (fscanf(precedences, "%d %d", &operation1, &operation2) != EOF) {
        if(operation1>usine->ordre){
            printf("probleme a la ligne %d du fichier precedences.txt ou le premier numero n'est pas indique dans operations.txt\n", compteurindiqueligne);
            fin=1;
        }
        if(operation2>usine->ordre){
            printf("probleme a la ligne %d du fichier precedences.txt ou le deuxieme numero n'est pas indique dans operations.txt\n", compteurindiqueligne);
            fin=1;
        }
        compteurindiqueligne++;
    }
    return fin;
}


//on crée  l'usine

Usine* CreerUsine(int ordre)
{
    //On l'alloue dynamiquement
    Usine * Newusine=(Usine*)malloc(sizeof(Usine));
    Newusine->pActionUsine = (pAction*)malloc((ordre+1)*sizeof(pAction));
    //ordre+1 pour ne pas oublier une action

    //On initialise toutes les valeurs
    for(int i=0; i<=ordre; i++)
    {
        Newusine->pActionUsine[i] = (pAction)malloc(sizeof(struct Action));
        Newusine->pActionUsine[i]->exclusion = NULL;
        Newusine->pActionUsine[i]->precedence = NULL;
        Newusine->pActionUsine[i]->nbexclusions = 0;
        Newusine->pActionUsine[i]->nbprecedence = 0;
        Newusine->pActionUsine[i]->existe=false;
        Newusine->pActionUsine[i]->num=i;
        Newusine->pActionUsine[i]->welshpowell=0;
        Newusine->pActionUsine[i]->duree=0;
        Newusine->pActionUsine[i]->a_exclusion=NULL;
        Newusine->pActionUsine[i]->a_successeur=NULL;
        Newusine->pActionUsine[i]->niveau=-1;
        Newusine->couleurmax=0;
        Newusine->dureetotaleusine=0;

    }
    return Newusine;
}


Usine * lire_usine(int* fin)
{
    Usine* Usine;
    FILE * ifs = fopen("operations.txt","r");
    if (!ifs)
    {
        printf("Erreur de lecture du fichier operations.txt\n");
        fflush(stdout);
        exit(-1);
    }

    int ordre=-1;
    int num=0;
    float duree=0;
    //on enregistre le numéro de l'action avec le numéro le plus élevé
    while (fscanf(ifs, "%d %f", &num, &duree) != EOF) {
        if(num>ordre){
            ordre=num;
        }
    }

    num=-1;
    duree=0;

    //on crée l'usine
    Usine=CreerUsine(ordre);

    //On enregistre l'ordre  (numéro de l'action avec le numéro le plus élevé)
    Usine->ordre=ordre;

    //on revient au debut du fichier
    rewind(ifs);

    Usine->nbreactions=0;
    //on initialise
    //on enregistre les actions avec leurs durées
    fscanf(ifs,"%d %f",&num,&duree);
    rewind(ifs);
    for(int i=0;i<=ordre;i++){
        duree=0;
        Usine->pActionUsine[i]->num=i;
        Usine->pActionUsine[i]->marquage=0;
        if(num==i){
            fscanf(ifs,"%d %f",&num,&duree);
            Usine->nbreactions++;

            Usine->pActionUsine[num]->existe=true;
            Usine->pActionUsine[num]->duree=duree;

            num++;

        }

        }

    fclose(ifs);

    ifs = fopen("exclusions.txt","r");
    if (!ifs)
    {
        printf("Erreur de lecture du fichier exclusions.txt\n");
        fflush(stdout);
        exit(-1);
    }


    *fin= Verification(Usine);
    //fonction qui permet de verifier les fichiers txt et renvoie 1ou 0selon s'il y a un prbleme ou non
    if(*fin==1){
        return Usine;
        //stop la fonction
    }


    //On enregistre les exclusions de chaque action

    int num1, num2;
    while (fscanf(ifs, "%d %d", &num1, &num2) != EOF) {
        if (num1 >= 0 && num1 <= Usine->ordre) {
            ajouterExclusion(Usine->pActionUsine[num1], num2);
            ajouterExclusion(Usine->pActionUsine[num2], num1);

        }
    }

    fclose(ifs);

    ifs = fopen("precedences.txt","r");
    if (!ifs)
    {
        printf("Erreur de lecture du fichier precedences.txt\n");
        fflush(stdout);
        exit(-1);
    }

    //On enregistre les précédences de chaque action
    while (fscanf(ifs, "%d %d", &num1, &num2) != EOF) {
        if (num1 >= 0 && num1 <= Usine->ordre) {
            ajouterPrecedence(Usine->pActionUsine[num2], num1);
        }
    }

    fclose(ifs);

    ifs = fopen("temps_cycle.txt","r");
    if (!ifs)
    {
        printf("Erreur de lecture du fichier temps_cycle.txt\n");
        fflush(stdout);
        exit(-1);
    }

    fscanf(ifs, "%f", &Usine->temp_max);

    fclose(ifs);

    return Usine;
}

void verif(Usine *Usine){
    //vérification de l'enregistrement
    //permet surtout le test
    for(int i=0;i<=Usine->ordre;i++){
        int a=0;
        if(Usine->pActionUsine[i]->existe==true){
            a=1;
        }
        printf("num de l'action : %d marquage : %d existe t elle : %d duree : %.2f ",Usine->pActionUsine[i]->num,
              Usine->pActionUsine[i]->marquage,a,Usine->pActionUsine[i]->duree);
        fflush(stdout);
        //affichage des exclusions
        if (Usine->pActionUsine[i]->nbexclusions > 0) {
            printf("Exclusions: ");
            fflush(stdout);
            for (int j = 0; j < Usine->pActionUsine[i]->nbexclusions; j++) {
                printf("%d ", Usine->pActionUsine[i]->exclusion[j]);
                fflush(stdout);

            }
        } else {
           printf("Aucune exclusion. ");
            fflush(stdout);

        }

        //fin d'affichage des exclusions

        //début affichage precedence
        if (Usine->pActionUsine[i]->nbprecedence > 0) {
            printf("Precedences: ");
            fflush(stdout);
            for (int j = 0; j < Usine->pActionUsine[i]->nbprecedence; j++) {
                printf("%d ", Usine->pActionUsine[i]->precedence[j]);
                fflush(stdout);

            }
        } else {
            printf("Aucune precedences. ");
            fflush(stdout);

        }
        //fin d'affichage des precedences
        //affichage de la couleur
        printf("couleur:%d ",Usine->pActionUsine[i]->welshpowell);


        //affichage du niveau
        //printf("niveau:%d\n",Usine->pActionUsine[i]->niveau);
        fflush(stdout);
        printf("\n");

    }
    printf("temp max par station: %f\n\n",Usine->temp_max);
    fflush(stdout);
}

void libererMemoireUsine(Usine *usine) {
    // Libérer la mémoire allouée pour chaque action
    for (int i = 0; i <= usine->ordre; i++) {
        if (usine->pActionUsine[i] != NULL) {
            // Libérer les tableaux de précédence et d'exclusion pour chaque action
            if (usine->pActionUsine[i]->precedence != NULL) {
                free(usine->pActionUsine[i]->precedence);
            }
            if (usine->pActionUsine[i]->exclusion != NULL) {
                free(usine->pActionUsine[i]->exclusion);
            }

            // Libérer les arêtes des listes d'adjacence (pour précédence et exclusion)
            pArete areteTemp, areteASupprimer;
            areteTemp = usine->pActionUsine[i]->a_successeur;
            while (areteTemp != NULL) {
                areteASupprimer = areteTemp;
                areteTemp = areteTemp->arete_suivante;
                free(areteASupprimer);
            }
            areteTemp = usine->pActionUsine[i]->a_exclusion;
            while (areteTemp != NULL) {
                areteASupprimer = areteTemp;
                areteTemp = areteTemp->arete_suivante;
                free(areteASupprimer);
            }

            // Libérer la structure de l'action elle-même
            free(usine->pActionUsine[i]);
        }
    }

    // Libérer le tableau des actions
    free(usine->pActionUsine);

    // Libérer les stations et leurs actions
    for (int i = 0; i < usine->taille; i++) {
        if (usine->pStation[i] != NULL) {
            if (usine->pStation[i]->pAction != NULL) {
                free(usine->pStation[i]->pAction);
            }
            free(usine->pStation[i]);
        }
    }

    // Libérer le tableau des stations
    free(usine->pStation);

    // Libérer la mémoire pour les actions triées, si allouées
    if (usine->actionstries != NULL) {
        free(usine->actionstries);
    }

    // Enfin, libérer la structure de l'usine elle-même
    free(usine);
}