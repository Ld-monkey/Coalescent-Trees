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
  //printf("random float [0,1] : %g\n", rand_float);
  float logarithme = -log(rand_float);
  //printf("-log(%g) : %g\n",rand_float,logarithme);
  float esperance = ((all_individu_recombinaison*(all_individu_recombinaison-1))/2);;
  //printf("esperance : %g\n", esperance);
  float result = logarithme/esperance;
  //printf("logarithme/esperance : %f\n",result);;
  //printf("logarithme/esperance + real_time_recombinaison %f\n",result + real_time_recombinaison);
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
  
  for (int i = 0; i < last_individu + 1; ++i)
    {
      //printf("%f > %f\n",matrix[i].Temps,old_event_coalescence);
      if(old_event_coalescence > matrix[i].Temps){
	new_time = matrix[i].Temps;
	//printf("new_time : %f, et i %d\n",new_time, i);
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
  if ( all_individu_recombinaison != all_individu_coalescent)
    {
      printf("--------------------------------------------------------------\n");
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
      printf("le nombre d'individu pour la recombinaison est %d\n", all_individu_recombinaison);
      printf("le nombre d'individu pour la coalescence est %d\n",all_individu_coalescent);
      if(all_individu_recombinaison != all_individu_coalescent){
	event_coalescent = verif_same_number_individu(matrix, 
						      all_individu_recombinaison, 
						      all_individu_coalescent, 
						      event_recombinaison, 
						      event_coalescent, 
						      nombre_individu,
						      last_individu);
      }
    }
  printf("(AVANT) - individu recombinaison %d = %d individu coalescence\n", all_individu_recombinaison, all_individu_coalescent);
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
    {
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

static void my_pause(void)
{
  puts("Appuyez sur ENTREE pour continuer...");
  getchar();
}

Matrice_arbre coalescent_event(Matrice_arbre *matrix, int individu_selectioned, int recombinaison_individu, float event_coalescent, float event_recombinaison, int last_individu, int *compteur_cache, int *compteur_silencieux, int *compteur_non_silencieux)
{
  printf("Dans la fonction coalescent_event ==> event_recombinaison : %f\n", event_recombinaison);

  // Verifie que le nombre d'individu total soit au minimum à 2 ou plus.
  if (last_individu < 2) 
    {
      printf("Erreur le nombre d'individu doit être > 2 pour appliquer la théorie de la coalescence\n");
      exit(1);
    }
  
  // Compte le nombre de recombinaison dite "silencieuse" et "non-silencieuse".
  if (individu_selectioned != recombinaison_individu)
    {
      /*
	Dans le cas rare ou l'évènement de coalescence se situe au dessus du
	dernier temps du dernier individu et que l'individu de recombinaison
	ou de selection soit un	descendant direct du dernier individu.
      */

      if (event_recombinaison > matrix[last_individu].Temps &&
	  (recombinaison_individu == matrix[last_individu].descendant_1 ||
	   recombinaison_individu == matrix[last_individu].descendant_2))
	{
          //my_pause();
          printf("Cas 1 \n");
          (*compteur_silencieux)++;
	  /*
	    On change seulement le temps du dernier individu = temps de 
	    recombinaison. Avec ce nouveau temps on actualise les longueurs
	    de branches de ces descendants. Si le temps d'un de ces descendants
	    == 0 alors sa longueur de branche est égale au temps de recombinaison.
	    Sinon sa longueur de branche est égale au temps de son ancêtre - son
	    propre temps.
	   */

	  // Changement du temps du dernier individu.
	  matrix[last_individu].Temps = event_recombinaison;

	  // Changement de la longueur de branche du descendant 1.
	  if (matrix[matrix[last_individu].descendant_1].Temps == 0)
	    matrix[matrix[last_individu].descendant_1].longueur_branche = event_recombinaison;
	  else
	    {
	      matrix[matrix[last_individu].descendant_1].longueur_branche =
		matrix[last_individu].Temps - matrix[matrix[last_individu].descendant_1].Temps;
	    }

	  // Changement de la longueur de branche du descendant 2.
	  if (matrix[matrix[last_individu].descendant_2].Temps == 0)
	    matrix[matrix[last_individu].descendant_2].longueur_branche = event_recombinaison;
	  else
	    {
	      matrix[matrix[last_individu].descendant_2].longueur_branche =
		matrix[last_individu].Temps - matrix[matrix[last_individu].descendant_2].Temps;
	    }
	}
      
      /*
	Le cas général ou lorsque l'individu de selection et l'individu de 
	recombinaison ont le même ancêtre mais a la différence du cas 
	précèdent c'est que le temps de recombinaison n'est pas supérieur
	au dernier temps du dernier individu.
       */
      else if (matrix[individu_selectioned].ancetre == matrix[recombinaison_individu].ancetre)
        {
          //my_pause();
          printf("Cas 2 \n");

          // Changement du Temps de l'ancêtre
          matrix[matrix[recombinaison_individu].ancetre].Temps =
            event_recombinaison;

          // Changement du Tems de l'individu de selection
          if (matrix[individu_selectioned].descendant_1 != -1)
          {
              matrix[individu_selectioned].Temps =
                      (event_recombinaison - matrix[individu_selectioned].Temps);
          }

          // Changement du Temps de l'individu de recombinaison
          if (matrix[recombinaison_individu].descendant_1 != -1)
          {
              matrix[recombinaison_individu].Temps =
                      (event_recombinaison - matrix[individu_selectioned].Temps);
          }

          // Changement de la longueur de branche de l'ancêtre commun
          if ( matrix[matrix[recombinaison_individu].ancetre].ancetre == -1 )
          {
              matrix[matrix[recombinaison_individu].ancetre].longueur_branche = 0;
          }
          else
          {
              matrix[matrix[recombinaison_individu].ancetre].longueur_branche =
                      (matrix[matrix[matrix[recombinaison_individu].ancetre].ancetre].Temps -
                       matrix[matrix[recombinaison_individu].ancetre].Temps);
          }

          // Changement de la longueur de branche de l'individu de sélection
          if (matrix[individu_selectioned].descendant_1 == -1)
          {
              matrix[individu_selectioned].longueur_branche = event_recombinaison;
          }
          else
          {
              matrix[individu_selectioned].longueur_branche =
                      (matrix[matrix[recombinaison_individu].ancetre].Temps -
                      matrix[individu_selectioned].Temps);
          }

          // Changement de la longueur de branche de l'individu de recombinaison
          if (matrix[individu_selectioned].descendant_1 == -1)
          {
              matrix[recombinaison_individu].longueur_branche = event_recombinaison;
          }
          else
          {
              matrix[recombinaison_individu].longueur_branche =
                      (matrix[matrix[recombinaison_individu].ancetre].Temps -
                      matrix[recombinaison_individu].Temps);
          }
        }

      /*
      Dans le cas ou l'évènement de recombinaison est au dessus de
       l'individu de recombinaison sans pour autant que le temps
       de recombinaison dépasse de dernier temps. A la différence des
       deux autres algorithmes précédant l'individu de recombinaison n'a
       pas d'ancêtre commun avec l'individu selectionné. Néanmoins ils ont
       entre eux une distance de 1 branche donc l'individu de selection
       a pour descendant 1 ou 2 de l'individu de recombinaison (ce qui
       assure une distance de 1 branche). A contrario, si cette distance d'une
       branche n'est pas respectée on n'est plus dans une cas de recombinaison
       silencieuse mais dans un cas de recombinaison non silencieuse avec
       changement topologique.
       */
      else if (recombinaison_individu == matrix[individu_selectioned].descendant_1
	       || recombinaison_individu == matrix[individu_selectioned].descendant_2)
        {
          //my_pause();
          printf("Cas 3 \n");
          (*compteur_silencieux)++;

          /*
           Changement du Temps de l'ancêtre de l'individu de recombinaison ==
           individu selectionné
           */
          matrix[individu_selectioned].Temps = event_recombinaison;

          // Changement de la longueur de branche de l'individu selectionné
          matrix[individu_selectioned].longueur_branche =
                  matrix[matrix[individu_selectioned].ancetre].Temps -
                  matrix[individu_selectioned].Temps;

          // Changement de la longueur de branche du descendant 1 de l'individu selectionné
          if (matrix[matrix[individu_selectioned].descendant_1].descendant_1 == -1)
              matrix[matrix[individu_selectioned].descendant_1].longueur_branche = event_recombinaison;
          else
          {
              matrix[matrix[individu_selectioned].descendant_1].longueur_branche =
                      matrix[individu_selectioned].Temps -
                              matrix[matrix[individu_selectioned].descendant_1].Temps;
          }

          // Changement de la longueur de branche du descendant 2 de l'individu selectionné
          if (matrix[matrix[individu_selectioned].descendant_2].descendant_1 == -1)
              matrix[matrix[individu_selectioned].descendant_2].longueur_branche = event_recombinaison;
          else
          {
              matrix[matrix[individu_selectioned].descendant_2].longueur_branche =
                      matrix[individu_selectioned].Temps -
                              matrix[matrix[individu_selectioned].descendant_2].Temps;
          }
        }

      /*
       A ce niveau toutes les possibilités sur la recombinaison silencieuse
       ont été testées. Le cas des recombinaisons non-silencieuses
       avec changement topologique sont testées. Une caractéristique commune
       a toutes les recombinaisons non-silencieuses c'est qu'elles ont aucun
       ancêtre en commun et une distance de branche entre l'individu de
       recombinaison et l'individu de sélection > à 1.
       */
      else if(matrix[individu_selectioned].ancetre != matrix[recombinaison_individu].ancetre)
        {
          (*compteur_non_silencieux)++;

          /*
           * On peut généraliser le changement topoplogie d'une recombininaison
           * non-silencieuse en 3 cas. Il y a lorsque l'individu de selection a
           * pour ancêtre -1, lorsque l'ancetre de l'ancetre de recombinaison == -1
           * et l'ancetre de l'ancetre de recombinaison est différent de -1.
           */

          // Prenons le premiers cas lorsque l'individu de selection a pour ancetre -1.
          if (matrix[individu_selectioned].ancetre == -1)
          {
              if (recombinaison_individu == matrix[matrix[recombinaison_individu].ancetre].descendant_1)
              {
                  printf("Cas 4.1\n");
                  // Pour les descendants de l'indvidu de sélection
                  int descendant_1_individu_selection, descendant_2_individu_selection;

                  // Pour les descendants de l'ancetre de l'individu de recombinaison
                  int descendant_1_ancetre_recombinaison, descendant_2_ancetre_recombinaison;

                  // Si on est a une distance d'une branche.
                  if (matrix[recombinaison_individu].ancetre == matrix[individu_selectioned].descendant_1)
                  {
                      descendant_1_individu_selection =
                              matrix[matrix[recombinaison_individu].ancetre].descendant_2;
                      descendant_2_individu_selection = matrix[individu_selectioned].descendant_2;
                  }
                  else if (matrix[recombinaison_individu].ancetre == matrix[individu_selectioned].descendant_2)
                  {
                      descendant_1_individu_selection = matrix[individu_selectioned].descendant_1;
                      descendant_2_individu_selection =
                              matrix[matrix[recombinaison_individu].ancetre].descendant_2;
                  }
                  else // Si on est a une distance supérieur a une branche
                  {
                      int ancetre_suivant = matrix[recombinaison_individu].ancetre;
                      int ancetre_precedant;


                      while (ancetre_suivant != -1)
                      {
                          printf("Cas 4.1\n");
                          printf("ancetre suivant = %d\n",ancetre_suivant);
                          printf("individu_selectionned = %d\n", individu_selectioned);
                          printf("individu recombinaison = %d\n", recombinaison_individu);
                          if (ancetre_suivant == individu_selectioned)
                          {
                              if (ancetre_precedant == matrix[individu_selectioned].descendant_1)
                              {
                                  descendant_1_individu_selection =
                                          matrix[matrix[recombinaison_individu].ancetre].descendant_2;
                                  descendant_2_individu_selection = matrix[individu_selectioned].descendant_2;
                              }
                              else if (ancetre_precedant == matrix[individu_selectioned].descendant_2)
                              {
                                  descendant_1_individu_selection = matrix[individu_selectioned].descendant_1;
                                  descendant_2_individu_selection =
                                          matrix[matrix[recombinaison_individu].ancetre].descendant_2;
                              }
                              ancetre_suivant = -1;
                          }
                          else
                          {
                              ancetre_precedant = ancetre_suivant;
                              printf("ancetre precedant : %d",ancetre_precedant);
                              ancetre_suivant = matrix[ancetre_suivant].ancetre;
                              printf("ancetre suivant : %d",ancetre_suivant);
                          }
                      }
                  }

                  if (recombinaison_individu == matrix[matrix[recombinaison_individu].ancetre].descendant_1)
                  {
                      descendant_1_ancetre_recombinaison = matrix[matrix[recombinaison_individu].ancetre].descendant_1;
                      descendant_2_ancetre_recombinaison = individu_selectioned;
                  }
                  else if (recombinaison_individu == matrix[matrix[recombinaison_individu].ancetre].descendant_2)
                  {
                      descendant_1_ancetre_recombinaison = individu_selectioned;
                      descendant_2_ancetre_recombinaison = matrix[matrix[recombinaison_individu].ancetre].descendant_2;
                  }
                  // Peut être envisager un retour de matrix

                  // Pour l'ancetre de l'individu de recombinaison a pour ancêtre -1
                  matrix[matrix[recombinaison_individu].ancetre].ancetre = -1;

                  // Une longueur de branche = 0
                  matrix[matrix[recombinaison_individu].ancetre].longueur_branche = 0;

                  // Pour l'ancetre de l'individu selectionné = ancetre de l'individu de recombinaison
                  matrix[individu_selectioned].ancetre = matrix[recombinaison_individu].ancetre;

                  // Pour les descendants de l'individu selectionné
                  matrix[individu_selectioned].descendant_1 = descendant_1_individu_selection;
                  matrix[individu_selectioned].descendant_2 = descendant_2_individu_selection;

                  // Pour les descendants de l'ancetre de recombinaison
                  matrix[matrix[recombinaison_individu].ancetre].descendant_1 = descendant_1_ancetre_recombinaison;
                  matrix[matrix[recombinaison_individu].ancetre].descendant_2 = descendant_2_ancetre_recombinaison;
              }

              if (recombinaison_individu == matrix[matrix[recombinaison_individu].ancetre].descendant_2)
              {
                  printf("Cas 4.2\n");
                  // Pour les descendants de l'indvidu de sélection
                  int descendant_1_individu_selection, descendant_2_individu_selection;

                  // Pour les descendants de l'ancetre de l'individu de recombinaison
                  int descendant_1_ancetre_recombinaison, descendant_2_ancetre_recombinaison;

                  // Si on est a une distance d'une branche.
                  if (matrix[recombinaison_individu].ancetre == matrix[individu_selectioned].descendant_1)
                  {
                      descendant_1_individu_selection =
                              matrix[matrix[recombinaison_individu].ancetre].descendant_1;
                      descendant_2_individu_selection = matrix[individu_selectioned].descendant_2;
                  }
                  else if (matrix[recombinaison_individu].ancetre == matrix[individu_selectioned].descendant_2)
                  {
                      descendant_1_individu_selection = matrix[individu_selectioned].descendant_1;
                      descendant_2_individu_selection =
                              matrix[matrix[recombinaison_individu].ancetre].descendant_1;
                  }
                  else // Si on est a une distance supérieur a une branche
                  {
                      int ancetre_suivant = matrix[recombinaison_individu].ancetre;
                      int ancetre_precedant;

                      while (ancetre_suivant != -1)
                      {
                          printf("Cas 4.2\n");
                          printf("ancetre suivant = %d\n",ancetre_suivant);
                          printf("individu_selectionned = %d\n", individu_selectioned);
                          printf("individu recombinaison = %d\n", recombinaison_individu);
                          if (ancetre_suivant == individu_selectioned)
                          {
                              if (ancetre_precedant == matrix[individu_selectioned].descendant_1)
                              {
                                  descendant_1_individu_selection =
                                          matrix[matrix[recombinaison_individu].ancetre].descendant_1;
                                  descendant_2_individu_selection = matrix[individu_selectioned].descendant_2;
                              }
                              else if (ancetre_precedant == matrix[individu_selectioned].descendant_2)
                              {
                                  descendant_1_individu_selection = matrix[individu_selectioned].descendant_1;
                                  descendant_2_individu_selection =
                                          matrix[matrix[recombinaison_individu].ancetre].descendant_1;
                              }
                              ancetre_suivant = -1;
                          }
                          else
                          {
                              ancetre_precedant = ancetre_suivant;
                              printf("ancetre precedant : %d",ancetre_precedant);
                              ancetre_suivant = matrix[ancetre_suivant].ancetre;
                              printf("ancetre suivant : %d",ancetre_suivant);
                          }
                      }
                  }
                  // Peut être envisager un retour de matrix
                  if (recombinaison_individu == matrix[matrix[recombinaison_individu].ancetre].descendant_1)
                  {
                      descendant_1_ancetre_recombinaison = matrix[matrix[recombinaison_individu].ancetre].descendant_1;
                      descendant_2_ancetre_recombinaison = individu_selectioned;
                  }
                  else if (recombinaison_individu == matrix[matrix[recombinaison_individu].ancetre].descendant_2)
                  {
                      descendant_1_ancetre_recombinaison = individu_selectioned;
                      descendant_2_ancetre_recombinaison = matrix[matrix[recombinaison_individu].ancetre].descendant_2;
                  }

                  // Pour l'ancetre de l'individu de recombinaison a pour ancêtre -1
                  matrix[matrix[recombinaison_individu].ancetre].ancetre = -1;

                  // Une longueur de branche = 0
                  matrix[matrix[recombinaison_individu].ancetre].longueur_branche = 0;

                  // Pour l'ancetre de l'individu selectionné = ancetre de l'individu de recombinaison
                  matrix[individu_selectioned].ancetre = matrix[recombinaison_individu].ancetre;

                  // Pour les descendants de l'individu selectionné
                  matrix[individu_selectioned].descendant_1 = descendant_1_individu_selection;
                  matrix[individu_selectioned].descendant_2 = descendant_2_individu_selection;

                  // Pour les descendants de l'ancetre de recombinaison
                  matrix[matrix[recombinaison_individu].ancetre].descendant_1 = descendant_1_ancetre_recombinaison;
                  matrix[matrix[recombinaison_individu].ancetre].descendant_2 = descendant_2_ancetre_recombinaison;
              }
              return *matrix;
          }

          // Prenons le 2nd cas lorsque l'ancetre de l'ancetre de recombinaison == -1.
          if (matrix[matrix[recombinaison_individu].ancetre].ancetre == -1)
          {
              printf("Cas 5");
              // Les descendants de l'ancêtre de recombinaison
              int descendant_1_ancetre_recombinaison, descendant_2_ancetre_recombinaison;

              // Les descendants de l'ancêtre de l'individu de selection
              int descendant_1_ancetre_individu_selection, descendant_2_ancetre_individu_selection;

              // Pour l'ancetre de l'ancetre de l'individu de recombinaison;
              int ancetre_ancetre_individu_recombinaison;

              if (recombinaison_individu == matrix[matrix[recombinaison_individu].ancetre].descendant_1)
              {
                  matrix[matrix[matrix[recombinaison_individu].ancetre].descendant_2].ancetre =
                          -1;
              }
              else if (recombinaison_individu == matrix[matrix[recombinaison_individu].ancetre].descendant_2)
              {
                  matrix[matrix[matrix[recombinaison_individu].ancetre].descendant_1].ancetre =
                          -1;
              }

              // Au niveau des descendants de l'ancêtre de recombinaison
              if (recombinaison_individu == matrix[matrix[recombinaison_individu].ancetre].descendant_1)
              {
                  descendant_1_ancetre_recombinaison = recombinaison_individu;
                  // on change le descendant 2 de l'ancetre de recombinaison = individu_selection.
                  descendant_2_ancetre_recombinaison = individu_selectioned;
              }
              else if (recombinaison_individu == matrix[matrix[recombinaison_individu].ancetre].descendant_2)
              {
                  // on change le descendant 1 de l'ancetre de recombinaison = individu selection.
                  descendant_1_ancetre_recombinaison = individu_selectioned;
                  descendant_1_ancetre_recombinaison = recombinaison_individu;
              }

              // Au niveau des descendants de l'ancêtre de selection
              if (individu_selectioned == matrix[matrix[individu_selectioned].ancetre].descendant_1)
              {
                  // on modifie le descendant 1 de l'ancetre de selection
                  descendant_1_ancetre_individu_selection = matrix[recombinaison_individu].ancetre;
                  descendant_2_ancetre_individu_selection =
                          matrix[matrix[individu_selectioned].ancetre].descendant_2;
              }
              else if (individu_selectioned == matrix[matrix[individu_selectioned].ancetre].descendant_2)
              {
                  descendant_1_ancetre_individu_selection =
                          matrix[matrix[individu_selectioned].ancetre].descendant_1;
                  // on modifie le descendant 2 le l'ancetre de selection
                  descendant_2_ancetre_individu_selection = matrix[recombinaison_individu].ancetre;
              }

              // Au niveau de l'ancêtre de l'ancêtre de l'individu de recombinaison
              ancetre_ancetre_individu_recombinaison = matrix[individu_selectioned].ancetre;

              // Pour les descendants de l'ancetre de recombinaison
              matrix[matrix[recombinaison_individu].ancetre].descendant_1 = descendant_1_ancetre_recombinaison;
              matrix[matrix[recombinaison_individu].ancetre].descendant_2 = descendant_2_ancetre_recombinaison;

              // Pour les descendants de l'ancetre de l'individu de selection
              matrix[matrix[individu_selectioned].ancetre].descendant_1 = descendant_1_ancetre_individu_selection;
              matrix[matrix[individu_selectioned].ancetre].descendant_2 = descendant_2_ancetre_individu_selection;

              // Pour l'ancetre de l'ancetre de l'individu de recombinaison
              matrix[matrix[recombinaison_individu].ancetre].ancetre = ancetre_ancetre_individu_recombinaison;

              return *matrix;
          }

          // Prenons le dernier cas lorsque l'ancetre de l'ancetre de recombinaison != -1.
          if (matrix[matrix[recombinaison_individu].ancetre].ancetre != -1)
          {
              printf("Cas 6\n");
              // Les descendants de l'ancêtre de recombinaison
              int descendant_1_ancetre_recombinaison, descendant_2_ancetre_recombinaison;

              // Les descendants de l'ancêtre de l'individu de selection
              int descendant_1_ancetre_individu_selection, descendant_2_ancetre_individu_selection;

              // Pour l'ancetre de l'ancetre de l'individu de recombinaison;
              int ancetre_ancetre_individu_recombinaison;

              // Pour les descendants le l'individu de selection (qui peuvent changer dans de rare cas).
              int descendant_1_individu_selection, descendant_2_individu_selection;

              // Au niveau des descendants de l'ancêtre de recombinaison
              if (recombinaison_individu == matrix[matrix[recombinaison_individu].ancetre].descendant_1)
              {
                  descendant_1_ancetre_recombinaison = recombinaison_individu;
                  // on change le descendant 2 de l'ancetre de recombinaison = individu_selection.
                  descendant_2_ancetre_recombinaison = individu_selectioned;
                  /* et le descendant 2 par rapport a l'ancetre de recombinaison a pour ancetre
                   * l'ancetre de l'ancetre de recombinaison */
                  matrix[matrix[matrix[recombinaison_individu].ancetre].descendant_2].ancetre =
                          matrix[matrix[recombinaison_individu].ancetre].ancetre;
              }
              else if (recombinaison_individu == matrix[matrix[recombinaison_individu].ancetre].descendant_2)
              {
                  // on change le descendant 1 de l'ancetre de recombinaison = individu selection.
                  descendant_1_ancetre_recombinaison = individu_selectioned;
                  descendant_2_ancetre_recombinaison = recombinaison_individu;
                  /* et le descendant 2 par rapport a l'ancetre de recombinaison a pour ancetre
                   * l'ancetre de l'ancetre de recombinaison */
                  matrix[matrix[matrix[recombinaison_individu].ancetre].descendant_1].ancetre =
                          matrix[matrix[recombinaison_individu].ancetre].ancetre;
              }

              // Au niveau des descendants de l'individu de selection
              if (matrix[individu_selectioned].descendant_1 != -1 &&
              matrix[individu_selectioned].descendant_2 != -1)
              {
                  //on fait le test pour savoir s'ils ont un ancetre en commun
                  int ancetre_suivant = matrix[recombinaison_individu].ancetre;
                  int ancetre_precedant;

                  while (ancetre_suivant != -1)
                  {
                      printf("Cas 6.2\n");
                      printf("ancetre suivant = %d\n",ancetre_suivant);
                      printf("individu_selectionned = %d\n", individu_selectioned);
                      printf("individu recombinaison = %d\n", recombinaison_individu);
                      if (ancetre_suivant == individu_selectioned)
                      {
                          if (ancetre_precedant == matrix[individu_selectioned].descendant_1)
                          {
                              if (recombinaison_individu == matrix[matrix[recombinaison_individu].ancetre].descendant_1)
                              {
                                  descendant_1_individu_selection =
                                          matrix[matrix[recombinaison_individu].ancetre].descendant_2;
                                  descendant_2_individu_selection = matrix[individu_selectioned].descendant_2;
                              }
                              else if (recombinaison_individu == matrix[matrix[recombinaison_individu].ancetre].descendant_2)
                              {
                                  descendant_1_individu_selection =
                                          matrix[matrix[recombinaison_individu].ancetre].descendant_1;
                                  descendant_2_individu_selection = matrix[individu_selectioned].descendant_2;
                              }
                          }
                          if (ancetre_precedant == matrix[individu_selectioned].descendant_2)
                          {
;                             if (recombinaison_individu == matrix[matrix[recombinaison_individu].ancetre].descendant_1)
                              {
                                  descendant_1_individu_selection = matrix[individu_selectioned].descendant_1;
                                  descendant_2_individu_selection =
                                          matrix[matrix[recombinaison_individu].ancetre].descendant_2;
                              }
                              else if (recombinaison_individu == matrix[matrix[recombinaison_individu].ancetre].descendant_2)
                              {
                                  descendant_1_individu_selection = matrix[individu_selectioned].descendant_1;
                                  descendant_2_individu_selection =
                                          matrix[matrix[recombinaison_individu].ancetre].descendant_1;
                              }
                          }
                          ancetre_suivant = -1;
                      }
                      else
                      {
                          ancetre_precedant = ancetre_suivant;
                          printf("ancetre precedant : %d",ancetre_precedant);
                          ancetre_suivant = matrix[ancetre_suivant].ancetre;
                          printf("ancetre suivant : %d",ancetre_suivant);
                          descendant_1_individu_selection = matrix[individu_selectioned].descendant_1;
                          descendant_2_individu_selection = matrix[individu_selectioned].descendant_2;
                      }
                  }
              }
              else
              {
                  //On ne modifie pas les descendants des individus de selection
                  descendant_1_individu_selection = matrix[individu_selectioned].descendant_1;
                  descendant_2_individu_selection = matrix[individu_selectioned].descendant_2;
              }

              // Au niveau des descendants de l'ancêtre de selection
              if (individu_selectioned == matrix[matrix[individu_selectioned].ancetre].descendant_1)
              {
                  // on modifie le descendant 1 de l'ancetre de selection
                  descendant_1_ancetre_individu_selection = matrix[recombinaison_individu].ancetre;
                  descendant_2_ancetre_individu_selection =
                          matrix[matrix[individu_selectioned].ancetre].descendant_2;
              }
              else if (individu_selectioned == matrix[matrix[individu_selectioned].ancetre].descendant_2)
              {
                  //descendant_1_ancetre_individu_selection = matrix[matrix[individu_selectioned].ancetre].descendant_1;
                  descendant_1_ancetre_individu_selection = matrix[recombinaison_individu].ancetre;
                  // on modifie le descendant 2 le l'ancetre de selection
                  //descendant_2_ancetre_individu_selection = matrix[recombinaison_individu].ancetre;
                  descendant_2_ancetre_individu_selection = matrix[matrix[recombinaison_individu].ancetre].descendant_1;
              }

              // Au niveau de l'ancêtre de l'ancêtre de l'individu de recombinaison
              ancetre_ancetre_individu_recombinaison = matrix[individu_selectioned].ancetre;

              // Pour les descendants de l'ancetre de recombinaison
              matrix[matrix[recombinaison_individu].ancetre].descendant_1 = descendant_1_ancetre_recombinaison;
              matrix[matrix[recombinaison_individu].ancetre].descendant_2 = descendant_2_ancetre_recombinaison;

              // Pour les descendants de l'individu de selection
              matrix[individu_selectioned].descendant_1 = descendant_1_individu_selection;
              matrix[individu_selectioned].descendant_2 = descendant_2_individu_selection;

              // Pour les descendants de l'ancetre de l'individu de selection
              matrix[matrix[individu_selectioned].ancetre].descendant_1 = descendant_1_ancetre_individu_selection;
              matrix[matrix[individu_selectioned].ancetre].descendant_2 = descendant_2_ancetre_individu_selection;

              // Pour l'ancetre de l'ancetre de l'individu de recombinaison
              matrix[matrix[recombinaison_individu].ancetre].ancetre = ancetre_ancetre_individu_recombinaison;

              return *matrix;
          }
          return *matrix;
        }
    }
    else
    {
      printf("On ne change rien car la coalescence se situe sur la meme branche.\n");
      (*compteur_cache)++;
      return *matrix;
    }
  return *matrix;
}
