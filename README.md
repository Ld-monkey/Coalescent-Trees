# Generator phylogenetic tree
----
## Principe :
----
Le programme "generator phylogenetic tree" écrit en C est un petit programme qui génère aléatoirement un arbre phylogénétique. Plusieurs paramètres permettent de varier son l'application comme le nombre d'indivu ou la simulation du phénomène de recombinaison.

## Installation :
----
Aller dans le répertoire ou se trouve le programme et taper dans l'invite de commande :

* sous (Debian/Ubuntu):
```bash 
make
```
Si le programme n'est pas exécutable :

* sous (Debian/Ubuntu):
```bash
chmod +x generator_phylogenetic_tree
```

#### Il aura pour arborescence :

```bash 
├── generator_phylogenetic_tree
├── inc
│   ├── errors.h
│   ├── matrice_arbre.h
│   └── newick.h
├── Makefile
├── obj
├── output
├── README
├── README.md
└── src
    ├── errors.c
    ├── main_arbre.c
    ├── matrice_arbre.c
    └── newick.c
```

## Utilisation :
----

L'utilisation est la suivante :

```sh
./generator_phylogenetic_tree <interger> <ouput/namefile>
```

Example sous (Dedian/Ubuntu) :

```sh
./generator_phylogenetic_tree 10 output/namefile
```
La commande suivante a généré un arbre phylogénétique comportant 10 individus selectionnés de manière aléatoire et un fichier de sortie "newick" (.nwk) qui nous permettra de visualiser le résultat.

#### paramètres :
* __interger__ :
Ce paramètre correspond au nombre d'individu choisi et il peut être très grand.

* __output/namefile__ : Ce paramètre indique le répertoire (ici c'est output par défaut) ainsi que le nom du fichier de sortie.

## Resultats :
-----
Les résultats s'affichent de la manière suivante :

##### Exemples pour 5 individus:
---


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

##### Visualisation du fichier de sortie
----
![alt text](namefile.png "Example_newick_file")

Visualisation du résultat à l'aide du logiciel FigTree.

## Erreurs :
-----
* Mauvais paramètres :

Lors de l'excution du programme l'erreur suivante apparait :

```sh
erreur dans le nombre d'individus - ex pour un arbre avec 4 individus : ./generator_phylogenetic_tree 4 output/namefile
```
* L'erreur provient du fait qu'aucun paramètre ne précise le nombre de d'individu souhaité.

```sh
erreur dans le nom du fichier - ex : ./generator_phylogenetic_tree 4 output/namefile
```
* L'erreur vient du fait qu'il n'y a aucun nom qui a été précisé dans les paramètres.




