#include "precedences.h"
#include "usine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <values.h>
#include <stdbool.h>
#include <limits.h>




//Fonction qui ajoute une précédence a une action
void ajouterPrecedence(pAction action, int precedence) {
    int* temp = realloc(action->precedence, (action->nbprecedence + 1) * sizeof(int));
    if (!temp) {
        perror("Erreur de reallocation pour precedence");
        exit(EXIT_FAILURE);
    }
    action->precedence = temp;
    action->precedence[action->nbprecedence] = precedence;
    action->nbprecedence++;
}



//Fonction qui crée des niveaux comme dans MPM
void Niveaux (Usine* usine){
    int compteur=0;
    //compteur pour le nombre de sommet qui ont été traités
    for (int i = 0; i < usine->ordre; ++i) {
        //initialise tous les marquages et niveaux
        usine->pActionUsine[i]->marquage=0;
        usine->pActionUsine[i]->niveau=-1;
        //niveaux -1 et si le sommets est inexistant il restera à -1
    }
    int niveau = 0; //designe le niveau actuel qui doit etre parcouru


    while(compteur<usine->nbreactions){
        //pour verifier que tous les sommets sont marqués
        for (int i = 0; i <= usine->ordre; i++) {
            //pour parcourir tous les sommets
            if (usine->pActionUsine[i]->existe && !usine->pActionUsine[i]->marquage) {
                //verifie si l'action existe
                int verifieprec=0;
                //permet de voir si toutes ses précédences ont déjà été traitées
                for (int j = 0; j < usine->pActionUsine[i]->nbprecedence; ++j) {
                    //parcourir toutes les precedences
                    int actionprecedente = usine->pActionUsine[i]->precedence[j];
                    if(usine->pActionUsine[actionprecedente]->marquage==1){
                        verifieprec++;
                        //incremente le compteur de précédences si celles ci sont marquées
                    }
                }

                if(verifieprec==usine->pActionUsine[i]->nbprecedence){
                    //verifie si toutes les prec sont bien marque
                    usine->pActionUsine[i]->niveau=niveau;
                    compteur++;
                    //incremente le compteur de sommet
                }
            }
        }

        for (int i = 0; i < usine->ordre+1; ++i) {
            if (usine->pActionUsine[i]->niveau==niveau){
                usine->pActionUsine[i]->marquage=1;
                //marque tous les sommets traités avec le sommet actuel et les marque
            }
        }
        niveau++;
        //incremente le niveau actuel à traiter
    }

}


//Fonction qui crée et affiche les stations en appliquant les contraintes de temps et précédence
void PrecedencesEtTemps(Usine* usine) {
    // On réinitialise le marquage
    for (int i = 0; i <= usine->ordre; i++) {
        usine->pActionUsine[i]->marquage = 0;
    }

    // Initialisation de la première station
    usine->pStation = (pStation*)malloc(sizeof(pStation));
    usine->pStation[0] = (pStation)malloc(sizeof(struct Station));
    usine->pStation[0]->pAction = NULL;
    usine->pStation[0]->nbactions = 0;
    usine->pStation[0]->num = 1;
    usine->pStation[0]->duree = 0;
    usine->pStation[0]->temp_max = usine->temp_max;
    usine->pStation[0]->marquage = 0;

    int totalStations = 1;
    int niveauActuel = 0;
    bool toutmarque = false;

    // Boucle tant que toutes les actions ne sont pas marquées
    while (!toutmarque) {
        bool actionAjoutee = false;

        for (int i = 0; i <= usine->ordre; i++) {
            pAction action = usine->pActionUsine[i];

            if (action->existe && action->niveau == niveauActuel && action->marquage == 0) {
                // On vérifie que la création de station est possible ici
                if (action->duree > usine->temp_max) {
                    printf("Erreur : L'action %d a une duree (%.2f) qui depasse la duree maximale par station (%.2f).\n", action->num, action->duree, usine->temp_max);
                    fflush(stdout);
                    exit(EXIT_FAILURE);
                }

                // On regarde si on peut ajouter l'action à une station
                for (int j = 0; j < totalStations; j++) {
                    if (usine->pStation[j]->duree + action->duree <= usine->temp_max &&usine->pStation[j]->marquage==0) {
                        // On ajoute l'action à la station
                        int nbactions = usine->pStation[j]->nbactions;
                        usine->pStation[j]->pAction = realloc(usine->pStation[j]->pAction, (nbactions + 1) * sizeof(pAction));
                        usine->pStation[j]->pAction[nbactions] = action;
                        usine->pStation[j]->nbactions++;
                        usine->pStation[j]->duree += action->duree;
                        action->marquage = 1;
                        actionAjoutee = true;
                        break;
                    }
                }

                // On crée  une nouvelle station si on peut pas ajouter d'actions
                if (!actionAjoutee) {
                    usine->pStation[totalStations-1]->marquage=1;
                    usine->pStation = realloc(usine->pStation, (totalStations + 1) * sizeof(pStation));
                    usine->pStation[totalStations] = (pStation)malloc(sizeof(struct Station));
                    usine->pStation[totalStations]->pAction = NULL;
                    usine->pStation[totalStations]->nbactions = 0;
                    usine->pStation[totalStations]->num = totalStations + 1;
                    usine->pStation[totalStations]->duree = action->duree;
                    usine->pStation[totalStations]->temp_max = usine->temp_max;
                    usine->pStation[totalStations]->marquage = 0;


                    // Ajouter l'action à la nouvelle station
                    usine->pStation[totalStations]->pAction = realloc(usine->pStation[totalStations]->pAction, sizeof(pAction));
                    usine->pStation[totalStations]->pAction[0] = action;
                    usine->pStation[totalStations]->nbactions = 1;
                    action->marquage = 1;
                    totalStations++;
                }
            }
        }

        // On incréemente le niveau
        if (!actionAjoutee) {
            niveauActuel++;
        }

        // on sort de la boucle si toutes les actions sont marqués
        int marquee = 0;
        for (int i = 0; i <= usine->ordre; i++) {
            if (usine->pActionUsine[i]->existe && usine->pActionUsine[i]->marquage) {
                marquee++;
            }
        }
        toutmarque = (marquee == usine->nbreactions);
    }

    // Mise à jour du nombre total de stations dans l'usine
    usine->taille = totalStations;

    //mise a jour duree totale de l'usine
    usine->dureetotaleusine=0;
    for (int i = 0; i < totalStations; i++) {
        usine->dureetotaleusine+=usine->pStation[i]->duree;}

    // Affichage des stations
    printf("\nAffichage des Stations (Contraintes de precedence et de temps):\n\n");
    fflush(stdout);
    printf("Duree totale de l'usine : %.2f\n",usine->dureetotaleusine);
    fflush(stdout);
    for (int i = 0; i < totalStations; i++) {
        pStation station = usine->pStation[i];
        printf("Station %d:\n", station->num);
        fflush(stdout);
        printf("Nombre d'actions: %d\n", station->nbactions);
        fflush(stdout);
        printf("Duree totale: %.2f (max%.2f)\n", station->duree,station->temp_max);
        fflush(stdout);
        printf("Actions dans la station:\n");
        fflush(stdout);
        for (int j = 0; j < station->nbactions; j++) {
            pAction action = station->pAction[j];
            printf("Action %d - Duree: %.2f - Niveau: %d\n", action->num, action->duree, action->niveau);
            fflush(stdout);
        }
        printf("\n");
        fflush(stdout);
    }
    printf("\n\n");
    fflush(stdout);


}

//Fonction permettant de creer les aretes du graphe de précédence
pAction* CreerAretePrecedence(pAction* action,int s1,int s2)
{
    if(action[s1]->a_successeur==NULL)
    {
        pArete Newarete=(pArete)malloc(sizeof(struct Arete));
        Newarete->action=s2;
        Newarete->arete_suivante=NULL;
        action[s1]->a_successeur=Newarete;
        return action;
    }

    else
    {
        pArete temp=action[s1]->a_successeur;
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

void dfs2contraintes(Usine *usine, pAction action, pStation stationCourante, float *tempsCourant) {
    if (action->marquage) {
        return;
    }

    // On vérifie que l'action est le seul prédécesseur non marqué

    for (int i = 0; i < action->nbprecedence; i++) {
        int predecesseur = action->precedence[i];
        if (!usine->pActionUsine[predecesseur]->marquage) {
            return;
        }
    }
    if(stationCourante->nbactions!=0) {
        for (int i = 0; i < stationCourante->nbactions; i++) {
            pAction actionDejaDansLaStation = stationCourante->pAction[i];
            for (int j = 0; j < action->nbexclusions; j++) {
                if (actionDejaDansLaStation->num == action->exclusion[j]) {
                    return; // Il y a une exclusion avec une action déjà dans le chemin
                }
            }
        }
    }



    // Ajouter l'action à la station courante
    stationCourante->pAction = realloc(stationCourante->pAction, (stationCourante->nbactions + 1) * sizeof(pAction));
    stationCourante->pAction[stationCourante->nbactions] = action;
    stationCourante->nbactions++;
    *tempsCourant += action->duree;
    action->marquage = 1;

    // Parcourir les successeurs de l'action
    pArete arete = action->a_successeur;
    while (arete != NULL) {
        pAction actionSuivante = usine->pActionUsine[arete->action];
        dfs2contraintes(usine, actionSuivante, stationCourante, tempsCourant);
        arete = arete->arete_suivante;
    }
}



void PrecedenceExclusion(Usine *usine) {
    // On initialise à NULL les arêtes
    for (int i = 0; i <= usine->ordre; i++) {
        usine->pActionUsine[i]->a_successeur = NULL;
    }

    // On crée le graphe de précédence
    for (int i = 0; i <= usine->ordre; i++) {
        if (usine->pActionUsine[i]->existe && usine->pActionUsine[i]->nbprecedence != 0) {
            for (int j = 0; j < usine->pActionUsine[i]->nbprecedence; j++) {
                CreerAretePrecedence(usine->pActionUsine, usine->pActionUsine[i]->precedence[j], i);
            }
        }
    }

    // Réinitialisation des marquages pour toutes les actions
    for (int i = 0; i <= usine->ordre; i++) {
        if (usine->pActionUsine[i]->existe) {
            usine->pActionUsine[i]->marquage = 0;
        }
    }

    // Initialisation ou redimensionnement du tableau de stations
    usine->pStation = (pStation*)realloc(usine->pStation, sizeof(pStation));
    usine->taille = 0; // Aucune station pour l'instant

    bool toutesActionsMarquees = false;
    while (!toutesActionsMarquees) {
        toutesActionsMarquees = true;
        int niveauMin = INT_MAX;
        pAction actionDeDepart = NULL;

        // On cherche une action sans prédécesseurs non marqués et avec le niveau le plus bas possible
        for (int i = 0; i <= usine->ordre; i++) {
            if (usine->pActionUsine[i]->existe && !usine->pActionUsine[i]->marquage) {
                bool aUnPredecesseurNonMarque = false;
                for (int j = 0; j < usine->pActionUsine[i]->nbprecedence; j++) {
                    if (!usine->pActionUsine[usine->pActionUsine[i]->precedence[j]]->marquage) {
                        aUnPredecesseurNonMarque = true;
                        break;
                    }
                }
                if (!aUnPredecesseurNonMarque && usine->pActionUsine[i]->niveau < niveauMin) {
                    niveauMin = usine->pActionUsine[i]->niveau;
                    actionDeDepart = usine->pActionUsine[i];
                }
                toutesActionsMarquees = false;
            }
        }


        if (actionDeDepart != NULL) {
            //On regarde si on peut ajouter actionDeDepart a la dernière station crée
            int NouvelleStation=0;
            if(usine->taille==0){
                NouvelleStation++;
            }
            if(usine->taille!=0){
                for(int i=0;i<actionDeDepart->nbexclusions;i++){
                    for(int j=0;j<usine->pStation[usine->taille-1]->nbactions;j++){
                        if(actionDeDepart->exclusion[i]==usine->pStation[usine->taille-1]->pAction[j]->num){
                            NouvelleStation++;
                        }
                    }
                }
            }

            float tempsCourant;
            if(NouvelleStation!=0){
                tempsCourant = 0;
                // Redimensionner le tableau pour ajouter une nouvelle station
                usine->pStation = (pStation*)realloc(usine->pStation, (usine->taille + 1) * sizeof(pStation));
                if (usine->pStation == NULL) {
                    fprintf(stderr, "Erreur de redimensionnement de la mémoire pour usine->pStation.\n");
                    exit(EXIT_FAILURE);
                }

                // Création de la nouvelle station
                usine->pStation[usine->taille] = (pStation)malloc(sizeof(struct Station));
                if (usine->pStation[usine->taille] == NULL) {
                    fprintf(stderr, "Erreur d'allocation mémoire pour une nouvelle station.\n");
                    exit(EXIT_FAILURE);
                }

                // Initialisation de la nouvelle station
                usine->pStation[usine->taille]->pAction = NULL;
                usine->pStation[usine->taille]->nbactions = 0;
                usine->pStation[usine->taille]->num = usine->taille + 1;
                usine->pStation[usine->taille]->duree = 0;
                usine->pStation[usine->taille]->temp_max = usine->temp_max;
                usine->pStation[usine->taille]->marquage = 0;
                usine->taille++; // Incrémenter le nombre total de stations
            }
            else{ tempsCourant = usine->pStation[usine->taille-1]->duree;}

            // Exécuter DFS

            dfs2contraintes(usine, actionDeDepart, usine->pStation[usine->taille-1], &tempsCourant);
            usine->pStation[usine->taille-1]->duree = tempsCourant;
        }
    }

    // Calculer la durée totale de l'usine
    usine->dureetotaleusine = 0;
    for (int i = 0; i < usine->taille; i++) {
        usine->dureetotaleusine += usine->pStation[i]->duree;
    }

    // Affichage des stations
    printf("\nAffichage des Stations (Contraintes de precedence et d'exclusion):\n");
    fflush(stdout);
    printf("Nombre d'actions dans l'usine : %d\n",usine->nbreactions);
    fflush(stdout);
    printf("Duree totale de l'usine: %.2f\n\n", usine->dureetotaleusine);
    fflush(stdout);
    for (int i = 0; i < usine->taille; i++) {
        pStation station = usine->pStation[i];
        printf("Station %d:\n", station->num);
        fflush(stdout);
        printf("Nombre d'actions: %d\n", station->nbactions);
        fflush(stdout);
        for (int j = 0; j < station->nbactions; j++) {
            pAction action = station->pAction[j];
            printf("Action %d - Exclusions:", action->num);
            fflush(stdout);
            for(int z=0;z<action->nbexclusions;z++){
                if(z==action->nbexclusions-1){
                    printf(" %d",action->exclusion[z]);
                    fflush(stdout);
                }
                else{
                printf(" %d,",action->exclusion[z]);
                fflush(stdout);}
            }
            printf(" - Precedences :");
            fflush(stdout);
            for(int z=0;z<action->nbprecedence;z++){
                if(z==action->nbprecedence-1){
                    printf(" %d",action->precedence[z]);
                    fflush(stdout);
                }
                else{
                printf(" %d,",action->precedence[z]);
                fflush(stdout);}
            }
            printf("\n");
            fflush(stdout);
        }
        printf("\n");
        fflush(stdout);
    }
    printf("\n\n");
    fflush(stdout);
}

void dfs3contraintes(Usine *usine, pAction action, pStation stationCourante, float *tempsCourant) {

    if (action->duree > usine->temp_max) {
        printf("Erreur : L'action %d a une duree (%.2f) qui depasse la duree maximale par station (%.2f).\n", action->num, action->duree, usine->temp_max);
        fflush(stdout);
        exit(EXIT_FAILURE);
    }

    if (action->marquage || *tempsCourant + action->duree > usine->temp_max) {
        return;
    }

    // Vérifier que l'action est le seul prédécesseur non marqué

    for (int i = 0; i < action->nbprecedence; i++) {
        int predecesseur = action->precedence[i];
        if (!usine->pActionUsine[predecesseur]->marquage) {
            return;
        }
    }
    if(stationCourante->nbactions!=0) {
        for (int i = 0; i < stationCourante->nbactions; i++) {
            pAction actionDejaDansLaStation = stationCourante->pAction[i];
            for (int j = 0; j < action->nbexclusions; j++) {
                if (actionDejaDansLaStation->num == action->exclusion[j]) {
                    return; // Il y a une exclusion avec une action déjà dans le chemin
                }
            }
        }
    }



    // Ajouter l'action à la station courante
    stationCourante->pAction = realloc(stationCourante->pAction, (stationCourante->nbactions + 1) * sizeof(pAction));
    stationCourante->pAction[stationCourante->nbactions] = action;
    stationCourante->nbactions++;
    *tempsCourant += action->duree;
    action->marquage = 1;

    // Parcourir les successeurs de l'action
    pArete arete = action->a_successeur;
    while (arete != NULL) {
        pAction actionSuivante = usine->pActionUsine[arete->action];
        dfs3contraintes(usine, actionSuivante, stationCourante, tempsCourant);
        arete = arete->arete_suivante;
    }
}



void PrecedenceExclusionTemps(Usine *usine) {
    // Initialiser à NULL les arêtes
    for (int i = 0; i <= usine->ordre; i++) {
        usine->pActionUsine[i]->a_successeur = NULL;
    }

    // Créer le graphe de précédence
    for (int i = 0; i <= usine->ordre; i++) {
        if (usine->pActionUsine[i]->existe && usine->pActionUsine[i]->nbprecedence != 0) {
            for (int j = 0; j < usine->pActionUsine[i]->nbprecedence; j++) {
                CreerAretePrecedence(usine->pActionUsine, usine->pActionUsine[i]->precedence[j], i);
            }
        }
    }

    // Réinitialisation des marquages pour toutes les actions
    for (int i = 0; i <= usine->ordre; i++) {
        if (usine->pActionUsine[i]->existe) {
            usine->pActionUsine[i]->marquage = 0;
        }
    }

    // Initialisation ou redimensionnement du tableau de stations
    usine->pStation = (pStation*)realloc(usine->pStation, sizeof(pStation));
    usine->taille = 0; // Aucune station pour l'instant

    bool toutesActionsMarquees = false;
    while (!toutesActionsMarquees) {
        toutesActionsMarquees = true;
        int niveauMin = INT_MAX;
        pAction actionDeDepart = NULL;

        // Trouver une action sans prédécesseurs non marqués et avec le niveau le plus bas
        for (int i = 0; i <= usine->ordre; i++) {
            if (usine->pActionUsine[i]->existe && !usine->pActionUsine[i]->marquage) {
                bool aUnPredecesseurNonMarque = false;
                for (int j = 0; j < usine->pActionUsine[i]->nbprecedence; j++) {
                    if (!usine->pActionUsine[usine->pActionUsine[i]->precedence[j]]->marquage) {
                        aUnPredecesseurNonMarque = true;
                        break;
                    }
                }
                if (!aUnPredecesseurNonMarque && usine->pActionUsine[i]->niveau < niveauMin) {
                    niveauMin = usine->pActionUsine[i]->niveau;
                    actionDeDepart = usine->pActionUsine[i];
                }
                toutesActionsMarquees = false;
            }
        }


        if (actionDeDepart != NULL) {

            int NouvelleStation=0;
            if(usine->taille==0){
                NouvelleStation++;
            }
            if(usine->taille!=0){
                if(actionDeDepart->duree+usine->pStation[usine->taille-1]->duree>usine->temp_max){
                    NouvelleStation++;
                }
                for(int i=0;i<actionDeDepart->nbexclusions;i++){
                    for(int j=0;j<usine->pStation[usine->taille-1]->nbactions;j++){
                        if(actionDeDepart->exclusion[i]==usine->pStation[usine->taille-1]->pAction[j]->num){
                            NouvelleStation++;
                        }
                    }
                }
            }

            float tempsCourant;
            if(NouvelleStation!=0){
            tempsCourant = 0;
            // Redimensionner le tableau pour ajouter une nouvelle station
            usine->pStation = (pStation*)realloc(usine->pStation, (usine->taille + 1) * sizeof(pStation));
            if (usine->pStation == NULL) {
                fprintf(stderr, "Erreur de redimensionnement de la mémoire pour usine->pStation.\n");
                exit(EXIT_FAILURE);
            }

            // Création de la nouvelle station
            usine->pStation[usine->taille] = (pStation)malloc(sizeof(struct Station));
            if (usine->pStation[usine->taille] == NULL) {
                fprintf(stderr, "Erreur d'allocation mémoire pour une nouvelle station.\n");
                exit(EXIT_FAILURE);
            }

            // Initialisation de la nouvelle station
            usine->pStation[usine->taille]->pAction = NULL;
            usine->pStation[usine->taille]->nbactions = 0;
            usine->pStation[usine->taille]->num = usine->taille + 1;
            usine->pStation[usine->taille]->duree = 0;
            usine->pStation[usine->taille]->temp_max = usine->temp_max;
            usine->pStation[usine->taille]->marquage = 0;
                usine->taille++; // Incrémenter le nombre total de stations
            }
            else{ tempsCourant = usine->pStation[usine->taille-1]->duree;}

            // Exécuter DFS

            dfs3contraintes(usine, actionDeDepart, usine->pStation[usine->taille-1], &tempsCourant);
            usine->pStation[usine->taille-1]->duree = tempsCourant;
        }
    }

    // Calculer la durée totale de l'usine
    usine->dureetotaleusine = 0;
    for (int i = 0; i < usine->taille; i++) {
        usine->dureetotaleusine += usine->pStation[i]->duree;
    }

    // Affichage des stations
    printf("\nAffichage des Stations (Contraintes de precedence, d'exclusion et de temps):\n");
    fflush(stdout);
    printf("Nombre d'actions dans l'usine : %d\n",usine->nbreactions);
    fflush(stdout);
    printf("Duree totale de l'usine: %.2f\n\n", usine->dureetotaleusine);
    fflush(stdout);
    for (int i = 0; i < usine->taille; i++) {
        pStation station = usine->pStation[i];
        printf("Station %d:\n", station->num);
        fflush(stdout);
        printf("Nombre d'actions: %d\n", station->nbactions);
        fflush(stdout);
        printf("Duree de la station: %.2f\n", station->duree);
        fflush(stdout);
        for (int j = 0; j < station->nbactions; j++) {
            pAction action = station->pAction[j];
            printf("Action %d - duree: %.2f - Exclusions :", action->num,action->duree);
            fflush(stdout);
            for(int z=0;z<action->nbexclusions;z++){
                if(z==action->nbexclusions-1){
                    printf(" %d",action->exclusion[z]);
                    fflush(stdout);
                }
                else{
                    printf(" %d,",action->exclusion[z]);
                    fflush(stdout);}
            }
            printf(" - Precedences :");
            fflush(stdout);
            for(int z=0;z<action->nbprecedence;z++){
                if(z==action->nbprecedence-1){
                    printf(" %d",action->precedence[z]);
                    fflush(stdout);
                }
                else{
                    printf(" %d,",action->precedence[z]);
                    fflush(stdout);}
            }
            printf("\n");
            fflush(stdout);
        }
        printf("\n");
        fflush(stdout);
    }
    printf("\n\n");
    fflush(stdout);
}