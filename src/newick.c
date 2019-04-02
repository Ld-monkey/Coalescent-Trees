#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <math.h>
#include "matrice_arbre.h"
#include "newick.h"

static void is_fileNull(FILE *file){
  if( file == NULL){
    fprintf(stderr,"Pointor file is null\n");
    exit(EXIT_FAILURE);
  }
}

void put_string(FILE *file, char *name, char *c){
	strcat(name,".nwk");
	file = fopen(name,"w+");
	is_fileNull(file);
	fputs(c,file);
	fclose(file);
}

// Fonction pour afficher au format newick une structure Matrice_arbre
char* PrintTree(int indiv, Matrice_arbre *matrix, char *strtree, int * pos, int N)
{
	if ((*pos)==0)
    {
        int i;
        for (i=N;i<N*2-1;i++)
        {
            if (matrix[i].ancetre==-1)
                indiv=i;
        }
    }
    if (malloc_usable_size(strtree)-(*pos) < 150)
    {
        size_t sizestrtree = strlen(strtree);
        strtree=realloc(strtree, 2*sizestrtree );//2*strlen(strtree)
        if (!strtree)
        {
            fprintf(stdout, "je ne peux pas reallouer");
            exit(254);
        }
    }
    
    if (matrix[indiv].descendant_1==-1)
    {
        sprintf(strtree+(*pos),"%d",indiv);
        float ajout=ceil(log10(indiv+1));
        int ajout_pos= (int) ajout;
        if (ajout_pos==0) ajout_pos=1; //PB log(1)=0
        (*pos)+=ajout_pos;
        sprintf(strtree+(*pos),":%7.6f",matrix[indiv].longueur_branche);
        (*pos)+=9;     
    }
    else
    {
        sprintf(strtree+(*pos)++,"(");
        strtree=PrintTree(matrix[indiv].descendant_1,matrix,strtree,pos,N);
        sprintf(strtree+(*pos)++,",");
        strtree=PrintTree(matrix[indiv].descendant_2,matrix,strtree,pos,N);
        sprintf(strtree+(*pos)++,")");
        sprintf(strtree+(*pos),":%7.6f",matrix[indiv].longueur_branche);
        (*pos)+=9;
        
    }
    return strtree;
}

/*
void recusif_descendant(Matrice_arbre *matrix, int nombre_individu_total){
  int a = nombre_individu_total;
  //printf("L'indidu selectionné  = %d\n", nombre_individu_total);
  if( matrix[matrix[a].descendant_1].descendant_1 != -1 || matrix[matrix[a].descendant_2].descendant_2 != -1){
    //printf("coucou\n");
    if( matrix[matrix[a].descendant_1].descendant_1 == -1){
      //printf("l'individu %d a pour descendant 1: (%d, et ancetre %d et qui aussi le descendant %d\n",a, matrix[a].descendant_1, matrix[a].ancetre, matrix[matrix[a].descendant_1].descendant_1);
      printf("(%d,", matrix[a].descendant_1);
      recusif_descendant(matrix, matrix[a].descendant_2);
    }
    if( matrix[matrix[a].descendant_2].descendant_2 == -1){
      //printf("l'individu %d a pour descendant 2: (%d, et ancetre %d et qui aussi le descendant %d\n", a, matrix[a].descendant_2, matrix[a].ancetre, matrix[matrix[a].descendant_2].descendant_2);
      printf("(%d,", matrix[a].descendant_2);
      recusif_descendant(matrix, matrix[a].descendant_1);
    }
    if( matrix[matrix[a].descendant_1].descendant_1 != -1){
      //printf("coucou mdr 1\n");
      recusif_descendant(matrix, matrix[a].descendant_1);
      recusif_descendant(matrix, matrix[a].descendant_2);
    }
    if( matrix[matrix[a].descendant_2].descendant_2 == -1 ){
      //printf("coucou mdr 3\n");
      recusif_descendant(matrix, matrix[a].descendant_2);
      recusif_descendant(matrix, matrix[a].descendant_1);
    }
         printf(")");
  }else{
     printf("(%d,%d)",matrix[a].descendant_1, matrix[a].descendant_2);
     
     if()
       printf(")");
     else
       printf("),\n");
     
	}
}
*/
//((2,3),(1,4),0);
//((2,3),(1,4),0);
//((1,2),(3,5),4,0);
//(0,(3,(2,(1,4))));
/*
      //printf("l'individu %d a pour descendant 1: (%d, et ancetre %d et qui aussi le descendant %d\n",a, matrix[a].descendant_1, matrix[a].ancetre, matrix[matrix[a].descendant_1].descendant_1);
      //printf("l'individu %d a pour descendant 2: (%d, et ancetre %d et qui aussi le descendant %d\n", a, matrix[a].descendant_2, matrix[a].ancetre, matrix[matrix[a].descendant_2].descendant_2);
*/