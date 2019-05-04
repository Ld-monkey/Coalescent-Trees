#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <float.h>
#include "random.h"

//tire aléatoirement un entier dans un interval donné
int 
random_position(int a, int b){
  return rand()%(b-a)+a;
}

//tire aléatoirement un float entre 0 exclus et 1 inclus
float 
random_float(){
  float result;
  do{
    result = (float)rand()/RAND_MAX;
  }while(result == 0.0);
  return result;
}