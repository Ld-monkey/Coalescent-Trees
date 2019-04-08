#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <float.h>
#include "matrice_arbre.h"
#include "tableau.h"
#include "random.h"

//ajoute un individu
static void add_individu(Matrice_arbre *m, int individu){
  m->individu = individu;
}

//ajoute tous les individus dans une matrice
static void add_all_individu_in_matrix(Matrice_arbre *m, int nombre_individu){
  for( int i =  0; i < nombre_individu; i++)
    add_individu(&m[i], i);
}

void readme_information(){
  printf("PLEASE README FOR MORE INFORMATIONS\n");
  printf("================ABI================\n");
}

//initialise le tableau par le nombre d'individu
void initiate_table(int *tableau, int taille){
  if ( tableau != NULL ){
    for( int i = 0; i < taille; i++)
      tableau[i] = i;
  }
}

//initialise la matrice
void initiate_matrix(Matrice_arbre *matrix, int nombre_individu_total){
  add_all_individu_in_matrix(matrix, nombre_individu_total);
  for( int i = 0; i < nombre_individu_total; i++){
    matrix[i].descendant_1 = -1;
    matrix[i].descendant_2 = -1;
    matrix[i].longueur_branche = 0;
    matrix[i].somme_lb = 0;
    matrix[i].Ti = 0.0;
    matrix[i].Temps = 0.0;
  }
}

//retourne le nombre d'individu (2 * individu)-1
int get_individus_number(int n){
  return (n*2)-1;
}

//affiche toutes les informations pour un individu donné
void get_all_informations(Matrice_arbre *matrix, int nombre_individu, int nombre_individu_total){
  for(int i = nombre_individu; i < nombre_individu_total; i++){
    printf("i : %d ",matrix[i].individu);
    printf(",d_1: %d ",matrix[i].descendant_1);
    printf(",d_2: %d ",matrix[i].descendant_2);
    printf(",Anc: %d ",matrix[i].ancetre);
    printf("T%d : %g ",i,matrix[i].Ti);
    printf(",l_b : %g ",matrix[i].longueur_branche);
    printf("s_lb : %f ",matrix[i].somme_lb);
    printf(",Tps : %g\n",matrix[i].Temps);
  }
}
  
//ajoute un ancêtre
static void add_ancestor_in_matrix(Matrice_arbre *m, int ancestor){
  m->ancetre = ancestor;
}

//ajoute le Ti d'un individu
static float add_ti(int indice){
  float rand_float = random_float();
  float logarithme = -log(rand_float);
  float esperance = ((indice*(indice-1))/2);;
  return (logarithme/esperance);
}

//selectionne 1 individu dans un tableau
static int select_random_individu(int *array, int taille){
  int i = random_position(0, taille);
  int resultat = array[i];
  switch_value(array, i, taille);
  array = resize_size_table(array, taille);
  return resultat;
}

void create_phylogenetic_tree(Matrice_arbre *matrix, int *array, int array_start_individu, int nombre_individu, int nombre_individu_total){
  //Selectionne au hazard deux individus  
  for( int i = array_start_individu; i < nombre_individu_total; i++){
    for( int y = 0; y < 2; y++){
      int resultat = select_random_individu(array, nombre_individu);
      add_ancestor_in_matrix(&matrix[resultat], i);
      nombre_individu = nombre_individu - 1;
    }
    add_value_in_table(array, i, nombre_individu);
    nombre_individu += 1;
  }
  //le dernier individu n'a pas d'ancêtre
  matrix[nombre_individu_total-1].ancetre = -1;

  //une fois les ancêtres ajoutés on peut retrouver les descendants
  for( int i = array_start_individu; i < nombre_individu_total; i++){
    for( int y = 0; y < nombre_individu_total; y++){
      if( matrix[y].ancetre == i && matrix[i].descendant_1 == -1)
	       matrix[i].descendant_1 = matrix[y].individu;
      else if( matrix[y].ancetre == i && matrix[i].descendant_1 != -1)
	       matrix[i].descendant_2 = matrix[y].individu;
    }
  }

  //ajoute les Ti correspondant + Temps
  for( int i = array_start_individu, y = array_start_individu; i < nombre_individu_total; i++, y--){
    //printf("i = %d et y = %d\n",i,y);
    matrix[i].Ti = add_ti(y);
    matrix[i].Temps +=  matrix[i].Ti + matrix[i-1].Temps;
  }

  float result = 0.0;
  //ajoute les longueurs de branches
  for (int i = 0 ; i < nombre_individu_total - 1; ++i)
  {
    matrix[i].longueur_branche = matrix[matrix[i].ancetre].Temps - matrix[i].Temps;
    result += matrix[i].longueur_branche;
    matrix[i].somme_lb = result;
  }
  matrix[nombre_individu_total - 1].somme_lb = result;
}

//retourne la somme des longueurs des branches
float get_somme_branches(Matrice_arbre *matrix, int last_individu)
{
  float result = 0;
  for (int i = 0; i < last_individu; ++i)
    result += matrix[i].longueur_branche;
  return result; 
}

//returne le dernier individu
int get_last_individu(Matrice_arbre *matrix, int individu)
{
  return matrix[individu-1].individu;
}
