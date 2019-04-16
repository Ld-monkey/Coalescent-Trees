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

    int compteur_cache;
    int compteur_silencieux;
    int compteur_non_silencieux;

    if (individu_selectioned != recombinaison_individu)
    {
        printf("individu_selectioned %d != %d recombinaison_individu\n",individu_selectioned, recombinaison_individu);

        //dans le cas rare ou l'évènement de coalescence se situe au dessus du dernier temps
        if (individus_matrix[last_individu].Temps < event_coalescent )
        {
            compteur_silencieux++;
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
            compteur_silencieux++;
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
            compteur_silencieux++;
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
            compteur_non_silencieux++;
            printf("--------------------------------------------------------------\n");
            int ind_select_desc_1, ind_select_desc_2, ind_select_anc;
            int anc_ind_recomb_desc_1, anc_ind_recomb_desc_2, anc_ind_recomb_anc;

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
                    //application dans des variables
                                        ind_select_desc_1 = individus_matrix[individu_selectioned].descendant_1;
                                        ind_select_desc_2 = individus_matrix[recombinaison_individu].descendant_1;
                                        ind_select_anc = individus_matrix[recombinaison_individu].ancetre;


                }else{
                    printf("on modifie le second descendant_1\n");
                    printf("i : %d, d_1 : %d, d_2 : %d, anc : %d\n",individu_selectioned,
                                        individus_matrix[individu_selectioned].descendant_1,
                                        individus_matrix[recombinaison_individu].descendant_2,
                                        individus_matrix[recombinaison_individu].ancetre);
                    //applicaiton des variables
                                        ind_select_desc_1 = individus_matrix[individu_selectioned].descendant_1;
                                        ind_select_desc_2 = individus_matrix[recombinaison_individu].descendant_2;
                                        ind_select_anc = individus_matrix[recombinaison_individu].ancetre;
                }
            
            }else{
            printf("i : %d, d_1 : %d, d_2 : %d, anc : %d\n",individu_selectioned,
                                        individus_matrix[individu_selectioned].descendant_1,
                                        individus_matrix[individu_selectioned].descendant_2,
                                        individus_matrix[recombinaison_individu].ancetre);
                    //applicaiton des variables
                                        ind_select_desc_1 = individus_matrix[individu_selectioned].descendant_1;
                                        ind_select_desc_2 = individus_matrix[individu_selectioned].descendant_2;
                                        ind_select_anc = individus_matrix[recombinaison_individu].ancetre;
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
                //application sur des variables:
                anc_ind_recomb_desc_1 = individus_matrix[individus_matrix[recombinaison_individu].ancetre].descendant_1;
                anc_ind_recomb_desc_2 = individu_selectioned;
                anc_ind_recomb_anc = individus_matrix[individu_selectioned].ancetre;

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
                //application sur des variables:
                anc_ind_recomb_desc_1 = individu_selectioned;
                anc_ind_recomb_desc_2 = individus_matrix[individus_matrix[recombinaison_individu].ancetre].descendant_2;
                anc_ind_recomb_anc = individus_matrix[individu_selectioned].ancetre;
            }

            int anc_anc_recombinaison_desc_1, anc_anc_recombinaison_desc_2;
            //int anc_select_recomb_desc_1, anc_select_recomb_desc_2;

            if (individus_matrix[individus_matrix[individu_selectioned].ancetre].descendant_2 == individu_selectioned)
            {
                //on modifie le descendant 2
                printf("on parle de l'individu %d\n",individus_matrix[individu_selectioned].ancetre);
                printf("l'individu de recombinaison est %d\n",recombinaison_individu );
                printf("l'individu de selection est %d\n",individu_selectioned );
                printf("Dans le 2eme cas A\n");
                if (individus_matrix[individus_matrix[recombinaison_individu].ancetre].descendant_1 == recombinaison_individu)
                {
                    printf("A.1\n");
                    if (individus_matrix[individus_matrix[individu_selectioned].ancetre].descendant_1 == individu_selectioned)
                    {
                        //si la section est sur le 1 on modifie la 1
                        //le descendant 1 (de l'ancetre de lindiviud selec)
                        //est donc egale a l'ancetre de la recombinaire
                        anc_anc_recombinaison_desc_1= 
                        individus_matrix[recombinaison_individu].ancetre;
                        printf("descendant_1 %d\n",anc_anc_recombinaison_desc_1);

                    }else{
                        //c'est sur l'individu 2
                        anc_anc_recombinaison_desc_2= 
                        individus_matrix[recombinaison_individu].ancetre;
                        printf("descendant_2 %d\n",anc_anc_recombinaison_desc_2);
                    }
                    /*                    anc_anc_recombinaison_desc_2 = 
                    individus_matrix[individus_matrix[individu_selectioned].ancetre].descendant_2;
                    printf("descendant_1 : %d\n",anc_anc_recombinaison_desc_1);
                    */
                }
                if (individus_matrix[individus_matrix[recombinaison_individu].ancetre].descendant_2 == recombinaison_individu)
                {
                    printf("A.2\n");
                    if (individus_matrix[individus_matrix[individu_selectioned].ancetre].descendant_1 == individu_selectioned)
                    {
                        //si la section est sur le 1 on modifie la 1
                        //le descendant 1 (de l'ancetre de lindiviud selec)
                        //est donc egale a l'ancetre de la recombinaire
                        anc_anc_recombinaison_desc_1 = 
                        individus_matrix[recombinaison_individu].ancetre;
                        printf("descendant_1 %d\n", anc_anc_recombinaison_desc_1);  

                    }else{
                        //c'est sur l'individu 2
                        anc_anc_recombinaison_desc_2= 
                        individus_matrix[recombinaison_individu].ancetre;
                        printf("descendant_2 %d\n", anc_anc_recombinaison_desc_2); 
                    }
                    /*
                    anc_anc_recombinaison_desc_1 = 
                    individus_matrix[individus_matrix[individu_selectioned].ancetre].descendant_1;
                    printf("descendant_1 %d\n",anc_anc_recombinaison_desc_1);
                    */
                }
                printf("mais faut aussi changer le descendant 2 dans le cas A\n");
                if (individus_matrix[individus_matrix[recombinaison_individu].ancetre].descendant_1 == recombinaison_individu)
                {
                    printf("A.3\n");
                    // depend du référentiel soit individu_selectioned soit recombinaison_individu
                    if (individus_matrix[individus_matrix[individu_selectioned].ancetre].descendant_2 == individu_selectioned)
                    {
                        //si la section est sur le 1 on modifie la 1
                        //le descendant 1 (de l'ancetre de lindiviud selec)
                        //est donc egale a l'ancetre de la recombinaire
                        anc_anc_recombinaison_desc_1 = 
                        individus_matrix[individus_matrix[individu_selectioned].ancetre].descendant_1;
                        printf("descendant_1 %d\n", anc_anc_recombinaison_desc_1);  

                    }else{
                        //c'est sur l'individu 2
                        anc_anc_recombinaison_desc_2 = 
                        individus_matrix[individus_matrix[individu_selectioned].ancetre].descendant_2;
                        printf("descendant_1 %d\n", anc_anc_recombinaison_desc_2);  
                    }
                    /*
                    anc_anc_recombinaison_desc_2 = 
                    individus_matrix[individus_matrix[individu_selectioned].ancetre].descendant_1;
                    printf("descendant_2 %d\n",anc_anc_recombinaison_desc_2);
                    */
                }
                if (individus_matrix[individus_matrix[recombinaison_individu].ancetre].descendant_2 == recombinaison_individu)
                {
                    printf("A.4\n");
                    if (individus_matrix[individus_matrix[individu_selectioned].ancetre].descendant_2 == individu_selectioned)
                    {
                        //si la section est sur le 1 on modifie la 1
                        //le descendant 1 (de l'ancetre de lindiviud selec)
                        //est donc egale a l'ancetre de la recombinaire
                        anc_anc_recombinaison_desc_1 = 
                        individus_matrix[individus_matrix[individu_selectioned].ancetre].descendant_1;
                        printf("descendant_1 %d\n", anc_anc_recombinaison_desc_1);  

                    }else{
                        //c'est sur l'individu 2
                        anc_anc_recombinaison_desc_2 = 
                        individus_matrix[individus_matrix[individu_selectioned].ancetre].descendant_2;
                        printf("descendant_1 %d\n", anc_anc_recombinaison_desc_2);  
                    }
                    /*
                    anc_anc_recombinaison_desc_2 = 
                    individus_matrix[individus_matrix[individu_selectioned].ancetre].descendant_2;
                    printf("descendant_2 %d\n",anc_anc_recombinaison_desc_2);
                    */
                }

            }
            if (individus_matrix[individus_matrix[individu_selectioned].ancetre].descendant_1 == individu_selectioned)
            {
                //on modifie le descendant 1
                printf("on parle de l'individu %d\n",individus_matrix[individu_selectioned].ancetre);
                printf("l'individu de recombinaison est %d\n",recombinaison_individu );
                printf("l'individu de selection est %d\n",individu_selectioned );
                printf("Dans le 2eme cas B\n");
                if (individus_matrix[individus_matrix[recombinaison_individu].ancetre].descendant_1 == recombinaison_individu)
                {
                    printf("B.1 on modifie \n");
                    if (individus_matrix[individus_matrix[individu_selectioned].ancetre].descendant_1 == individu_selectioned)
                    {
                        //si la section est sur le 1 on modifie la 1
                        //le descendant 1 (de l'ancetre de lindiviud selec)
                        //est donc egale a l'ancetre de la recombinaire
                        anc_anc_recombinaison_desc_1= 
                        individus_matrix[recombinaison_individu].ancetre;
                        printf("descendant_1 %d\n", anc_anc_recombinaison_desc_1); 
                    }else{
                        //c'est sur l'individu 2
                        anc_anc_recombinaison_desc_2= 
                        individus_matrix[recombinaison_individu].ancetre;
                        printf("descendant_2 %d\n", anc_anc_recombinaison_desc_1); 
                    }
                }
                if (individus_matrix[individus_matrix[recombinaison_individu].ancetre].descendant_2 == recombinaison_individu)
                {
                    printf("B.2\n");
                    if (individus_matrix[individus_matrix[individu_selectioned].ancetre].descendant_1 == individu_selectioned)
                    {
                        //si la section est sur le 1 on modifie la 1
                        //le descendant 1 (de l'ancetre de lindiviud selec)
                        //est donc egale a l'ancetre de la recombinaire
                        anc_anc_recombinaison_desc_1= 
                        individus_matrix[recombinaison_individu].ancetre;
                        printf("descendant_1 %d\n", anc_anc_recombinaison_desc_1); 

                    }else{
                        //c'est sur l'individu 2
                        anc_anc_recombinaison_desc_2= 
                        individus_matrix[recombinaison_individu].ancetre;
                        printf("descendant_2 %d\n", anc_anc_recombinaison_desc_1); 
                    }
                }
                //mais faut aussi modifier le descendant 2 e l'ancetre de l'individu selectionné
                printf("mais faut aussi changer le descendant 2 dans le cas B\n");
                if (individus_matrix[individus_matrix[recombinaison_individu].ancetre].descendant_1 == recombinaison_individu)
                {

                    int referentiel;
                        if (individus_matrix[individus_matrix[recombinaison_individu].ancetre].descendant_1 == 
                            individus_matrix[individu_selectioned].ancetre ||
                            individus_matrix[individus_matrix[recombinaison_individu].ancetre].descendant_2 == 
                            individus_matrix[individu_selectioned].ancetre)
                        {
                            referentiel = individu_selectioned;
                            printf("référentiel individu selectionned\n");

                        }else{
                            referentiel = recombinaison_individu;
                            printf("référentiel recombinaison individu\n");
                        }

                    printf("B.3\n");
                    if (individus_matrix[individus_matrix[recombinaison_individu].ancetre].descendant_1 == recombinaison_individu)
                    {
                        //si la section est sur le 1 on modifie la 1
                        //le descendant 1 (de l'ancetre de lindiviud selec)
                        //est donc egale a l'ancetre de la recombinaire
                        //le reférentiel 
                        anc_anc_recombinaison_desc_2 = 
                        individus_matrix[individus_matrix[referentiel].ancetre].descendant_2;
                        printf("B.3.1 descendant_2 %d\n",anc_anc_recombinaison_desc_2);

                    }else{
                        //c'est sur l'individu 2
                        //c'est sur l'individu 2
                        anc_anc_recombinaison_desc_2 = 
                        individus_matrix[individus_matrix[referentiel].ancetre].descendant_1;
                        printf("B.3.2 descendant_2 %d\n",anc_anc_recombinaison_desc_2);
                    }
                    /*
                    anc_anc_recombinaison_desc_2 = 
                    individus_matrix[individus_matrix[recombinaison_individu].ancetre].descendant_2;
                    printf("descendant_2 %d\n",anc_anc_recombinaison_desc_2);
                    */
                }
                if (individus_matrix[individus_matrix[recombinaison_individu].ancetre].descendant_2 == recombinaison_individu)
                {
                    int referentiel;
                        if (individus_matrix[individus_matrix[recombinaison_individu].ancetre].descendant_1 == 
                            individus_matrix[individu_selectioned].ancetre ||
                            individus_matrix[individus_matrix[recombinaison_individu].ancetre].descendant_2 == 
                            individus_matrix[individu_selectioned].ancetre)
                        {
                            referentiel = individu_selectioned;
                            printf("référentiel individu selectionned\n");
                        }else{
                            
                            referentiel = recombinaison_individu;
                            printf("référentiel recombinaison individu\n");
                        }
                    printf("B.4\n");
                    if (individus_matrix[individus_matrix[recombinaison_individu].ancetre].descendant_1 == recombinaison_individu)
                    {
                        //si la section est sur le 1 on modifie la 1
                        //le descendant 1 (de l'ancetre de lindiviud selec)
                        //est donc egale a l'ancetre de la recombinaire
                        anc_anc_recombinaison_desc_2 = 
                        individus_matrix[individus_matrix[referentiel].ancetre].descendant_2;
                        printf("B.4.1 descendant_2 %d\n",anc_anc_recombinaison_desc_2);

                    }else{
                        //c'est sur l'individu 2
                        anc_anc_recombinaison_desc_2 = 
                        individus_matrix[individus_matrix[referentiel].ancetre].descendant_1;
                        printf("B.4.2 descendant_2 %d\n",anc_anc_recombinaison_desc_2);
                    }
                    /*
                    anc_anc_recombinaison_desc_2 = 
                    individus_matrix[individus_matrix[recombinaison_individu].ancetre].descendant_2;
                    printf("descendant_2 %d\n",anc_anc_recombinaison_desc_2);
                    */
                }

            }


            

            int ancetre_de_machin;
            if (individus_matrix[individus_matrix[recombinaison_individu].ancetre].descendant_1 == 
                individus_matrix[individu_selectioned].ancetre ||
                individus_matrix[individus_matrix[recombinaison_individu].ancetre].descendant_2 == 
                individus_matrix[individu_selectioned].ancetre )
            {
                ancetre_de_machin = individus_matrix[individus_matrix[recombinaison_individu].ancetre].ancetre;
            }else{
                printf("l'ancetre de l'ancetre de l'individu selectioné ?\n");
                 ancetre_de_machin = individus_matrix[individus_matrix[individu_selectioned].ancetre].ancetre;
                 printf("l'individu recombinaison %d\n",recombinaison_individu);
                 printf("individus_matrix[individus_matrix[recombinaison_individu].ancetre].ancetre = %d\n",
                 individus_matrix[individus_matrix[recombinaison_individu].ancetre].ancetre );
                if (individus_matrix[individus_matrix[recombinaison_individu].ancetre].ancetre == -1)
                {
                    printf("c'est -1 l'ancetre?\n");
                    ancetre_de_machin = -1;                          
                }else{
                 individus_matrix[individus_matrix[individu_selectioned].ancetre].ancetre = 
                 ancetre_de_machin;

                 printf("pour l'individu %d on a l'ancetre %d\n",individus_matrix[recombinaison_individu].ancetre,
                                                                ancetre_de_machin);
             }
            }
            printf("ancetre machin après %d\n",ancetre_de_machin);
                        //partie modification des variables
            //pour l'individu selectionné
            //pour l'individu de recombinaison
            individus_matrix[individus_matrix[recombinaison_individu].ancetre].descendant_1 = anc_ind_recomb_desc_1;
            individus_matrix[individus_matrix[recombinaison_individu].ancetre].descendant_2 = anc_ind_recomb_desc_2;
            individus_matrix[individus_matrix[recombinaison_individu].ancetre].ancetre = anc_ind_recomb_anc;
            //on modifie l'ancetre de l'ancetre et ces descendant
            /*
            individus_matrix[individus_matrix[recombinaison_individu].ancetre].descendant_1 = anc_anc_recombinaison_desc_1;
            individus_matrix[individus_matrix[recombinaison_individu].ancetre].descendant_2 = anc_anc_recombinaison_desc_2;
            */

            //on modifie l'ancetre de la selection
            individus_matrix[individus_matrix[individu_selectioned].ancetre].descendant_1 = anc_anc_recombinaison_desc_1;
            individus_matrix[individus_matrix[individu_selectioned].ancetre].descendant_2 = anc_anc_recombinaison_desc_2;

            individus_matrix[individus_matrix[individu_selectioned].ancetre].ancetre = ancetre_de_machin;
            
            individus_matrix[individu_selectioned].ancetre = ind_select_anc;
            individus_matrix[individu_selectioned].descendant_1 = ind_select_desc_1;
            individus_matrix[individu_selectioned].descendant_2 = ind_select_desc_2;
                
            printf("--------------------------------------------------------------\n");
            get_all_informations(individus_matrix, 0, nombre_individu_total);
        }
    }else{
        printf("On ne change rien car la coalescence se situe sur la meme branche.\n");
        compteur_cache++;
    }
    

    //creation d'un nouveau file newick
    char *strtree2 = (char *)malloc(1000*(sizeof(char)));
    //faire une copy de pointeur
    char name_file2[17] = "after_coalescence";
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
