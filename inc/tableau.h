#ifndef TABLEAU_H
#define TABLEAU_H

extern void diplay_table(int *tableau, int taille);
extern int* resize_size_table(int *tableau, int taille);
extern void switch_value(int *tableau, int indice, int taille);
extern void add_value_in_table(int *tableau, int x, int taille);

#endif /*TABLEAU*/