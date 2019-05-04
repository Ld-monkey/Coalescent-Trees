#ifndef NEWICK_H 
#define  NEWICK_H

extern void create_newick_file(FILE *file, char *name, char *c, int i);
extern char* PrintTree(int indiv, Matrice_arbre *matrix, char *strtree, int * pos, int N);

#endif /*NEWICK_H*/