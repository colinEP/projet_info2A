#ifndef _ELF_32_H
#define _ELF_32_H

Elf32_Sym* make_syms(int size_table, char** sym_tab, section strtab, section shstrtab, LIST table_des_symboles);
Elf32_Rel* make_elf32_reloc(int size_table, LIST reloc_table, section symtab, section strtab, section shstrtab );

#endif
