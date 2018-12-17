
Projet réalisé dans le cadre du projet-info de 2A à Phelma
Septembre->décembre 2018
Par Eléonore Barges & Colin Epalle


------ Desciption globale des fonctions de ce repertoire -------

	Le code fourni permet d'assembler un fichier assembleur MIPS. Il affiche à l'écran
les différents lexèmes qui le composent, avec leur type et leur numéro de ligne.
Il les stocke dans 3 tables : list_instr, list_data et list_bss, lesquelles regroupent
les instructions et directives, ainsi que leurs arguments, propres au section .TEXT, .DATA et .BSS.
Les étiquettes sont, elles, stockées dans la table des symboles avec un ordre au choix
(voir plus bas, variable reorder). En outre, ce programme permet de stocker dans
les tables de relocations .data et .text les indications qui permettront de faire
une relocation lors de certains appels d'étiquettes spécifiques. Les informations
qui y sont stockées sont la section dans laquelle doit être faite la relocation,
le décalage correspondant à la zone devant être réallouée, le type de relocation
et enfin le symbole (c'est-à-dire le nom de la section ou le nom de l'étiquette
lors de symboles globaux) par rapport auquel il faudra faire la relocation.
Enfin, les instructions et les données de la list_data sont traduites en binaire,
swapées de little Endian à big Endian, avant d'être fournies à une fonction qui
produit le fichier .o associé au fichier .s analysé.

La variable reorder à la ligne 140 de main.c permet de choisir l'ordre de la table des symboles :
	TRUE  => table des symboles par ordre d'apparition
	FALSE => table des symboles par ordre de définition
	         symboles globaux à la fin


Le répertoire est organisé ainsi :
.
├── tests
│   └── *.s
│   └── *.o
│   └── *.o.ref


├── src
│   └──	analyse_synth.c
│   └── dictionnaire.c
│   └── Elf32.c
│   └── error.c
│   └── etiq.c
│   └── fct_analyse_1.c
│   └── in_binar.c
│   └── lex.c
│   └── main.c
│   └──	make_mips_elf_initial.c
│   └──	print_functions.c
│   └──	queue_list.c
│   └──	register.c
│   └──	reloc.c
│   └──	test_register.c
│   └── test_state_machine.c


├── include
│   └──	analyse_synth.h
│   └── dictionnaire.h
│   └── elf-mips32.h
│   └── elf.h
│   └── Elf32.h
│   └── error.h
│   └── etiq.h
│   └── fct_analyse_1.h
│   └── global.h
│   └── in_binar.h
│   └── lex.h
│   └──	make_elf.h
│   └── notify.h
│   └── pelf.h
│   └──	print_functions.h
│   └──	queue_list.h
│   └──	reloc.h
│   └── section.h
│   └── test.h

├── doc
│   └── livrable_1.pdf
    └── livrable_2.pdf
    └── livrable_3.pdf
    └── livrable_4.pdf
├── README.txt
├── Makefile
├── Doxyfile
├── dictionnaire.txt
├── libpelf.so
└── biblioth_pelf


- tests : contient les fichiers assembleur et ELF pour tester le programme
		  Certains fichiers comportent volontairement des erreurs
- src : qui contient le code C de l'interpréteur et les fonctions de test.
- include : qui contient toutes les définitions de types et prototypes de votre programme.
- doc : qui contient les livrables.
- README.txt : ce document
- Makefile : pour compiler soit en mode debug (afficher les traces du programme) soit en mode release (produit final)
- Doxyfile : fichier de configuration du générateur automatique de documentation doxygen
- dictionnaire.txt : contient la liste des instructions avec le nombre d'arguments attendus, ainsi que leur type, et des informations sur le codage en binaire pour chaque instruction.
- libpelf.so et biblioth_pelf : fournisse les fonctions nécessaires pour la traduction finale des données en binaire en un fichier .o. (dans les fonctions appelées dans make_mips_elf_initial.c) .



--- pour compiler le code en mode debug (il créé l'exécutable 'as-mips' qui affiche les traces)
$ make debug


--- pour l'executer
$ ./as-mips FICHIER.S


--- pour executer des tests de fonctions
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
