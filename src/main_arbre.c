#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <ctype.h>
#include <string.h>
#include "matrice_arbre.h"
#include "tableau.h"
#include "errors.h"
#include "newick.h"
#include "events.h"

int main(int argc, char *argv[])
{
  if( argc != 3)
    get_null_argv(argc, argv);
  {   
    int nombre_individu = atoi(argv[1]);
    int array_start_individu = nombre_individu;
    int nombre_individu_total = get_individus_number(nombre_individu);
    int *individus_table = malloc(sizeof(int) * nombre_individu);
    float somme_branches, random_time_recombinaison, real_time_recombinaison;
    int last_individu, recombinaison_individu;
    Matrice_arbre individus_matrix[nombre_individu_total];

    // variables nécessaires a la création du fichier newick
    char *strtree = (char *)malloc(1000*(sizeof(char)));
    char *name_file = argv[2];
    int position = 0;
    FILE fptr; 

    readme_information();
    initiate_table(individus_table, nombre_individu);
    initiate_matrix(individus_matrix, nombre_individu_total);
    create_phylogenetic_tree(individus_matrix, individus_table, array_start_individu, nombre_individu, nombre_individu_total);
    get_all_informations(individus_matrix, 0, nombre_individu_total);
    
    //chaine de caractère recupere la typologie du format newick puis creation d'un fichier newick .nwk
    strcat(name_file, "_before_coalescence");
    strtree = PrintTree(0, individus_matrix, strtree, &position, nombre_individu);
    create_newick_file(&fptr, name_file, strtree);

    // variables correspondant aux branches de l'arbre
    last_individu = get_last_individu(individus_matrix, nombre_individu_total);
    somme_branches = get_somme_branches(individus_matrix, last_individu);
    random_time_recombinaison = random_recombinaison(somme_branches);
    recombinaison_individu = get_individu_event_recombinaison(individus_matrix, random_time_recombinaison, last_individu);
    real_time_recombinaison = get_time_event_recombinaison(individus_matrix, random_time_recombinaison, last_individu);
    int all_individu_recombinaison;
    all_individu_recombinaison = get_all_individu_concerned_by_event(individus_matrix, real_time_recombinaison, nombre_individu);

    printf("--------------------------------------------------------------\n");
    //printf("L'évènement de recombinaison pris au hazard entre [0, %f] = %f.\n",somme_branches, random_time_recombinaison);
    printf("L'évènement de recombinaison a eu lieu sur l'individu %d.\n",recombinaison_individu);
    printf("Le temps pour la recombinaison est %f.\n", real_time_recombinaison);  
    printf("le nombre d'individu pour la recombinaison est %d.\n", all_individu_recombinaison);
    printf("--------------------------------------------------------------\n");

    float event_coalescent;
    event_coalescent = random_event_coalescencence(all_individu_recombinaison, real_time_recombinaison);
    printf("L'évènement de coalescence a lieu en %f.\n",event_coalescent);

    //on regarde pour ce temps de coalescence
    int all_individu_coalescent;
    all_individu_coalescent = get_all_individu_concerned_by_event(individus_matrix, event_coalescent, nombre_individu);
    printf("le nombre d'individu pour la coalescence est %d.\n",all_individu_coalescent);

    /* on verifie si le nombre d'individu n'est pas différent
    entre la recombinaison et la coalescence :
        * si c'est différent on re-calcule un l'évènement de 
        coalescence en fonction du nombre d'individu pour la coalescence.
        * sinon on ne fait rien.
    */
    event_coalescent = verif_same_number_individu(individus_matrix, 
                                                    all_individu_recombinaison,
                                                    all_individu_coalescent, 
                                                    real_time_recombinaison, 
                                                    event_coalescent, 
                                                    nombre_individu,
                                                    last_individu);

    /*
    Maintenant que l'on sait ou se situe l'évènement de recombinaison
    on doit déterminer quelle sont les indidividus exacte qui la compose
    */
    printf("--------------------------------------------------------------\n");
    int *individu_concerned_by_coalescence = malloc(sizeof(int) * 1);
    int length_table = 0;
    determine(individus_matrix, individu_concerned_by_coalescence, &length_table, nombre_individu, last_individu, event_coalescent);
    diplay_table(individu_concerned_by_coalescence, length_table);

    //prend un individu au hazard parmit le tableau || Tirer au sort parmi les lignées présentes
    int individu_selectioned;
    individu_selectioned = get_random_int_table(individu_concerned_by_coalescence, length_table);
    printf("L'individu selectionné au hazard dans le tableau est %d.\n",individu_selectioned);

    if (individu_selectioned != recombinaison_individu)
    {
        printf("individu_selectioned %d != %d recombinaison_individu\n",individu_selectioned, recombinaison_individu);

        //dans le cas rare ou l'évènement de coalescence se situe au dessus du dernier temps
        if (individus_matrix[last_individu].Temps < event_coalescent )
        {
            //faire attention a la somme des descendant
            if (individus_matrix[individus_matrix[last_individu].descendant_1].descendant_1 == -1 &&
                individus_matrix[individus_matrix[last_individu].descendant_1].descendant_2 == -1)
            {
                //si tout va bien 
                individus_matrix[individus_matrix[last_individu].descendant_1].longueur_branche += (event_coalescent-individus_matrix[individus_matrix[last_individu].descendant_1].longueur_branche);
            }else{
                if (individus_matrix[individus_matrix[individus_matrix[last_individu].descendant_1].descendant_1].longueur_branche > 
                    individus_matrix[individus_matrix[individus_matrix[last_individu].descendant_1].descendant_2].longueur_branche)
                {
                    individus_matrix[individus_matrix[last_individu].descendant_1].longueur_branche += 
                    (event_coalescent-individus_matrix[individus_matrix[last_individu].descendant_1].longueur_branche);
                    individus_matrix[individus_matrix[last_individu].descendant_1].longueur_branche -= 
                    individus_matrix[individus_matrix[individus_matrix[last_individu].descendant_1].descendant_1].longueur_branche;
                }else{
                    individus_matrix[individus_matrix[last_individu].descendant_1].longueur_branche += 
                    (event_coalescent-individus_matrix[individus_matrix[last_individu].descendant_1].longueur_branche);
                    individus_matrix[individus_matrix[last_individu].descendant_1].longueur_branche -= 
                    individus_matrix[individus_matrix[individus_matrix[last_individu].descendant_1].descendant_2].longueur_branche;
                }
            }
            if (individus_matrix[individus_matrix[last_individu].descendant_2].descendant_1 == -1 &&
                individus_matrix[individus_matrix[last_individu].descendant_2].descendant_2 == -1)
            {
                //si tout va bien
                individus_matrix[individus_matrix[last_individu].descendant_2].longueur_branche += (event_coalescent-individus_matrix[individus_matrix[last_individu].descendant_2].longueur_branche);
            }else{
                if (individus_matrix[individus_matrix[individus_matrix[last_individu].descendant_2].descendant_1].longueur_branche > 
                    individus_matrix[individus_matrix[individus_matrix[last_individu].descendant_2].descendant_2].longueur_branche)
                {
                    individus_matrix[individus_matrix[last_individu].descendant_2].longueur_branche += 
                    (event_coalescent-individus_matrix[individus_matrix[last_individu].descendant_2].longueur_branche);
                    individus_matrix[individus_matrix[last_individu].descendant_2].longueur_branche -= 
                    individus_matrix[individus_matrix[individus_matrix[last_individu].descendant_2].descendant_1].longueur_branche;
                }else{
                    individus_matrix[individus_matrix[last_individu].descendant_2].longueur_branche += 
                    (event_coalescent-individus_matrix[individus_matrix[last_individu].descendant_2].longueur_branche);
                    individus_matrix[individus_matrix[last_individu].descendant_1].longueur_branche -= 
                    individus_matrix[individus_matrix[individus_matrix[last_individu].descendant_2].descendant_2].longueur_branche;
                }
            }
            //get_all_informations(individus_matrix, 0, nombre_individu_total);
                        //actualiser les sommes des branches
            //float result = 0.0;
            //ajoute les longueurs de branches (même bout de code dans la fonction create_arbre_phylogenetic spliter)
            /*
            for (int i = 0 ; i < nombre_individu_total - 1; ++i)
                {
                    result += individus_matrix[i].longueur_branche;
                    individus_matrix[i].somme_lb = result;
                }
            individus_matrix[nombre_individu_total - 1].somme_lb = result;
            */
        }//fin de la condition du cas rare

        //le cas ou ils ont le même ancêtre 
        else if (individus_matrix[individu_selectioned].ancetre == individus_matrix[recombinaison_individu].ancetre)
        {
            printf("On a les même ancêtres\n");
            int i = individus_matrix[individu_selectioned].ancetre;
            float total_lb_ancetre_commun = 0.0;
            while (individus_matrix[i].descendant_1 != -1){
                i = individus_matrix[i].descendant_1;
                total_lb_ancetre_commun += individus_matrix[i].longueur_branche;
            }
            printf("total_lb_ancetre_commun %f\n", total_lb_ancetre_commun);

            //si l'ancetre de son ancetre est la dernier branche sinon on cherche cette derniere branche
            individus_matrix[individus_matrix[individu_selectioned].ancetre].longueur_branche += 
            individus_matrix[individu_selectioned].longueur_branche + (total_lb_ancetre_commun - event_coalescent);

            individus_matrix[individus_matrix[individus_matrix[individu_selectioned].ancetre].descendant_1].longueur_branche -=
            (total_lb_ancetre_commun - event_coalescent);

            individus_matrix[individus_matrix[individus_matrix[individu_selectioned].ancetre].descendant_2].longueur_branche -=
            (total_lb_ancetre_commun - event_coalescent);
        }

        // dans le cas ou l'ancetre du l'individu de recombinaison est un descendant de l'évènement de recombinaison
        //très similaire a la premier condition
        else if (recombinaison_individu == individus_matrix[individu_selectioned].descendant_1
            || recombinaison_individu == individus_matrix[individu_selectioned].descendant_2)
        {
            printf("Cas particulier : L'ancetre de l'individu de recombinaison est un descendant de l'évènement de recombinaison.\n");
            int i = individus_matrix[individu_selectioned].descendant_1;
            float somme_total = individus_matrix[individus_matrix[individu_selectioned].descendant_1].longueur_branche;
            while (individus_matrix[i].descendant_1 != -1){
                i = individus_matrix[i].descendant_1;
                somme_total += individus_matrix[i].longueur_branche;
            }
            /*
            printf("somme total : %f\n",somme_total);
            
            printf("somme total - event_coalescence %f\n",
                (event_coalescent - somme_total));
            */
            individus_matrix[individus_matrix[individu_selectioned].descendant_1].longueur_branche +=
            (event_coalescent - somme_total);

            individus_matrix[individus_matrix[individu_selectioned].descendant_2].longueur_branche +=
            (event_coalescent - somme_total);

            individus_matrix[individu_selectioned].longueur_branche -=
            (event_coalescent - somme_total);    
        }
        //le cas ou ils n'ont pas le même ancêtre
        else if(individus_matrix[individu_selectioned].ancetre != individus_matrix[recombinaison_individu].ancetre)
        {
            printf("--------------------------------------------------------------\n");

            //ok
            if (individus_matrix[individu_selectioned].descendant_1 == individus_matrix[recombinaison_individu].ancetre
                || individus_matrix[individu_selectioned].descendant_2 == individus_matrix[recombinaison_individu].ancetre){
                if (individus_matrix[individus_matrix[recombinaison_individu].ancetre].descendant_1 == recombinaison_individu)
                {
                    printf("on modifie le second descendant_2\n");
                    printf("i : %d, d_1 : %d, d_2 : %d, anc : %d\n",individu_selectioned,
                                        individus_matrix[individu_selectioned].descendant_1,
                                        individus_matrix[recombinaison_individu].descendant_1,
                                        individus_matrix[recombinaison_individu].ancetre);
                }else{
                    printf("on modifie le second descendant_1\n");
                    printf("i : %d, d_1 : %d, d_2 : %d, anc : %d\n",individu_selectioned,
                                        individus_matrix[individu_selectioned].descendant_1,
                                        individus_matrix[recombinaison_individu].descendant_2,
                                        individus_matrix[recombinaison_individu].ancetre);
                }
            
            }else{
            printf("i : %d, d_1 : %d, d_2 : %d, anc : %d\n",individu_selectioned,
                                        individus_matrix[individu_selectioned].descendant_1,
                                        individus_matrix[individu_selectioned].descendant_2,
                                        individus_matrix[recombinaison_individu].ancetre);
            }

            //ok
            if (individus_matrix[individus_matrix[recombinaison_individu].ancetre].descendant_1 == recombinaison_individu)
            { 
                /*
                printf("on est dans le cas classique ou :\n");
                printf("individus_matrix[individus_matrix[recombinaison_individu].ancetre].descendant_1 == recombinaison_individu\n");
                printf("on modifie le d_2\n");
                printf("l'ancetre de l'individu selectionné va devenir l'ancetre pour l'individu qui est l'ancetre de l'individu recombiné\n");
                printf("l'ancetre de l'individu_selectioné est %d\n",individus_matrix[individu_selectioned].ancetre);
                printf("l'ancetre de l'individu de recombinaison est %d\n",individus_matrix[recombinaison_individu].ancetre);
                printf("on aurait donc :\n");
                */
                printf("i : %d, d_1 : %d, d_2 : %d, anc : %d\n", individus_matrix[recombinaison_individu].ancetre,
                                                                individus_matrix[individus_matrix[recombinaison_individu].ancetre].descendant_1,
                                                                individu_selectioned,
                                                                individus_matrix[individu_selectioned].ancetre); 
            }else{
                /*
                printf("on est dans le moin cas classique ou :\n");
                printf("individus_matrix[individus_matrix[recombinaison_individu].ancetre].descendant_1 != recombinaison_individu\n");
                printf("on modifie le d_1\n");
                printf("l'ancetre de l'individu selectionné va devenir l'ancetre pour l'individu qui est l'ancetre de l'individu recombiné\n");
                printf("l'ancetre de l'individu_selectioné est %d\n",individus_matrix[individu_selectioned].ancetre);
                printf("l'ancetre de l'individu de recombinaison est %d\n",individus_matrix[recombinaison_individu].ancetre);
                printf("on aurait donc :\n");
                */
                printf("i : %d, d_1 : %d, d_2 : %d, anc : %d\n", individus_matrix[recombinaison_individu].ancetre,
                                                                individu_selectioned,
                                                                individus_matrix[individus_matrix[recombinaison_individu].ancetre].descendant_2,
                                                                individus_matrix[individu_selectioned].ancetre);
            }


        }
    }else{
        printf("On ne change rien car la coalescence se situe sur la meme branche.\n");
    }

    //creation d'un nouveau file newick
    char *strtree2 = (char *)malloc(1000*(sizeof(char)));
    //faire une copy de pointeur
    char *name_file2 = strcat(argv[2], "_after_coalescence");
    int position2 = 0;
    FILE fptr2; 

    //chaine de caractère recupere la typologie du format newick puis creation d'un fichier newick .nwk
    strtree = PrintTree(0, individus_matrix, strtree2, &position2, nombre_individu);
    create_newick_file(&fptr2, name_file2, strtree2);
    
    free(strtree);
    free(strtree2);
    free(individu_concerned_by_coalescence);
    return(EXIT_SUCCESS);
  }
}
