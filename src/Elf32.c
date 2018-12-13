#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <strings.h>

#include <fct_analyse_1.h>
#include <global.h>
#include <notify.h>
#include <lex.h>
#include <queue_list.h>
#include <test.h>
#include <etiq.h>
#include <dictionnaire.h>
#include <in_binar.h>

#include <error.h>
#include <make_elf.h>
#include <etiq.h>
#include <fct_analyse_1.h>

#include <pelf.h>   // initialement <pelf/pelf.h>
#include <section.h> // initialement <pelf/section.h>


// Elf32_Sym syms[2]= {{0}};
// // boucle
// syms[0].st_name = elf_get_string_offset( strtab->start, strtab->sz, sym_char[0] );
// syms[0].st_size = 0;
// syms[0].st_value = 4;
// syms[0].st_info = ELF32_ST_INFO( STB_LOCAL, STT_NOTYPE );
// syms[0].st_other = 0;
// syms[0].st_shndx  = elf_get_string_index( shstrtab->start, shstrtab->sz, ".text" );
// // tab
// syms[1].st_name = elf_get_string_offset( strtab->start, strtab->sz, sym_char[1]);
// syms[1].st_value = 16;
// syms[1].st_size = 0;
// syms[1].st_info = ELF32_ST_INFO( STB_LOCAL, STT_NOTYPE );
// syms[1].st_other = 0;
// syms[1].st_shndx = elf_get_string_index( shstrtab->start, shstrtab->sz, ".bss" );


Elf32_Sym* make_syms(int size_table, char** sym_tab, section strtab, section shstrtab, LIST table_des_symboles){ // OK 

    Elf32_Sym* syms = calloc(size_table, sizeof(Elf32_Sym));
    int i = 0;
    ETIQ E;
    while (i < size_table){
        syms[i].st_name = elf_get_string_offset( strtab->start, strtab->sz, sym_tab[i] );
        printf("st_name = %d\n", syms[i].st_name);
        syms[i].st_size = 0; // toujours à 0
        printf("st_size = %d\n", syms[i].st_size);
        E = look_for_etiq_and_return( table_des_symboles, sym_tab[i]);
        syms[i].st_value = E->decalage ; // aller chercher l'adresse relative dans la table des symboles
        if (E->def_in_file){
            syms[i].st_info = ELF32_ST_INFO( STB_LOCAL, STT_NOTYPE );
        }
        else{
            syms[i].st_info = ELF32_ST_INFO( STB_GLOBAL, STT_NOTYPE );
        }
        printf("st_info = %d\n", syms[i].st_info);
        syms[i].st_other = 0; // toujours à 0
        printf("st_other = %d\n", syms[i].st_other);
        if (E->section == TEXT){
            syms[i].st_shndx  = elf_get_string_index( shstrtab->start, shstrtab->sz, ".text" );
        }
        if (E->section == PDATA){
            syms[i].st_shndx  = elf_get_string_index( shstrtab->start, shstrtab->sz, ".data" );
        }
        if (E->section == BSS){
            syms[i].st_shndx  = elf_get_string_index( shstrtab->start, shstrtab->sz, ".bss" );
        }
        printf("st_shndx = %d\n", syms[i].st_shndx);
        i = i+1;
    }
    return syms;

}
