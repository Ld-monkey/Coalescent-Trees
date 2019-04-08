#ifndef MATRICE_ARBRE
#define MATRICE_ARBRE

typedef struct matrice_arbre
{
  int individu;
  int descendant_1;
  int descendant_2;
  int ancetre;
  float longueur_branche;
  float somme_lb;
  float Ti;
  float Temps;
} Matrice_arbre;

extern void readme_information();
extern void initiate_table(int *tableau,int taille);
extern void initiate_matrix(Matrice_arbre *matrix, int nombre_individu_total);
extern void display_table(int *tableau, int taille);
extern int  get_individus_number(int n);
extern void get_all_informations(Matrice_arbre *matrix, int nombre_individu, int nombre_individu_total);
extern void create_phylogenetic_tree(Matrice_arbre *matrix, int *array, int array_start_individu, int nombre_individu, int nombre_individu_total);

//concerne essentiellement les évènement de recombinaison faire un object spécial recombinaison
extern float get_somme_branches(Matrice_arbre *matrix, int last_individu);
extern int get_last_individu(Matrice_arbre *matrix, int individu);



#endif /*MATRICE_ARBRE*/
