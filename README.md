# Generator phylogenetic tree
----
## Principe :
----
Le programme "generator phylogenetic tree" écrit en C permet de créer aléatoirement un arbre phylogénétique en fonction de son nombre d'indivu.

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

## Utilisation :
----
L'utilisation est la suivante :

```bash
./generator_phylogenetic_tree <interger>
```

Example sous (Dedian/Ubuntu) :

```bash
./generator_phylogenetic_tree 4
```
La commande suivante génére un arbre aléatoire avec un nombre d'individu égal à 4.
Le paramètre correspondant au nombre d'individu choisi peut être très grand.

## Resultats :
-----
Les résultats s'affichent de la manière suivante :


|individu : | 1er descendant : | 2nd descendant : | Ancêtre : | Ti : | T total : |
|:---------:|:----------------:|:----------------:|:---------:|:----:|:---------:|
|0          |-1                |-1                | 4         |-1    | 0         |

Lorsque la valeur "-1" est renvoyée cela correpond a une valeur NULL c'est à dire qu'elle ne renvoie aucun paramètres illustrés ci-dessus.




## Erreurs :
-----
* Mauvais paramètre :

Lors de l'excution du programme l'erreur suivante apparait :

```C
erreur d'argument - ex pour un arbre avec 4 individus : ./generator_phylogenetic_tree 4 
```
L'erreur provient d'une mauvaise utilisation des parametres voir le paragraphe "Utilisation" pour utiliser correctement la ligne de commande.



