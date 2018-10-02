Le code fourni permet d'ouvrir un fichier assembleur MIPS,
et d'afficher à l'écran les différents lexèmes qui le composent, avec leur type et leur numéro de ligne.
Ces lexèmes avec leurs informations sont également stockés dans une liste chaînée, renvoyée par lex_load_file.


Le répertoire est organisé ainsi :
.
├── tests
│   └── donnees_simple.s
│   └── miam_sujet_err.s
│   └── miam_sujet.s

├── src
│   └──	error.c
│   └── lex.c
│   └── main.c
│   └──	queue_list.c
│   └──	test.c

├── include
│   └── error.h
│   └── global.h
│   └── lex.h
│   └── notify.h
│   └── queue_list.h
│   └── test.h

├── doc
│   └── livrables_1.pdf
├── README.txt
├── Makefile
└── Doxyfile

- tests : contient les fichiers assembleur pour tester le programme
          miam_sujet_err.s contient volontairement des erreurs détectées par le programme.
- src : qui contient le code C de votre interpréteur et les fonctions de test.
- include : qui contient toutes les définitions de types et prototypes de votre programme.
- doc : qui contient les livrables.
- README.txt : ce document
- Makefile : pour compiler soit en mode debug (afficher les traces du programme) soit en mode release (produit final)
- Doxyfile : fichier de configuration du générateur automatique de documentation doxygen



--- pour compiler le code en mode debug (il créé l'exécutable 'as-mips' qui affiche les traces)
$ make debug


--- pour l'executer
$ ./as-mips FICHIER.S


--- pour executer les tests les tests de fonctions
$ ./as-mips -t


--- pour compiler le code en mode release (il créé l'exécutable 'as-mips' qui n'affiche pas les traces)
$ make release


--- pour activer/deactiver les traces dans l'execution du programme :
ajouter -DVERBOSE aux CFLAGS dans le Makefile


--- Génération de l'archive

Tapez "make archive". Vous aurez produit un fichier se terminant par "tgz".


--- Documentation du code

Si vous voulez documenter le code, regardez la syntaxe appropriée pour
les tags Doxygen :

http://www.stack.nl/~dimitri/doxygen/docblocks.html

Une fois le code commente, tapez "make documentation" (nous vous fournissons
un Doxyfile qui ira bien pour ce que vous avez a faire).

Note : il faut avoir installé le package doxygen. Sous Ubuntu / Debian, tapez :
sudo apt-get install doxygen
