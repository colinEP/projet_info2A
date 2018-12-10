COMPILATION DE LA BIBLIOTHEQUE PELF : 

Pour compiler la bibliothèque décompressez l'archive est placez-vous dans le repertoire racine
	$ make 

Si tous ce passe bien, une bibliothèque dynamique 'libpelf.so' se trouve dans build/


COMPILATION ET EXECUTION DE L'EXEMPLE :

le fichier exemple 'make_mips_elf.c' se trouve à la racine de l'archive 

pour créer l'executable de l'exemple :

	$ make exemple_elf

l'execution créée le fichier binaire ELF exemple.o:
	$./exemple_elf 

que l'on peut analyser à l'aide des utilitaires readelf et objdump
