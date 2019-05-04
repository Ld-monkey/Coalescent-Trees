#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include "matrice_arbre.h"
#include "tableau.h"
#include "errors.h"
#include "newick.h"
#include "events.h"

int main(int argc, char *argv[])
{
  if( argc > 4){
    get_null_argv(argc, argv);
  }else
  {
    srand(time(NULL));

    /*
    int compteur_cache = 0;
    int compteur_silencieux = 0;
    int compteur_non_silencieux = 0;
    */
    int nbre_echantillon;
    if (argv[3] == NULL)
    {
        //par defaut le nombre d'échantillon est 1
        nbre_echantillon = 1;
        printf("nbre_echantillon %d\n",nbre_echantillon);
    }else{
        nbre_echantillon = atoi(argv[3]);
        printf("nbre_echantillon %d\n",nbre_echantillon);
    }

    char *name_file = argv[2];
    char name_file2[50];
    strcpy(name_file2, name_file);
    strcat(name_file, "_before_coalescence");
    strcat(name_file2, "_after_coalescence");

    for (int i = 0; i < nbre_echantillon; ++i)
    {
            //srand(time(NULL));  
    int nombre_individu = atoi(argv[1]);
    int array_start_individu = nombre_individu;
    int nombre_individu_total = get_individus_number(nombre_individu);
    int *individus_table = malloc(sizeof(int) * nombre_individu);
    float somme_branches, random_time_recombinaison, real_time_recombinaison;
    int last_individu, recombinaison_individu;
    Matrice_arbre individus_matrix[nombre_individu_total];

    /* variables nécessaires a la création du fichier newick*/
    char *strtree = (char *)malloc(1000*(sizeof(char)));


    int position = 0;
    FILE fptr;
    

    readme_information();
    initiate_table(individus_table, nombre_individu);
    initiate_matrix(individus_matrix, nombre_individu_total);
    create_phylogenetic_tree(individus_matrix, individus_table, array_start_individu, nombre_individu, nombre_individu_total);
    get_all_informations(individus_matrix, 0, nombre_individu_total);
    
    /*chaine de caractère recupere la typologie du format newick puis creation d'un fichier newick .nwk*/

    strtree = PrintTree(0, individus_matrix, strtree, &position, nombre_individu);
    create_newick_file(&fptr, name_file, strtree, i);

    free(strtree);

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
    free(individu_concerned_by_coalescence);
    //printf("main : last_individu = %d\n",last_individu);

    individus_matrix[nombre_individu_total] = coalescent_event(individus_matrix, individu_selectioned, recombinaison_individu, event_coalescent, last_individu);
    
    printf("Deuxième get_all_informations\n");
    get_all_informations(individus_matrix, 0, nombre_individu_total);
    /*creation d'un nouveau file newick*/
    char *strtree2 = (char *)malloc(1000*(sizeof(char)));
    //faire une copy de pointeur
    //char name_file2[18] = "after_coalescence";
    int position2 = 0;
    FILE fptr2; 
    //chaine de caractère recupere la typologie du format newick puis creation d'un fichier newick .nwk
    strtree = PrintTree(0, individus_matrix, strtree2, &position2, nombre_individu);
    create_newick_file(&fptr2, name_file2, strtree2, i);
    free(strtree2);
    }
    //printf("cache : %d, silencieux : %d, non_silencieux: %d\n", compteur_cache, compteur_silencieux, compteur_non_silencieux);
  }
    return(EXIT_SUCCESS);
}
