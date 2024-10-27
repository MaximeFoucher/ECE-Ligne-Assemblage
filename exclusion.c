#include "exclusion.h"
#include "usine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <values.h>
#include <stdbool.h>

void ajouterExclusion(pAction action, int exclusion) {
    //attribution des exclusions par action
    //allocation dynamique du tableau
    int* temp = realloc(action->exclusion, (action->nbexclusions + 1) * sizeof(int));
    if (!temp) {
        perror("Erreur de reallocation pour exclusion");
        exit(EXIT_FAILURE);
    }
    action->exclusion = temp;
    action->exclusion[action->nbexclusions] = exclusion;
    action->nbexclusions++;
}

pAction* CreerArete(pAction* action,int s1,int s2){
    //permet de creer des aretes pour construire un graphe
    if(action[s1]->a_exclusion==NULL)
    {
        pArete Newarete=(pArete)malloc(sizeof(struct Arete));
        Newarete->action=s2;
        Newarete->arete_suivante=NULL;
        action[s1]->a_exclusion=Newarete;
        return action;
    }

    else
    {
        pArete temp=action[s1]->a_exclusion;
        while(temp->arete_suivante!=NULL)
        {
            temp=temp->arete_suivante;
        }
        pArete Newarete=(pArete)malloc(sizeof(struct Arete));
        Newarete->action=s2;
        Newarete->arete_suivante=NULL;

        if(temp->action>s2)
        {
            Newarete->arete_suivante=temp->arete_suivante;
            Newarete->action=temp->action;
            temp->action=s2;
            temp->arete_suivante=Newarete;
            return action;
        }

        temp->arete_suivante=Newarete;
        return action;
    }
}

int compareActions(const void *a, const void *b) {
    // Convertir les pointeurs void en pointeurs de structures Action
    pAction actionA = *(const pAction*)a;
    pAction actionB = *(const pAction*)b;

    // Comparer les demidegres pour un tri décroissant
    return actionB->demidegre - actionA->demidegre;
}


pAction* TriageActions(Usine* usine) {
    usine->actionstries = (pAction*)malloc(usine->nbreactions * sizeof(pAction));
    if (usine->actionstries == NULL) {
        perror("erreur allocation dynamique, fonction TriageActions");
        exit(EXIT_FAILURE);
    }
    int num=0;
    for (int i = 0; i <= usine->ordre; i++) {
        if(usine->pActionUsine[i]->existe==true){
            usine->actionstries[num] = usine->pActionUsine[i];
        num++;}
    }

    qsort(usine->actionstries, num, sizeof(pAction), compareActions);

    return usine->actionstries;
}

void WelshPowell(Usine* usine) {
    //permet de colorer les actions et pouvoir les trier en fonction de cette couleur
    usine->couleurmax=0;
    //initialiser a NULL les aretes
    for (int i = 0; i <= usine->ordre; i++) {
        usine->pActionUsine[i]->demidegre = 0;

            usine->pActionUsine[i]->a_exclusion = NULL;
            usine->pActionUsine[i]->welshpowell = 0;

    }
    //incrementation des demis degres
    for(int i=0;i<=usine->ordre;i++){
        if(usine->pActionUsine[i]->existe==true){
            for(int j=0;j<usine->pActionUsine[i]->nbexclusions;j++){
                usine->pActionUsine[i]->demidegre++;
            }
        }
    }


    bool toutmarque = false;
    //initialisation pour voir si on a coloré tous les sommets

    usine->actionstries = TriageActions(usine);


    for (int i = 0; i < usine->nbreactions; i++) {
        int a = 0;
    if(usine->actionstries[i]->a_exclusion!=NULL){
        printf("%d",i);
    }
            a++;
            for (int j = 0; j < usine->actionstries[i]->nbexclusions; j++) {
                int b=0;
                while(usine->actionstries[i]->exclusion[j]!=usine->actionstries[b]->num){
                    b++;
                }
                CreerArete(usine->actionstries,i,b);
            }


        }

int couleur=0;
while(toutmarque==false){


    //on verifie que les sommets ne sont pas tous deja marques
    int nbremarque=0;
    for(int i=0;i<usine->nbreactions;i++){
        if(usine->actionstries[i]->welshpowell!=0){
            nbremarque++;
        }
    }
    if(nbremarque==usine->nbreactions){
        toutmarque=true;
    }
    couleur++;


    for(int i=0;i<usine->nbreactions;i++){
        int check=0;
         struct Arete *areteactuel = usine->actionstries[i]->a_exclusion;
         while(areteactuel!=NULL){
             if(usine->actionstries[areteactuel->action]->welshpowell==couleur){
                 check++;
                 break;
             }
            areteactuel=areteactuel->arete_suivante;
         }
        if(usine->actionstries[i]->welshpowell==0 && check==0)    {
             usine->actionstries[i]->welshpowell=couleur;
        }


    }
//on verifie que les sommets ne sont pas tous deja marques
     nbremarque=0;
    for(int i=0;i<usine->nbreactions;i++){
        if(usine->actionstries[i]->welshpowell!=0){
            nbremarque++;
        }
    }
    if(nbremarque==usine->nbreactions){
        toutmarque=true;
    }



}
for(int i=0;i<=usine->ordre;i++){
    if(usine->pActionUsine[i]->existe==true){
        int b=0;
        while(usine->pActionUsine[i]->num!=usine->actionstries[b]->num){
            b++;
        }
       usine->pActionUsine[i]->welshpowell=usine->actionstries[b]->welshpowell;
    }
}
    usine->couleurmax=couleur;

}

void Exclusion(Usine* usine) {
    //on cree le tableau de station
    usine->pStation = (pStation*)malloc(usine->couleurmax * sizeof(pStation));
    if (usine->pStation == NULL) {
        perror("Erreur d'allocation pour les stations");
        exit(EXIT_FAILURE);
    }

    // Initialisations des stations
    for (int i = 0; i < usine->couleurmax; i++) {
        usine->pStation[i] = (pStation)malloc(sizeof(struct Station));
        usine->pStation[i]->pAction = NULL;
        usine->pStation[i]->nbactions = 0;
        usine->pStation[i]->num = i + 1;
        usine->pStation[i]->duree = 0;
        usine->pStation[i]->temp_max = usine->temp_max;
    }

    // On ajoute l'action a sa station en fonction de sa couleur
    for (int i = 0; i <= usine->ordre; i++) {
        if (usine->pActionUsine[i]->existe) {
            int couleur = usine->pActionUsine[i]->welshpowell - 1;
            int nbactions = usine->pStation[couleur]->nbactions;

            // On rajoute une case dans la tableau pour  ajouter l'action
            usine->pStation[couleur]->pAction = realloc(usine->pStation[couleur]->pAction, (nbactions + 1) * sizeof(pAction));
            if (usine->pStation[couleur]->pAction == NULL) {
                perror("Erreur de reallocation pour les actions de la station");
                exit(EXIT_FAILURE);
            }

            usine->pStation[couleur]->pAction[nbactions] = usine->pActionUsine[i];
            usine->pStation[couleur]->nbactions++;
            usine->pStation[couleur]->duree += usine->pActionUsine[i]->duree;
        }
    }
    //mise a jour duree totale de l'usine
    usine->dureetotaleusine=0;
    for (int i = 0; i < usine->couleurmax; i++) {
        usine->dureetotaleusine+=usine->pStation[i]->duree;}

    // Afficher les détails de chaque station
    printf("\nAffichage des Stations (Contrainte d'exclusion):\n");
    fflush(stdout);
    printf("Nombre d'actions dans l'usine : %d\n",usine->nbreactions);
    fflush(stdout);
    printf("Duree totale de l'usine : %.2f\n\n",usine->dureetotaleusine);
    fflush(stdout);
    for (int i = 0; i < usine->couleurmax; i++) {
        printf("Station %d:\n", usine->pStation[i]->num);
        fflush(stdout);
        printf("Nombre d'actions: %d\n", usine->pStation[i]->nbactions);
        fflush(stdout);
        printf("Actions:\n");
        fflush(stdout);
        for (int j = 0; j < usine->pStation[i]->nbactions; j++) {
            printf("Action: %d - Couleur: %d\n", usine->pStation[i]->pAction[j]->num, usine->pStation[i]->pAction[j]->welshpowell);
            fflush(stdout);
        }
        printf("\n");
        fflush(stdout);
    }
    printf("\n\n");
    fflush(stdout);
    usine->taille=usine->couleurmax;

}


void ExclusionEtTemps(Usine* usine) {
    // Initialisation des stations
    usine->pStation = (pStation*)malloc(usine->couleurmax * sizeof(pStation));
    for (int i = 0; i < usine->couleurmax; i++) {
        usine->pStation[i] = (pStation)malloc(sizeof(struct Station));
        usine->pStation[i]->pAction = NULL;
        usine->pStation[i]->nbactions = 0;
        usine->pStation[i]->num = i + 1;
        usine->pStation[i]->duree = 0;
        usine->pStation[i]->temp_max = usine->temp_max;
        usine->pStation[i]->welshpowell = i + 1;
    }

    int totalStations = usine->couleurmax;

    // Pour chaque couleur on parcourt toutes les actions
    for (int couleur = 1; couleur <= usine->couleurmax; couleur++) {
        for (int i = 0; i <= usine->ordre; i++) {
            pAction action = usine->pActionUsine[i];

            //on vérifie que la création de stations n'est pas impossible
            if (action->existe && action->welshpowell == couleur) {
                if (action->duree > usine->temp_max) {
                    printf("Erreur : L'action %d a une duree (%.2f) superieure au temps maximum permis (%.2f).\n", action->num, action->duree, usine->temp_max);
                    exit(EXIT_FAILURE);
                }

                bool actionAjoutee = false;
                // On regarde toutes les stations de cette couleur
                for (int j = 0; j < totalStations; j++) {
                    if (usine->pStation[j]->welshpowell == couleur && usine->pStation[j]->duree + action->duree <= usine->temp_max) {
                        // On ajoute l'action
                        int nbactions = usine->pStation[j]->nbactions;
                        usine->pStation[j]->pAction = realloc(usine->pStation[j]->pAction, (nbactions + 1) * sizeof(pAction));
                        usine->pStation[j]->pAction[nbactions] = action;
                        usine->pStation[j]->nbactions++;
                        usine->pStation[j]->duree += action->duree;
                        actionAjoutee = true;
                        break;
                    }
                }

                // On crée une nouvelle station si besoin
                if (!actionAjoutee) {
                    totalStations++;
                    usine->pStation = realloc(usine->pStation, totalStations * sizeof(pStation));
                    usine->pStation[totalStations - 1] = (pStation)malloc(sizeof(struct Station));
                    usine->pStation[totalStations - 1]->pAction = NULL;
                    usine->pStation[totalStations - 1]->nbactions = 0;
                    usine->pStation[totalStations - 1]->num = totalStations;
                    usine->pStation[totalStations - 1]->duree = action->duree;
                    usine->pStation[totalStations - 1]->temp_max = usine->temp_max;
                    usine->pStation[totalStations - 1]->welshpowell = couleur;

                    // Ajouter l'action à la nouvelle station
                    usine->pStation[totalStations - 1]->pAction = realloc(usine->pStation[totalStations - 1]->pAction, sizeof(pAction));
                    usine->pStation[totalStations - 1]->pAction[0] = action;
                    usine->pStation[totalStations - 1]->nbactions = 1;
                }
            }
        }
    }

    // Mise à jour du nombre total de stations dans l'usine
    usine->taille = totalStations;

    //mise a jour duree totale de l'usine
    usine->dureetotaleusine=0;
    for (int i = 0; i < totalStations; i++) {
        usine->dureetotaleusine+=usine->pStation[i]->duree;}

    // Affichage des stations
    printf("\nAffichage des Stations (Contrainte d'exclusion et de temps):\n");
    printf("Nombre d'actions dans l'usine : %d\n",usine->nbreactions);
    printf("Duree totale de l'usine : %.2f\n\n",usine->dureetotaleusine);
    fflush(stdout);
    for (int i = 0; i < totalStations; i++) {
        printf("Station %d:\n", usine->pStation[i]->num);
        printf("Nombre d'actions: %d\n", usine->pStation[i]->nbactions);
        printf("Duree totale: %.2f (max: %.2f)\n", usine->pStation[i]->duree, usine->pStation[i]->temp_max);
        printf("Actions dans la station:\n");
        for (int j = 0; j < usine->pStation[i]->nbactions; j++) {
            printf("Action %d - Duree: %.2f - Couleur: %d\n", usine->pStation[i]->pAction[j]->num, usine->pStation[i]->pAction[j]->duree, usine->pStation[i]->pAction[j]->welshpowell);
        }
        printf("\n");
    }
    printf("\n\n");
    fflush(stdout);

}






















