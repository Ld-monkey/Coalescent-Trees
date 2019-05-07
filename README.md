# Coalescent trees

## Principe :

Le programme "coalescent trees" est un programme écrit en C qui s'appuie sur la théorie de la coalescence avec recombinaison pour générer de manièrer stochastique des arbres phylogénétiques.

## Installation :

Aller dans le répertoire ou se trouve le programme "coalescent_trees" et taper dans l'invite de commande ou le terminal :

* sous (Debian/Ubuntu):
```bash 
make
```

Si le programme n'est pas exécutable :
* sous (Debian/Ubuntu):
```bash
chmod +x coalescent_trees
```

De même pour rendre le fichier Bash éxecutable :
* sous (Debian/Ubuntu):
```bash
chmod +x run_coalescence.sh
```

#### L'arborescence du programme:

```bash 
.
├── coalescent_trees
├── inc
│   ├── errors.h
│   ├── events.h
│   ├── matrice_arbre.h
│   ├── newick.h
│   ├── random.h
│   └── tableau.h
├── Makefile
├── README.md
├── run_coalescence.sh
└── src
    ├── errors.c
    ├── events.c
    ├── main_arbre.c
    ├── matrice_arbre.c
    ├── newick.c
    ├── random.c
    └── tableau.c
```

# Utilisation :
## 2 approches sont possibles :
### L'approche avec le fichier Bash "run_coalescence.sh"
La mise a disposition du fichier Bash facilite l'utilisation du programme en créant en autre le ou les dossiers a l'interieur desquels sont mis les résultats obtenus.

L'utilisation du fichier Bash est la suivante :
```sh
./run_coalescence.sh -ind <interger> -name <namefile> -size <size_of_sample> -folder <name_of_folder>
```
#### paramètres :

* __-ind__ :
Nombre d'individu.
* __-name__ :
Nom du fichier qui sera mis sous format .nwk ou newick par défaut et suivi par _before_coalescence_ ou _after_coalescence_. Par défaut le nom est "newick".

* __-size__ :
Taille de l'échantillon par défaut il est a 1.

* __-folder__ :
Nom du dossier qui contient les arbres phylogénétiques sous format newick. Par défaut le nom du dossier est "output".

Exemple sous terminal (Debian/Ubuntu):
```sh
./run_coalescence.sh -ind 5 -name newick -size 10 -folder output
```
L'exemple ci-dessus montre qu'il y a 5 individus dans mes arbres phylogénétiques avec un nom de sortie qui est "newick" et qui nomera les fichiers de la façon suivante : "newick_before_coalescence_0.nwk" et "newick_after_coalescence_0.nwk" et avec une taille d'échantillon de 10 et un dossier sous le nom de "output" qui contient nos résultats.

### L'approche sans le fichier Bash 

L'utilisation est la suivante :

```sh
./coalescent_trees <interger> <namefile> <size_of_sample>
```

#### paramètres :

* __interger__ :
Ce paramètre correspond au nombre d'individu choisi et il peut être très grand.

* __namefile__ : Ce paramètre indique le répertoire (ici c'est output par défaut) ainsi que le nom du fichier de sortie.

* __size-of-sample__ :
Taille de l'échantillon par défaut il est a 1. 

Example sous (Dedian/Ubuntu) :

```sh
./coalescent_trees 5 newick 10
```
La commande suivante a généré des arbres phylogénétiques comportant 5 individus selectionnés de manière aléatoire une taille d'échantillon de 10 et des fichier de sortie "newick" (.nwk) qui nous permet de visualiser le résultat.

## Resultats :

##### Les sorties sous format newick ou .nwk

<img src="https://user-images.githubusercontent.com/39624027/57314481-fd438280-70f1-11e9-8626-d0e884081d29.jpg"  width="100" height="100"/>

<img src="https://user-images.githubusercontent.com/39624027/57314500-09c7db00-70f2-11e9-91fe-11c67e76c764.jpg"  width="100" height="100"/>

Exemple de résultats obtenus. On peut observer l'avant et l'après d'un évènement de recombinaison pour un arbre reconstitué aléatoirement.

La visualisation des arbres grâce au logiciel [FigTree.](http://tree.bio.ed.ac.uk/software/figtree/)


##### Les sorties sous forme d'un tableau dans le terminal:


|individu : | 1er descendant : | 2nd descendant : | Ancêtre : | Ti :     | longueur branche   : | Temps    :|
|:---------:|:----------------:|:----------------:|:---------:|:--------:|:--------------------:|:---------:|
|0          |-1                |-1                | 6         |0         | 0.188046             |0          |
|1          |-1                |-1                | 5         |0         | 0.0305699            |0          |
|2          |-1                |-1                | 6         |0         | 0.188046             |0          |
|3          |-1                |-1                | 5         |0         | 0.0305699            |0          |
|4          |-1                |-1                | 7         |0         | 0.398623             |0          |
|5          | 1                | 3                | 8         |0.0305699 | 1.85586              |0.0305699  |
|6          | 0                | 2                | 7         |0.157476  | 0.210577             |0.188046   |
|7          | 4                | 6                | 8         |0.210577  | 1.4878               |0.398623   |
|8          | 5                | 7                |-1         |1.4878    | 0.0                  |1.88643    |


Lorsque la valeur "-1" est renvoyée cela correpond a une valeur NULL c'est à dire qu'elle ne renvoie aucun paramètres illustrés ci-dessus.

## Erreurs :

* Mauvais paramètres :

Lors de l'excution du programme l'erreur suivante apparait :

```sh
erreur dans le nombre d'individus - ex pour un arbre avec 4 individus : ./coalescent_trees 4 namefile
```
* L'erreur provient du fait qu'aucun paramètre ne précise le nombre de d'individu souhaité.

```sh
erreur dans le nom du fichier - ex : ./coalescent_trees 4 namefile
```
* L'erreur vient du fait qu'il n'y a aucun nom qui a été précisé dans les paramètres.




