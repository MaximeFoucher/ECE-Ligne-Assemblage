#ifndef PROJET_TG_LIGNE_ASSEMBLAGE_2D_EXCLUSION_H
#define PROJET_TG_LIGNE_ASSEMBLAGE_2D_EXCLUSION_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <values.h>
#include <stdbool.h>
#include "usine.h"


void ajouterExclusion(pAction action, int exclusion);

void WelshPowell(Usine* usine);

void Exclusion(Usine* usine);

void ExclusionEtTemps(Usine* usine);

void ExclusionEtPrecedences(Usine* usine);

void trierStationsParCouleur(pStation* stations, int totalStations);



#endif //PROJET_TG_LIGNE_ASSEMBLAGE_2D_EXCLUSION_H


