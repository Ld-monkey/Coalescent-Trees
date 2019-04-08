#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "matrice_arbre.h"
#include "tableau.h"
#include "random.h"
#include "events.h"

//retourne un nombre aléatoire dans un interval de temps
float random_recombinaison(float Temps)
{
  float result;
  do{
    result = (float)rand()/RAND_MAX*(Temps);//mise à l'échelle en multipliant
  }while(result == 0.0 || result >= Temps);
  return result;
}

//retourne l'individu ou est la recombinaison
int get_individu_event_recombinaison(Matrice_arbre *matrix, float event_recombinaison, int last_individu)
{
  float result = matrix[0].longueur_branche;
  int count = 0;
  for (int i = 1; i < last_individu; ++i)
  {
    if (result >= event_recombinaison)
      return count ;
    else{
      result += matrix[i].longueur_branche;
      count++;
    }
  }
  return count;
}

//retourne le temps ou a eu lieu la recombinaison
float get_time_event_recombinaison(Matrice_arbre *matrix, float event_recombinaison, int last_individu)
{
  float result = matrix[0].longueur_branche;
  for (int i = 1; i < last_individu + 1; ++i)
  {
    if (result >= event_recombinaison) // matrix[i-1] car la boucle commendce a 1 et fini a last_individu + 1    
      return ( matrix[i-1].Temps + (matrix[i-1].longueur_branche - (matrix[i-1].somme_lb - event_recombinaison)));
    else
      result += matrix[i].longueur_branche;
  }
  return -1;
}


// retourne le nombre d'individu pour un évènement de recombinaison ou de coalescence
int get_all_individu_concerned_by_event(Matrice_arbre *matrix, float event_recombinaison, int nombre_individu)
{
  for (int i = nombre_individu; i < (nombre_individu * 2)-1; ++i)
  {
    //printf("matrix[%d].Temps = %f ",i, matrix[i].Temps);
    //printf("et l'évènement de recombinaison est %f\n",event_recombinaison);
    if (event_recombinaison < matrix[i].Temps )
    {
      //printf("le nombe d'individu %d \n",nombre_individu);
      //printf("le nombre d'individu est %d\n", (nombre_individu - (i - nombre_individu)));
      return(nombre_individu - (i - nombre_individu));
    }
  }
  return -1;
}

//faire un évènement -log(U)/E[i] ou random int => U = ]0,1[ et E[i] = (i*(i-1))/2
//très similaire a random_Ti
float random_event_coalescencence(int all_individu_recombinaison, float real_time_recombinaison)
{
  //printf("--------------------------------------------------------------\n");
  float rand_float = random_float();
  printf("random float [0,1] : %g\n", rand_float);
  float logarithme = -log(rand_float);
  //printf("-log(%g) : %g\n",rand_float,logarithme);
  float esperance = ((all_individu_recombinaison*(all_individu_recombinaison-1))/2);;
  //printf("esperance : %g\n", esperance);
  float result = logarithme/esperance;
  //printf("logarithme/esperance : %f\n",result);;
  printf("logarithme/esperance + real_time_recombinaison %f\n",result + real_time_recombinaison);
  return (result + real_time_recombinaison);
}

int get_individu_event_coalescence(Matrice_arbre *matrix, float event_recombinaison, int last_individu)
{
  float result = matrix[0].Temps;
  int count = 0;
  for (int i = 1; i < last_individu; ++i)
  {
    if (result >= event_recombinaison)
      return count ;
    else{
      result += matrix[i].Temps;
      count++;
    }
  }
  return count;
}

//on peut modifier last individu par (2*N)-1 ou N = nombre d'individu
float 
find_closest_time(Matrice_arbre *matrix, float old_event_coalescence, int last_individu)
{
  float new_time = 0;
  printf("Rappel l'évènement de coalescente a eu lieu : %f\n", old_event_coalescence);
  //printf("last_individu : %d\n",last_individu);
  
  for (int i = 0; i < last_individu; ++i)
  {
    //printf("%f > %f\n",matrix[i].Temps,old_event_coalescence);
    if(old_event_coalescence > matrix[i].Temps){
      new_time = matrix[i].Temps;
    }
  }
  return new_time;
}

//condition (on peux enlever le last_individu est mettre (2*N)-1)
float 
verif_same_number_individu(Matrice_arbre *matrix, 
                                int all_individu_recombinaison, 
                                int all_individu_coalescent ,
                                float event_recombinaison,
                                float event_coalescent, 
                                int nombre_individu,
                                int last_individu)
{
    printf("--------------------------------------------------------------\n");
    if ( all_individu_recombinaison != all_individu_coalescent)
    {
        printf("individu recombinaison %d ≠ %d individu coalescence\n", all_individu_recombinaison, all_individu_coalescent);   
        //on a deja le nombre d'individu = all_individu_coalescent ( qui est différent)
        //+ trouver l'ancêtre Tx ( et plus le temps réel de recombinaison) qui lui correspond 
        //et lui ajouter cette evenement de coalescence précèdent
        //redeterminer un evènement de coalescence (en fonction de ce nouveaux nombre d'individu) 
        //et on reregarde si le nombre d'individu n'a pas changé
        //afficher les résultats
        float closest_time;
        closest_time = find_closest_time(matrix, event_coalescent, last_individu);
        printf("Le temps le plus proche est %f.\n",closest_time);
        //on recalcule un temps de coalescente
        event_coalescent = random_event_coalescencence(all_individu_coalescent, closest_time);
        printf("L'évènement de coalescence a lieu en %f.\n",event_coalescent);

        all_individu_recombinaison = get_all_individu_concerned_by_event(matrix, closest_time, nombre_individu);
        all_individu_coalescent = get_all_individu_concerned_by_event(matrix, event_coalescent, nombre_individu);
        //printf("le nombre d'individu pour la recombinaison est %d\n", all_individu_recombinaison);
        //printf("le nombre d'individu pour la coalescence est %d\n",all_individu_coalescent);
        if(all_individu_recombinaison != all_individu_coalescent){
          event_coalescent = verif_same_number_individu(matrix, 
                                    all_individu_recombinaison, 
                                    all_individu_coalescent, 
                                    event_recombinaison, 
                                    event_coalescent, 
                                    nombre_individu,
                                    last_individu);
        }
    }else{
      printf("individu recombinaison %d = %d individu coalescence\n", all_individu_recombinaison, all_individu_coalescent);
    }
  return event_coalescent; 
}

void 
determine(Matrice_arbre *matrix, int *array, int *lenght, int nombre_individu ,int last_individu, float event_coalescent)
{
	//event_coalescent = 5.0;											
	if(event_coalescent >= matrix[last_individu].Temps && ((nombre_individu*2)-2)  == last_individu){
			//printf("%d\n",matrix[last_individu].individu);
			add_value_in_table(array, matrix[last_individu].individu, *lenght);
			*lenght += 1;
			//printf("lenght %d\n",*lenght);
	}
	if (event_coalescent >= matrix[matrix[last_individu].descendant_1].Temps &&
		event_coalescent <= matrix[last_individu].Temps)
	{
		add_value_in_table(array, matrix[last_individu].descendant_1, *lenght);
		*lenght += 1;
		//printf("lenght %d\n",*lenght);
		//printf("%d\n",matrix[last_individu].descendant_1);
	}
	if (event_coalescent >= matrix[matrix[last_individu].descendant_2].Temps &&
		event_coalescent <= matrix[last_individu].Temps)
	{;
		add_value_in_table(array, matrix[last_individu].descendant_2, *lenght);
		*lenght += 1;
		//printf("lenght %d\n",*lenght);
		//printf("%d\n",matrix[last_individu].descendant_2);
	}
	if( matrix[last_individu].descendant_1 != -1 || matrix[last_individu].descendant_2 != -1){
		determine(matrix, array, lenght, nombre_individu, matrix[last_individu].descendant_1, event_coalescent);
		determine(matrix, array, lenght, nombre_individu, matrix[last_individu].descendant_2, event_coalescent);
	}

}