#ifndef _RELOC_H_
#define _RELOC_H_

// enum type_reloc {R_MIPS_32=2, R_MIPS_26=4, R_MIPS_HI16=5, R_MIPS_LO16=6}
//
// typedef struct {
//     char*        name;     //nom de la section
//     unsigned int offset;   //adresse relative du code (instruction ou data) à reloge
//     type_reloc   type;
//     ETIQ*        symb;     //pointeur vers le symbole de l’opérande dans la table des symboles
// }* RELOC;
//
//
// build_reloc_text( /*section*/ , /*LIST*/ , /*list_section*/ , );
// build_reloc_data( /*section*/ , /*LIST*/ , /*list_section*/ , );
//
//     LIST
//     parcour list_data

#endif
