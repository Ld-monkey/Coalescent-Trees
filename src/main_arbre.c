#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <ctype.h>
#include "matrice_arbre.h"
#include "errors.h"
#include "newick.h"

int main(int argc, char *argv[])
{
  if( argc != 3)
    get_null_argv(argc, argv);
  {
    FILE fptr;      
    char *name = argv[2];
    int nombre_individu = atoi(argv[1]);
    int array_start_individu = nombre_individu;
    int nombre_individu_total = get_individus_number(nombre_individu);
    int *individus_table = malloc(sizeof(int) * nombre_individu);
    float somme_branches, time_recombinaison;
    int last_individu, recombinaison_individu;
    Matrice_arbre individus_matrix[nombre_individu_total];

    readme_information();
    initiate_table(individus_table, nombre_individu);
    initiate_matrix(individus_matrix, nombre_individu_total);
    create_phylogenetic_tree(individus_matrix, individus_table, array_start_individu, nombre_individu, nombre_individu_total);
    get_all_informations(individus_matrix, 0, nombre_individu_total);
    
    //Afficher le format newick
    char * strtree; //chaine de caractere dans laquelle on ecrit
    strtree=(char*)malloc(1000*(sizeof(char)));
    if (strtree==NULL)
    {
        fprintf(stdout, "je ne peux pas afficher printtree"), exit(35);
    }
    strtree[999]=0; //Fin de la chaine
    int pos=0;
    strtree=PrintTree(0,individus_matrix,strtree,&pos,nombre_individu);
    strtree[pos]=0;
    //printf("%s;\n", strtree);
    put_string(&fptr, name, strtree);

    last_individu = get_last_individu(individus_matrix, nombre_individu_total);
    somme_branches = get_somme_branches(individus_matrix, last_individu);
    time_recombinaison = random_recombinaison(somme_branches);
    
    printf("Somme des branches = %f\n",somme_branches);
    printf("Pour le chiffre au hazard entre 0 et somme_branches : %f on obtient : %f\n",somme_branches, time_recombinaison); 
    
    recombinaison_individu = get_individu_event_recombinaison(individus_matrix, time_recombinaison, last_individu);
    printf("L'évènement de recombinaison a eu lieu sur l'individu %d\n",recombinaison_individu); 

    free(strtree);
    return(EXIT_SUCCESS);
    
  }
}
