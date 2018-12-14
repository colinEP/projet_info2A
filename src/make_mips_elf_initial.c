/**
 * @file section.c
 * @author François Cayre <francois.cayre@grenoble-inp.fr>
 * @date Wed Sep 20 11:15:28 2017
 * @author François Portet
 * @date Nov 20, 2018
 * @brief Example file to generate ELF object file.
 *
 */

#include <stdlib.h>
#include <string.h>
#include <make_elf.h>

#include <pelf.h>   // initialement <pelf/pelf.h>
#include <section.h> // initialement <pelf/section.h>
#include <queue_list.h>
#include <Elf32.h>


/**
* Looks for a specific name in strtab and symtab and return the index of the symbol corresponding to the name
*
* @param symtab the symbole table
* @param strtab the string table
* @param shstrtab the section string table
* @param sym_name the name for which a symbol is searched for
*
* @return the index of the symbol in symtab or -1 if not found
*/
int elf_get_sym_index_from_name(section symtab, section shstrtab, section strtab, char* sym_name) {
    int i=0;
    Elf32_Sym* symboles = (Elf32_Sym*) symtab->start;
    int section_offset = elf_get_string_index( shstrtab->start, shstrtab->sz, sym_name );
    int offset = elf_get_string_offset( strtab->start, strtab->sz, sym_name );

    if (offset > 0 || section_offset > 0 ) {
        for (i=0; i < (symtab->sz)/sizeof(Elf32_Sym); i++) {
            if (ELF32_R_TYPE(symboles[i].st_info) == STT_SECTION) {
                if (symboles[i].st_shndx == section_offset) {
                    return i;
                }
            }
            else if (symboles[i].st_name == offset)
                return i;
        }
    }
    return -1;

}


#define add_string_to_table( section, symbol ) /* Explicitly add terminal '\0' */ \
  write_section( section, (unsigned char *)symbol, 1+strlen(symbol), section->sz)
/**
* @brief Generates a predefined shstrtab section containing the basic sections names
* @return the completed shstrtab section
*/
section make_shstrtab_section( void ) {

    section shstrtab = new_section( ".shstrtab", SECTION_CHUNK_SZ );
    //printf("JE SUIS DANS LE make_shstrtab_section 1 ! \n\n");
    add_string_to_table( shstrtab, "" ); /* ELF string tables start with a '0' */
    add_string_to_table( shstrtab, ".text" );
    add_string_to_table( shstrtab, ".rel.text" );
    add_string_to_table( shstrtab, ".data" );
    add_string_to_table( shstrtab, ".rel.data" );
    add_string_to_table( shstrtab, ".bss" );
    add_string_to_table( shstrtab, ".symtab" );
    add_string_to_table( shstrtab, ".strtab" );
    add_string_to_table( shstrtab, ".shstrtab" );

    //printf("JE SUIS DANS LE make_shstrtab_section 2 ! \n\n");

    return shstrtab;
}


/**
 * @brief Generates the strtab section from an array of char *
 * @param symbols the a sorted array or char *
 * @param nb_sym the number of symbol to include in the section
 *
 * @return the completed strtab section
 */

section make_strtab_section( char* symbols[], int nb_sym) {
    section strtab = new_section( ".strtab", SECTION_CHUNK_SZ );

    add_string_to_table( strtab, "" ); /* ELF string tables start with a '0' */
    int i= 0;
    for (i= 0; i< nb_sym; i++) {
        add_string_to_table( strtab, symbols[i] );

    }

    return strtab;
}


/**
 * @brief Generates the data section from an array of int already in big endian
 *
 * @param  data_prog array of binary value
 * @param  nb_data size of the array
 *
 * @return the completed section
 *
 * the function writes the whole data in one shot to the section.
 */
section make_data_section( int data_prog[], int nb_data ) {
    section        data = new_section( ".data", SECTION_CHUNK_SZ );

    if (nb_data>0) {
        write_section( data, (unsigned char *)(data_prog), sizeof(*data_prog)*nb_data, 0);
    }
    return data;
}

/**
 * @brief Generates the text section from an array of int already in big endian
 *
 * @param  instructions array of binary value
 * @param  nb_inst size of the array
 *
 * @return the completed section
 *
 * the function writes the whole data in one shot to the section.
 */
section make_text_section( int instructions[], int nb_inst) {
    section        text = new_section( ".text", SECTION_CHUNK_SZ );

    if (nb_inst>0) {
        write_section( text, (unsigned char *)(instructions), sizeof(*instructions)*nb_inst, 0);
    }
    return text;
}

/**
 * @brief Generates the bss section from the size of the section
 *
 * @param  bss_prog the size of the bss section in number of bytes
 *
 * @return the completed section
 *
 * Since bss is only composed of zero, only the size of the section is necessary in the elf file
 */
section make_bss_section( int bss_prog) {
    section bss = new_section( ".bss", SECTION_CHUNK_SZ );
    bss -> sz =bss_prog;
    return bss;
}


/**
 * @brief Generates the symtab section from an array of symbols
 *
 * @param  shstrtab the section of the string table of sections
 * @param  strtab the string table section
 * @param  symbols array of Elf32_Sym. The symbols are supposed to be already SORTED.
 * @param  nb_syms size of the array
 *
 *
 * @return the completed section
 *
 * shstrtab and strtab are necessary to bind the symbol names to the symbol entries.
 * Every symtab starts with an empty entry followed by the 3 standard sections :
 *     - text of type SECTION
 *     - data of type SECTION
 *     - bss of type SECTION
 *
 * These entries are present even if they are not in the original text code
 *
 */
section make_symtab_section(section shstrtab, section strtab,  Elf32_Sym symbols[], int nb_syms  ) {

    section    symtab = new_section( ".symtab", SECTION_CHUNK_SZ );
    int i;
    unsigned int offset = 0;

    Elf32_Sym   entry= {0};
    entry.st_name  = 0;
    entry.st_value = 0;
    entry.st_size  = 0;
    entry.st_info  = 0;
    entry.st_other = 0;
    entry.st_shndx = SHN_UNDEF;

    write_section( symtab, (unsigned char *)&entry, sizeof(entry), offset);


    entry.st_name  = 0;
    entry.st_value = 0;
    entry.st_size  = 0;
    entry.st_info  = ELF32_ST_INFO( STB_LOCAL, STT_SECTION );
    entry.st_other = 0;
    entry.st_shndx = elf_get_string_index( shstrtab->start, shstrtab->sz, ".text" );

    write_section( symtab, (unsigned char *)&entry, sizeof(entry), symtab->sz);

    entry.st_name  = 0;
    entry.st_value = 0;
    entry.st_size  = 0;
    entry.st_info  = ELF32_ST_INFO( STB_LOCAL, STT_SECTION );
    entry.st_other = 0;
    entry.st_shndx = elf_get_string_index( shstrtab->start, shstrtab->sz, ".data" );

    write_section( symtab, (unsigned char *)&entry, sizeof(entry), symtab->sz);

    entry.st_name  = 0;
    entry.st_value = 0;
    entry.st_size  = 0;
    entry.st_info  = ELF32_ST_INFO( STB_LOCAL, STT_SECTION );
    entry.st_other = 0;
    entry.st_shndx = elf_get_string_index( shstrtab->start, shstrtab->sz, ".bss" );

    write_section( symtab, (unsigned char *)&entry, sizeof(entry), symtab->sz);

    for (i =0; i< nb_syms ; i++) {
        write_section( symtab, (unsigned char *)&symbols[i], sizeof(symbols[i]), symtab->sz);
    }
    return symtab;
}


/**
 * @brief Generates a relocation section from an array of Elf32_Rel
 *
 * @param relname the name of the relocation section (e.g., rel.text, rel.data)
 * @param relocations relocation entries already ready to be writen and SORTED
 * @param nb_reloc size of the array
 *
 * @return the completed section
 *
 */
section make_rel32_section(char *relname, Elf32_Rel relocations[], int nb_reloc) {

    section reltab = new_section( relname, SECTION_CHUNK_SZ );
    int i ;

    for (i=0; i<nb_reloc; i++) {
        Elf32_Rel rel = relocations[i];
        write_section( reltab, (unsigned char *)&rel, sizeof( rel ), reltab->sz);

    }
    return reltab;
}

/**
* create the following program as an ELF object file :
*
*==========================
*
* .text
* ADD $2, $3, $4
* boucle : ADDI $2, $3, boucle
* BEQ $2, $3, boucle
* .data
* .word tab
* .bss
* .space 0x10
* tab: .space 8
*
*==========================
*
* expected symtab :
*    .text
*    .data
*    .bss
*    boucle @4 .text
*    tab @16 .bss
*
* expected text_reloc :
*    @4 R_MIPS_LO16 .text
* expected data_reloc :
*    @16 R_MIPS_32 .bss
*
*
* the output binary is called exemple.o
*/



int main_init_function(int* text_tab, int* data_tab, char** sym_tab, int size_instr, int size_data, int size_table, int spaces_needed_in_bss, LIST table_des_symboles, LIST reloc_table_text, LIST reloc_table_data, int size_reloc_text, int size_reloc_data) {

    /* prepare sections*/
    section     text = NULL;
    section     data = NULL;
    section      bss = NULL;
    section shstrtab = NULL;
    section   strtab = NULL;
    section   symtab = NULL;
    section  reltext = NULL;
    section  reldata = NULL;


    /* make predefined section table*/
    shstrtab = make_shstrtab_section();

    int* text_prog= text_tab;  // INUTILE ! utilise driectement text_tab
    int* data_prog= data_tab;  // IDEM
    int bss_prog = spaces_needed_in_bss;   //IDEM
    char ** sym_char = sym_tab;
    char* machine = "mips";
    char* name = "donnees_simples.o";
    /* pelf options */
    int noreorder =1;


    /* Create text, data and bss sections*/
    text     = make_text_section(text_prog, size_instr);

    if ( !text ) {
        fprintf( stderr, "Unable to write .text section (missing information).\n" );
        return -1;
    }

    data = make_data_section(  data_prog, size_data);
    if ( !data ) {
        fprintf( stderr, "Unable to write .data section (missing information).\n" );
        return -1;
    }


    bss = make_bss_section(  bss_prog);
    if ( !bss ) {
        fprintf( stderr, "Unable to write .bss section (missing information).\n" );
        return -1;
    }


    /* Write all string linked to symbols*/
    strtab   = make_strtab_section( sym_char, size_table);

    /* make hard coded symbols data (needs shstrtab and strtab)
       "boucle" is in strtab, has value (relative @) 4 and is defined in the text section
       "tab" is in strtab, has value (relative @) 16 and is defined in the bss section
    */
    Elf32_Sym *syms = make_syms(size_table, sym_tab, strtab, shstrtab, table_des_symboles);

    symtab   = make_symtab_section( shstrtab, strtab, syms, size_table);


    /* make hard coded text and data relocation entries (needs symtab and strtab)
      first relocation "ADDI $2, $3, boucle" at adress 4 of the text section is a R_MIPS_LO16 with respect to the symbole "boucle" so the relocation is made with respect to the .text section symbol (since the value of boucle, 4, will be the addend)
      second relocation ".word tab" at adress 0 of the data section is a R_MIPS_32 with respect to the symbole "tab" so the relocation is made with respect to the .bss section symbol (since the value of tab, 16, will be the addend)
    */
    Elf32_Rel* text_reloc = make_elf32_reloc(size_reloc_text, reloc_table_text, symtab, strtab, shstrtab );
    Elf32_Rel* data_reloc = make_elf32_reloc(size_reloc_data, reloc_table_data, symtab, strtab, shstrtab );

    reltext  = make_rel32_section( ".rel.text", text_reloc, size_reloc_text);
    reldata  = make_rel32_section( ".rel.data", data_reloc, size_reloc_data);


    /*write these sections in file*/
    elf_write_relocatable( name, machine, noreorder,
                           text->start, text->sz,
                           data->start, data->sz,
                           bss->start, bss->sz,
                           shstrtab->start, shstrtab->sz,
                           strtab->start, strtab->sz,
                           symtab->start, symtab->sz,
                           reltext->start, reltext->sz,
                           reldata->start, reldata->sz);


    print_section( text );
    print_section( data );
    print_section( bss );
    print_section( strtab );
    print_section( symtab );


    /*clean up */
    del_section(     text );
    del_section(     data );
    del_section(      bss );
    del_section( shstrtab );
    del_section(   strtab );
    del_section(   symtab );
    del_section(  reltext );
    del_section(  reldata );

    return 0;

}




//--------------------------------------------------------------------------------
//--------------------------- VERSION DES PROFS-----------------------------------
//--------------------------- ne pas modifier ! ----------------------------------
// int main_init_function(int* text_tab, int* data_tab, char** sym_tab, int size_instr, int size_data, int size_table) {
//
//     /* prepare sections*/
//     section     text = NULL;
//     section     data = NULL;
//     section      bss = NULL;
//     section shstrtab = NULL;
//     section   strtab = NULL;
//     section   symtab = NULL;
//     section  reltext = NULL;
//     section  reldata = NULL;
//
//     /* make predefined section table*/
//     shstrtab = make_shstrtab_section();
//
//     /* make hard coded program data already in big endian */
//     int text_prog[]= {0x20106400,0x04006220,0xFEFF4310};
//     int data_prog[]= {0x10000000};
//     int bss_prog = 16+8;
//     char * sym_char[] = {"boucle","tab"};
//     char* machine = "mips";
//     char* name = "exemple.o";
//     /* pelf options */
//     int noreorder =1;
//
//
//     /* Create text, data and bss sections*/
//     text     = make_text_section(text_prog, 3);
//
//     if ( !text ) {
//         fprintf( stderr, "Unable to write .text section (missing information).\n" );
//         return -1;
//     }
//
//     data = make_data_section(  data_prog, 1);
//     if ( !data ) {
//         fprintf( stderr, "Unable to write .data section (missing information).\n" );
//         return -1;
//     }
//
//     bss = make_bss_section(  bss_prog);
//     if ( !bss ) {
//         fprintf( stderr, "Unable to write .bss section (missing information).\n" );
//         return -1;
//     }
//
//     /* Write all string linked to symbols*/
//     strtab   = make_strtab_section( sym_char, 2);
//
//
//     /* make hard coded symbols data (needs shstrtab and strtab)
//        "boucle" is in strtab, has value (relative @) 4 and is defined in the text section
//        "tab" is in strtab, has value (relative @) 16 and is defined in the bss section
//     */
//     Elf32_Sym syms[2]= {{0}};
//     // boucle
//     syms[0].st_name = elf_get_string_offset( strtab->start, strtab->sz, sym_char[0] );
//     syms[0].st_size = 0;
//     syms[0].st_value = 4;
//     syms[0].st_info = ELF32_ST_INFO( STB_LOCAL, STT_NOTYPE );
//     syms[0].st_other = 0;
//     syms[0].st_shndx  = elf_get_string_index( shstrtab->start, shstrtab->sz, ".text" );
//     // tab
//     syms[1].st_name = elf_get_string_offset( strtab->start, strtab->sz, sym_char[1]);
//     syms[1].st_value = 16;
//     syms[1].st_size = 0;
//     syms[1].st_info = ELF32_ST_INFO( STB_LOCAL, STT_NOTYPE );
//     syms[1].st_other = 0;
//     syms[1].st_shndx = elf_get_string_index( shstrtab->start, shstrtab->sz, ".bss" );
//
//     symtab   = make_symtab_section( shstrtab, strtab, syms,2);
//
//
//     /* make hard coded text and data relocation entries (needs symtab and strtab)
//       first relocation "ADDI $2, $3, boucle" at adress 4 of the text section is a R_MIPS_LO16 with respect to the symbole "boucle" so the relocation is made with respect to the .text section symbol (since the value of boucle, 4, will be the addend)
//       second relocation ".word tab" at adress 0 of the data section is a R_MIPS_32 with respect to the symbole "tab" so the relocation is made with respect to the .bss section symbol (since the value of tab, 16, will be the addend)
//     */
//     Elf32_Rel text_reloc[1];
//     text_reloc[0].r_offset =4;
//     text_reloc[0].r_info=ELF32_R_INFO(elf_get_sym_index_from_name(symtab, shstrtab, strtab,".text"),R_MIPS_LO16);
//     Elf32_Rel data_reloc[1];
//     data_reloc[0].r_offset =0;
//     data_reloc[0].r_info=ELF32_R_INFO(elf_get_sym_index_from_name(symtab, shstrtab,strtab,".bss"),R_MIPS_32);
//
//
//     reltext  = make_rel32_section( ".rel.text", text_reloc,1);
//     reldata  = make_rel32_section( ".rel.data", data_reloc,1);
//
//
//     /*write these sections in file*/
//     elf_write_relocatable( name, machine, noreorder,
//                            text->start, text->sz,
//                            data->start, data->sz,
//                            bss->start, bss->sz,
//                            shstrtab->start, shstrtab->sz,
//                            strtab->start, strtab->sz,
//                            symtab->start, symtab->sz,
//                            reltext->start, reltext->sz,
//                            reldata->start, reldata->sz);
//
//
//     print_section( text );
//     print_section( data );
//     print_section( bss );
//     print_section( strtab );
//     print_section( symtab );
//
//
//     /*clean up */
//     del_section(     text );
//     del_section(     data );
//     del_section(      bss );
//     del_section( shstrtab );
//     del_section(   strtab );
//     del_section(   symtab );
//     del_section(  reltext );
//     del_section(  reldata );
//
//     return 0;
//
// }
