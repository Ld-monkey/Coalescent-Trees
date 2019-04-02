#ifndef NEWICK_H 
#define  NEWICK_H

extern void put_string(FILE *file, char *name, char *c);
extern char* PrintTree(int indiv, Matrice_arbre InfosIndiv[], char *strtree, int * pos, int N);

#endif /*NEWICK_H*/