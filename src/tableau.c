#include <stdio.h>
#include <malloc.h>
#include "tableau.h"

//affiche un tableau en fonction de sa taille
void 
diplay_table(int *tableau, int taille)
{
  printf("[");
  for ( int i = 0; i < taille; i++ )
  {
    printf(" %d", tableau[i]);
  }
  printf(" ]\n");
}

//redimentionne le tableau
int*
resize_size_table(int *tableau, int taille)
{
  int *tmp = malloc(sizeof(int) *(taille-1));
  for ( int i = 0; i < taille - 1; i++)
  {
    tmp[i] = tableau[i];
  }
  tableau = tmp;
  taille = taille - 1;  
  return tmp;
}

//decaler une valeur d'un tableau
void
switch_value(int *tableau, int indice, int taille)
{
  int i;
  for(i = indice; i < taille - 1; i++)
  {
    tableau[i] = tableau[i+1];
  }
}

//ajoute une valeur dans un tableau
void
add_value_in_table(int *tableau, int x, int taille){
  tableau = realloc(tableau, sizeof(int)*(taille+1));
  tableau[taille] = x;
}