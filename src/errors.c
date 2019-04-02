#include <stdio.h>
#include <stdlib.h>
#include "errors.h"

static void choise_errors(int a){
  switch (a) {
  case 0 : {
    fprintf(stderr, "erreur dans le nombre d'individus - ex pour un arbre avec 4 individus : ./generator_phylogenetic_tree 4 namefile\n");
    break;
  }
  case 1 : {
    fprintf(stderr, "erreur dans le nom du fichier - ex : ./generator_phylogenetic_tree 4 namefile\n");
    break;
  }
  default:
    fprintf(stderr, "Fatal error : trop d'arguments\n");
    break;
  }
}

void get_null_argv(int argc, char *argv[]){
  for( int i = 0; i < argc; i++){
    if( argv[i+1] == NULL ){
      choise_errors(i);
      exit(EXIT_FAILURE);
    }
  }
}

