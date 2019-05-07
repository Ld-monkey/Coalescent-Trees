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
        //printf("Le temps le plus proche est %f.\n",closest_time);
        //on recalcule un temps de coalescente
        event_coalescent = random_event_coalescencence(all_individu_coalescent, closest_time);
        //printf("L'évènement de coalescence a lieu en %f.\n",event_coalescent);

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
    }
  //printf("individu recombinaison %d = %d individu coalescence\n", all_individu_recombinaison, all_individu_coalescent);
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

Matrice_arbre coalescent_event(Matrice_arbre *matrix, int individu_selectioned, int recombinaison_individu, float event_coalescent, int last_individu, int *compteur_cache, int *compteur_silencieux, int *compteur_non_silencieux)
{
  if (individu_selectioned != recombinaison_individu)
    {
        printf("individu_selectioned %d != %d recombinaison_individu\n",individu_selectioned, recombinaison_individu);
        //dans le cas rare ou l'évènement de coalescence se situe au dessus du dernier temps
        if (event_coalescent > matrix[last_individu].Temps )
        {
            (*compteur_silencieux)++;            //faire attention a la somme des descendant
            if (matrix[matrix[last_individu].descendant_1].descendant_1 == -1 &&
                matrix[matrix[last_individu].descendant_1].descendant_2 == -1)
            {
                //si tout va bien 
                matrix[matrix[last_individu].descendant_1].longueur_branche += (event_coalescent-matrix[matrix[last_individu].descendant_1].longueur_branche);
            }else{
                if (matrix[matrix[matrix[last_individu].descendant_1].descendant_1].longueur_branche > 
                    matrix[matrix[matrix[last_individu].descendant_1].descendant_2].longueur_branche)
                {
                    matrix[matrix[last_individu].descendant_1].longueur_branche += 
                    (event_coalescent-matrix[matrix[last_individu].descendant_1].longueur_branche);
                    matrix[matrix[last_individu].descendant_1].longueur_branche -= 
                    matrix[matrix[matrix[last_individu].descendant_1].descendant_1].longueur_branche;
                }else{
                    matrix[matrix[last_individu].descendant_1].longueur_branche += 
                    (event_coalescent-matrix[matrix[last_individu].descendant_1].longueur_branche);
                    matrix[matrix[last_individu].descendant_1].longueur_branche -= 
                    matrix[matrix[matrix[last_individu].descendant_1].descendant_2].longueur_branche;
                }
            }
            if (matrix[matrix[last_individu].descendant_2].descendant_1 == -1 &&
                matrix[matrix[last_individu].descendant_2].descendant_2 == -1)
            {
                //si tout va bien
                matrix[matrix[last_individu].descendant_2].longueur_branche += (event_coalescent-matrix[matrix[last_individu].descendant_2].longueur_branche);
            }else{
                if (matrix[matrix[matrix[last_individu].descendant_2].descendant_1].longueur_branche > 
                    matrix[matrix[matrix[last_individu].descendant_2].descendant_2].longueur_branche)
                {
                    matrix[matrix[last_individu].descendant_2].longueur_branche += 
                    (event_coalescent-matrix[matrix[last_individu].descendant_2].longueur_branche);
                    matrix[matrix[last_individu].descendant_2].longueur_branche -= 
                    matrix[matrix[matrix[last_individu].descendant_2].descendant_1].longueur_branche;
                }else{
                    matrix[matrix[last_individu].descendant_2].longueur_branche += 
                    (event_coalescent-matrix[matrix[last_individu].descendant_2].longueur_branche);
                    matrix[matrix[last_individu].descendant_1].longueur_branche -= 
                    matrix[matrix[matrix[last_individu].descendant_2].descendant_2].longueur_branche;
                }
            }
            //get_all_informations(matrix, 0, nombre_individu_total);
                        //actualiser les sommes des branches
            //float result = 0.0;
            //ajoute les longueurs de branches (même bout de code dans la fonction create_arbre_phylogenetic spliter)
            /*
            for (int i = 0 ; i < nombre_individu_total - 1; ++i)
                {
                    result += matrix[i].longueur_branche;
                    matrix[i].somme_lb = result;
                }
            matrix[nombre_individu_total - 1].somme_lb = result;
            */
        }//fin de la condition du cas rare
        //le cas ou ils ont le même ancêtre 
        else if (matrix[individu_selectioned].ancetre == matrix[recombinaison_individu].ancetre)
        {
            (*compteur_silencieux)++;
            printf("On a les même ancêtres\n");
            int i = matrix[individu_selectioned].ancetre;
            float total_lb_ancetre_commun = 0.0;
            while (matrix[i].descendant_1 != -1){
                i = matrix[i].descendant_1;
                total_lb_ancetre_commun += matrix[i].longueur_branche;
            }
            printf("total_lb_ancetre_commun %f\n", total_lb_ancetre_commun);

            //si l'ancetre de son ancetre est la dernier branche sinon on cherche cette derniere branche
            matrix[matrix[individu_selectioned].ancetre].longueur_branche += 
            matrix[individu_selectioned].longueur_branche + (total_lb_ancetre_commun - event_coalescent);

            matrix[matrix[matrix[individu_selectioned].ancetre].descendant_1].longueur_branche -=
            (total_lb_ancetre_commun - event_coalescent);

            matrix[matrix[matrix[individu_selectioned].ancetre].descendant_2].longueur_branche -=
            (total_lb_ancetre_commun - event_coalescent);
        }
        // dans le cas ou l'ancetre du l'individu de recombinaison est un descendant de l'évènement de recombinaison
        //très similaire a la premier condition
        else if (recombinaison_individu == matrix[individu_selectioned].descendant_1
            || recombinaison_individu == matrix[individu_selectioned].descendant_2)
        {
            (*compteur_silencieux)++;
            printf("Cas particulier : L'ancetre de l'individu de recombinaison est un descendant de l'évènement de recombinaison.\n");
            int i = matrix[individu_selectioned].descendant_1;
            float somme_total = matrix[matrix[individu_selectioned].descendant_1].longueur_branche;
            while (matrix[i].descendant_1 != -1){
                i = matrix[i].descendant_1;
                somme_total += matrix[i].longueur_branche;
            }
            
            printf("somme total : %f\n",somme_total);
            printf("event_coalescent : %f\n",event_coalescent);
            
            printf("event_coalescence - somme total = %f\n",
                (event_coalescent - somme_total) );
            
            matrix[matrix[individu_selectioned].descendant_1].longueur_branche +=
            (event_coalescent - somme_total);

            matrix[matrix[individu_selectioned].descendant_2].longueur_branche +=
            (event_coalescent - somme_total);
            if (matrix[individu_selectioned].ancetre == -1)
            {
              matrix[individu_selectioned].ancetre = -1;
            }else{
              matrix[individu_selectioned].longueur_branche -=
              (event_coalescent - somme_total);    
            }
        }
        //le cas ou ils n'ont pas le même ancêtre
        else if(matrix[individu_selectioned].ancetre != matrix[recombinaison_individu].ancetre)
        {
            (*compteur_non_silencieux)++;

            printf("--------------------------------------------------------------\n");
            

            int ind_select_desc_1, ind_select_desc_2, ind_select_anc;
            int anc_ind_recomb_desc_1, anc_ind_recomb_desc_2, anc_ind_recomb_anc;
            
            //on est dans la situation ou l'évènement de coalescence survient au dessus de lui sans que se soit sont ancetre 
            if (matrix[individu_selectioned].descendant_1 == matrix[recombinaison_individu].ancetre
                || matrix[individu_selectioned].descendant_2 == matrix[recombinaison_individu].ancetre){

                if (matrix[recombinaison_individu].ancetre == matrix[individu_selectioned].descendant_1)
                {
                  printf("probleme ? B\n");
                  printf("------------------------------------------------\n");
                  if (matrix[matrix[recombinaison_individu].ancetre].descendant_2 == recombinaison_individu)
                  {
                    ind_select_desc_1 = matrix[matrix[recombinaison_individu].ancetre].descendant_1;
                    printf("B.1 ind_select_desc_1 : %d\n",ind_select_desc_1);
                  }
                  if (matrix[matrix[recombinaison_individu].ancetre].descendant_1 == recombinaison_individu)
                  {
                    ind_select_desc_1 = matrix[matrix[recombinaison_individu].ancetre].descendant_2;
                    printf("B.2 ind_select_desc_1 : %d\n",ind_select_desc_1);
                  }

                  anc_ind_recomb_anc = matrix[individu_selectioned].ancetre;
                  ind_select_anc = matrix[recombinaison_individu].ancetre;
                  printf("ind_select_anc : %d\n",ind_select_anc);
                  matrix[matrix[recombinaison_individu].ancetre].descendant_2 = individu_selectioned;
                   
                //my_pause();
                  printf("matrix[individu_selectioned].ancetre %d -> %d ind_select_anc\n",matrix[individu_selectioned].ancetre,ind_select_anc);
                  matrix[individu_selectioned].ancetre = ind_select_anc;
                  printf("matrix[individu_selectioned].descendant_1 %d -> %d ind_select_desc_1\n",matrix[individu_selectioned].descendant_1,ind_select_desc_1);
                  matrix[individu_selectioned].descendant_1 = ind_select_desc_1;
                  printf("matrix[matrix[recombinaison_individu].ancetre].ancetre %d -> %d anc_ind_recomb_anc\n",matrix[matrix[recombinaison_individu].ancetre].ancetre,anc_ind_recomb_anc);
                  matrix[matrix[recombinaison_individu].ancetre].ancetre = anc_ind_recomb_anc;
                  printf("fin\n");
                  return *matrix;

                }
                
                if (matrix[recombinaison_individu].ancetre == matrix[individu_selectioned].descendant_2)
                {
                  /*
                  on modifie le descendant 2 de l'individu selectionné.
                  Mais il faut savoir qui lui donner par rapport
                  a la position des descendant 1 ou 2 de l'individu
                  de recombinaison
                  */
                  printf("probleme ? A\n");
                  printf("------------------------------------------------\n");
                  if (matrix[matrix[recombinaison_individu].ancetre].descendant_2 == recombinaison_individu)
                  {
                    ind_select_desc_2 = matrix[matrix[recombinaison_individu].ancetre].descendant_1;
                    printf("A.1 ind_select_desc_2 : %d\n",ind_select_desc_2);
                  }
                  if (matrix[matrix[recombinaison_individu].ancetre].descendant_1 == recombinaison_individu)
                  {
                    ind_select_desc_2 = matrix[matrix[recombinaison_individu].ancetre].descendant_2;
                    printf("A.2 ind_select_desc_2 : %d\n",ind_select_desc_2);
                  }

                  anc_ind_recomb_anc = matrix[individu_selectioned].ancetre;
                  ind_select_anc = matrix[recombinaison_individu].ancetre;
                  printf("ind_select_anc : %d\n",ind_select_anc);
                  matrix[matrix[recombinaison_individu].ancetre].descendant_1 = individu_selectioned;
                   
                //my_pause();
                  printf("matrix[individu_selectioned].ancetre %d -> %d ind_select_anc\n",matrix[individu_selectioned].ancetre,ind_select_anc);
                  matrix[individu_selectioned].ancetre = ind_select_anc;
                  printf("matrix[individu_selectioned].descendant_2 %d -> %d ind_select_desc_2\n",matrix[individu_selectioned].descendant_2,ind_select_desc_2);
                  matrix[individu_selectioned].descendant_2 = ind_select_desc_2;
                  printf("matrix[matrix[recombinaison_individu].ancetre].ancetre %d -> %d anc_ind_recomb_anc\n",matrix[matrix[recombinaison_individu].ancetre].ancetre,anc_ind_recomb_anc);                  
                  matrix[matrix[recombinaison_individu].ancetre].ancetre = anc_ind_recomb_anc;
                  printf("fin\n");
                  return *matrix;
                }            
            }

            //si lors l'évènement de recombinaison se trouve a plus de 1 de l'individu de selection
            int ancetre_suivant = matrix[recombinaison_individu].ancetre;
            int ancetre_precedant;
            while(ancetre_suivant != -1)
            {
              if (ancetre_suivant == individu_selectioned)
              {
                if (ancetre_precedant == matrix[individu_selectioned].descendant_2)
                {
                  //my_pause();
                  printf("-----------Est dans ce cas spécifique -------------------\n");
                  /*
                  on modifie le descendant 2 de l'individu selectionné.
                  Mais il faut savoir qui lui donner par rapport
                  a la position des descendant 1 ou 2 de l'individu
                  de recombinaison
                  */
                  printf("probleme ? C\n");
                  printf("------------------------------------------------\n");
                  if (matrix[matrix[recombinaison_individu].ancetre].descendant_2 == recombinaison_individu)
                  {
                    ind_select_desc_2 = matrix[matrix[recombinaison_individu].ancetre].descendant_1;
                    printf("C.1 ind_select_desc_2 : %d\n",ind_select_desc_2);
                    matrix[matrix[matrix[recombinaison_individu].ancetre].descendant_1].ancetre = 
                    matrix[matrix[recombinaison_individu].ancetre].ancetre;
                    matrix[matrix[recombinaison_individu].ancetre].descendant_1 = individu_selectioned;
                  }
                  if (matrix[matrix[recombinaison_individu].ancetre].descendant_1 == recombinaison_individu)
                  {
                    ind_select_desc_2 = matrix[matrix[recombinaison_individu].ancetre].descendant_2;
                    printf("C.2 ind_select_desc_2 : %d\n",ind_select_desc_2);
                    matrix[matrix[matrix[recombinaison_individu].ancetre].descendant_2].ancetre = 
                    matrix[matrix[recombinaison_individu].ancetre].ancetre;
                    matrix[matrix[recombinaison_individu].ancetre].descendant_2 = individu_selectioned;
                  }

                  //normalement ici il prend l'ancetre de l'individu selectionné mais sur 5 indididu c'est -1 souvant
                  anc_ind_recomb_anc = -1;
                  ind_select_anc = matrix[recombinaison_individu].ancetre;
                  printf("ind_select_anc : %d\n",ind_select_anc);

                   
                //my_pause();
                  printf("matrix[individu_selectioned].ancetre %d -> %d ind_select_anc\n",matrix[individu_selectioned].ancetre,ind_select_anc);
                  matrix[individu_selectioned].ancetre = ind_select_anc;
                  
                  //peut etre très bien le descendant 1 comme le descedant 2
                  if (matrix[recombinaison_individu].ancetre == matrix[matrix[matrix[recombinaison_individu].ancetre].ancetre].descendant_2)
                  {
                    //printf("matrix[individu_selectioned].descendant_2 %d -> %d ind_select_desc_2\n",matrix[individu_selectioned].descendant_2,ind_select_desc_2);
                    matrix[matrix[matrix[recombinaison_individu].ancetre].ancetre].descendant_2 = ind_select_desc_2;
                  }
                  if (matrix[recombinaison_individu].ancetre == matrix[matrix[matrix[recombinaison_individu].ancetre].ancetre].descendant_1)
                  {
                    //printf("matrix[individu_selectioned].descendant_2 %d -> %d ind_select_desc_2\n",matrix[individu_selectioned].descendant_2,ind_select_desc_2);
                    matrix[matrix[matrix[recombinaison_individu].ancetre].ancetre].descendant_1 = ind_select_desc_2;
                  }
                  
                  printf("matrix[matrix[recombinaison_individu].ancetre].ancetre %d -> %d anc_ind_recomb_anc\n",matrix[matrix[recombinaison_individu].ancetre].ancetre,anc_ind_recomb_anc);                                  
                  matrix[matrix[recombinaison_individu].ancetre].ancetre = anc_ind_recomb_anc;
                  printf("fin\n");
                  return *matrix;
                }

                if (ancetre_precedant == matrix[individu_selectioned].descendant_1)
                {
                  //my_pause();
                  printf("-----------Est dans ce cas spécifique -------------------\n");
                  printf("probleme ? D\n");
                  printf("------------------------------------------------\n");
                  if (matrix[matrix[recombinaison_individu].ancetre].descendant_2 == recombinaison_individu)
                  {
                    ind_select_desc_1 = matrix[matrix[recombinaison_individu].ancetre].descendant_1;
                    printf("D.1 ind_select_desc_1 : %d\n",ind_select_desc_1);
                    matrix[matrix[matrix[recombinaison_individu].ancetre].descendant_1].ancetre = 
                    matrix[matrix[recombinaison_individu].ancetre].ancetre;
                    matrix[matrix[recombinaison_individu].ancetre].descendant_1 = individu_selectioned;
                  }
                  if (matrix[matrix[recombinaison_individu].ancetre].descendant_1 == recombinaison_individu)
                  {
                    ind_select_desc_1 = matrix[matrix[recombinaison_individu].ancetre].descendant_2;
                    printf("D.2 ind_select_desc_1 : %d\n",ind_select_desc_1);
                    matrix[matrix[matrix[recombinaison_individu].ancetre].descendant_2].ancetre = 
                    matrix[matrix[recombinaison_individu].ancetre].ancetre;
                    matrix[matrix[recombinaison_individu].ancetre].descendant_2 = individu_selectioned;
                  }

                  anc_ind_recomb_anc = -1;
                  ind_select_anc = matrix[recombinaison_individu].ancetre;
                  printf("ind_select_anc : %d\n",ind_select_anc);
                  matrix[matrix[recombinaison_individu].ancetre].descendant_2 = individu_selectioned;
                   
                //my_pause();
                  printf("matrix[individu_selectioned].ancetre %d -> %d ind_select_anc\n",matrix[individu_selectioned].ancetre,ind_select_anc);
                  matrix[individu_selectioned].ancetre = ind_select_anc;

                    //peut etre très bien le descendant 1 comme le descedant 2
                  if (matrix[recombinaison_individu].ancetre == matrix[matrix[matrix[recombinaison_individu].ancetre].ancetre].descendant_2)
                  {
                    //printf("matrix[individu_selectioned].descendant_2 %d -> %d ind_select_desc_2\n",matrix[individu_selectioned].descendant_2,ind_select_desc_2);
                    matrix[matrix[matrix[recombinaison_individu].ancetre].ancetre].descendant_2 = ind_select_desc_1;
                  }
                  if (matrix[recombinaison_individu].ancetre == matrix[matrix[matrix[recombinaison_individu].ancetre].ancetre].descendant_1)
                  {
                    //printf("matrix[individu_selectioned].descendant_2 %d -> %d ind_select_desc_2\n",matrix[individu_selectioned].descendant_2,ind_select_desc_2);
                    matrix[matrix[matrix[recombinaison_individu].ancetre].ancetre].descendant_1 = ind_select_desc_1;
                  }

                  //printf("matrix[individu_selectioned].descendant_1 %d -> %d ind_select_desc_1\n",matrix[individu_selectioned].descendant_1,ind_select_desc_1);
                  printf("matrix[matrix[recombinaison_individu].ancetre].ancetre %d -> %d anc_ind_recomb_anc\n",matrix[matrix[recombinaison_individu].ancetre].ancetre,anc_ind_recomb_anc);
                  matrix[matrix[recombinaison_individu].ancetre].ancetre = anc_ind_recomb_anc;
                  printf("fin\n");
                  return *matrix;
                }                
              }else{
                ancetre_precedant = ancetre_suivant;
                printf("ancetre précèdent = %d \n",ancetre_precedant);
                ancetre_suivant = matrix[ancetre_suivant].ancetre;
                printf("ancetre_suivant = %d\n",ancetre_suivant);
              }
            }
            printf("-----------N'est pas dans ce cas-------------------\n");
            //
            int anc_anc_recombinaison_desc_1, anc_anc_recombinaison_desc_2;
            int ancetre_machin;

            //du point de vue de la recombinaison.desc + anc.anc.recombinaison-> desc
            if (matrix[matrix[recombinaison_individu].ancetre].descendant_1 == recombinaison_individu)
            {
              anc_ind_recomb_desc_1 = matrix[matrix[recombinaison_individu].ancetre].descendant_1;
              anc_ind_recomb_desc_2 = individu_selectioned;
              if (matrix[matrix[recombinaison_individu].ancetre].ancetre == -1)
              {
                anc_ind_recomb_anc = matrix[individu_selectioned].ancetre;
                ancetre_machin = -1;
              }
              else if (matrix[recombinaison_individu].ancetre == matrix[matrix[matrix[recombinaison_individu].ancetre].ancetre].descendant_2)
              {
                ancetre_machin = matrix[matrix[individu_selectioned].ancetre].ancetre;
                anc_ind_recomb_anc = matrix[individu_selectioned].ancetre;
                matrix[matrix[matrix[recombinaison_individu].ancetre].ancetre].descendant_2 = matrix[matrix[recombinaison_individu].ancetre].descendant_2;
              }
              // else
              else if (matrix[recombinaison_individu].ancetre == matrix[matrix[matrix[recombinaison_individu].ancetre].ancetre].descendant_1)
              {
                ancetre_machin = matrix[matrix[individu_selectioned].ancetre].ancetre;
                anc_ind_recomb_anc = matrix[individu_selectioned].ancetre;
                matrix[matrix[matrix[recombinaison_individu].ancetre].ancetre].descendant_1 = matrix[matrix[recombinaison_individu].ancetre].descendant_2;
              }
            }
            if (matrix[matrix[recombinaison_individu].ancetre].descendant_2 == recombinaison_individu)
            {
              anc_ind_recomb_desc_1 = individu_selectioned;
              anc_ind_recomb_desc_2 = matrix[matrix[recombinaison_individu].ancetre].descendant_2;
              if (matrix[matrix[recombinaison_individu].ancetre].ancetre == -1)
              {
                anc_ind_recomb_anc = matrix[individu_selectioned].ancetre;
                ancetre_machin = -1;
              }
              else if (matrix[recombinaison_individu].ancetre == matrix[matrix[matrix[recombinaison_individu].ancetre].ancetre].descendant_2)
              {
                ancetre_machin = matrix[matrix[individu_selectioned].ancetre].ancetre;
                anc_ind_recomb_anc = matrix[individu_selectioned].ancetre;
                matrix[matrix[matrix[recombinaison_individu].ancetre].ancetre].descendant_2 = matrix[matrix[recombinaison_individu].ancetre].descendant_1;
              }
              //else
              else if (matrix[recombinaison_individu].ancetre == matrix[matrix[matrix[recombinaison_individu].ancetre].ancetre].descendant_1)
              {
                ancetre_machin = matrix[matrix[individu_selectioned].ancetre].ancetre;
                anc_ind_recomb_anc = matrix[individu_selectioned].ancetre;
                matrix[matrix[matrix[recombinaison_individu].ancetre].ancetre].descendant_1 = matrix[matrix[recombinaison_individu].ancetre].descendant_1;
              }
            }
            //du point de vue de l'individu selectionné
            if (individu_selectioned == matrix[matrix[individu_selectioned].ancetre].descendant_1)
            {
              anc_anc_recombinaison_desc_1 = matrix[recombinaison_individu].ancetre;
              anc_anc_recombinaison_desc_2 = matrix[matrix[individu_selectioned].ancetre].descendant_2;
            }
            if (individu_selectioned == matrix[matrix[individu_selectioned].ancetre].descendant_2)
            {
              anc_anc_recombinaison_desc_1 = matrix[matrix[individu_selectioned].ancetre].descendant_1;
              anc_anc_recombinaison_desc_2 = matrix[recombinaison_individu].ancetre;
            }

            //dans tout les cas 
            int ancetre_individu_selectionne;
            ancetre_individu_selectionne = matrix[recombinaison_individu].ancetre;

            matrix[matrix[recombinaison_individu].ancetre].descendant_1 = anc_ind_recomb_desc_1;
            matrix[matrix[recombinaison_individu].ancetre].descendant_2 = anc_ind_recomb_desc_2;
            matrix[matrix[recombinaison_individu].ancetre].ancetre = anc_ind_recomb_anc;

            //printf("probleme fin A\n");
            matrix[matrix[individu_selectioned].ancetre].descendant_1 = anc_anc_recombinaison_desc_1;
            matrix[matrix[individu_selectioned].ancetre].descendant_2 = anc_anc_recombinaison_desc_2;
            matrix[matrix[individu_selectioned].ancetre].ancetre = ancetre_machin;
            matrix[individu_selectioned].ancetre = ancetre_individu_selectionne;

         
            //printf("probleme fin B\n");     
            printf("--------------------------------------------------------------\n");
            return *matrix;
        }
    }else{
        printf("On ne change rien car la coalescence se situe sur la meme branche.\n");
        (*compteur_cache)++;
        return *matrix;
    }
  return *matrix;
}