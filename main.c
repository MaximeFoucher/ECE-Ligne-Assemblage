#include "usine.h"
#include "exclusion.h"
#include "precedences.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <values.h>
#include <stdbool.h>

int main(){

    Usine * usine;

    //On utilise cette variable pour vérifier qu'il n y a aucune erreur dans les ficiers texte
    int exit=0;

    //On crée l'usine, etc et on lit les fichiers textes
    usine=lire_usine(&exit);
    //on modifie la variable qui permet de verifier dans lire usine
    if(exit==1){
        return 1;
        //si il y a un probleme la variable devient 1 et stop le code
    }


    //On applique l'algorithme de Welsh-Powell sur les opérations de l'usine
    WelshPowell(usine);

    //On applique l'algorithme MPM (seulement la premiere etape avec les niveaux) sur les opérations de l'usine
    Niveaux(usine);

    //Fonction qui permet de vérifier le bon enregistrement des informations des fichiers texte, et du bon déroulement
    //des algorithmes WelshPowell et Niveaux, pas besoin d'activer cette fonction pour le projet
    //verif(usine);

    //Contrainte d'exclusion
    Exclusion(usine);

    //Contraintes d'exclusion et de temps
    ExclusionEtTemps(usine);

    //Contraintes de précédences et de temps
    PrecedencesEtTemps(usine);

    //Contraintes de précédences et d'exclusion
    PrecedenceExclusion(usine);

    //Contraintes de Precedence, exclusion et temps
    PrecedenceExclusionTemps(usine);

    //On libere tout ce qui été alloué au cours des fonctions
    libererMemoireUsine(usine);






    return 0;
}