#ifndef EVENTS_H
#define EVENTS_H

#include "matrice_arbre.h"

extern float random_recombinaison(float Temps);
extern int get_individu_event_recombinaison(Matrice_arbre *matrix, float event_recombinaison, int last_individu);
extern float get_time_event_recombinaison(Matrice_arbre *matrix, float event_recombinaison, int last_individu);
extern int get_all_individu_concerned_by_event(Matrice_arbre *matrix, float event_recombinaison, int nombre_individu);
extern float random_event_coalescencence(int all_individu_recombinaison, float real_time_recombinaison);
extern int get_individu_event_coalescence(Matrice_arbre *matrix, float event_recombinaison, int last_individu);
extern float find_closest_time(Matrice_arbre *matrix, float old_event_coalescence, int last_individu);
extern float verif_same_number_individu(Matrice_arbre *matrix, int all_individu_recombinaison, int all_individu_coalescent ,float event_recombinaison, float event_coalescent, int nombre_individu, int last_individu);
extern void determine(Matrice_arbre *matrix, int *array , int *lenght, int nombre_individu ,int last_individu, float event_coalescent);
extern Matrice_arbre coalescent_event(Matrice_arbre *matrix, int individu_selectioned, int recombinaison_individu, float event_coalescent, float event_recombinaison, int last_individu, int *compteur_cache, int *compteur_silencieux, int *compteur_non_silencieux);

#endif /*EVENTS_H*/