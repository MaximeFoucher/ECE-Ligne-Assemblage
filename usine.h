#ifndef PROJET_TG_LIGNE_ASSEMBLAGE_2D_USINE_H
#define PROJET_TG_LIGNE_ASSEMBLAGE_2D_USINE_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <values.h>
#include <stdbool.h>


//Alias de pointeur sur station
typedef struct Station* pStation;

//Alias de pointeur sur action
typedef struct Action*  pAction;

/* Structure d'une usine */
typedef struct Usine
{

    //tableau stockant toutes les opérations
    pAction* pActionUsine;

    //nombre de stations dans l'usine
    int taille;

    //tableau stockant les actions triés dans WelshPowell
    pAction* actionstries;

    //valeur de l'action avec le numéro le plus élévé
    int ordre;

    //tableau de stations
    pStation* pStation;

    //durée maximale par station lue dans temps_cycle.txt
    float temp_max;

    //nombre d'actions présentes dans operations.txt
    int nbreactions;

    //couleur la plus élevée trouvée dans Welsh-Powell
    int couleurmax;

    //durée totale de l'usine
    float dureetotaleusine;




} Usine;


/* Structure d'une station*/
struct Station
{

    //tableau stockant les actions présentes dans cette usine
    pAction* pAction;

    //numéro de la station
    int num;

    //durée de la station
    float duree;

    //durée maximale par station lue dans temps_cycle.txt
    float temp_max;

    //Nombre d'actions présentes dans la station
    int nbactions;

    //Niveau de l'action trouvé grâce à un algo type MPM
    int niveau;

    //Valeur de la couleur des actions présentes dans la station trouvé par  l'algorithme de Welsh-Powell
    int welshpowell;

    //marquage utilisé dans differentes fonctions
    int marquage;
};

/* Structure d'une action*/
struct Action
        {
    //Arete utilisé dans un graphe d'exclusion dans l'Algorithme de Welsh-Powell
    struct Arete* a_exclusion;

    //Arete utilisé dans un graphe de precedences dans l'Algorithme DFS
    struct Arete* a_successeur;

    //Couleur de l'action trouvépar Welsh-Powell
    int welshpowell;

    //Degre de l'action dans le graphe d'exclusion (pour Welsh-Powell)
    int demidegre;

    //duree de l'action
    float duree;

    //marquage utilisé dans differentes fonctions
    int marquage;

    //Nombres de précedences que possède cette action
    int nbprecedence;

    //Tableau stockant les précédences de cette action
    int* precedence;

    //Nombres d'exclusions que possède cette action
    int nbexclusions;

    //Tableau stockant les exclusions de cette action
    int* exclusion;

    //permet  de savoir si une action existe, par exemplesi dans operations.txt, on a l'operation 16 puis 18, 17
    //n'existe pas
    bool existe;

    //numéro de l'action
    int num;

    //niveau de l'action trouvé grâce a un algo type MPM
    int niveau;

        };


/* Structure d'une arete*/
struct Arete
{

    //numéro de l'opération/action vers qui l'arete se dirige
    int action;

    //Pointe sur une autre arete partant du même sommet
    struct Arete* arete_suivante;
};

//Alias de pointeur sur arete
typedef struct Arete* pArete;

//fonction qui crée une Usine
Usine* CreerUsine(int ordre);


//lit les fichiers texte et enregistre les données
Usine * lire_usine(int* fin);


//Permet de vérifier les informations de chaque action/opération
void verif(Usine* Usine);


int Verification(Usine* usine);


//Libération de la mémoire
void libererMemoireUsine(Usine *usine);






#endif //PROJET_TG_LIGNE_ASSEMBLAGE_2D_USINE_H
