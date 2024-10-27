#ifndef PROJET_TG_LIGNE_ASSEMBLAGE_2D_PRECEDENCES_H
#define PROJET_TG_LIGNE_ASSEMBLAGE_2D_PRECEDENCES_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <values.h>
#include <stdbool.h>
#include "usine.h"

typedef struct {
    pAction* elements;
    int taille;
    int capacite;
} Pile;

void ajouterPrecedence(pAction action, int precedence);


void Niveaux(Usine* usine);

void PrecedencesEtTemps(Usine* usine);

pAction* CreerAretePrecedence(pAction* action,int s1,int s2);

void PrecedenceExclusionTemps(Usine *usine);

void dfs2contraintes(Usine *usine, pAction action, pStation stationCourante, float *tempsCourant);

void PrecedenceExclusion(Usine *usine);

void dfs3contraintes(Usine *usine, pAction action, pStation stationCourante, float *tempsCourant);




#endif //PROJET_TG_LIGNE_ASSEMBLAGE_2D_PRECEDENCES_H
