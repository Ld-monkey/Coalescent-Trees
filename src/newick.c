#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include "matrice_arbre.h"
#include "newick.h"


// regarde si la chaine de caractère a bien été chargé et qu'elle n'est pas vide
static void is_charNull(char *str)
{
  if ( str == NULL )
  {
    fprintf(stderr, "je ne peux pas afficher la chaine de caractère : NULL\n");
    exit(EXIT_FAILURE);
  }
}

// regarde si le fichier a été chargé et qu'il n'est pas vide
static void is_fileNull(FILE *file)
{
  if ( file == NULL )
  {
    fprintf(stderr,"Pointor file is null\n");
    exit(EXIT_FAILURE);
  }
}

//ajoute la chaine de charactère dans un fichier avec un nom sous format newick .nwk
void create_newick_file(FILE *file, char *name, char *c, int i)
{
    char before_change[1000];
    char indice[1000];
    strcpy(before_change, name);
    sprintf(indice, "%d", i);
    strcat(name, "_");
    strcat(name, indice);
	strcat(name,".nwk");
    strcat(c, ";");
	file = fopen(name,"w+");
	is_fileNull(file);
	fputs(c,file);
	fclose(file);
    strcpy(name, before_change);
    printf("%s\n",before_change);
    printf("%s\n",indice);
}

// Fonction qui returne une chaine de caractère correspondant au format newick
char* PrintTree(int indiv, Matrice_arbre *matrix, char *strtree, int *position, int nombre_individu)
{
  is_charNull(strtree);

  // recupère le dernier individu qui doit avoir un ancetre -1
	if ( (*position) == 0 )
    {
        for (int i = nombre_individu; i < nombre_individu*2-1; i++)
        {
            if ( matrix[i].ancetre == -1 )
            {
                indiv=i;
                //printf("coucou i = %d\n",i);
            }
        }
    }

    //printf("malloc_usable_size(strtree) : %ld\n", malloc_usable_size(strtree));
    //printf("malloc_usable_size(strtree) -(*position) : %ld\n", malloc_usable_size(strtree)-(*position));
    if ( malloc_usable_size(strtree)-(*position) < 150 )
    {
        size_t sizestrtree = strlen(strtree);
        strtree = realloc(strtree, 2 * sizestrtree ); //2*strlen(strtree)
        if ( strtree == 0 ) 
        {
            fprintf(stdout, "je ne peux pas reallouer");
            exit(EXIT_FAILURE);
        }
    }
    if ( matrix[indiv].descendant_1 == -1 )
    {
        sprintf( strtree + (*position), "%d", indiv);
        float ajout = ceil(log10(indiv+1)) ;
        int ajout_position= (int) ajout;
        if ( ajout_position == 0 ) 
          ajout_position = 1; //PB log(1)=0
        (*position) += ajout_position;

        sprintf( strtree + (*position), ":%7.6f", matrix[indiv].longueur_branche);
        (*position) += 9;     
    }else{
        sprintf(strtree+(*position)++,"(");
        strtree = PrintTree(matrix[indiv].descendant_1, matrix, strtree, position, nombre_individu);
        sprintf(strtree+(*position)++,",");
        strtree = PrintTree(matrix[indiv].descendant_2, matrix, strtree, position, nombre_individu);
        sprintf(strtree+(*position)++,")");
        sprintf(strtree+(*position),":%7.6f",matrix[indiv].longueur_branche);
        (*position)+=9;    
    }
    return strtree;
}