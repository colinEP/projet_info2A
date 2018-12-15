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
#include <elf.h>


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
        printf("st_value = %d\n", syms[i].st_value);
        if (E->def_in_file){
            syms[i].st_info = ELF32_ST_INFO( STB_LOCAL, STT_NOTYPE );
        }
        else{
            syms[i].st_info = ELF32_ST_INFO( STB_GLOBAL, STT_NOTYPE );
        }
        printf("st_info = %d\n", syms[i].st_info);
        syms[i].st_other = 0; // toujours à 0
        printf("st_other = %d\n", syms[i].st_other);
        if (E->def_in_file) {
            if      (E->section == TEXT) {
                syms[i].st_shndx  = elf_get_string_index( shstrtab->start, shstrtab->sz, ".text" );
            }
            else if (E->section == PDATA) {
                syms[i].st_shndx  = elf_get_string_index( shstrtab->start, shstrtab->sz, ".data" );
            }
            else if (E->section == BSS) {
                syms[i].st_shndx  = elf_get_string_index( shstrtab->start, shstrtab->sz, ".bss" );
            }
        }
        else syms[i].st_shndx = SHN_UNDEF;

        printf("st_shndx = %d\n", syms[i].st_shndx);
        i = i+1;
    }
    return syms;
}

// Elf32_Rel text_reloc[1];
// text_reloc[0].r_offset =4;
// text_reloc[0].r_info=ELF32_R_INFO(elf_get_sym_index_from_name(symtab, shstrtab, strtab,".text"),R_MIPS_LO16);
// Elf32_Rel data_reloc[1];
// data_reloc[0].r_offset =0;
// data_reloc[0].r_info=ELF32_R_INFO(elf_get_sym_index_from_name(symtab, shstrtab,strtab,".bss"),R_MIPS_32);


Elf32_Rel* make_elf32_reloc(int size_table, LIST reloc_table, section symtab, section strtab, section shstrtab ){
    Elf32_Rel* elf32_reloc = calloc(size_table, sizeof(Elf32_Rel));
    int i = 0;
    RELOC R;
    while(i < size_table){
        R = reloc_table->element;
        elf32_reloc[i].r_offset = R->adress; // mettre l'adresse relative par rapport à la section =
        elf32_reloc[i].r_info=ELF32_R_INFO(elf_get_sym_index_from_name(symtab, shstrtab, strtab, R->addend),R->type_r);
        i = i + 1;
        reloc_table = reloc_table-> next;
    }
    return elf32_reloc;
}
